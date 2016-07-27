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
#include "badge.h"
#include "metrics.h"

// TODO: base.
#include "oled.h"

/*
 *
 * Peripherals
 * ===========
 *
 * RFM:    USCI_B0
 * OLED:   USCI_A1
 * Light:  P1.0
 * Temp:   P1.1
 *
 */

// Flags to main loop raised by interrupts (so must be volatile):
volatile uint8_t f_time_loop = 0;
volatile uint8_t f_rfm75_interrupt = 0;

// Base related:

const char sk_labels[SK_SEL_MAX+1][12] = {
        "Unlock",
        "Lock",
        "E: Off",
        "E: trans", // base ID 0, so we send sk_label index - 3
        "E: thu mix",
        "E: talk",
        "E: fri mix",
        "E: pool",
        "E: women",
        "E: sat mix",
        "E: karaoke",
        "E: sun mix",
        "psh F hat",
        "psh L hat",
};

const char base_labels[][12] = {
        "trans",
        "thu mix",
        "talk",
        "fri mix",
        "pool",
        "women",
        "sat mix",
        "karaoke",
        "sun mix",
};

uint8_t op_mode = OP_MODE_IDLE; // In the "modal" sense:
uint8_t suppress_softkey = 0;
uint16_t softkey_en = SK_BIT_UNLOCK; // UNLOCK only.
uint8_t idle_mode_softkey_sel = 0;
uint8_t idle_mode_softkey_dis = 0;
volatile uint8_t f_bl = 0;
volatile uint8_t f_br = 0;
volatile uint8_t f_bs = 0; // TODO: nonvolatile.
uint8_t s_oled_needs_redrawn_idle = 0;
uint8_t s_new_pane = 1;

tRectangle name_erase_rect = {0, NAME_Y_OFFSET, 63, NAME_Y_OFFSET + NAME_FONT_HEIGHT + SYS_FONT_HEIGHT};
uint8_t update_disp = 0;
uint8_t char_entry_index = 0;
uint8_t curr_char = ' ';
uint8_t underchar_x = 0;
uint8_t text_width = 0;
uint8_t last_char_index = 0;
uint16_t bs_down_loops = 0;
const char undername[2] = {NAME_SEL_CHAR, 0};
char name[NAME_MAX_LEN+1] = {' ', 0};

void disp_mode_unlock();
void enter_unlock();
uint8_t softkey_enabled(uint8_t index);
void disp_mode_txt();

// Initialization functions
///////////////////////////

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

}

void enable_correct_softkeys() {
    if (my_conf.locked) {
        softkey_en = BIT0;
    } else if ((my_conf.base_id == BASE_BPOOL && !my_conf.hat_sent_pool_start) ||
               (my_conf.base_id == BASE_BKARAOKE && !my_conf.hat_sent_sat_start) ||
               (my_conf.base_id == BASE_BTALK && !my_conf.hat_sent_talk) ||
               (my_conf.base_id == BASE_BSATMIX && !my_conf.hat_sent_sat_mix_start)) {
        softkey_en = SK_MASK_STARTABLE;
    } else if ((my_conf.base_id == BASE_BPOOL && !my_conf.hat_sent_pool_end) ||
               (my_conf.base_id == BASE_BKARAOKE && !my_conf.hat_sent_sat_end)) {
        softkey_en = SK_MASK_ENDABLE;
    } else {
        softkey_en = SK_MASK_NOPUSH;
    }

    while (!softkey_enabled(idle_mode_softkey_sel)) {
        idle_mode_softkey_sel = (idle_mode_softkey_sel+1) % (SK_SEL_MAX+1);
    }

    s_new_pane = 1;

}

void my_conf_write_crc() {
    enable_correct_softkeys();

    CRC_setSeed(CRC_BASE, 0xc13c);
    for (uint8_t i = 0; i < sizeof(qc13conf) - 2; i++) {
        CRC_set8BitData(CRC_BASE, ((uint8_t *) &my_conf)[i]);
    }
    my_conf.crc16 = CRC_getResult(CRC_BASE);
}

