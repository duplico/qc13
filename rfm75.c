/*
 * rfm75.c
 *
 *  Created on: Jun 7, 2016
 *      Author: George
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

//Bank0 register initialization value
const uint8_t bank0_init_data[][2] = {
    { 0x00, 0x0F }, //reflect RX_DR\TX_DS\MAX_RT,Enable CRC ,2byte,POWER UP,PRX
    { 0x01, 0x3F }, //Enable auto acknowledgement data pipe5\4\3\2\1\0
    { 0x02, 0x3F }, //Enable RX Addresses pipe5\4\3\2\1\0
    { 0x03, 0x03 }, //RX/TX address field width 5byte
    { 0x04, 0xff }, //auto retransmission dalay (4000us),auto retransmission count(15)
    { 0x05, 0x3c }, //channel
    { 0x06, 0x07 }, //air data rate-1M,out power 5dbm,setup LNA gain.
    { 0x07, 0x07 }, //
    { 0x08, 0x00 }, //
    { 0x09, 0x00 }, { 12, 0xc3 }, //only LSB Receive address data pipe 2, MSB bytes is equal to RX_ADDR_P1[39:8]
    // 0x0a - RX_ADDR_P0 - 5 bytes
    // 0x0b - RX_ADDR_P1 - 5 bytes
    { 0x0c, 0xc3 }, // RX_ADDR_P2
    { 0x0d, 0xc4 }, //only LSB Receive address data pipe 3, MSB bytes is equal to RX_ADDR_P1[39:8]
    { 0x0e, 0xc5 }, //only LSB Receive address data pipe 4, MSB bytes is equal to RX_ADDR_P1[39:8]
    { 0x0f, 0xc6 }, //only LSB Receive address data pipe 5, MSB bytes is equal to RX_ADDR_P1[39:8]
    // 0x10 - TX_ADDR - 5 bytes
    { 0x11, 0x20 }, //Number of bytes in RX payload in data pipe0(32 byte)
    { 0x12, 0x20 }, //Number of bytes in RX payload in data pipe1(32 byte)
    { 0x13, 0x20 }, //Number of bytes in RX payload in data pipe2(32 byte)
    { 0x14, 0x20 }, //Number of bytes in RX payload in data pipe3(32 byte)
    { 0x15, 0x20 }, //Number of bytes in RX payload in data pipe4(32 byte)
    { 0x16, 0x20 }, //Number of bytes in RX payload in data pipe5(32 byte)
    { 0x17, 0x00 }, //fifo status
    { 0x1c, 0x3F }, //Enable dynamic payload length data pipe5\4\3\2\1\0
    { 0x1d, 0x07 } //Enables Dynamic Payload Length,Enables Payload with ACK,Enables the W_TX_PAYLOAD_NOACK command
};

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
    for (uint8_t i=0; i<data_len; i++) {
        usci_b0_send_sync(data[i]);
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

    delay_millis(100); // Delay more than 50ms.
    rfm75_post();

    // Activate:
    uint8_t i = send_rfm75_cmd(READ_REG|0x1d, 0x00);
    if(i==0) // If already active this would be nonzero. Maybe.
        send_rfm75_cmd(ACTIVATE_CMD, 0x73);

    // Let's start with bank 0:
    rfm75_select_bank(0);

    for(i=0;i<23;i++)
        rfm75_write_reg(bank0_init_data[i][0], bank0_init_data[i][1]);

    volatile uint8_t temp = 0;
    volatile uint8_t test = 0;
    for(i=0;i<23;i++) {
        temp = rfm75_read_byte(bank0_init_data[i][0]);
        test = temp == bank0_init_data[i][1];
        __no_operation();
    }

    // Next fill address buffers (TODO):
    //  Reg 0x0a: 5 bytes RX0 addr (unicast)
    //  Reg 0x0b: 5 bytes RX1 addr (broadcast)
    //  Reg 0x10: 5 bytes TX0 addr (same as RX0)

    // OK, that's bank 0 done. Next is bank 1.

    rfm75_select_bank(1);

    // We're going to send the first three words (like they're buffers).
    // They get sent LEAST SIGNIFICANT BYTE FIRST: so we start with {0x40, 0c4B, 0x01, 0xE2}
    // Like this:
    uint8_t bank1_config_0x00[][4] = {
        {0x40, 0x4b, 0x01, 0xe2}, // reserved (prescribed)
        {0xc0, 0x4b, 0x00, 0x00}, // reserved (prescribed)
        {0xd0, 0xfc, 0x8c, 0x02}, // reserved (prescribed)
        {0x99, 0x00, 0x39, 0x41}, // reserved (prescribed)
        {0xf9, 0x96, 0x82, 0x1b}, // 1 Mbps
        {0x24, 0x06, 0x0f, 0xa6}, // 1 Mbps
    };

    for (uint8_t i=0; i<6; i++) {
        rfm75_write_reg_buf(i, bank1_config_0x00[i], 4);
    }

    uint8_t bank1_config_0x0c[][4] = {
        {0x00, 0x73, 0x12, 0x00}, // 120 us mode (PLL settle time?)
        {0x00, 0x80, 0xb4, 0x36}, // reserved?
    };

    for (uint8_t i=0; i<2; i++) {
        rfm75_write_reg_buf(i, bank1_config_0x0c[0x0c+i], 4);
    }

    uint8_t bank1_config_0x0e[11] = {0x41, 0x20, 0x08, 0x04, 0x81, 0x20, 0xcf, 0xf7, 0xfe, 0xff, 0xff}; // ramp curve, prescribed
    rfm75_write_reg_buf(i, bank1_config_0x0e, 11);

    // TODO: Then the sample code does some kind of toggle thing that isn't in the datasheet.

    rfm75_select_bank(0);

    // And we're off to see the wizard!

    // Go into RX mode:

    __no_operation();
    rfm75_write_reg(0x00, 0b00001011); // power-up
    delay_millis(3); // 1.5 ms at least.

    temp = rfm75_read_byte(0x00);
    test = temp == 0b00001011;
    __no_operation();

    CE_ACTIVATE;
}
