/*
 * rfm75.c
 *
 * Queercon 13 radio driver for the HopeRF RFM75.
 *
 * (c) 2016 George Louthan
 * 3-clause BSD license; see license.md.
 */

#include "rfm75.h"
#include <stdint.h>
#include "qc13.h"
#include "badge.h"
#include "leg_anims.h"
#include "metrics.h"

// Radio (RFM75):
// CE   P3.2 (or 1.2 for launchpad)
// CSN  P1.3
// SCK  P2.2
// MOSI P1.6
// MISO P1.7
// IRQ  P3.1 (or 3.0 for launchpad)

#define RFM75_CSN_OUT P1OUT
#define RFM75_CSN_PIN  GPIO_PIN3

#define CSN_LOW_START RFM75_CSN_OUT &= ~RFM75_CSN_PIN
#define CSN_HIGH_END  RFM75_CSN_OUT |= RFM75_CSN_PIN

#if BADGE_TARGET
// Target is the actual badge:
#define CE_ACTIVATE P3OUT   |=  BIT2
#define CE_DEACTIVATE P3OUT &= ~BIT2
#else
// Target is the Launchpad+shield:
#define CE_ACTIVATE P1OUT   |=  BIT2
#define CE_DEACTIVATE P1OUT &= ~BIT2
#endif

uint8_t rx_addr_p0[3] = {0xff, 0xff, 0xff};
uint8_t rx_addr_p1[3] = {0xff, 0xff, 0x00};
uint8_t tx_addr[3] = {0xff, 0xff, 0xff};

uint8_t rfm75_retransmit_num = 0;
uint32_t rfm75_seqnum = 0;
uint32_t rfm75_prev_seqnum = 0xFFFFFFFF;

// State values:
#define RFM75_BOOT 0
#define RFM75_RX_INIT 1
#define RFM75_RX_LISTEN 2
#define RFM75_RX_READY 3
#define RFM75_TX_INIT 4
#define RFM75_TX_READY 5
#define RFM75_TX_FIFO 6
#define RFM75_TX_SEND 7
#define RFM75_TX_DONE 8

uint8_t rfm75_state = RFM75_BOOT;

///////////////////////////////
// Bank initialization values:

#define BANK0_INITS 17

//Bank0 register initialization value
const uint8_t bank0_init_data[BANK0_INITS][2] = {
        { CONFIG, 0b00001111 }, //
        { 0x01, 0b00000000 }, //No auto-ack
        { 0x02, 0b00000011 }, //Enable RX pipe 0 and 1
        { 0x03, 0b00000001 }, //RX/TX address field width 3byte
        { 0x04, 0b00000000 }, //no auto-RT
        { 0x05, 0x53 }, //channel: 2400 + LS 7 bits of this field = channel (2.483)
        { 0x06, 0b00000101 }, //air data rate-1M,out power 5dbm,setup LNA gain.
        { 0x07, 0b01110000 }, // Clear interrupt flags
        // 0x0a - RX_ADDR_P0 - 3 bytes
        // 0x0b - RX_ADDR_P1 - 3 bytes
        // 0x10 - TX_ADDR - 5 bytes
        { 0x11, RFM75_PAYLOAD_SIZE }, //Number of bytes in RX payload in data pipe0(32 byte)
        { 0x12, RFM75_PAYLOAD_SIZE }, //Number of bytes in RX payload in data pipe1(32 byte)
        { 0x13, 0 }, //Number of bytes in RX payload in data pipe2 - disable
        { 0x14, 0 }, //Number of bytes in RX payload in data pipe3 - disable
        { 0x15, 0 }, //Number of bytes in RX payload in data pipe4 - disable
        { 0x16, 0 }, //Number of bytes in RX payload in data pipe5 - disable
        { 0x17, 0 },
        { 0x1c, 0x00 }, // No dynamic packet length
        { 0x1d, 0b00000000 } // 00000 | DPL | ACK | DYN_ACK
};

uint8_t payload_serial_bytes[RFM75_PAYLOAD_SIZE] = {0xff, 0x00, 0xff, 0};

uint8_t payload_in[RFM75_PAYLOAD_SIZE] = {0};
uint8_t payload_out[RFM75_PAYLOAD_SIZE] = {0};

uint8_t usci_b0_recv_sync(uint8_t data) {
    EUSCI_A_SPI_transmitData(EUSCI_B0_BASE, data);
    while (!EUSCI_B_SPI_getInterruptStatus(EUSCI_B0_BASE,
            EUSCI_B_SPI_TRANSMIT_INTERRUPT));
    while (!EUSCI_B_SPI_getInterruptStatus(EUSCI_B0_BASE,
            EUSCI_B_SPI_RECEIVE_INTERRUPT));
    return EUSCI_B_SPI_receiveData(EUSCI_B0_BASE);
}

