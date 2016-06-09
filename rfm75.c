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
// CE   P3.2
// CSN  P1.3
// SCK  P2.2
// MOSI P1.6
// MISO P1.7
// IRQ  P3.1

//Bank0 register initialization value
const uint8_t bank0_init_data[][2] = {
        { 0, 0x0F }, //reflect RX_DR\TX_DS\MAX_RT,Enable CRC ,2byte,POWER UP,PRX
        { 1, 0x3F }, //Enable auto acknowledgement data pipe5\4\3\2\1\0
        { 2, 0x3F }, //Enable RX Addresses pipe5\4\3\2\1\0
        { 3, 0x03 }, //RX/TX address field width 5byte
        { 4, 0xff }, //auto retransmission dalay (4000us),auto retransmission count(15)
        { 5, 0x17 }, //23 channel
        { 6, 0x07 }, //air data rate-1M,out power 5dbm,setup LNA gain.
        { 7, 0x07 }, //
        { 8, 0x00 }, //
        { 9, 0x00 }, { 12, 0xc3 }, //only LSB Receive address data pipe 2, MSB bytes is equal to RX_ADDR_P1[39:8]
        { 13, 0xc4 }, //only LSB Receive address data pipe 3, MSB bytes is equal to RX_ADDR_P1[39:8]
        { 14, 0xc5 }, //only LSB Receive address data pipe 4, MSB bytes is equal to RX_ADDR_P1[39:8]
        { 15, 0xc6 }, //only LSB Receive address data pipe 5, MSB bytes is equal to RX_ADDR_P1[39:8]
        { 17, 0x20 }, //Number of bytes in RX payload in data pipe0(32 byte)
        { 18, 0x20 }, //Number of bytes in RX payload in data pipe1(32 byte)
        { 19, 0x20 }, //Number of bytes in RX payload in data pipe2(32 byte)
        { 20, 0x20 }, //Number of bytes in RX payload in data pipe3(32 byte)
        { 21, 0x20 }, //Number of bytes in RX payload in data pipe4(32 byte)
        { 22, 0x20 }, //Number of bytes in RX payload in data pipe5(32 byte)
        { 23, 0x00 }, //fifo status
        { 28, 0x3F }, //Enable dynamic payload length data pipe5\4\3\2\1\0
        { 29, 0x07 } //Enables Dynamic Payload Length,Enables Payload with ACK,Enables the W_TX_PAYLOAD_NOACK command
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

void send_rfm75_cmd(uint8_t cmd, uint8_t *data, uint8_t data_len) {
    usci_b0_send_sync(cmd);
    for (uint8_t i=0; i<data_len; i++) {
        usci_b0_send_sync(data[i]);
    }
}

uint8_t rfm75_read_byte(uint8_t cmd) {
    usci_b0_send_sync(cmd);
    return usci_b0_recv_sync(0b10101010);
}

void rfm75_select_bank(uint8_t bank) {
    volatile uint8_t currbank = rfm75_read_byte(STATUS);
    if ((currbank && (bank==0)) || ((currbank==0) && bank)) {
        uint8_t switch_bank = 0x53;
        send_rfm75_cmd(ACTIVATE_CMD, &switch_bank, 1);
    }
}

//void delay_millis(unsigned long mils) {
//    while (mils) {
//        __delay_cycles(4000);
//    }
//}

void rfm75_init()
{

    delay_millis(100);//delay more than 50ms.

    // Switch to Bank0.
    rfm75_select_bank(0);
    rfm75_select_bank(1);
    rfm75_select_bank(0);

    // Write Bank0 registers
    //  Write [0..5) from buffer
    //  Write WRITE_REG|5 : 0x3c //channel 2.460G
    //  uint8_t datarate = 0x00 // 1M (0x28 = 2M)
    //  uint8_t power = 0x03; // -3dbm (0x01=-7; 0x05=0; 0x07=5)
    //  datarate_temp |= 0x03;
    //  Write WRITE_REG|6 : datarate|power
    //  Write [7..21) from buffer
    //  Reg 10: 5 bytes RX0 addr (unicast)
    //  Reg 11: 5 bytes RX1 addr (broadcast)
    //  Reg 16: 5 bytes TX0 addr (same as RX0)
    // Activate:
    //  i=SPI_Read_Reg(29)
    //  if(i==0) // i!=0 showed that chip has been actived.so do not active again.
    //      SPI_Write_Reg(ACTIVATE_CMD,0x73);// Active
    //
    // Maybe the following should be 28 and 29:
    /*
     * for(i=22;i>=21;i--)
     * {
     *   SPI_Write_Reg((WRITE_REG|Bank0_Reg[i][0]),Bank0_Reg[i][1]);//Enable Dynamic Payload length ,Enables the W_TX_PAYLOAD_NOACK command
     * }
     */

    // OK, that's bank0 done. Next is a write of Bank1.

    // TODO SWITCH TO BANK1

    // We're going to send the first three words (like they're buffers).
    // They get sent LEAST SIGNIFICANT BYTE FIRST: so we start with {0x40, 0c4B, 0x01, 0xE2}
    // Like this:
    /*
     * 0xE2014B40, // reserved
     * 0x00004BC0, // reserved
     * 0x028CFCD0, // reserved
     * 0x41390099, // reserved
     * 0x1B8296F9, // 1Mbps
     * 0xA60F0624, // 1Mbps
     * ... skip to 0x0C:
     * 0x00127300, // 120 us mode, whatever that means. (PLL settle time?)
     * 0x36B48000, // reserved
     * Lastly: 0x41,0x20,0x08,0x04,0x81,0x20,0xCF,0xF7,0xFE,0xFF,0xFF
     */

    // Then the sample code does some kind of toggle thing that isn't in the datasheet.

    // TODO: Switch back to bank0

    // And we're off to see the wizard!


    //

    //**************************Test spi*****************************//
    //SPI_Write_Reg((WRITE_REG|Bank0_Reg[3][0]),0x04);
    //test_data = SPI_Read_Reg(3);
//
////********************Write Bank0 register******************
//
//    for(i=0;i<5;i++)
//    {
//        SPI_Write_Reg((WRITE_REG|Bank0_Reg[i][0]),Bank0_Reg[i][1]);
//    }
//
////********************select channel*************************
//    switch(channel)
//    {
//        case 0:
//
//            SPI_Write_Reg(WRITE_REG|5,0x0a); //channel 2.410G
//            break;
//
//        case 1:
//
//            SPI_Write_Reg(WRITE_REG|5,0x1e); //channel 2.430G
//            break;
//        case 2:
//
//            SPI_Write_Reg(WRITE_REG|5,0x3c); //channel 2.460G
//            break;
//
//        case 3:
//
//            SPI_Write_Reg(WRITE_REG|5,0x53); //channel 2.483G
//            break;
//
//        default:
//            break;
//    }
//    if(data_rate)
//    {
//        temp = 0x28;      //data rate 2M
//    }
//    else
//    {
//        temp = 0x00;     //data rate 1M
//    }
//
//    switch(power)
//    {
//        case 0:
//
//            temp |= 0x01;
//            SPI_Write_Reg(WRITE_REG|6,temp); //power -7dbm
//            break;
//
//        case 1:
//
//            temp |= 0x03;
//            SPI_Write_Reg(WRITE_REG|6,temp); //power -3dbm
//            break;
//        case 2:
//
//            temp |= 0x05;
//            SPI_Write_Reg(WRITE_REG|6,temp); //power 0dbm
//            break;
//
//        case 3:
//
//            temp |= 0x07;
//            SPI_Write_Reg(WRITE_REG|6,temp); //power 5dbm
//            break;
//
//        default:
//            break;
//    }
//    for(i=7;i<21;i++)
//    {
//        SPI_Write_Reg((WRITE_REG|Bank0_Reg[i][0]),Bank0_Reg[i][1]);
//    }
///************************************************************/
////reg 10 - Rx0 addr
//    for(j=0;j<5;j++)
//    {
//        WriteArr[j]=RX0_Address[j];
//    }
//    SPI_Write_Buf((WRITE_REG|10),&(WriteArr[0]),5);
//
////REG 11 - Rx1 addr
//    for(j=0;j<5;j++)
//    {
//        WriteArr[j]=RX1_Address[j];
//    }
//    SPI_Write_Buf((WRITE_REG|11),&(WriteArr[0]),5);
////REG 16 - TX addr
//    for(j=0;j<5;j++)
//    {
//        WriteArr[j]=RX0_Address[j];
//    }
//    SPI_Write_Buf((WRITE_REG|16),&(WriteArr[0]),5);
//
//
//    i=SPI_Read_Reg(29);//read Feature Register 如果要支持动态长度或者 Payload With ACK，需要先给芯片发送 ACTIVATE命令（数据为0x73),然后使能动态长度或者 Payload With ACK (REG28,REG29).
//    if(i==0) // i!=0 showed that chip has been actived.so do not active again.
//        SPI_Write_Reg(ACTIVATE_CMD,0x73);// Active
//    for(i=22;i>=21;i--)
//    {
//        SPI_Write_Reg((WRITE_REG|Bank0_Reg[i][0]),Bank0_Reg[i][1]);//Enable Dynamic Payload length ,Enables the W_TX_PAYLOAD_NOACK command
//    }
//
////********************Write Bank1 register******************
//    SwitchCFG(1);
//
////  for(i=0;i<=8;i++)//reverse
////  {
////      for(j=0;j<4;j++)
////          WriteArr[j]=(Bank1_Reg0_13[i]>>(8*(j) ) )&0xff;
////
////      SPI_Write_Buf((WRITE_REG|i),&(WriteArr[0]),4);
////  }
//    for(i=0;i<=3;i++)//reverse
//    {
//        for(j=0;j<4;j++)
//            WriteArr[j]=(Bank1_Reg0_13[i]>>(8*(j) ) )&0xff;
//
//        SPI_Write_Buf((WRITE_REG|i),&(WriteArr[0]),4);
//    }
//
//    for(j=0;j<4;j++)
//        WriteArr[j]=(Bank1_Reg0_4[data_rate+1]>>(8*(j) ) )&0xff;
//    SPI_Write_Buf((WRITE_REG|i),&(WriteArr[0]),4);
//    for(j=0;j<4;j++)
//        WriteArr[j]=(Bank1_Reg0_5[data_rate+1]>>(8*(j) ) )&0xff;
//    SPI_Write_Buf((WRITE_REG|i),&(WriteArr[0]),4);
//
//    for(i=6;i<=8;i++)//reverse
//    {
//        for(j=0;j<4;j++)
//            WriteArr[j]=(Bank1_Reg0_13[i]>>(8*(j) ) )&0xff;
//
//        SPI_Write_Buf((WRITE_REG|i),&(WriteArr[0]),4);
//    }
//
//    for(i=9;i<=13;i++)
//    {
//        for(j=0;j<4;j++)
//            WriteArr[j]=(Bank1_Reg0_13[i]>>(8*(3-j) ) )&0xff;
//
//        SPI_Write_Buf((WRITE_REG|i),&(WriteArr[0]),4);
//    }
//
//    for(j=0;j<11;j++)
//    {
//        WriteArr[j]=Bank1_Reg14[j];
//    }
//    SPI_Write_Buf((WRITE_REG|14),&(WriteArr[0]),11);
//
////toggle REG4<25,26>
//    for(j=0;j<4;j++)
//        WriteArr[j]=(Bank1_Reg0_13[4]>>(8*(j) ) )&0xff;
//
//    WriteArr[0]=WriteArr[0]|0x06;
//    SPI_Write_Buf((WRITE_REG|4),&(WriteArr[0]),4);
//
//    WriteArr[0]=WriteArr[0]&0xf9;
//    SPI_Write_Buf((WRITE_REG|4),&(WriteArr[0]),4);
//
//    DelayMs(10);
//
////********************switch back to Bank0 register access******************
//    SwitchCFG(0);
//
//    SwitchToRxMode();//switch to RX mode
//    //SwitchToTxMode();//switch to RX mode
}
