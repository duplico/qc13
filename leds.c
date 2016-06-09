/*
 * leds.c
 * (c) 2014 George Louthan
 * 3-clause BSD license; see license.md.
 */

#include "leds.h"
#include <stdint.h>
#include <string.h>
#include "qc13.h"

/*
 *   LED controller (TLC5948A)
 *        (write on rise, change on fall,
 *         clock inactive low, MSB first)
 *        eUSCI_A0 - LEDs
 *        somi, miso, clk
 *        GSCLK     P1.2 (timer TA1.1)
 *        LAT       P1.4
 */

#define TLC_THISISGS    0x00
#define TLC_THISISFUN   0x01

// Current TLC sending state:
uint8_t tlc_send_type = TLC_SEND_IDLE;

uint8_t tlc_tx_index = 0;   // Index of currently sending buffer


uint8_t tlc_loopback_data_out = 0x00;
volatile uint8_t tlc_loopback_data_in = 0x00;

// Buffers containing actual data to send to the TLC:

uint8_t fun_base[] = {
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00, // ...reserved...
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        // B135 / PSM(D1)       0
        // B134 / PSM(D0)       0
        // B133 / OLDENA        0
        // B132 / IDMCUR(D1)    0
        // B131 / IDMCUR(D0)    0
        // B130 / IDMRPT(D0)    0
        // B129 / IDMENA        0
        // B128 / LATTMG(D1)    1:
        0x01,
        // B127 / LATTMG(D0)    1
        // B126 / LSDVLT(D1)    0
        // B125 / LSDVLT(D0)    0
        // B124 / LODVLT(D1)    0
        // B123 / LODVLT(D0)    0
        // B122 / ESPWM         1
        // B121 / TMGRST        1
        // B120 / DSPRPT        1:
        0x87,
        // B119 / BLANK
        // and 7 bits of global brightness correction:
        0x08,
        // HERE WE SWITCH TO 7-BIT SPI.
        // The following index is 18:
        0x7F,
        0x7F,
        0x7F,
        0x7F,
        0x7F,
        0x7F,
        0x7F,
        0x7F,
        0x7F,
        0x7F,
        0x7F,
        0x7F,
        0x7F,
        0x7F,
        0x7F,
        0x7F,
};

void tlc_set_gs() {
    if (tlc_send_type != TLC_SEND_IDLE)
        return;
    tlc_send_type = TLC_SEND_TYPE_GS;
    tlc_tx_index = 0;
    EUSCI_A_SPI_transmitData(EUSCI_A0_BASE, TLC_THISISGS);
}

void tlc_set_fun() {
    while (tlc_send_type != TLC_SEND_IDLE)
        __no_operation(); // shouldn't ever actually have to block on this.
    tlc_send_type = TLC_SEND_TYPE_FUN;
    tlc_tx_index = 0;
    EUSCI_A_SPI_transmitData(EUSCI_A0_BASE, TLC_THISISFUN);
}

// Stage the blank bit:
void tlc_stage_blank(uint8_t blank) {
    if (blank) {
        fun_base[17] |= BIT7;
        fun_base[16] &= ~BIT1;
    } else {
        fun_base[17] &= ~BIT7;
        fun_base[16] |= BIT1;
    }
}

// Test the TLC chip with a shift-register loopback.
// Returns 0 for success and 1 for failure.
uint8_t tlc_test_loopback(uint8_t test_pattern) {
    // Send the test pattern 34 times, and expect to receive it shifted
    // a bit.
    tlc_loopback_data_out = test_pattern;
    while (tlc_send_type != TLC_SEND_IDLE); // I don't see this happening...

    EUSCI_A_SPI_clearInterrupt(EUSCI_A0_BASE, EUSCI_A_SPI_RECEIVE_INTERRUPT);
    EUSCI_A_SPI_enableInterrupt(EUSCI_A0_BASE, EUSCI_A_SPI_RECEIVE_INTERRUPT);

    tlc_send_type = TLC_SEND_TYPE_LB;
    tlc_tx_index = 0;
    EUSCI_A_SPI_transmitData(EUSCI_A0_BASE, test_pattern);
    // Spin while we send and receive:
    while (tlc_send_type != TLC_SEND_IDLE);

    EUSCI_A_SPI_disableInterrupt(EUSCI_A0_BASE, EUSCI_A_SPI_RECEIVE_INTERRUPT);

    return tlc_loopback_data_in != (uint8_t) ((test_pattern << 7) | (test_pattern >> 1));
}

// Stage global brightness if different from default:
void tlc_stage_bc(uint8_t bc) {
    bc = bc & 0b01111111; // Mask out BLANK just in case.
    fun_base[17] &= 0b10000000;
    fun_base[17] |= bc;
}