uint8_t my_conf_is_valid() {
    // TODO: Additional validation?

    CRC_setSeed(CRC_BASE, 0xc13c);
    for (uint8_t i = 0; i < sizeof(qc13conf) - 2; i++) {
        CRC_set8BitData(CRC_BASE, ((uint8_t *) &my_conf)[i]);
    }

    return my_conf.crc16 == CRC_getResult(CRC_BASE);
}

void make_fresh_conf() {
    memcpy(&my_conf, &default_conf, sizeof(qc13conf));
    my_conf_write_crc();
}

void setup_my_conf() {
    if (!my_conf_is_valid()) {
        make_fresh_conf();
    } else {
//        my_conf.locked = 1;
        my_conf_write_crc();
    }

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

    // No waiting at all, because we're running <= 8MHz:
    FRAMCtl_configureWaitStateControl(FRAMCTL_ACCESS_TIME_CYCLES_0);
    term_gpio(); // Terminate all GPIO.
    init_clocks();

    setup_my_conf();
    __bis_SR_register(GIE);

    rfm75_init(); // Initialize our radio (including GPIO)
    init_oled();

    // Buttons:

    GPIO_setAsInputPin(GPIO_PORT_P3, GPIO_PIN4);
    GPIO_setAsInputPin(GPIO_PORT_P3, GPIO_PIN5);
    GPIO_setAsInputPin(GPIO_PORT_P3, GPIO_PIN6);

    // A0 / LED channel timer:    // Next we configure the clock that tells us when it's time to select the
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
    Timer_A_initUpMode(TIMER_A0_BASE, &next_channel_timer_init);
    Timer_A_startCounter(TIMER_A0_BASE, TIMER_A_UP_MODE);
}

void post() {
    // test RFM75:
    uint8_t bad_radio = !rfm75_post();
    uint8_t bad_clocks = clocks_post_errors();
    if (!(bad_radio || bad_clocks)) {
        return;
    }

    GrClearDisplay(&g_sContext);
    GrContextFontSet(&g_sContext, &SYS_FONT);
    oled_print(0, 5, "- POST -", 1, 1);

    if (bad_radio) {
        oled_print(0, 10 + SYS_FONT_HEIGHT, "rfm75 err", 1, 1);
    }
    if (bad_clocks) { // bad clock flag:
        oled_print(0, 15 + 2*SYS_FONT_HEIGHT, "clock err", 1, 1);
    }
    GrFlush(&g_sContext);
    delay_millis(5000);

}

void delay_millis(unsigned long mils) {
    while (mils) {
        __delay_cycles(8000);
        mils--;
    }
}

void poll_buttons() {

    static uint8_t bl_read_prev = 1;
    static uint8_t bl_read = 1;
    static uint8_t bl_state = 1;

    static uint8_t br_read_prev = 1;
    static uint8_t br_read = 1;
    static uint8_t br_state = 1;

    static uint8_t bs_read_prev = 1;
    static uint8_t bs_read = 1;
    static uint8_t bs_state = 1;

    // Poll the buttons two time loops in a row to debounce and
    // if there's a change, raise a flag.
    // Left button:
    bl_read = GPIO_getInputPinValue(GPIO_PORT_P3, GPIO_PIN6);
    if (bl_read == bl_read_prev && bl_read != bl_state) {
        f_bl = bl_read? BUTTON_RELEASE : BUTTON_PRESS; // active low
        bl_state = bl_read;
    }
    bl_read_prev = bl_read;

    // Softkey button:
    bs_read = GPIO_getInputPinValue(GPIO_PORT_P3, GPIO_PIN5);
    if (bs_read == bs_read_prev && bs_read != bs_state) {
        if (suppress_softkey) {
            // suppress_softkey means we don't generate a flag for the next
            // release (or press, I guess, but we mostly care about releases.)
            suppress_softkey = 0;
        } else {
            f_bs = bs_read? BUTTON_RELEASE : BUTTON_PRESS; // active low
        }
        bs_state = bs_read;
    }
    bs_read_prev = bs_read;

    // Right button:
    br_read = GPIO_getInputPinValue(GPIO_PORT_P3, GPIO_PIN4);
    if (br_read == br_read_prev && br_read != br_state) {
        f_br = br_read? BUTTON_RELEASE : BUTTON_PRESS; // active low
        br_state = br_read;
    }
    br_read_prev = br_read;
} // poll_buttons

