/*
 * ======== Standard MSP430 includes ========
 */
#include <msp430fr5949.h>
#include <driverlib/MSP430FR5xx_6xx/driverlib.h>
#include <stdint.h>
#include <stdio.h>

// Grace includes:
#include <ti/mcu/msp430/Grace.h>

// Project includes:
#include <stdint.h>
#include "qc13.h"
#include "rfm75.h"
#include "leds.h"

/*
 *
 * Peripherals
 * ===========
 *
 * TLC:    USCI_A0
 * RFM:    USCI_B0
 * Pair:   USCI_A1
 * Light:  P1.0
 * Temp:   P1.1
 * B1:     P3.4
 * B2:     P2.7
 *
 */

void init() {
    Grace_init(); // Activate Grace-generated configuration

    LED_BANK1_OUT |= LED_BANK1_PIN | LED_BANK2_PIN | LED_BANK3_PIN | LED_BANK4_PIN;
    LED_BANK5_OUT |= LED_BANK5_PIN | LED_BANK6_PIN;

    PM5CTL0 &= ~LOCKLPM5;

    /*
     * Peripherals:
     *
     *  Radio (RFM69CW)
     *        (MSB first, clock inactive low,
     *         write on rise, change on fall, MSB first)
     *        eUSCI_B0 - radio
     *        somi, miso, clk, ste
     *        DIO0      P3.1
     *        RESET     P3.2
     */
//    init_radio();

    init_tlc();

    Timer_A_initUpModeParam gsclk_init = {};
    gsclk_init.clockSource = TIMER_A_CLOCKSOURCE_SMCLK;
    gsclk_init.clockSourceDivider = TIMER_A_CLOCKSOURCE_DIVIDER_1;
    gsclk_init.timerPeriod = 2;
    gsclk_init.timerInterruptEnable_TAIE = TIMER_A_TAIE_INTERRUPT_DISABLE;
    gsclk_init.captureCompareInterruptEnable_CCR0_CCIE = TIMER_A_CCIE_CCR0_INTERRUPT_DISABLE;
    gsclk_init.timerClear = TIMER_A_SKIP_CLEAR;
    gsclk_init.startTimer = false;
    GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P1, GPIO_PIN2, GPIO_PRIMARY_MODULE_FUNCTION);

    Timer_A_initUpMode(TIMER_A1_BASE, &gsclk_init);
    Timer_A_setOutputMode(TIMER_A1_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_1, TIMER_A_OUTPUTMODE_TOGGLE_RESET);
    Timer_A_setCompareValue(TIMER_A1_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_1, 1);
    Timer_A_startCounter(TIMER_A1_BASE, TIMER_A_UP_MODE);

    Timer_A_initUpModeParam initUpParam = {};
    initUpParam.clockSource = TIMER_A_CLOCKSOURCE_ACLK;
    initUpParam.clockSourceDivider = TIMER_A_CLOCKSOURCE_DIVIDER_1;
    initUpParam.timerPeriod = 50;
    initUpParam.timerInterruptEnable_TAIE = TIMER_A_TAIE_INTERRUPT_DISABLE;
    initUpParam.captureCompareInterruptEnable_CCR0_CCIE = TIMER_A_CCIE_CCR0_INTERRUPT_ENABLE;
    initUpParam.timerClear = TIMER_A_SKIP_CLEAR;
    initUpParam.startTimer = false;
    Timer_A_initUpMode(TIMER_A0_BASE, &initUpParam);
    Timer_A_startCounter(TIMER_A0_BASE, TIMER_A_UP_MODE);
}

void delay_millis(unsigned long mils) {
    while (mils) {
        __delay_cycles(4000);
        mils--;
    }
}
volatile uint16_t light = 0;
volatile uint16_t temp = 0;

int main(void)
{
    volatile uint8_t in = 0;

    init();


    tlc_stage_blank(0);
    tlc_set_fun();

    delay_millis(10);

    while(1)
    {
        light = ADC12MEM0;
        temp = ADC12MEM1;

        __bis_SR_register(LPM0_bits + GIE);     // LPM0, ADC12_B_ISR will force exit
        __no_operation();                       // For debugger
    }

}

volatile uint8_t shift = 0;

//#pragma vector=TIMER0_A1_VECTOR
//__interrupt void isr_ta0_other(void) {
//    switch (__even_in_range(TA0IV, 10)) {
//    case 0x00: break; // None
//    case 0x02: break; // CCR1
//    case 0x04: break; // CCR2;
//    case 0x06: break; // CCR3
//    case 0x08: break; // CCR4
//    case 0x0a: break; // CCR5
//    case 0x0c: break; // CCR6
//    case 0x0e: // TA0IFG
//        LED_BANK1_OUT |= LED_BANK1_PIN | LED_BANK2_PIN | LED_BANK3_PIN
//                | LED_BANK4_PIN;
//        LED_BANK5_OUT |= LED_BANK5_PIN | LED_BANK6_PIN;
//        bank++;
//        tlc_set_gs();
//        break;
//    }
//}

#pragma vector=TIMER0_A0_VECTOR
__interrupt void TIMER0_A0_ISR_HOOK(void)
{
    LED_BANK1_OUT |= LED_BANK1_PIN | LED_BANK2_PIN | LED_BANK3_PIN
            | LED_BANK4_PIN;
    LED_BANK5_OUT |= LED_BANK5_PIN | LED_BANK6_PIN;
    tlc_set_gs();
    __bic_SR_register_on_exit(LPM0_bits);
}
