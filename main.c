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
#include "qc12.h"
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

// Defines for the TLC:
#define LATPORT     GPIO_PORT_P1
#define LATPIN      GPIO_PIN4

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

    init_leds();
}

// 1-origined because fuck you
#define LED_BANK1_PORT GPIO_PORT_PJ
#define LED_BANK2_PORT GPIO_PORT_PJ
#define LED_BANK3_PORT GPIO_PORT_PJ
#define LED_BANK4_PORT GPIO_PORT_PJ
#define LED_BANK5_PORT GPIO_PORT_P3
#define LED_BANK6_PORT GPIO_PORT_P3

#define LED_BANK1_OUT PJOUT
#define LED_BANK2_OUT PJOUT
#define LED_BANK3_OUT PJOUT
#define LED_BANK4_OUT PJOUT
#define LED_BANK5_OUT P3OUT
#define LED_BANK6_OUT P3OUT

#define LED_BANK1_PIN GPIO_PIN0
#define LED_BANK2_PIN GPIO_PIN1
#define LED_BANK3_PIN GPIO_PIN2
#define LED_BANK4_PIN GPIO_PIN3
#define LED_BANK5_PIN GPIO_PIN7
#define LED_BANK6_PIN GPIO_PIN6

void delay_millis(unsigned long mils) {
    while (mils) {
        __delay_cycles(4000);
        mils--;
    }
}

int main(void)
{
    volatile uint8_t in = 0;

    init();

    uint8_t shift = 0;
    while (1) {

        LED_BANK1_OUT |= LED_BANK1_PIN | LED_BANK2_PIN | LED_BANK3_PIN | LED_BANK4_PIN;
        LED_BANK5_OUT |= LED_BANK5_PIN | LED_BANK6_PIN;

        tlc_set_gs(0);

        switch (shift) {
        case 0:
//            GPIO_setOutputLowOnPin(LED_BANK1_PORT, LED_BANK1_PIN);
            LED_BANK1_OUT &= ~LED_BANK1_PIN;
            shift++;
            break;
        case 1:
//            GPIO_setOutputLowOnPin(LED_BANK2_PORT, LED_BANK2_PIN);
            LED_BANK2_OUT &= ~LED_BANK2_PIN;
            shift++;
            break;
        case 2:
//            GPIO_setOutputLowOnPin(LED_BANK3_PORT, LED_BANK3_PIN);
            LED_BANK3_OUT &= ~LED_BANK3_PIN;
            shift++;
            break;
        case 3:
//            GPIO_setOutputLowOnPin(LED_BANK4_PORT, LED_BANK4_PIN);
            LED_BANK4_OUT &= ~LED_BANK4_PIN;
            shift++;
            break;
        case 4:
//            GPIO_setOutputLowOnPin(LED_BANK5_PORT, LED_BANK5_PIN);
            LED_BANK5_OUT &= ~LED_BANK5_PIN;
            shift++;
            break;
        case 5:
//            GPIO_setOutputLowOnPin(LED_BANK6_PORT, LED_BANK6_PIN);
            LED_BANK6_OUT &= ~LED_BANK6_PIN;
            shift=0;
            break;
        }

        delay_millis(5);
    }

    /*
    {
    	tlc_set_fun(1);
//    	tlc_set_gs(shift);
//    	tlc_set_fun(0);
//    	shift = (shift + 3) % 15;
//    	__delay_cycles(2000000);
    }
    */
}