uint8_t softkey_enabled(uint8_t index) {
    return ((1<<index) & softkey_en)? 1 : 0;
}

// unlocked:
void disp_mode_idle() {
    if (idle_mode_softkey_dis) {
        f_br = f_bl = f_bs = 0;
    }

    if (f_br == BUTTON_PRESS) {
        // Left button
        do {
            idle_mode_softkey_sel = (idle_mode_softkey_sel+1) % (SK_SEL_MAX+1);
        } while (!softkey_enabled(idle_mode_softkey_sel));
        s_new_pane = 1;
    }
    f_br = 0;

    if (f_bl == BUTTON_PRESS) {
        do {
            idle_mode_softkey_sel = (idle_mode_softkey_sel+SK_SEL_MAX) % (SK_SEL_MAX+1);
        } while (!softkey_enabled(idle_mode_softkey_sel));
        s_new_pane = 1;
    }
    f_bl = 0;

    if (f_bs == BUTTON_RELEASE) {
        f_bs = 0;
        // Select button
        switch (idle_mode_softkey_sel) {
        case SK_SEL_UNLOCK:
            enter_unlock();
            break;
        case SK_SEL_LOCK:
            my_conf.locked = 1;
            my_conf_write_crc();
            idle_mode_softkey_sel = SK_SEL_UNLOCK;
            s_new_pane = 1;
            break;
        case SK_SEL_BOFF:
            my_conf.base_id = NOT_A_BASE;
            my_conf_write_crc();
            s_new_pane = 1;
            break;
        case SK_SEL_TRIGGER_START:
            if (my_conf.base_id == BASE_BPOOL) {
                award_hat(HAT_POOL_FIRST);
            } else if (my_conf.base_id == BASE_BKARAOKE) {
                award_hat(HAT_KARAOKE_FIRST);
            } else if (my_conf.base_id == BASE_BTALK) {
                award_hat(HAT_BADGE_TALK);
            } else if (my_conf.base_id == BASE_BSATMIX) {
                award_hat(HAT_MIXER_SAT_FIRST);
            }
            update_disp = 1;
            op_mode = OP_MODE_TXT;
            break;
        case SK_SEL_TRIGGER_END:
            if (my_conf.base_id == BASE_BPOOL) {
                award_hat(HAT_POOL_LAST);
            } else if (my_conf.base_id == BASE_BKARAOKE) {
                award_hat(HAT_KARAOKE_LAST);
            }
            update_disp = 1;
            op_mode = OP_MODE_TXT;
            break;
        default:
            if (idle_mode_softkey_sel > SK_SEL_MAX) {
                break;
            }
            my_conf.base_id = idle_mode_softkey_sel - 3;
            my_conf_write_crc();
            s_new_pane = 1;
        }
    }
}

void handle_display() {
    switch(op_mode) {
    case OP_MODE_IDLE:
        disp_mode_idle();
        break;
    case OP_MODE_UNLOCK:
        disp_mode_unlock();
        break;
    case OP_MODE_TXT:
        disp_mode_txt();
        break;
    }

    if (s_new_pane) {
        // Title or softkey or something changed:
        s_new_pane = 0;
        oled_draw_pane_and_flush(idle_mode_softkey_sel);
    }
}

void time_loop() {
    static uint8_t interval_seconds_remaining = BEACON_INTERVAL_SECS;
    static uint16_t second_loops = LOOPS_PER_SECOND;
    static uint16_t quarter_second_loops = LOOPS_PER_QUARTER_SECOND;
    static uint8_t loops = 0;
    static uint8_t q_loops = 0;

    static uint8_t display_loops = 10;

    if (q_loops) {
        q_loops--;
    } else {
        q_loops = LOOPS_PER_QUARTER_SECOND;
        quarter_second();
    }

    if (second_loops) {
        second_loops--;
    } else {
        loops += 1;
        second_loops = LOOPS_PER_SECOND;
        second();
        if (interval_seconds_remaining) {
            interval_seconds_remaining--;
        } else {
            radio_beacon_interval();
            interval_seconds_remaining = BEACON_INTERVAL_SECS;
        }
    }

    if (display_loops) {
        display_loops--;
    } else {
        poll_buttons();
        handle_display();
        display_loops = 10;
    }

}

