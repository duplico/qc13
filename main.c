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
#include "led_display.h"
#include "tlc5948a.h"
#include "etc/tentacles/leg_anims.h"
#include "badge.h"
#include "mating.h"

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

// Flags to main loop raised by interrupts (so must be volatile):
volatile uint8_t f_time_loop = 0;
volatile uint8_t f_rfm75_interrupt = 0;
volatile uint8_t f_mate_interrupt = 0;

// Signals to the main loop (not caused by interrupts):
uint8_t s_b_start = 0;
uint8_t s_b_select = 0;
uint8_t s_b_ohai = 0;
uint8_t s_face_anim_done = 0;

// ADC related:
uint16_t lights[ADC_WINDOW] = {0};
uint16_t temps[ADC_WINDOW] = {0};
uint16_t light = 0;
uint8_t light_order = 0;
uint16_t temp = 0;
uint16_t light_tot = 0;
uint16_t temp_tot = 0;
uint8_t light_index = 0;
uint8_t temp_index = 0;

// Initialization functions
///////////////////////////

void init_adc() {
    /* Struct to pass to ADC12_B_init */
    ADC12_B_initParam initParam = {0};

    /* Struct to pass to ADC12_B_configureMemory */
    ADC12_B_configureMemoryParam configureMemoryParam = {0};

    /* Initializes ADC12_B */
    initParam.sampleHoldSignalSourceSelect = ADC12_B_SAMPLEHOLDSOURCE_SC;
    initParam.clockSourceSelect = ADC12_B_CLOCKSOURCE_ADC12OSC;
    initParam.clockSourceDivider = ADC12_B_CLOCKDIVIDER_1;
    initParam.clockSourcePredivider = ADC12_B_CLOCKPREDIVIDER__32;
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

    ADC12_B_startConversion(ADC12_B_BASE, ADC12_B_START_AT_ADC12MEM0, ADC12_B_REPEATED_SEQOFCHANNELS);
}

void poll_adc() {
    light_tot -= lights[light_index];
    temp_tot -= temps[temp_index];
    lights[light_index] = ADC12_B_getResults(ADC12_B_BASE, ADC12_B_MEMORY_0) >> 1;
    temps[temp_index] = ADC12_B_getResults(ADC12_B_BASE, ADC12_B_MEMORY_1) >> 1;

    if (lights[light_index] < 3) lights[light_index] = 3;

    light_tot += lights[light_index];
    temp_tot += temps[temp_index];

    light_index++;
    temp_index++;

    if (light_index == ADC_WINDOW) light_index = 0;
    if (temp_index == ADC_WINDOW) temp_index = 0;
    // light gets set from light_tot in badge.c.


    temp = temp_tot / ADC_WINDOW;
    // Temp has 3 bands: COLD < NORMAL < HOT
}

void term_gpio() {
    P1DIR = 0xFF;
    P1OUT = 0x00;
    P2DIR = 0xFF;
    P2OUT = 0x00;
    P3DIR = 0xFF;
    P3OUT = 0x00;
    P4DIR = 0xFF;
    P4OUT = 0x00;
    PJDIR = 0xFF;
    PJOUT = 0x00;
}

// TODO:
void setup_my_conf() {
    memcpy(&my_conf, &default_conf, sizeof(qc13conf));
    badges_seen[my_conf.badge_id] = 1; // TODO: More of this.
}

void init() {
    PM5CTL0 &= ~LOCKLPM5; // Unlock pins.
    term_gpio();
    Grace_init(); // Activate Grace-generated configuration

    // Buttons:
    P3DIR &= ~BIT4;
    P3REN |= BIT4;
    P3OUT |= BIT4;

    P3DIR &= ~BIT0;
    P3REN |= BIT0;
    P3OUT |= BIT0;

    P2DIR &= ~BIT7;
    P2REN |= BIT7;
    P2OUT |= BIT7;

    setup_my_conf();

    tlc_init();   // Initialize our LED system
    rfm75_init(); // Initialize our radio
    init_mating();// Initialize mating port
    init_adc();   // Start up the ADC for light and temp sensors.

}

