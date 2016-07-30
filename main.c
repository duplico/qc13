/*
 * ======== Standard MSP430 includes ========
 */
#include <stdio.h>

// Project includes:
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "qc13.h"
#include "rfm75.h"
#include "led_display.h"
#include "tlc5948a.h"
#include "etc/tentacles/leg_anims.h"
#include "badge.h"
#include "mating.h"
#include "metrics.h"

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

uint8_t fresh_power = 0; // Turned on for the first time.

uint8_t hat_check_this_cycle = 0;

uint8_t hat_state = HS_NONE;

// Signals to the main loop (not caused by interrupts):
uint8_t s_b_start = 0;
uint8_t s_b_select = 0;
uint8_t s_b_ohai = 0;
uint8_t s_face_anim_done = 0;
uint8_t s_hat_check = 0;

// ADC related:
//  light:
uint16_t lights[ADC_WINDOW] = {0};
uint16_t light = 0;
uint8_t light_order = 0;
uint16_t light_tot = 0;
uint8_t light_index = 0;

//  temp:
uint16_t temps[ADC_WINDOW] = {0};
uint16_t temp = 0;
uint8_t temp_band = TEMP_NORMAL;
uint16_t temp_tot = 0;
uint8_t temp_index = 0;

// hat voltage:
uint16_t hat_potentials[ADC_WINDOW] = {0};
uint16_t hat_potential = 0;
uint32_t hat_v_tot = 0;
uint8_t hat_v_index = 0;

// Initialization functions
///////////////////////////

void init_adc() {
    // Set 1.0 and 1.1 to ternary module function (A0 and A1 respectively)
    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P1, GPIO_PIN0, GPIO_TERNARY_MODULE_FUNCTION);
    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P1, GPIO_PIN1, GPIO_TERNARY_MODULE_FUNCTION);

    // Set 3.3 to ternary (A15)
    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P3, GPIO_PIN3, GPIO_TERNARY_MODULE_FUNCTION);

    /* Struct to pass to ADC12_B_init */
    ADC12_B_initParam initParam = {0};

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

    /* Struct to pass to ADC12_B_configureMemory */
    ADC12_B_configureMemoryParam configureMemoryParam = {0};

    // Buffer 0
    configureMemoryParam.memoryBufferControlIndex = ADC12_B_MEMORY_0;
    configureMemoryParam.inputSourceSelect = ADC12_B_INPUT_A0;
    configureMemoryParam.refVoltageSourceSelect = ADC12_B_VREFPOS_AVCC_VREFNEG_VSS;
    configureMemoryParam.endOfSequence = ADC12_B_NOTENDOFSEQUENCE;
    configureMemoryParam.windowComparatorSelect = ADC12_B_WINDOW_COMPARATOR_DISABLE;
    configureMemoryParam.differentialModeSelect = ADC12_B_DIFFERENTIAL_MODE_DISABLE;
    ADC12_B_configureMemory(ADC12_B_BASE, &configureMemoryParam);

    // Buffer 1
    configureMemoryParam.memoryBufferControlIndex = ADC12_B_MEMORY_1;
    configureMemoryParam.inputSourceSelect = ADC12_B_INPUT_A1;
    configureMemoryParam.refVoltageSourceSelect = ADC12_B_VREFPOS_AVCC_VREFNEG_VSS;
    configureMemoryParam.endOfSequence = ADC12_B_NOTENDOFSEQUENCE;
    configureMemoryParam.windowComparatorSelect = ADC12_B_WINDOW_COMPARATOR_DISABLE;
    configureMemoryParam.differentialModeSelect = ADC12_B_DIFFERENTIAL_MODE_DISABLE;
    ADC12_B_configureMemory(ADC12_B_BASE, &configureMemoryParam);

    // Buffer 3
    configureMemoryParam.memoryBufferControlIndex = ADC12_B_MEMORY_2;
    configureMemoryParam.inputSourceSelect = ADC12_B_INPUT_A15;
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
    static uint8_t prev_temp_band = 5;

    if (!being_inked) {
        // Light:
        light_tot -= lights[light_index];
        lights[light_index] = ADC12_B_getResults(ADC12_B_BASE, ADC12_B_MEMORY_0) >> 1;
        if (lights[light_index] < 3) lights[light_index] = 3;
        light_tot += lights[light_index];
        light_index++;
        if (light_index == ADC_WINDOW) light_index = 0;
    }

    // Temp:
    temp_tot -= temps[temp_index];
    temps[temp_index] = ADC12_B_getResults(ADC12_B_BASE, ADC12_B_MEMORY_1);
    temp_tot += temps[temp_index];
    temp_index++;
    if (temp_index == ADC_WINDOW) temp_index = 0;
    temp = temp_tot / ADC_WINDOW;

    // Temp has 3 bands: COLD < NORMAL < HOT
    if (temp > TEMP_THRESH_HOT) {
        temp_band = TEMP_HOT;
    } else if (temp > TEMP_THRESH_COLD) {
        temp_band = TEMP_NORMAL;
    } else {
        temp_band = TEMP_COLD;
    }

    if (temp_band != prev_temp_band) {
        // changed
        temp_band_change(prev_temp_band, temp_band);
        prev_temp_band = temp_band;
    }

    // Hat:
    hat_v_tot -= hat_potentials[hat_v_index];
    hat_potentials[hat_v_index] = ADC12_B_getResults(ADC12_B_BASE, ADC12_B_MEMORY_2);
    hat_v_tot += hat_potentials[hat_v_index];
    hat_v_index++;
    if (hat_v_index == ADC_WINDOW) {
        hat_v_index = 0;
        if (hat_check_this_cycle)
            s_hat_check = 1;
        hat_check_this_cycle = !hat_check_this_cycle;
    }
}

