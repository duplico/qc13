/*
 * leds.c
 * (c) 2014 George Louthan
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
 */

#define TLC_THISISGS    0x00
#define TLC_THISISFUN   0x01

#define ADC_WINDOW 32

volatile uint16_t lights[ADC_WINDOW] = {0};
volatile uint16_t temps[ADC_WINDOW] = {0};

volatile uint16_t light = 0;
volatile uint16_t temp = 0;

volatile uint16_t light_tot = 0;
volatile uint16_t temp_tot = 0;

volatile uint8_t light_index = 0;
volatile uint8_t temp_index = 0;

// Current TLC sending state:
uint8_t tlc_send_type = TLC_SEND_IDLE;
uint8_t tlc_tx_index = 0;   // Index of currently sending buffer

uint8_t tlc_loopback_data_out = 0x00;
volatile uint8_t tlc_loopback_data_in = 0x00;

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

    P3DIR |= (LED_BANK5_PIN | LED_BANK6_PIN);
    PJDIR |= (LED_BANK1_PIN | LED_BANK2_PIN | LED_BANK3_PIN | LED_BANK4_PIN);


    LED_BANK1_OUT |= (LED_BANK1_PIN | LED_BANK2_PIN | LED_BANK3_PIN
            | LED_BANK4_PIN);
    LED_BANK5_OUT |= (LED_BANK5_PIN | LED_BANK6_PIN);

    __no_operation();

    // First, we're going to configure the timer that outputs GSCLK.
    //  We want this to go as fast as possible.
    //   (its max, 33 MHz, is faster than our fastest possible source, 24MHz)
    //  Below this is configured to toggle every cycle of SMCLK,
    //  which should be our fastest clock.

    Timer_A_initUpModeParam gsclk_init = {};
    gsclk_init.clockSource = TIMER_A_CLOCKSOURCE_SMCLK;
    gsclk_init.clockSourceDivider = TIMER_A_CLOCKSOURCE_DIVIDER_1;
    gsclk_init.timerPeriod = 2;
    gsclk_init.timerInterruptEnable_TAIE = TIMER_A_TAIE_INTERRUPT_DISABLE;
    gsclk_init.captureCompareInterruptEnable_CCR0_CCIE = TIMER_A_CCIE_CCR0_INTERRUPT_DISABLE;
    gsclk_init.timerClear = TIMER_A_SKIP_CLEAR;
    gsclk_init.startTimer = false;
    GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P1, GPIO_PIN2, GPIO_PRIMARY_MODULE_FUNCTION);

    // Next we configure the clock that tells us when it's time to select the
    //  next LED channel bank.
    // We'll run this off of ACLK, which is driven by our 32K LFXT.

    Timer_A_initUpModeParam next_channel_timer_init = {};
    next_channel_timer_init.clockSource = TIMER_A_CLOCKSOURCE_ACLK;
    next_channel_timer_init.clockSourceDivider = TIMER_A_CLOCKSOURCE_DIVIDER_1;
    next_channel_timer_init.timerPeriod = 100; // previously 50
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

    // This is just out of an abundance of caution:
    UCA0CTLW0 |= UCSWRST;  // Shut down USCI_A0,
    UCA0CTLW0 &= ~UC7BIT;  //  put it in 8-bit mode
    UCA0CTLW0 &= ~UCSWRST; //  and enable it again.

    EUSCI_A_SPI_clearInterrupt(EUSCI_A0_BASE, EUSCI_A_SPI_TRANSMIT_INTERRUPT);
    EUSCI_A_SPI_enableInterrupt(EUSCI_A0_BASE, EUSCI_A_SPI_TRANSMIT_INTERRUPT);

    volatile uint8_t tlc_fault = tlc_test_loopback(0x7a);
    if (tlc_fault)
        __no_operation();

    tlc_set_gs();
    tlc_stage_blank(1);
    tlc_set_fun();
}

uint8_t tlc_active_bank = 0;

// Let's make these 12-bit. So the most significant hexadigit will be brightness-correct.
uint16_t tlc_bank_gs[6][16] = {
    {0, 0xfff, 0xfff, 0xfff, 0xfff, 0xfff, 0xfff, 0xfff, 0}, //xfff, 0xfff, 0xfff, 0xfff, 0xfff, 0xfff, 0xfff, 0xfff},
    {0, 0xfff, 0xfff, 0xfff, 0xfff, 0xfff, 0xfff, 0xfff, 0xfff, 0xfff, 0xfff, 0xfff, 0xfff, 0xfff, 0xfff, 0xfff},
    {0, 0xfff, 0xfff, 0xfff, 0xfff, 0xfff, 0xfff, 0xfff, 0xfff, 0xfff, 0xfff, 0xfff, 0xfff, 0xfff, 0xfff, 0xfff},
    {0, 0xfff, 0xfff, 0xfff, 0xfff, 0xfff, 0xfff, 0xfff, 0xfff, 0xfff, 0xfff, 0xfff, 0xfff, 0xfff, 0xfff, 0xfff},
    {0, 0, 0, 0, 0xfff, 0xfff, 0x0f, 0xfff, 0xfff, 0x0f, 0xfff, 0xfff, 0x0f, 0xfff, 0xfff, 0x0f},
    {0, 0, 0, 0, 0xfff, 0, 0xfff, 0xfff, 0, 0xfff, 0xfff, 0, 0xfff, 0xfff, 0, 0xfff},
};

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
                GPIO_pulse(TLC_LATPORT, TLC_LATPIN); // LATCH.
                tlc_send_type = TLC_SEND_IDLE;

                switch (tlc_active_bank) {
                case 0:
                    LED_BANK1_OUT &= ~LED_BANK1_PIN;
                    tlc_active_bank++;
//                    light_tot -= lights[light_index];
//                    temp_tot -= temps[temp_index];
//                    lights[light_index] = ADC12_B_getResults(ADC12_B_BASE, ADC12_B_MEMORY_0) >> 1;
//                    temps[temp_index] = ADC12_B_getResults(ADC12_B_BASE, ADC12_B_MEMORY_1) >> 1;
//
//                    if (lights[light_index] < 3) lights[light_index] = 3;
//                    light_tot += lights[light_index];
//                    temp_tot += temps[temp_index];
//                    light_index++;
//                    temp_index++;
//                    if (light_index == ADC_WINDOW) light_index = 0;
//                    if (temp_index == ADC_WINDOW) temp_index = 0;
//
//                    light = light_tot / ADC_WINDOW;
//                    if (light > 2047) light = 2047;
//                    temp = temp_tot / ADC_WINDOW;
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
                channel_gs = 0x0010; // (tlc_bank_gs[tlc_active_bank][tlc_tx_index>>1]); // & 0x0fff) | ((light<<5) &   0xf000);
                if (tlc_tx_index & 0x01) { // odd; less significant byte
                    EUSCI_A_SPI_transmitData(EUSCI_A0_BASE, (uint8_t) (channel_gs & 0xff));
                } else { // even; more significant byte
                    EUSCI_A_SPI_transmitData(EUSCI_A0_BASE, (uint8_t) ((channel_gs >> 8) & 0xff));
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