void usci_b0_send_sync(uint8_t data) {
    usci_b0_recv_sync(data);
}

uint8_t rfm75_get_status() {
    uint8_t recv;
    CSN_LOW_START;
    recv = usci_b0_recv_sync(NOP_NOP);
    CSN_HIGH_END;
    return recv;
}

uint8_t send_rfm75_cmd(uint8_t cmd, uint8_t data) {
    uint8_t ret;
    CSN_LOW_START;
    usci_b0_send_sync(cmd);
    ret = usci_b0_recv_sync(data);
    CSN_HIGH_END;
    return ret;
}

void send_rfm75_cmd_buf(uint8_t cmd, uint8_t *data, uint8_t data_len) {
    CSN_LOW_START;
    usci_b0_send_sync(cmd);
    for (uint8_t i=1; i<=data_len; i++) {
        usci_b0_send_sync(data[data_len-i]);
    }
    CSN_HIGH_END;
}

void read_rfm75_cmd_buf(uint8_t cmd, uint8_t *data, uint8_t data_len) {
    CSN_LOW_START;
    usci_b0_send_sync(cmd);
    for (uint8_t i=1; i<=data_len; i++) {
        data[data_len-i] = usci_b0_recv_sync(0xab);
    }
    CSN_HIGH_END;
}

uint8_t rfm75_read_byte(uint8_t cmd) {
    cmd &= 0b00011111;
    CSN_LOW_START;
    usci_b0_send_sync(cmd);
    uint8_t recv = usci_b0_recv_sync(0xff);
    CSN_HIGH_END;
    return recv;
}

void rfm75_write_reg(uint8_t reg, uint8_t data) {
    reg &= 0b00011111;
    send_rfm75_cmd(WRITE_REG | reg, data);
}

void rfm75_write_reg_buf(uint8_t reg, uint8_t *data, uint8_t data_len) {
    reg &= 0b00011111;
    send_rfm75_cmd_buf(WRITE_REG | reg, data, data_len);
}

void rfm75_select_bank(uint8_t bank) {
    volatile uint8_t currbank = rfm75_get_status() & 0x80; // Get MSB, which is active bank.
    if ((currbank && (bank==0)) || ((currbank==0) && bank)) {
        uint8_t switch_bank = 0x53;
        send_rfm75_cmd(ACTIVATE_CMD, switch_bank);
    }
}

uint8_t rfm75_post() {
    volatile uint8_t bank_one = rfm75_get_status() & 0x80; // Get MSB, which is active bank.
    send_rfm75_cmd(ACTIVATE_CMD, 0x53);
    volatile uint8_t bank_two = rfm75_get_status() & 0x80; // Get MSB, which is active bank.

    rfm75_select_bank(0); // Go back to the normal bank.

    if (bank_one == bank_two) {
        return 0;
    }
    return 1;
}

void rfm75_enter_prx() {
    rfm75_state = RFM75_RX_INIT;
    CE_DEACTIVATE;
    // Power up & PRX: CONFIG=0b01101011
    rfm75_select_bank(0);
    rfm75_write_reg(CONFIG, 0b00111111);
    // Clear interrupts: STATUS=BIT4|BIT5|BIT6
    rfm75_write_reg(STATUS, BIT4|BIT5|BIT6);

    // Enter RX mode.
    CE_ACTIVATE;

    rfm75_state = RFM75_RX_LISTEN;
}

void rfm75_tx() {
    rfm75_retransmit_num = 0;

    // Fill'er up:
    memcpy(payload_out, &out_payload, RFM75_PAYLOAD_SIZE);

    rfm75_state = RFM75_TX_INIT;
    CE_DEACTIVATE;
    rfm75_select_bank(0);
    rfm75_write_reg(CONFIG, 0b01011110);
    // Clear interrupts: STATUS=BIT4|BIT5|BIT6
    rfm75_write_reg(STATUS, BIT4|BIT5|BIT6);

    rfm75_state = RFM75_TX_FIFO;
    // Write the payload:
    send_rfm75_cmd_buf(WR_TX_PLOAD, payload_out, RFM75_PAYLOAD_SIZE);
    rfm75_state = RFM75_TX_SEND;
    CE_ACTIVATE;
    // Now we wait for an IRQ to let us know it's sent.
}