void init_tlc() {
    // This is just out of an abundance of caution:
    UCA0CTLW0 |= UCSWRST;
    UCA0CTLW0 &= ~UC7BIT;
    UCA0CTLW0 &= ~UCSWRST;

    EUSCI_A_SPI_clearInterrupt(EUSCI_A0_BASE, EUSCI_A_SPI_TRANSMIT_INTERRUPT);
    EUSCI_A_SPI_enableInterrupt(EUSCI_A0_BASE, EUSCI_A_SPI_TRANSMIT_INTERRUPT);

    tlc_set_gs();

    tlc_stage_blank(1);
    tlc_set_fun();
}

uint8_t bank = 0;
uint8_t bank_brightness[] = {0xff, 0x01, 0x10, 0x80, 0x01, 0x0f};

#pragma vector=USCI_A0_VECTOR
__interrupt void EUSCI_A0_ISR(void)
{
    switch (__even_in_range(UCA0IV, 4)) {
    //Vector 2 - RXIFG
    case 2:
        // We received some garbage sent to us while we were sending.
        if (tlc_send_type == TLC_SEND_TYPE_LB) {
            // We're only interested in it if we're doing a loopback test.
            tlc_loopback_data_in = EUSCI_B_SPI_receiveData(EUSCI_A0_BASE);
        } else {
            EUSCI_B_SPI_receiveData(EUSCI_A0_BASE); // Throw it away.
        }
        break; // End of RXIFG ///////////////////////////////////////////////////////

    case 4: // Vector 4 - TXIFG : I just sent a byte.
        if (tlc_send_type == TLC_SEND_TYPE_GS) {
            if (tlc_tx_index == 32) { // done
                GPIO_pulse(TLC_LATPORT, TLC_LATPIN); // LATCH.
                tlc_send_type = TLC_SEND_IDLE;

                switch (bank) {
                case 0:
                    LED_BANK1_OUT &= ~LED_BANK1_PIN;
                    bank++;
                    break;
                case 1:
                    LED_BANK2_OUT &= ~LED_BANK2_PIN;
                    bank++;
                    break;
                case 2:
                    LED_BANK3_OUT &= ~LED_BANK3_PIN;
                    bank++;
                    break;
                case 3:
                    LED_BANK4_OUT &= ~LED_BANK4_PIN;
                    bank++;
                    break;
                case 4:
                    LED_BANK5_OUT &= ~LED_BANK5_PIN;
                    bank++;
                    break;
                case 5:
                    LED_BANK6_OUT &= ~LED_BANK6_PIN;
                    bank = 0;
                    break;
                }

                break;
            } else { // gs - MSB first; this starts with 0.
                if (tlc_tx_index & 0x01) { // odd; less significant byte
                    EUSCI_A_SPI_transmitData(EUSCI_A0_BASE, 0x00);
                } else { // even; more significant byte
                    EUSCI_A_SPI_transmitData(EUSCI_A0_BASE, light); //bank_brightness[bank]); // TODO
                }
            }
            tlc_tx_index++;
        } else if (tlc_send_type == TLC_SEND_TYPE_FUN) {
            if (tlc_tx_index == 18) { // after 18 we have to switch to 7-bit mode.
                UCA0CTLW0 |= UCSWRST;
                UCA0CTLW0 |= UC7BIT;
                UCA0CTLW0 &= ~UCSWRST;
                EUSCI_A_SPI_clearInterrupt(EUSCI_A0_BASE, EUSCI_A_SPI_TRANSMIT_INTERRUPT);
                EUSCI_A_SPI_enableInterrupt(EUSCI_A0_BASE, EUSCI_A_SPI_TRANSMIT_INTERRUPT);
            } else if (tlc_tx_index == 34) {
                GPIO_pulse(TLC_LATPORT, TLC_LATPIN); // LATCH.
                UCA0CTLW0 |= UCSWRST;
                UCA0CTLW0 &= ~UC7BIT;
                UCA0CTLW0 &= ~UCSWRST;
                EUSCI_A_SPI_clearInterrupt(EUSCI_A0_BASE, EUSCI_A_SPI_TRANSMIT_INTERRUPT);
                EUSCI_A_SPI_enableInterrupt(EUSCI_A0_BASE, EUSCI_A_SPI_TRANSMIT_INTERRUPT);
                tlc_send_type = TLC_SEND_IDLE;
                break;
            }
            EUSCI_A_SPI_transmitData(EUSCI_A0_BASE, fun_base[tlc_tx_index]);
            tlc_tx_index++;
        } else if (tlc_send_type == TLC_SEND_TYPE_LB) { // Loopback for POST
            if (tlc_tx_index == 33) {
                tlc_send_type = TLC_SEND_IDLE;
                break;
            }
            EUSCI_A_SPI_transmitData(EUSCI_A0_BASE, tlc_loopback_data_out);
            tlc_tx_index++;
        } else {
            tlc_send_type = TLC_SEND_IDLE; // probably shouldn't reach.
        }
        break; // End of TXIFG /////////////////////////////////////////////////////

    default: break;
    } // End of ISR flag switch ////////////////////////////////////////////////////
}
