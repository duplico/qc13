/*
 * tlc5948a.c
 * (c) 2016 George Louthan
 * 3-clause BSD license; see license.md.
 */

#include "tlc5948a.h"

#include <stdint.h>
#include <string.h>
#include "qc13.h"

/*
 *   LED controller (TLC5948A)
 *        eUSCI_A0 - LEDs
 *        (write on rise, change on fall,
 *         clock inactive low, MSB first)
 *        somi, miso, clk (3-wire)
 *        GSCLK     P1.2 (timer TA1.1)
 *        LAT       P1.4
 *
 *   This file's job is to keep the display going. Application logic will go
 *   elsewhere - this is strictly a driver for our 6 banks and 15 channels.
 *
 */

#define TLC_THISISGS    0x00
#define TLC_THISISFUN   0x01

// Current TLC sending state:
uint8_t tlc_send_type = TLC_SEND_IDLE;
uint8_t tlc_tx_index = 0;   // Index of currently sending buffer

uint8_t tlc_loopback_data_out = 0x00;
volatile uint8_t tlc_loopback_data_in = 0x00;

uint8_t tlc_active_bank = 0;

// Let's make these 12-bit. So the most significant hexadigit will be brightness-correct.
uint16_t tlc_bank_gs[6][16] = {
    {0},
    {0},
    {0},
    {0},
    {0},
    {0},
};

