/*
 * ======== Standard MSP430 includes ========
 */
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

uint16_t light = 0;
uint16_t temp = 0;

void init_adc() {
    // TODO: Better documentation

    /* Struct to pass to ADC12_B_init */
    ADC12_B_initParam initParam = {0};

    /* Struct to pass to ADC12_B_configureMemory */
    ADC12_B_configureMemoryParam configureMemoryParam = {0};

    /* Initializes ADC12_B */
    initParam.sampleHoldSignalSourceSelect = ADC12_B_SAMPLEHOLDSOURCE_SC;
    initParam.clockSourceSelect = ADC12_B_CLOCKSOURCE_ADC12OSC;
    initParam.clockSourceDivider = ADC12_B_CLOCKDIVIDER_1;
    initParam.clockSourcePredivider = ADC12_B_CLOCKPREDIVIDER__1;
    initParam.internalChannelMap = 0;
    ADC12_B_init(ADC12_B_BASE, &initParam);

    /* Enables ADC12_B */
    ADC12_B_enable(ADC12_B_BASE);

    /* Sets up and enables the Sampling Timer Pulse Mode */
    ADC12_B_setupSamplingTimer(ADC12_B_BASE, ADC12_B_CYCLEHOLD_8_CYCLES, ADC12_B_CYCLEHOLD_8_CYCLES, ADC12_B_MULTIPLESAMPLESENABLE);

    /* Configures the memory buffer 1 */
    configureMemoryParam.memoryBufferControlIndex = ADC12_B_MEMORY_1;
    configureMemoryParam.inputSourceSelect = ADC12_B_INPUT_A1;
    configureMemoryParam.refVoltageSourceSelect = ADC12_B_VREFPOS_AVCC_VREFNEG_VSS;
    configureMemoryParam.endOfSequence = ADC12_B_ENDOFSEQUENCE;
    configureMemoryParam.windowComparatorSelect = ADC12_B_WINDOW_COMPARATOR_DISABLE;
    configureMemoryParam.differentialModeSelect = ADC12_B_DIFFERENTIAL_MODE_DISABLE;
    ADC12_B_configureMemory(ADC12_B_BASE, &configureMemoryParam);

    /* Inverts/Uninverts the sample/hold signal */
    ADC12_B_setSampleHoldSignalInversion(ADC12_B_BASE, ADC12_B_NONINVERTEDSIGNAL);

    /* Sets the read-back format of the converted data */
    ADC12_B_setDataReadBackFormat(ADC12_B_BASE, ADC12_B_UNSIGNED_BINARY);
    //
    //    ADC12_B_enableInterrupt(ADC12_B_BASE, ADC12_B_IE0, 0, 0); // MEM0
    //    ADC12_B_enableInterrupt(ADC12_B_BASE, ADC12_B_IE1, 0, 0); // MEM1

    ADC12_B_startConversion(ADC12_B_BASE, ADC12_B_START_AT_ADC12MEM0, ADC12_B_REPEATED_SEQOFCHANNELS);
}

void init() {
    PM5CTL0 &= ~LOCKLPM5; // Unlock pins.
    Grace_init(); // Activate Grace-generated configuration

    tlc_init();   // Initialize our LED system
    rfm75_init(); // Initialize our radio
    init_adc();   // Start up the ADC for light and temp sensors.
}

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

    tlc_stage_blank(0);
    tlc_set_fun();

    delay_millis(10);

    while(1)
    {
        delay_millis(100); // 10 Hz busywait.
        light = ADC12_B_getResults(ADC12_B_BASE, ADC12_B_MEMORY_0);
        temp = ADC12_B_getResults(ADC12_B_BASE, ADC12_B_MEMORY_1);
        __bis_SR_register(LPM0_bits + GIE);     // LPM0, ADC12_B_ISR will force exit
        __no_operation();                       // For debugger
    }

}

volatile uint8_t shift = 0;

#pragma vector=TIMER0_A0_VECTOR
__interrupt void TIMER0_A0_ISR_HOOK(void)
{
    LED_BANK1_OUT |= LED_BANK1_PIN | LED_BANK2_PIN | LED_BANK3_PIN
            | LED_BANK4_PIN;
    LED_BANK5_OUT |= LED_BANK5_PIN | LED_BANK6_PIN;
    tlc_set_gs();
    __bic_SR_register_on_exit(LPM0_bits);
}
//
//#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
//#pragma vector=ADC12_VECTOR
//__interrupt
//#elif defined(__GNUC__)
//__attribute__((interrupt(ADC12_VECTOR)))
//#endif
//void ADC12_ISR(void)
//{
//    light = ADC12_B_getResults(ADC12_B_BASE, ADC12_B_MEMORY_0);
//    temp = ADC12_B_getResults(ADC12_B_BASE, ADC12_B_MEMORY_1);
//}
//