void term_gpio() {
    P1SEL0 = 0;
    P1SEL1 = 0;
    P2SEL0 = 0;
    P2SEL1 = 0;
    P3SEL0 = 0;
    P3SEL1 = 0;
    P4SEL0 = 0;
    P4SEL1 = 0;
    PJSEL0 = 0;
    PJSEL1 = 0;

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

    // LED banks:
    P3DIR |= (LED_BANK5_PIN | LED_BANK6_PIN);
    PJDIR |= (LED_BANK1_PIN | LED_BANK2_PIN | LED_BANK3_PIN | LED_BANK4_PIN);
    LED_BANK1_OUT |= (LED_BANK1_PIN | LED_BANK2_PIN | LED_BANK3_PIN
            | LED_BANK4_PIN);
    LED_BANK5_OUT |= (LED_BANK5_PIN | LED_BANK6_PIN);

}

void my_conf_write_crc() {
    CRC_setSeed(CRC_BASE, 0xc13c);
    for (uint8_t i = 0; i < sizeof(qc13conf) - 2; i++) {
        CRC_set8BitData(CRC_BASE, ((uint8_t *) &my_conf)[i]);
    }
    my_conf.crc16 = CRC_getResult(CRC_BASE);
    memcpy(&backup_conf, &my_conf, sizeof(qc13conf));
}

uint8_t conf_is_valid(qc13conf *conf) {
    CRC_setSeed(CRC_BASE, 0xc13c);
    for (uint8_t i = 0; i < sizeof(qc13conf) - 2; i++) {
        CRC_set8BitData(CRC_BASE, ((uint8_t *) conf)[i]);
    }

    return conf->crc16 == CRC_getResult(CRC_BASE);
}

void make_fresh_conf() {
    memcpy(&my_conf, &default_conf, sizeof(qc13conf));
    memset(badges_seen, 0x00, BADGES_IN_SYSTEM);
    memset(badges_mated, 0x00, BADGES_IN_SYSTEM);
    memset(odh_badges_ticks, 0x00, HANDLER_MAX_INCLUSIVE+1);
    memset(uber_badges_ticks, 0x00, UBER_COUNT);

    make_eligible_for_pull_hat(HAT_MINUTEMAN);

    if (my_conf.badge_id == GEORGE_ID) {
        unlock_camo(LEG_ANIM_ZFLAG_LEATHER);
    }
    if (my_conf.badge_id == EVAN_ID) {
        unlock_camo(LEG_ANIM_ZFLAG_BI);
    }
    if (my_conf.badge_id == JONATHAN_ID) {
        unlock_camo(LEG_ANIM_ZFLAG_BEAR);
    }

    if (is_uber(my_conf.badge_id)) {
        unlock_camo(LEG_ANIM_UBER);
        my_conf.uber_hat_given = 0;
        award_push_hat(HAT_UBER);
        my_conf_write_crc();
    }
    if (is_handler(my_conf.badge_id)) {
        // We'll unlock the camo when we go on duty.
        award_push_hat(HAT_HANDLER);
    }
    if (is_donor(my_conf.badge_id)) {
        // Unlock the hat...
        award_push_hat(my_conf.badge_id);
    }
    if (my_conf.badge_id == JASON_ID) {
        unlock_camo(LEG_ANIM_SHUTDOWN);
    }

    unlock_camo(LEG_ANIM_DEF);

    set_badge_seen(my_conf.badge_id, is_handler(my_conf.badge_id));
    set_badge_mated(my_conf.badge_id, is_handler(my_conf.badge_id));
}

