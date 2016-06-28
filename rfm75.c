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

#define BANK0_INITS 17

//Bank0 register initialization value
const uint8_t bank0_init_data[BANK0_INITS][2] = {
    { 0x00, 0b00001011 }, //
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
    { 0x11, sizeof(qcpayload) }, //Number of bytes in RX payload in data pipe0(32 byte)
    { 0x12, sizeof(qcpayload) }, //Number of bytes in RX payload in data pipe1(32 byte)
    { 0x13, 0 }, //Number of bytes in RX payload in data pipe2 - disable
    { 0x14, 0 }, //Number of bytes in RX payload in data pipe3 - disable
    { 0x15, 0 }, //Number of bytes in RX payload in data pipe4 - disable
    { 0x16, 0 }, //Number of bytes in RX payload in data pipe5 - disable
    { 0x17, 0 },
    { 0x1c, 0x00 }, // No dynamic packet length
    { 0x1d, 0b00000000 } // 00000 | DPL | ACK | DYN_ACK
};

uint8_t payload[sizeof(qcpayload)] = {0xff, 0x00, 0xff, 0};

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
    volatile uint8_t active = send_rfm75_cmd(READ_REG|0x1d, 0x00);
    send_rfm75_cmd(ACTIVATE_CMD, 0x73);
    volatile uint8_t active2 = send_rfm75_cmd(READ_REG|0x1d, 0x00);
    if (active2 != (active ^ 0b00000111)) {
        return 0;
    }
    // TODO: More
    return 1;
}

void rfm75_init()
{
    // TODO: Power down, just in case.

    delay_millis(100); // Delay more than 50ms.
    rfm75_post();

    P3DIR |= BIT2;
    P3OUT &= ~BIT2;

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

    // Activate:
    /*
    test = send_rfm75_cmd(READ_REG|0x1d, 0x00);
    if(test!=0) // If already active this would be nonzero. Maybe.
        send_rfm75_cmd(ACTIVATE_CMD, 0x73);

    test = send_rfm75_cmd(READ_REG|0x1d, 0x00);
    if(test!=0) // If already active this would be nonzero. Maybe.
        send_rfm75_cmd(ACTIVATE_CMD, 0x73);

    test = send_rfm75_cmd(READ_REG|0x1d, 0x00);
    if(test!=0) // If already active this would be nonzero. Maybe.
        send_rfm75_cmd(ACTIVATE_CMD, 0x73);
        */
    // I don't think we need the features that ACTIVATE uses.

    // OK, that's bank 0 done. Next is bank 1.

    rfm75_select_bank(1);

    // We're going to send the first three words (like they're buffers).
    // They get sent MOST SIGNIFICANT BYTE FIRST: so we start with 0xE2.
    //  (we show them here LEAST SIGNIFICANT BYTE FIRST because we
    //   reverse everything we send.)
    // Like this:
    uint8_t bank1_config_0x00[][4] = {
        {0xe2, 0x01, 0x4b, 0x40}, // reserved (prescribed)
        {0x00, 0x00, 0x4b, 0xc0}, // reserved (prescribed)
        {0x02, 0x8c, 0xfc, 0xd0}, // reserved (prescribed)
        {0x41, 0x39, 0x00, 0x99}, // reserved (prescribed) // TODO: 41 or 21?
        {0x1b, 0x82, 0x96, 0xf9}, // 1 Mbps // The user guide flips it for us. // TODO: 1b or 03?
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

    // TODO: Then the sample code does some kind of toggle thing that isn't in the datasheet.

    volatile uint8_t currbank = rfm75_get_status() & 0x80; // Get MSB, which is active bank.
    __no_operation();

    rfm75_select_bank(0);

    currbank = rfm75_get_status() & 0x80; // Get MSB, which is active bank.
    __no_operation();
    __bis_SR_register(GIE);

    // Enable the interrupt.
    P3DIR &= ~BIT1;
    P3REN &= ~BIT1;
    P3SEL0 &= ~BIT1;
    P3SEL1 &= ~BIT1;
    P3IES |= BIT1;
    P3IFG &= ~BIT1;
    P3IE |= BIT1;

//    GPIO_setAsInputPin(GPIO_PORT_P3, GPIO_PIN1);
//    GPIO_selectInterruptEdge(GPIO_PORT_P3, GPIO_PIN1, GPIO_HIGH_TO_LOW_TRANSITION);
//    GPIO_enableInterrupt(GPIO_PORT_P3, GPIO_PORT_P1);

    // And we're off to see the wizard!

    CE_DEACTIVATE;
    delay_millis(5);

    __no_operation();
    // Flush TX first:

    CSN_LOW_START;
    usci_b0_send_sync(0b11100001);
    CSN_HIGH_END;

    // rx fifo
    CSN_LOW_START;
    usci_b0_send_sync(0b11100010);
    CSN_HIGH_END;

    rfm75_write_reg(0x00, 0b00011010); // PWR_UP! TX MODE.
    delay_millis(15); // 1.5 ms at least.
    rfm75_write_reg(0x07, BIT4|BIT5|BIT6); // clear interrupt flags
    delay_millis(15); // 1.5 ms at least.

    test = rfm75_read_byte(0x00);
    __no_operation();
    test = rfm75_read_byte(0x07);
    __no_operation();
    test = rfm75_read_byte(0x17);
    __no_operation();

    send_rfm75_cmd_buf(WR_TX_PLOAD, payload, sizeof(qcpayload));
    test = rfm75_read_byte(0x07);
    __no_operation();
    test = rfm75_read_byte(0x17);
    __no_operation();
    CE_ACTIVATE;
    delay_millis(100);
    test = rfm75_read_byte(0x07);
    __no_operation();
    test = rfm75_read_byte(0x17);
    __no_operation();

    while (P3IN & BIT1);
    __no_operation();
}

#pragma vector=PORT3_VECTOR
__interrupt void RFM_ISR(void)
{
    volatile uint16_t i = P3IV;
    switch(__even_in_range(i, 10)) {
    case 0x04:
        __no_operation(); // RFM75 interrupt
        break;
    default:
        __no_operation();
    }
}
