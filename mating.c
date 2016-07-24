/*
 * mating.c
 *
 *  Created on: Jul 13, 2016
 *      Author: George
 */

#include <qc13.h>
#include "mating.h"
#include "led_display.h"
#include "leg_anims.h"
#include "badge.h"
#include "metrics.h"

//typedef struct {
//    uint8_t proto_version;
//    uint8_t from_addr;
//    uint8_t hat_award_id;
//    uint8_t camo_id;
//    uint16_t flags;
//    uint16_t crc16;
//} matepayload;

matepayload mp_out = {
        MATE_VERSION | MATE_PROTO_BADGE,
        0, //my_conf.badge_id,
        0xff,
        0, //my_conf.camo_id,
        BIT6,
        0x00
};
matepayload mp_in = {0};

const uint8_t mate_sync_bytes[MATE_NUM_SYNC_BYTES] = {0xa0, 0xff, 0x00, 0xaa};

uint8_t mate_payload_out[sizeof(matepayload) + MATE_NUM_SYNC_BYTES] = {0};
uint8_t mate_payload_in[sizeof(matepayload)] = {0};

uint8_t mate_state = 0;

volatile uint8_t uart_in_ignore = 0;
volatile uint8_t uart_in_synced = 0;
volatile uint8_t uart_in_index = 0;
volatile uint8_t uart_in_len = 0;
volatile uint8_t uart_out_index = 0;
volatile uint8_t uart_sending = 0;
volatile uint8_t uart_out_len = sizeof(matepayload) + MATE_NUM_SYNC_BYTES;

void init_mating() {
    GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P2, GPIO_PIN5, GPIO_SECONDARY_MODULE_FUNCTION); // TX
    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P2, GPIO_PIN6, GPIO_SECONDARY_MODULE_FUNCTION); // RX

    EUSCI_A_UART_initParam ini = {};
    ini.selectClockSource = EUSCI_A_UART_CLOCKSOURCE_SMCLK;
    ini.clockPrescalar = 104;
    ini.firstModReg = 2;
    ini.secondModReg = 182;
    ini.parity = EUSCI_A_UART_NO_PARITY;
    ini.msborLsbFirst = EUSCI_A_UART_LSB_FIRST;
    ini.numberofStopBits = EUSCI_A_UART_ONE_STOP_BIT;
    ini.uartMode = EUSCI_A_UART_MODE;
    ini.overSampling = 1;

    EUSCI_A_UART_init(EUSCI_A1_BASE, &ini);
    EUSCI_A_UART_enable(EUSCI_A1_BASE);
    EUSCI_A_UART_selectDeglitchTime(EUSCI_A1_BASE, EUSCI_A_UART_DEGLITCH_TIME_200ns);

    EUSCI_A_UART_enableInterrupt(EUSCI_A1_BASE, EUSCI_A_UART_RECEIVE_INTERRUPT);
    EUSCI_A_UART_enableInterrupt(EUSCI_A1_BASE, EUSCI_A_UART_TRANSMIT_INTERRUPT);
}

uint8_t mate_camo = LEG_ANIM_NONE;
uint8_t mate_id = BADGES_IN_SYSTEM;
uint8_t mate_on_duty = 0;
uint8_t super_ink_waits_on_me = 0;
uint16_t mate_ink_wait = 0;
uint8_t in_rst = 1;

void mate_over_cleanup() {
    mate_camo = LEG_ANIM_NONE;
    mate_state = MS_IDLE;
    in_rst = 1;
}

void maybe_enter_ink_wait(uint8_t local) {
    if (my_conf.camo_id != mate_camo) {
        return; //nope. :-(
    }
    mate_state = MS_INK_WAIT;
    mate_ink_wait = 0;
    super_ink_waits_on_me = !local;
}

void enter_super_inking() {
    mate_ink_wait = 0;
    just_sent_superink = 1;
    mate_state = MS_SUPER_INK;
}

void ink_wait_timeout() {
    // We were in INK_WAIT, meaning we're waiting
    //  on either a local or remote button. But it didn't happen in time.
    mate_state = MS_PAIRED;
}

void super_ink_timeout() {
    // Cooldown is over!
    mate_state = MS_PAIRED;
}