void setup_my_conf() {
    fresh_power = 1;
    if (conf_is_valid(&my_conf)) {
        lock_camo(LEG_ANIM_HANDLER);
        if (my_conf.power_cycles < POWER_CYCLES_FOR_HAT)
            my_conf.power_cycles++;
        else
            make_eligible_for_pull_hat(HAT_POWER_CYCLES);
        my_conf_write_crc();
    } else {
        if (conf_is_valid(&backup_conf)) {
            memcpy(&my_conf, &backup_conf, sizeof(qc13conf));
        } else {
            make_fresh_conf();
        }
    }

    eyes_spinning = my_conf.hat_holder && !my_conf.hat_claimed;

    srand(my_conf.badge_id);
}

void init_clocks() {
    // MCLK: DCO /2
    CS_initClockSignal(CS_MCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_2);
    // SMCLK is DCO /11
    CS_initClockSignal(CS_SMCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1);
    // ACLK is LFMODOSC /1
    CS_initClockSignal(CS_ACLK, CS_LFMODOSC_SELECT, CS_CLOCK_DIVIDER_1);

    // DCO to 16 MHz (high freq, option 4)
    CS_setDCOFreq(CS_DCORSEL_1, CS_DCOFSEL_4);

    // Allow conditional module requests for MCLK, SMCLK, and ACLK:
    CS_disableClockRequest(CS_MCLK);
    CS_disableClockRequest(CS_SMCLK);
    CS_disableClockRequest(CS_ACLK);

    // Configure watchdog:
    WDT_A_initWatchdogTimer(WDT_A_BASE, WDT_A_CLOCKSOURCE_ACLK, WDT_A_CLOCKDIVIDER_32K); // Just under 1 second.
}

// 0: OK; nonzero: problems
uint8_t clocks_post_errors() {
    // Clear fault flags:
    return CS_clearAllOscFlagsWithTimeout(100000);
}

int _system_pre_init(void)
{
    // stop WDT
    WDTCTL = WDTPW + WDTHOLD;

    // Perform C/C++ global data initialization
    return 1;
}

void init() {
    PM5CTL0 &= ~LOCKLPM5; // Unlock pins.

    volatile uint16_t rsv = 0;
    rsv = SYSRSTIV;
    __no_operation();

    // No waiting at all, because we're running <= 8MHz:
    FRAMCtl_configureWaitStateControl(FRAMCTL_ACCESS_TIME_CYCLES_0);
    term_gpio(); // Terminate all GPIO.
    init_clocks();

    // Buttons and mating port:
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

    __bis_SR_register(GIE);
    tlc_init();   // Initialize our LED system (including GPIO)
    rfm75_init(); // Initialize our radio (including GPIO)
    init_mating();// Initialize mating port (GPIO is above)
    init_adc();   // Start up the ADC for light and temp sensors.
}

void post() {
    // test LEDs:
    if (fresh_power) {
        led_post();
    }
    else {
        set_face(0);
        tlc_stage_blank(0);
        tlc_set_fun();
    }

    // test RFM75:
    uint8_t ret = rfm75_post();
    if (!ret) { // bad radio:
        face_set_ambient_direct(0xffffffff00000000);
        delay_millis(3000);
    }

    // test clocks:
    ret = clocks_post_errors();
    if (ret) { // bad clock flag:
        face_set_ambient_direct(0x00000000ffffffff);
        delay_millis(3000);
    }

}

void delay_millis(unsigned long mils) {
    while (mils) {
        __delay_cycles(8000);
        mils--;
    }
}

uint8_t start_seconds_pressed = 0;
uint8_t start_pressed = 0;
uint8_t select_seconds_pressed = 0;
uint8_t select_pressed = 0;

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