void post() {
    led_post();
    uint8_t ret = rfm75_post();
    if (!ret) { // bad radio:
        face_set_ambient_direct(0b1111111111111111111111111111111111111111111111111111111111111111);
        delay_millis(5000);
    }
}

void delay_millis(unsigned long mils) {
    while (mils) {
        __delay_cycles(8000);
        mils--;
    }
}

void poll_buttons() {
    static uint8_t b_start_read_prev = 1;
    static uint8_t b_start_read = 1;
    static uint8_t b_start_state = 1;

    static uint8_t b_select_read_prev = 1;
    static uint8_t b_select_read = 1;
    static uint8_t b_select_state = 1;

    static uint8_t b_ohai_read_prev = 1;
    static uint8_t b_ohai_read = 1;
    static uint8_t b_ohai_state = 1;

    // Poll the buttons two time loops in a row to debounce and
    // if there's a change, raise a flag.
    b_select_read = GPIO_getInputPinValue(GPIO_PORT_P2, GPIO_PIN7);
    b_start_read = GPIO_getInputPinValue(GPIO_PORT_P3, GPIO_PIN4);

    if (b_start_read == b_start_read_prev && b_start_read != b_start_state) {
        s_b_start = b_start_read? BUTTON_RELEASE : BUTTON_PRESS; // active low
        b_start_state = b_start_read;
    }
    b_start_read_prev = b_start_read;

    if (b_select_read == b_select_read_prev && b_select_read != b_select_state) {
        s_b_select = b_select_read? BUTTON_RELEASE : BUTTON_PRESS; // active low
        b_select_state = b_select_read;
    }
    b_select_read_prev = b_select_read;

    b_ohai_read = GPIO_getInputPinValue(GPIO_PORT_P3, GPIO_PIN0);
    if (b_ohai_read == b_ohai_read_prev && b_ohai_read != b_ohai_state) {
        s_b_ohai = b_ohai_read? BUTTON_RELEASE : BUTTON_PRESS; // active low
        b_ohai_state = b_ohai_read;
    }
    b_ohai_read_prev = b_ohai_read;

} // poll_buttons

int main(void)
{
    init();
    post();

    initial_animations();

    while (1)
    {
        if (f_time_loop) {
            poll_buttons();
            poll_adc();
            leds_timestep();

            time_loop();

            f_time_loop = 0;
        }

        if (f_mate_interrupt) {
            mate_deferred_rx_interrupt();
            f_mate_interrupt = 0;
        }

        if (f_rfm75_interrupt) {
            rfm75_deferred_interrupt();
            f_rfm75_interrupt = 0;
        }

        if (s_b_start == BUTTON_PRESS) {
            s_b_start = 0;
        }

        if (s_b_start == BUTTON_RELEASE) {
            s_b_start = 0;

            start_button_clicked();
        }

        if (s_b_select == BUTTON_PRESS) {
            s_b_select = 0;
        }

        if (s_b_select == BUTTON_RELEASE) {
            select_button_clicked();
            s_b_select = 0;
        }

        if (s_b_ohai == BUTTON_PRESS) { // badges connected.
            mate_state = MS_PLUG; // TODO: make a function here?
            s_b_ohai = 0;
        }

        if (s_b_ohai == BUTTON_RELEASE) { // badges disconnected.
            mate_state = MS_IDLE; // TODO: definitely function.
            mate_end(0); // clean up.
            s_b_ohai = 0;
        }

        if (s_face_anim_done) {
            s_face_anim_done = 0;
            face_animation_done();
        }

        // If no more interrupt flags are set, go to sleep.
        if (!f_time_loop && !f_rfm75_interrupt)
            __bis_SR_register(SLEEP_BITS + GIE);
    }

}