void rfm75_init()
{
    GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P1, GPIO_PIN6, GPIO_SECONDARY_MODULE_FUNCTION); // TX
    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P1, GPIO_PIN7, GPIO_SECONDARY_MODULE_FUNCTION); // RX
    GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P2, GPIO_PIN2, GPIO_SECONDARY_MODULE_FUNCTION); // CLK
    // Setup USCI_B0.
    EUSCI_B_SPI_initMasterParam ini = {0};
    ini.selectClockSource = EUSCI_B_SPI_CLOCKSOURCE_SMCLK;
    ini.clockSourceFrequency = SMCLK_RATE_HZ;
    ini.desiredSpiClock = 4000000;
    ini.msbFirst = EUSCI_B_SPI_MSB_FIRST;
    ini.clockPhase = EUSCI_B_SPI_PHASE_DATA_CAPTURED_ONFIRST_CHANGED_ON_NEXT;
    ini.clockPolarity = EUSCI_B_SPI_CLOCKPOLARITY_INACTIVITY_LOW;
    ini.spiMode = EUSCI_B_SPI_3PIN;

    EUSCI_B_SPI_initMaster(EUSCI_B0_BASE, &ini);
    EUSCI_B_SPI_enable(EUSCI_B0_BASE);

    rfm75_seqnum = 0;
    rfm75_seqnum |= ((uint32_t) my_conf.badge_id) << 24;
    rfm75_prev_seqnum = 0xFFFFFFFF;

    // We're going totally synchronous on this; no interrupts at all.

    delay_millis(150); // Delay more than 50ms.

    // Setup GPIO:
    P1DIR |= BIT3; // CSN
    P1OUT |= BIT3; // init high.
    P3DIR |= BIT2;
    P3OUT &= ~BIT2;
    P3DIR &= ~BIT1;
    P3REN &= ~BIT1;
    P3SEL0 &= ~BIT1;
    P3SEL1 &= ~BIT1;
    // We'll wait on the interrupt enables though...

    // Let's start with bank 0:
    rfm75_select_bank(0);

    for(uint8_t i=0;i<BANK0_INITS;i++)
        rfm75_write_reg(bank0_init_data[i][0], bank0_init_data[i][1]);

    volatile uint8_t temp = 0;
    volatile uint8_t test = 0;
    for(uint8_t i=0;i<BANK0_INITS;i++) {
        temp = rfm75_read_byte(bank0_init_data[i][0]);
        test = temp == bank0_init_data[i][1];
        __no_operation();
    }

    // Next fill address buffers
    //  Reg 0x0a: 5 bytes RX0 addr (unicast)
    //  Reg 0x0b: 5 bytes RX1 addr (broadcast)
    //  Reg 0x10: 5 bytes TX0 addr (same as RX0)
    rfm75_write_reg_buf(RX_ADDR_P0, rx_addr_p0, 3);
    rfm75_write_reg_buf(RX_ADDR_P1, rx_addr_p1, 3);
    rfm75_write_reg_buf(TX_ADDR, tx_addr, 3);

    // OK, that's bank 0 done. Next is bank 1.

    rfm75_select_bank(1);


    // Some of these go MOST SIGNIFICANT BYTE FIRST: (so we start with 0xE2.)
    //  (we show them here LEAST SIGNIFICANT BYTE FIRST because we
    //   reverse everything we send.)
    // Like this:
    uint8_t bank1_config_0x00[][4] = {
            {0xe2, 0x01, 0x4b, 0x40}, // reserved (prescribed)
            {0x00, 0x00, 0x4b, 0xc0}, // reserved (prescribed)
            {0x02, 0x8c, 0xfc, 0xd0}, // reserved (prescribed)
            {0x41, 0x39, 0x00, 0x99}, // reserved (prescribed)
            {0x1b, 0x82, 0x96, 0xf9}, // 1 Mbps // The user guide flips it for us.
            {0xa6, 0x0f, 0x06, 0x24}, // 1 Mbps
    };

    for (uint8_t i=0; i<6; i++) {
        rfm75_write_reg_buf(i, bank1_config_0x00[i], 4);
    }

    uint8_t bank1_config_0x0c[][4] = {
            {0x00, 0x73, 0x12, 0x00}, // 120 us mode (PLL settle time?)
            {0x00, 0x80, 0xb4, 0x36}, // reserved?
    };

    for (uint8_t i=0; i<2; i++) {
        rfm75_write_reg_buf(0x0c+i, bank1_config_0x0c[i], 4);
    }

    uint8_t bank1_config_0x0e[11] = {0xff, 0xff, 0xfe, 0xf7, 0xcf, 0x20, 0x81, 0x04, 0x08, 0x20, 0x41};
    // {0x41, 0x20, 0x08, 0x04, 0x81, 0x20, 0xcf, 0xf7, 0xfe, 0xff, 0xff}; // ramp curve, prescribed
    rfm75_write_reg_buf(0x0e, bank1_config_0x0e, 11);

    // Now we go back to bank 0, because that's the one we normally
    //  care about.

    rfm75_select_bank(0);

    // Enable our interrupts:
    P3IES |= BIT1;
    P3IFG &= ~BIT1;
    P3IE |= BIT1;

    // And we're off to see the wizard!

    CSN_LOW_START;
    usci_b0_send_sync(FLUSH_RX);
    CSN_HIGH_END;
    CSN_LOW_START;
    usci_b0_send_sync(FLUSH_TX);
    CSN_HIGH_END;

    rfm75_enter_prx();
    __no_operation();
}