void do_hat_check() {
    hat_potential = hat_v_tot / ADC_WINDOW;

    uint8_t hat_type_detected = HS_NONE;
    if (1750 < hat_potential && hat_potential < 1900) {
        hat_type_detected = HS_HANDLER;
    } else if (hat_potential > 4050) {
        hat_type_detected = HS_HUMAN;
    } else if (2250 < hat_potential && hat_potential < 2350) {
        hat_type_detected = HS_UBER;
    }

    if (hat_type_detected != hat_state) {
        // switching hats
        hat_change(hat_state, hat_type_detected);
        hat_state = hat_type_detected;

        if ((!my_conf.hat_holder && hat_state) ||
                (!is_uber(my_conf.badge_id) && (hat_state & HS_UBER)) ||
                (!is_handler(my_conf.badge_id) && (hat_state & HS_HANDLER))) {
            borrowing_hat();
        }

    }
}

void time_loop() {
    static uint8_t interval_seconds_remaining = BEACON_INTERVAL_SECS;
    static uint16_t second_loops = LOOPS_PER_SECOND;
    static uint8_t second_minute_loops = 0;
    if (second_loops) {
        second_loops--;
    } else {
        second_minute_loops ++;
        if (second_minute_loops >= SECONDS_PER_MINUTE) {
            second_minute_loops = 0;
            minute();
        }
        second_loops = LOOPS_PER_SECOND;
        second();
        if (interval_seconds_remaining) {
            interval_seconds_remaining--;
        } else {
            radio_beacon_interval();
            interval_seconds_remaining = BEACON_INTERVAL_SECS;
        }

        if (start_pressed) {
            start_seconds_pressed++;
            if (start_seconds_pressed >= LONG_PRESS_THRESH) {
                start_button_longpressed();
            }
        }
        if (select_pressed) {
            select_seconds_pressed++;
            if (select_seconds_pressed >= LONG_PRESS_THRESH) {
                select_button_longpressed();
            }
        }

    }

    if (mate_state == MS_INK_WAIT) {
        mate_ink_wait++;
        if (mate_ink_wait == SUPER_INK_WINDOW_SECS * LOOPS_PER_SECOND) {
            ink_wait_timeout();
        }
    }

    if (mate_state == MS_SUPER_INK) {
        mate_ink_wait++;
        if (mate_ink_wait == SUPER_INK_DECAY_SECS * LOOPS_PER_SECOND) {
            super_ink_timeout();
        }
    }
    WDT_A_resetTimer(WDT_A_BASE);
}

int main(void)
{
    init();
    post();

    initial_animations();

    WDT_A_start(WDT_A_BASE);

    static uint8_t led_loops = 0;

    while (1)
    {
        if (f_time_loop) {
            poll_buttons();
            poll_adc();
            if (!led_loops) {
                leds_timestep();
                led_loops = LED_DUR_LOOPS;
            }
            led_loops--;
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

        if (s_hat_check && !waking_up) {
            do_hat_check();
            s_hat_check = 0;
        }

        if (s_b_start == BUTTON_PRESS) {
            s_b_start = 0;
            start_seconds_pressed = 0;
            start_pressed = 1;
        }

        if (s_b_start == BUTTON_RELEASE) {
            s_b_start = 0;
            start_pressed = 0;
            if (start_seconds_pressed < LONG_PRESS_THRESH) {
                start_button_clicked();
            }
        }

        if (s_b_select == BUTTON_PRESS) {
            s_b_select = 0;
            select_seconds_pressed = 0;
            select_pressed = 1;
        }

        if (s_b_select == BUTTON_RELEASE) {
            s_b_select = 0;
            select_pressed = 0;
            if (select_seconds_pressed < LONG_PRESS_THRESH) {
                select_button_clicked();
            }
        }

        if (s_b_ohai == BUTTON_PRESS) { // badges connected.
            mate_state = MS_PLUG;
            mate_plug();
            s_b_ohai = 0;
        }

        if (s_b_ohai == BUTTON_RELEASE) { // badges disconnected.
            mate_end(0); // clean up.
            s_b_ohai = 0;
        }

        if (s_face_anim_done) {
            s_face_anim_done = 0;
            face_animation_done();
        }

        // If no more interrupt flags are set, go to sleep.
        if (!f_time_loop && !f_rfm75_interrupt)
            __bis_SR_register(SLEEP_BITS);
    }

}