// This is the basic set of function data.
// A few of them can be edited.
uint8_t fun_base[] = {
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // ...reserved...
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
        0x7f,
        // HERE WE SWITCH TO 7-BIT SPI.
        // The following index is 18:
        0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F,
        0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F,
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

void tlc_init() {
	// Initialize all the TLC GPIO:
	// 6x banks
	// GSCLK
	// LAT
	// SCLK, tx, rx

    P1DIR |= BIT4; // TLC_LAT
    P1OUT &= ~BIT4;

    tlc_stage_blank(1);

    // GS_CLK:
    GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P1, GPIO_PIN2, GPIO_PRIMARY_MODULE_FUNCTION); // 1.2 TA1.1

    // Set up USCI_A0 GPIO:
    //1.5 SCLK
    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P1, GPIO_PIN5, GPIO_SECONDARY_MODULE_FUNCTION);
    //2.0 tx
    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P2, GPIO_PIN0, GPIO_SECONDARY_MODULE_FUNCTION);
    //2.1 rx
    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P2, GPIO_PIN1, GPIO_SECONDARY_MODULE_FUNCTION);

    // First, we're going to configure the timer that outputs GSCLK.
    //  We want this to go as fast as possible. (Meaning as fast as we can, as
    //   its max, 33 MHz, is faster than our fastest possible source, 24MHz)
    //  Below this is configured to toggle every cycle of SMCLK,
    //  which should always be our fastest clock.

    Timer_A_initUpModeParam gsclk_init = {};
    gsclk_init.clockSource = TIMER_A_CLOCKSOURCE_SMCLK;
    gsclk_init.clockSourceDivider = TIMER_A_CLOCKSOURCE_DIVIDER_1;
    gsclk_init.timerPeriod = 1;
    gsclk_init.timerInterruptEnable_TAIE = TIMER_A_TAIE_INTERRUPT_DISABLE;
    gsclk_init.captureCompareInterruptEnable_CCR0_CCIE = TIMER_A_CCIE_CCR0_INTERRUPT_DISABLE;
    gsclk_init.timerClear = TIMER_A_SKIP_CLEAR;
    gsclk_init.startTimer = false;

    // Next we configure the clock that tells us when it's time to select the
    //  next LED channel bank.
    // We'll run this off of ACLK, which is driven by our internal 39K clock.
    //  THIS IS OUR TIME LOOP!!!! :-D

    Timer_A_initUpModeParam next_channel_timer_init = {};
    next_channel_timer_init.clockSource = TIMER_A_CLOCKSOURCE_ACLK;
    next_channel_timer_init.clockSourceDivider = TIMER_A_CLOCKSOURCE_DIVIDER_28;
    next_channel_timer_init.timerPeriod = 2;
    next_channel_timer_init.timerInterruptEnable_TAIE = TIMER_A_TAIE_INTERRUPT_DISABLE;
    next_channel_timer_init.captureCompareInterruptEnable_CCR0_CCIE = TIMER_A_CCIE_CCR0_INTERRUPT_ENABLE;
    next_channel_timer_init.timerClear = TIMER_A_SKIP_CLEAR;
    next_channel_timer_init.startTimer = false;

    // Start the clocks:

    // A1 / GSCLK:
    Timer_A_initUpMode(TIMER_A1_BASE, &gsclk_init);
    Timer_A_setOutputMode(TIMER_A1_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_1, TIMER_A_OUTPUTMODE_TOGGLE_RESET);
    Timer_A_setCompareValue(TIMER_A1_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_1, 1);
    Timer_A_startCounter(TIMER_A1_BASE, TIMER_A_UP_MODE);

    // A0 / LED channel timer:
    Timer_A_initUpMode(TIMER_A0_BASE, &next_channel_timer_init);
    Timer_A_startCounter(TIMER_A0_BASE, TIMER_A_UP_MODE);

    UCA0CTLW0 |= UCSWRST;  // Shut down USCI_A0,

    // And USCI_A0 peripheral:
    EUSCI_A_SPI_initMasterParam ini = {0};
    ini.clockPhase = EUSCI_A_SPI_PHASE_DATA_CAPTURED_ONFIRST_CHANGED_ON_NEXT;
    ini.clockPolarity = EUSCI_A_SPI_CLOCKPOLARITY_INACTIVITY_LOW;
    ini.clockSourceFrequency = SMCLK_RATE_HZ;
    ini.desiredSpiClock = 8000000;
    ini.msbFirst = EUSCI_A_SPI_MSB_FIRST;
    ini.selectClockSource = EUSCI_A_SPI_CLOCKSOURCE_SMCLK;
    ini.spiMode = EUSCI_A_SPI_3PIN;
    EUSCI_A_SPI_initMaster(EUSCI_A0_BASE, &ini);

    UCA0CTLW0 &= ~UC7BIT;  //  put it in 8-bit mode out of caution.
    UCA0CTLW0 &= ~UCSWRST; //  and enable it.

    EUSCI_A_SPI_clearInterrupt(EUSCI_A0_BASE, EUSCI_A_SPI_TRANSMIT_INTERRUPT);
    EUSCI_A_SPI_enableInterrupt(EUSCI_A0_BASE, EUSCI_A_SPI_TRANSMIT_INTERRUPT);

    tlc_set_fun();
    tlc_set_gs();
}

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
                LED_BANK1_OUT |= LED_BANK1_PIN | LED_BANK2_PIN | LED_BANK3_PIN
                        | LED_BANK4_PIN;
                LED_BANK5_OUT |= LED_BANK5_PIN | LED_BANK6_PIN;
                P1OUT |= BIT4; P1OUT &= ~BIT4; // Pulse LAT
                tlc_send_type = TLC_SEND_IDLE;

                switch (tlc_active_bank) {
                case 0:
                    LED_BANK1_OUT &= ~LED_BANK1_PIN;
                    tlc_active_bank++;
                    break;
                case 1:
                    LED_BANK2_OUT &= ~LED_BANK2_PIN;
                    tlc_active_bank++;
                    break;
                case 2:
                    LED_BANK3_OUT &= ~LED_BANK3_PIN;
                    tlc_active_bank++;
                    break;
                case 3:
                    LED_BANK4_OUT &= ~LED_BANK4_PIN;
                    tlc_active_bank++;
                    break;
                case 4:
                    LED_BANK5_OUT &= ~LED_BANK5_PIN;
                    tlc_active_bank++;
                    break;
                case 5:
                    LED_BANK6_OUT &= ~LED_BANK6_PIN;
                    tlc_active_bank = 0;
                    break;
                }
                break;
            } else { // gs - MSB first; this starts with 0.
                volatile static uint16_t channel_gs = 0;
                channel_gs = (tlc_bank_gs[tlc_active_bank][tlc_tx_index>>1]);
                if (tlc_tx_index & 0x01) { // odd; less significant byte
                    UCA0TXBUF = (channel_gs & 0xff);
                } else { // even; more significant byte
                    UCA0TXBUF = (channel_gs >> 8) & 0xff;
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

// Dedicated ISR for CCR0. Vector is cleared on service.
#pragma vector=TIMER0_A0_VECTOR
__interrupt void TIMER0_A0_ISR_HOOK(void)
{
    f_time_loop = 1;
    tlc_set_gs();
    __bic_SR_register_on_exit(LPM0_bits);
}