uint8_t radio_payload_validate(rfbcpayload *payload) {
    // bad src ID
    if (!(payload->badge_addr < BADGES_IN_SYSTEM || payload->badge_addr == DEDICATED_BASE_ID)) {
        return 0;
    }

    // ink id overflow when ink is requested
    if ((payload->flags & RFBC_INK) && (payload->ink_id >= LEG_ANIM_COUNT) && (payload->ink_id != LEG_ANIM_NONE)) {
        return 0;
    }

    // bad ink id and ink flag set
    if (payload->ink_id == LEG_ANIM_NONE && (payload->flags & RFBC_INK)) {
        return 0;
    }

    // both badge and event set
    if (payload->flags & RFBC_BEACON && payload->flags & RFBC_EVENT) {
        return 0;
    }

    // double-ink set without ink set
    if (payload->flags & (RFBC_INK | RFBC_DINK) == RFBC_DINK) {
        return 0;
    }

    // event flag when base_addr overflows
    if (payload->flags & RFBC_EVENT && payload->base_addr >= EVENTS_IN_SYSTEM) {
        return 0;
    }

    // TODO:
    // incoming ID is same as local ID, and it's not from a base.
//    if (payload->from_addr == my_conf.badge_id && payload->base_addr != NOT_A_BASE) {
//        return 0;
//    }

    // handler on duty but source isn't a handler
    if (payload->flags & RFBC_HANDLER_ON_DUTY && !is_handler(payload->badge_addr)) {
        return 0;
    }

    // Same one we last saw:
    if (payload->seqnum == rfm75_prev_seqnum) {
        return 0;
    }

    // CRC it.
    CRC_setSeed(CRC_BASE, RFM75_CRC_SEED);
    for (uint8_t i = 0; i < sizeof(rfbcpayload) - 2; i++) {
        CRC_set8BitData(CRC_BASE, ((uint8_t *) payload)[i]);
    }

    if (payload->crc16 != CRC_getResult(CRC_BASE)) {
        // Bad CRC
        return 0;
    }

    rfm75_prev_seqnum = payload->seqnum;
    // CRC checks out.
    return 1;
}

void rfm75_deferred_interrupt() {
    // RFM75 interrupt:
    uint8_t iv = rfm75_get_status();
    __no_operation();

    if (iv & BIT5) { // TX interrupt
        if (rfm75_state != RFM75_TX_SEND) {
            return; // TODO: reset?
        }

        // We sent a thing.

        // Go back to standby:
        CE_DEACTIVATE;
        // Clear interrupt
        rfm75_write_reg(STATUS, BIT5);
        rfm75_state = RFM75_TX_DONE;

        if (rfm75_retransmit_num == RF_RESEND_COUNT) {
            // Raise the I-just-sent-a-thing event
            radio_transmit_done();

            rfm75_seqnum++;

            // Go back to listening.
            rfm75_enter_prx();
        } else {
            uint8_t seqnum = rfm75_retransmit_num + 1;
            rfm75_tx();
            rfm75_retransmit_num = seqnum;
        }
        return;
    }

    if (iv & BIT6) { // RX interrupt
        if (rfm75_state != RFM75_RX_LISTEN) {
            return; // TODO: reset?
        }

        // We've received something.
        rfm75_state = RFM75_RX_READY;
        // Which pipe?
        // Read the FIFO. No need to flush it; deleted when read.
        read_rfm75_cmd_buf(RD_RX_PLOAD, payload_in, RFM75_PAYLOAD_SIZE);
        // Clear the interrupt.
        rfm75_write_reg(STATUS, BIT6);
        memcpy(&in_payload, &payload_in, RFM75_PAYLOAD_SIZE);

        // There's a few types of payloads that this is allowed to be:
        //     ==Broadcast==
        //   Handled in the handler...
        //   We also may need to repeat this type of message.
        //     ===Unicast===
        //  # Badge award from base station.

        if (radio_payload_validate(&in_payload))
            radio_broadcast_received(&in_payload);

        // Payload is now allowed to go stale.
        // Assert CE: listen more.
        CE_ACTIVATE;
        rfm75_state = RFM75_RX_LISTEN;
        return;
    }
}

#pragma vector=PORT3_VECTOR
__interrupt void RFM_ISR(void)
{
    if (P3IV != 0x04) {
        return;
    }
    f_rfm75_interrupt = 1;
    CE_DEACTIVATE; // stop listening or sending.
    __bic_SR_register_on_exit(SLEEP_BITS);
}