void enter_unlock() {
    op_mode = OP_MODE_UNLOCK;
    update_disp = 1;

    GrClearDisplay(&g_sContext);
    GrContextFontSet(&g_sContext, &SYS_FONT);
    oled_print(0, 5, "Password please.", 1, 0);
    GrFlush(&g_sContext);

    // Switch to the NAME font so it's the expected width.
    GrContextFontSet(&g_sContext, &NAME_FONT);

    // Temporary buffer to hold the selected name.
    // (In the event of a power cycle we don't wand to be messing around
    //  with the actual config's handle)
    memset(name, 0, NAME_MAX_LEN+1);
    name[0] = ' ';
    char_entry_index = 0;
    curr_char = ' ';

    // String to display under the name; it's just the selection character,
    // configured in qc12.h

    // For figuring out where to put the underline & selection character:
    underchar_x = 0;
    text_width = 0;
    last_char_index = 0;
    bs_down_loops = 0;

    // For determining whether name entry is complete:
    text_width = GrStringWidthGet(&g_sContext, name, last_char_index+1);
}

void disp_mode_txt() {
    if (hat_award_state != HAS_IDLE || idle_mode_softkey_dis) {
        f_br = f_bl = f_bs = 0;
    }

    if (f_bs == BUTTON_RELEASE || f_bl == BUTTON_RELEASE || f_br == BUTTON_RELEASE) {
        f_bs = f_br = f_bl = 0;
        if (hat_award_state == HAS_IDLE) { // unnecessary.
            op_mode = OP_MODE_IDLE;
            s_new_pane = 1;
            return;
        }
        // clear...
    }

    if (hat_award_state == HAS_SUCCEED || hat_award_state == HAS_FAIL || update_disp) {
        update_disp = 0;

        GrClearDisplay(&g_sContext);
        GrContextForegroundSet(&g_sContext, ClrWhite);

        if (hat_award_state == HAS_OFFER) {
            oled_print(0,5,"Trying to award...", 0, 0);
        } else if (hat_award_state == HAS_FAIL) {
            oled_print(0,5,"Award failed. Press to close.", 0, 0);
            hat_award_state = HAS_IDLE;
        } else if (hat_award_state == HAS_SUCCEED) {
            oled_print(0,5,"Award success. Press to close.", 0, 0);
            hat_award_state = HAS_IDLE;

            switch(hat_award_offered) {
            case HAT_BADGE_TALK:
                my_conf.hat_sent_talk = 1;
                break;
            case HAT_POOL_FIRST:
                my_conf.hat_sent_pool_start = 1;
                break;
            case HAT_POOL_LAST:
                my_conf.hat_sent_pool_end = 1;
                break;
            case HAT_KARAOKE_FIRST:
                my_conf.hat_sent_sat_start = 1;
                break;
            case HAT_KARAOKE_LAST:
                my_conf.hat_sent_sat_end = 1;
                break;
            case HAT_MIXER_SAT_FIRST:
                my_conf.hat_sent_sat_mix_start = 1;
                break;
            }
            my_conf_write_crc();
            s_new_pane = 0;

        }

        GrFlush(&g_sContext);

    }

}