void mate_deferred_rx_interrupt() {
    // Here we handle the messages.
    // Load 'er up:
    memcpy(&mp_in, mate_payload_in, sizeof(matepayload));
    uart_in_ignore = 0;

    // Basic rules:
    //  If O_HAI isn't asserted, we stay in MS_IDLE.

    //  If we get a RST anywhere outside PLUG or HALF_PAIR we go back to PLUG.
    if (mate_state > MS_HALF_PAIR && mp_in.flags & M_RST) {
        mate_state = MS_PLUG;
        // TODO: CLEAN SWEEP
    }

    // TODO: Make sure these are all the same.
    mate_camo = mp_in.camo_id;
    mate_id = mp_in.from_addr;
    mate_on_duty = (mp_in.flags & M_HANDLER_ON_DUTY)? 1 : 0;

    if ((mp_in.flags & M_INK) && (my_conf.camo_id == mate_camo)) {
        // "received ink button" animation (and our camos match)
        tentacle_send_meta_mating(1, 0);
    }
    switch(mate_state) {
    case MS_IDLE:
        mate_over_cleanup();
        break;
    case MS_HALF_PAIR:
        // Here we expect a ~RST message. If we get it we go to PAIRED!
        // Technically this is the same behavior as below, so we...
        // fall through:
    case MS_PLUG:
        // Here we expect a RX from a badge or pipe.
        // We send a ~RST regardless.
        //  But what we get is probably going to have RST set: to HALF_PAIR
        //  But if it doesn't, we can go to PAIRED!
        in_rst = 0;
        mate_send_basic(0, 0, 0);
        if (mp_in.flags & M_RST) {
            // fresh message, go to HALF_PAIR
            mate_state = MS_HALF_PAIR;
        } else {
            // dank message, go to PAIRED
            if ((mp_in.flags & M_PIPE) && (mate_id == DEDICATED_BASE_ID)) {
                // pipe
                mate_state = MS_PIPE_PAIRED;
            } else if (!(mp_in.flags & M_PIPE) && mate_id != DEDICATED_BASE_ID){
                // badge
                mate_state = MS_PAIRED;
                mate_start(mate_id, mate_on_duty);
            }
        }
        break;
    case MS_INK_WAIT:
        // Here we might get an ink button from our partner.
        if (!super_ink_waits_on_me && (mp_in.flags & M_INK)) {
            // we were waiting on our partner, so this is what we wanted.
            enter_super_inking();
            break;
        } // otherwise, ignore it.
        // fall through:
    case MS_PAIRED:
        // Here we might get either:
        //  ink button from partner
        //   (go to INK_WAIT)
        if (mp_in.flags & M_INK) {
            maybe_enter_ink_wait(0); // waiting on me!
        }
        //  award message
        //   (get award! yay!)
        if ((is_uber(mp_in.from_addr) || (mp_in.from_addr == DEDICATED_BASE_ID)) && mp_in.flags & M_HAT_AWARD) {
            if (award_hat(mp_in.hat_award_id)) {
                mate_send_hat_response(1);
            } else {
                mate_send_hat_response(0);
            }
        }
        //  gild message
        if ((mp_in.flags & M_BESTOW_GILD) && !(my_conf.gilded & GILD_AVAIL)) {
            my_conf.gilded = GILD_AVAIL | GILD_ON;
            eye_twinkle_on();
        }
        //  or a regular old message that's just updating
        //  some of the state we have about our partner.
        //  That stuff is all handled above.
        break;
    case MS_SUPER_INK:
        // this is just a place we hang out. Ignore messages here probably.
        // We only care about the stuff that gets set above.
        break;
    case MS_PIPE_PAIRED:
        // We might get a reply from the pipe. It may give us stuff!
        //  Or it'll just print and leave us hanging. That's OK too.
        if ((mp_in.from_addr == DEDICATED_BASE_ID) && (mp_in.flags & M_CLAIMED_HAT)) {
            // If the incoming hat ID matches my current hat, OR it gets awarded successfully,
            //  CLAIM IT.
            if ((mp_in.hat_award_id == my_conf.hat_id) || award_hat(mp_in.hat_award_id)) { // TODO: I suspect fragility.
                mate_send_hat_response(1);
                claim_hat(mp_in.hat_award_id);
            } else {
                mate_send_hat_response(0);
            }
        }
        break;
    case MS_PIPE_DONE:
        // Yeah, we shouldn't get anything here.
        break;
    case MS_UBER_HAT_OFFER:
        // we expect an ACK or a NACK.
        if (mp_in.flags & M_HAT_AWARD_ACK) {
            // our hat offer was accepted.
            my_conf.uber_hat_given = 1;
            mate_state = MS_PAIRED;
            // TODO: save
        } else if (mp_in.flags & M_HAT_AWARD_NACK) {
            // nuttin.
            mate_state = MS_PAIRED;
        }
    }
}