void disp_mode_unlock() {
    // Clear the screen and display the instructions.

    // Check for left/right buttons to change character slot
    if (f_bl == BUTTON_RELEASE) {
        if (char_entry_index > 0) {
            // check for deletion:
            if (char_entry_index == last_char_index) { // was: && curr_char == ' ')
                name[char_entry_index] = ' ';
                last_char_index--;
            }
            char_entry_index--;
            curr_char = name[char_entry_index];
            update_disp = 1;
            text_width = GrStringWidthGet(&g_sContext, name, last_char_index+1);
        }
        f_bl = 0;
    }
    if (f_br == BUTTON_RELEASE) {
        if (char_entry_index < NAME_MAX_LEN && curr_char != ' ' && text_width < 58) {
            char_entry_index++;
            if (!name[char_entry_index])
                name[char_entry_index] = ' ';
            curr_char = name[char_entry_index];
            if (char_entry_index > last_char_index)
                last_char_index = char_entry_index;
            update_disp = 1;
            text_width = GrStringWidthGet(&g_sContext, name, last_char_index+1);
        }
        f_br = 0;
    }
    if (f_bs == BUTTON_RELEASE) {
        // Softkey button cycles the current character.
        // This is a massive PITA for the person entering their name,
        // but they only have to do it once so whatever.
        if (curr_char == 'Z') // First comes capital letters
            curr_char = 'a';
        else if (curr_char == 'z') // Then lower case
            curr_char = '0';
        else if (curr_char == '9') // Then numbers
            curr_char = ' ';
        else if (curr_char == ' ') // Then a space, and then we cycle.
            curr_char = 'A';
        else
            curr_char++;
        name[char_entry_index] = curr_char;
        update_disp = 1;
        f_bs = 0;
        // Since it's released, clear the depressed loop count.
        bs_down_loops = 0;
    } else if ((last_char_index || name[0] != ' ') && f_bs == BUTTON_PRESS) {
        // If we're in a valid state to complete the name entry, and
        // the softkey button is depressed, then it's time to start
        // counting the number of time loops for which it is depressed.
        bs_down_loops = 1;
        f_bs = 0;
    }

    if (update_disp) { // (was below the counter thingy below:
        update_disp = 0;
        underchar_x = GrStringWidthGet(&g_sContext, name, char_entry_index);

        // Clear the area:
        GrContextForegroundSet(&g_sContext, ClrBlack);
        GrRectFill(&g_sContext, &name_erase_rect);
        GrContextForegroundSet(&g_sContext, ClrWhite);

        // Rewrite it:
        GrStringDraw(&g_sContext, name, -1, 0, NAME_Y_OFFSET, 1);
        GrLineDrawH(&g_sContext, 0, text_width, NAME_Y_OFFSET+12);
        GrStringDraw(&g_sContext, undername, -1, underchar_x, NAME_Y_OFFSET+13, 1);
        GrFlush(&g_sContext);
    }

    // If we're counting the number of loops for which the softkey is
    // depressed, go ahead and increment it. This is going to do one
    // double-count at the beginning, but I don't care.
    if (bs_down_loops && bs_down_loops < NAME_COMMIT_LOOPS) {
        bs_down_loops++;
    } else if (bs_down_loops) {
        // Commit the name with a correctly placed null termination character..
        uint8_t name_len = 0;
        while (name[name_len] && name[name_len] != ' ')
            name_len++;
        name[name_len] = 0; // null terminate.

        GrClearDisplay(&g_sContext);
        GrFlush(&g_sContext);

        suppress_softkey = 1;

        if (!strcmp(name, "AA")) {
            // unlock
            my_conf.locked = 0;
            idle_mode_softkey_sel = SK_SEL_LOCK;
            my_conf_write_crc();
        }

        op_mode = OP_MODE_IDLE;
        s_new_pane = 1;

    }
} // handle_mode_name

void intro() {
    GrStringDrawCentered(&g_sContext, "qc13event", -1, 31, 10, 0);
    GrImageDraw(&g_sContext, &fingerprint_1BPP_UNCOMP, 0, 18);
    GrStringDrawCentered(&g_sContext, "* 2016 *", -1, 31, 125 - SYS_FONT_HEIGHT/2, 0);
    GrFlush(&g_sContext);
}

void delay(unsigned int i) {
    delay_millis(i);
}

int main(void)
{
    init();
    post();
    intro();
    delay(1000);

    while (1)
    {
        if (f_time_loop) {
            time_loop();
            f_time_loop = 0;
        }

        if (f_rfm75_interrupt) {
            rfm75_deferred_interrupt();
            f_rfm75_interrupt = 0;
        }

        // If no more interrupt flags are set, go to sleep.
        if (!f_time_loop && !f_rfm75_interrupt)
            __bis_SR_register(SLEEP_BITS);
    }

}

// Dedicated ISR for CCR0. Vector is cleared on service.
#pragma vector=TIMER0_A0_VECTOR
__interrupt void TIMER0_A0_ISR_HOOK(void)
{
    f_time_loop = 1;
    __bic_SR_register_on_exit(SLEEP_BITS);
}