void mate_send_preserve_flags() {
    if (uart_sending) {
        __no_operation();
        return; // don't. // TODO: like an ass.
    }

    mp_out.from_addr = my_conf.badge_id;
    mp_out.camo_id = my_conf.camo_id;

    mp_out.achievements = my_conf.achievements;

    if (0) // TODO
        mp_out.flags |= M_HANDLER_ON_DUTY;

    if (my_conf.hat_holder)
        mp_out.flags |= M_HAT_HOLDER;

    if (my_conf.hat_claimed)
        mp_out.flags |= M_HAT_CLAIMED;

    // CRC it.
    CRC_setSeed(CRC_BASE, MATE_CRC_SEED);
    for (uint8_t i = 0; i < sizeof(matepayload) - 2; i++) {
        CRC_set8BitData(CRC_BASE, ((uint8_t *) &mp_out)[i]);
    }
    mp_out.crc16 = CRC_getResult(CRC_BASE);

    // Fill'er up:
    memcpy(mate_payload_out, mate_sync_bytes, MATE_NUM_SYNC_BYTES);
    memcpy(mate_payload_out+MATE_NUM_SYNC_BYTES, &mp_out, sizeof(matepayload)); // TODO: assert/confirm proper length

    uart_out_index = 0;
    uart_sending = 1;
    uart_out_len = sizeof(matepayload) + MATE_NUM_SYNC_BYTES;
    EUSCI_A_UART_transmitData(EUSCI_A1_BASE, mate_payload_out[0]);
}

void mate_send_flags(uint16_t flags) {
    if (uart_sending) {
        __no_operation();
        return; // don't. // TODO: like an ass.
    }
    mp_out.flags = flags;
    mate_send_preserve_flags();
}

void mate_send_uber_hat_bestow() {
    // TODO: Make sure our partner doesn't have a hat already.
    mp_out.hat_award_id = my_conf.badge_id;
    mate_state = MS_UBER_HAT_OFFER;
    mate_send_flags(M_HAT_AWARD);
}

void mate_send_hat_response(uint8_t ack) {
    if (ack) { // Yes!
        mate_send_flags(M_HAT_AWARD_ACK);
    } else { // NACK
        mate_send_flags(M_HAT_AWARD_NACK);
    }
}

void mate_send_basic(uint8_t click, uint8_t rst, uint8_t gild) {
    uint16_t flags_out = 0;
    if (click)
        flags_out |= M_INK;
    if (rst)
        flags_out |= M_RST;
    if (gild)
        flags_out |= M_BESTOW_GILD;
    mate_send_flags(flags_out);
}

volatile uint8_t uart_in_byte = 0;

//// ISR for pairing:
//#pragma vector=USCI_A1_VECTOR
//__interrupt void EUSCI_A1_ISR(void)
//{
//    switch (__even_in_range(UCA1IV, 4)) {
//    //Vector 2 - RXIFG
//    case 2:
//        uart_in_byte = EUSCI_A_UART_receiveData(EUSCI_A1_BASE);
//        if (uart_in_ignore)
//            return;
//        // If we're not synced, we need to be checking against sync bytes.
//        if (!uart_in_synced) {
//            if (uart_in_byte == mate_sync_bytes[uart_in_index]) { // matchy:
//                uart_in_index++;
//                // If we're incremented past the end of the sync bytes:
//                if (uart_in_index >= MATE_NUM_SYNC_BYTES) {
//                    uart_in_synced = 1; // we're synced.
//                    uart_in_index = 0;
//                }
//            } else { // no matchy:
//                uart_in_index = 0;
//            }
//        } else { // If we are, let's add it to the payload.
//            mate_payload_in[uart_in_index] = uart_in_byte;
//            if (uart_in_index == 0) { // the protocol one:
//                uart_in_len = sizeof(matepayload);
//            } else {
//                // broken
//                // ???? TODO ????
//                //  CLEAN SWEEP
//            }
//            uart_in_index++;
//            if (uart_in_index >= uart_in_len) {
//                // Payload received.
//                uart_in_ignore = 1; // Don't clobber good stuff with new stuff.
//                f_mate_interrupt = 1;
//                uart_in_synced = 0;
//                uart_in_index = 0;
//                __bic_SR_register_on_exit(SLEEP_BITS);
//            }
//        }
//        break; // End of RXIFG ///////////////////////////////////////////////////////
//    case 4: // Vector 4 - TXIFG : I just sent a byte.
//        if (uart_sending) { // just finished sending uart_out_index.
//            uart_out_index++;
//            if (uart_out_index >= uart_out_len) { // done
//                uart_sending = 0;
//            } else { // more to send
//                EUSCI_A_UART_transmitData(EUSCI_A1_BASE, mate_payload_out[uart_out_index]);
//            }
//        }
//        break; // End of TXIFG /////////////////////////////////////////////////////
//
//    default: break;
//    } // End of ISR flag switch ////////////////////////////////////////////////////
//}
