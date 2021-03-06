/*
 * badge.c
 *
 *  Created on: Jun 27, 2016
 *      Author: George
 */

#include "qc13.h"
#include <stdlib.h>
#include "leg_anims.h"
#include "led_display.h"
#include "eye_anims.h"
#include "rfm75.h"
#include "mating.h"
#include "metrics.h"
#include "badge.h"

uint8_t neighbors_on_duty[HANDLER_MAX_INCLUSIVE+1] = {0};
uint8_t neighbor_badges[BADGES_IN_SYSTEM] = {0};
uint8_t neighbor_count = 0;

uint8_t blink_repeat_count = 0;

uint8_t ink_cooldown = 0;

uint8_t inks_available = INKS_PER_MINUTE;

#pragma DATA_SECTION (my_conf, ".infoA"); // A is .noinit
qc13conf my_conf = {0};

#pragma DATA_SECTION (backup_conf, ".infoC"); // C is .noinit
qc13conf backup_conf = {0};

#pragma PERSISTENT (badges_seen)
uint8_t badges_seen[BADGES_IN_SYSTEM] = {0};
#pragma PERSISTENT (odh_badges_ticks)
uint8_t odh_badges_ticks[HANDLER_MAX_INCLUSIVE+1] = {0};
#pragma PERSISTENT (uber_badges_ticks)
uint8_t uber_badges_ticks[UBER_COUNT] = {0};
#pragma PERSISTENT (badges_mated)
uint8_t badges_mated[BADGES_IN_SYSTEM] = {0};

uint64_t button_press_window = 0;
uint8_t buttons_pressed = 0;

uint16_t minutes_in_temp_band = 0;
uint16_t minutes_in_light_band = 0;

#pragma DATA_SECTION (default_conf, ".infoB"); // B is initialized on bootstrap
const qc13conf default_conf = {
        BADGE_ID,
        0, 0, 0, // seen counts
        0, 0, 0, // mate counts
        0, 0, 0, // hats!
        {0}, // event check-ins
        0, // camo_unlocks bitfield
        LEG_ANIM_DEF, // camo_id
        0, // Uber hat not given out
        0, // No achievements
        0, // Not gilded.
        0, // ink_margin
        0, 0, // ink_count, dink_count
        0, // power_cycles
        0 // rest is 0:
};

rfbcpayload in_payload, out_payload, cascade_payload;
uint8_t payload_cascade = 0;

uint8_t being_inked = 0;
uint8_t mated = 0;
uint8_t just_sent_superink = 0;
uint8_t seconds_to_next_face = 0;
uint8_t deferred_new_badges = 0;
uint8_t waking_up = 0;

void initial_animations() {
    face_set_ambient_direct(DEFAULT_EYES);
    tentacle_start_anim(LEG_ANIM_META_WAKEUP, LEG_CAMO_INDEX, 0, 0);
    tentacle_start_anim(my_conf.camo_id, LEG_CAMO_INDEX, 1, 1);
    face_start_anim(FACE_ANIM_META_WAKEUP);
    waking_up = 1;

    if (my_conf.gilded & GILD_ON)
        eye_twinkle_on();
    else
        eye_twinkle_off();
}

void blink_or_make_face() {
    if (seconds_to_next_face) {
        seconds_to_next_face--;
        return;
    }
    if (face_state != FACESTATE_AMBIENT)
        return;

    uint8_t to_blink = mate_state || ink_cooldown || (rand() % 5);
    uint8_t thing_to_do = 0;

    if (!to_blink) {
        thing_to_do = rand() % FACE_ANIM_COUNT;

        if (thing_to_do == FACE_ANIM_SAD && neighbor_count > 0) {
            thing_to_do = FACE_ANIM_CUTESY;
        } else if (thing_to_do == FACE_ANIM_CUTESY && neighbor_count == 0) {
            thing_to_do = FACE_ANIM_SAD;
        }

        if (thing_to_do == FACE_ANIM_FASTBLINKING || thing_to_do == FACE_ANIM_BLINKING)
            to_blink = 1;
        else
            face_start_anim(thing_to_do);
    }

    if (to_blink) {
        if (neighbor_count >= 10)
            blink_repeat_count = 3;
        else if (neighbor_count >= 3)
            blink_repeat_count = 2;
        else if (neighbor_count)
            blink_repeat_count = 1;
        else
            blink_repeat_count = 0;

        face_start_anim(blink_repeat_count? FACE_ANIM_FASTBLINKING : FACE_ANIM_BLINKING);
    }

    seconds_to_next_face = 255;
}

void second() {
    blink_or_make_face();
    tentacle_wiggle();

    if (mate_state == MS_PLUG) {
        mate_send_basic(0, 1, 0);
    }

    if (deferred_new_badges) {
        deferred_new_badges--;
        new_badge_seen(1);
    }

    do_light_step();
    do_brightness_correction(light_order, 0);

    if (ink_cooldown) {
        ink_cooldown--;
        if (!ink_cooldown) {
            // something something something
            face_restore_ambient();
            if (!(my_conf.gilded & GILD_ON))
                eye_twinkle_off();
        }
    }
}

void minute() {
    static uint8_t hour_mins = 0;
    inks_available = INKS_PER_MINUTE;
    minutes_in_light_band++;
    minutes_in_temp_band++;
    if (minutes_in_temp_band == 6) {
        if (temp_band == TEMP_HOT) {
            my_conf.been_hot = 1;
            my_conf_write_crc();
        } else if (temp_band == TEMP_COLD) {
            my_conf.been_cold = 1;
            my_conf_write_crc();
        }
        if (my_conf.been_cold && my_conf.been_hot) {
            make_eligible_for_pull_hat(HAT_HOT_COLD);
        }
    }

    if (minutes_in_temp_band == 200) {
        if (temp_band == TEMP_HOT) {
            make_eligible_for_pull_hat(HAT_HOT);
        } else if (temp_band == TEMP_COLD) {
            make_eligible_for_pull_hat(HAT_COLD);
            unlock_camo(LEG_ANIM_PUSHOVER);
        }
    }

    if (minutes_in_light_band == 60) {
        if (light_order == LIGHT_ORDER_MAX) {
            my_conf.been_bright = 1;
            my_conf_write_crc();
        } else if (light_order == 0) {
            my_conf.been_dark = 1;
            my_conf_write_crc();
        }
        if (my_conf.been_bright && my_conf.been_dark) {
            make_eligible_for_pull_hat(HAT_BRIGHT_DARK);
        }
    }

    if (my_conf.uptime < 10) {
        my_conf.uptime++;
        my_conf_write_crc();
    } else if (!my_conf.freeze_minuteman) {
        my_conf.achievements &= ~((uint64_t) 0x01 << HAT_MINUTEMAN);
        my_conf_write_crc();
    }

    hour_mins++;
    if (hour_mins == 60) {
        uint8_t neighbor_is_odh = 0;
        hour_mins = 0;
        for (uint8_t i=0; i<=HANDLER_MAX_INCLUSIVE; i++) {
            if (i == my_conf.badge_id) {
                continue;
            }
            if (neighbor_badges[i]) {
                neighbor_is_odh = 0;
                if (is_handler(i) && neighbors_on_duty[i])
                    neighbor_is_odh = 1;
                tick_badge_seen(i, neighbor_is_odh);
            }
        }
    }
}

void face_animation_done() {
    do_brightness_correction(light_order, 0);
    if (blink_repeat_count) {
        blink_repeat_count--;
        face_start_anim(FACE_ANIM_FASTBLINKING);
    } else {
        seconds_to_next_face = rand() % 5;
    }
}

// IMPORTANT: Call this last.
//  DON'T change stuff after calling it.
void complete_rfbc_payload(rfbcpayload *payload, uint8_t cascade_ttl) {
    payload->ttl = cascade_ttl;
    payload->seqnum = rfm75_seqnum;
    payload->base_addr = NOT_A_BASE;
    payload->badge_addr = my_conf.badge_id;
    if (is_handler(my_conf.badge_id) && (hat_state & HS_HANDLER))
        payload->flags |= RFBC_HANDLER_ON_DUTY;
    if (my_conf.hat_holder)
        payload->flags |= RFBC_HATHOLDER;

    // CRC it.
    CRC_setSeed(CRC_BASE, RFM75_CRC_SEED);
    for (uint8_t i = 0; i < sizeof(rfbcpayload) - 2; i++) {
        CRC_set8BitData(CRC_BASE, ((uint8_t *) payload)[i]);
    }
    payload->crc16 = CRC_getResult(CRC_BASE);
}

void send_ink() {
    if (ink_cooldown || !inks_available)
        return;
    inks_available--;
    ink_cooldown = INK_OUT_COOLDOWN_SECS;
    face_start_anim(FACE_ANIM_CUTESY);
    tentacle_send_meta_mating(2, 19);
    eye_twinkle_on();
    face_set_ambient_temp_direct(INKING_EYES);
    out_payload.ink_id = my_conf.camo_id;
    out_payload.flags = RFBC_INK;
    complete_rfbc_payload(&out_payload, 2);
    rfm75_tx();

    my_conf.ink_margin--;
    save_inks_and_check();
}

void send_super_ink() {
    out_payload.ink_id = my_conf.camo_id;
    out_payload.flags = RFBC_INK | RFBC_DINK;
    eye_twinkle_on();
    complete_rfbc_payload(&out_payload, 5);
    rfm75_tx();
}

void send_beacon() {
    out_payload.ink_id = LEG_ANIM_NONE;
    out_payload.flags = RFBC_BEACON;
    complete_rfbc_payload(&out_payload, 1);
    rfm75_tx();
}

void hat_change(uint8_t from, uint8_t to) {
    // This function MUST rely upon its parameters, not the global state,
    //  as the global state isn't settled yet when this function is called.
    // The borrowing event doesn't need to be fired from here.
    //  It's called elsewhere - after.

    if (is_handler(my_conf.badge_id) && (to & HS_HANDLER)) {
        // angry eyes.
        face_set_baseline_ambient_direct(ANGRY_EYES);
        // set the camo.
        unlock_camo(LEG_ANIM_HANDLER);
        tentacle_start_anim(LEG_ANIM_HANDLER, LEG_CAMO_INDEX, 1, 1);
    } else if (from & HS_HANDLER && !(to & HS_HANDLER)) {
        face_set_baseline_ambient_direct(DEFAULT_EYES);
        lock_camo(LEG_ANIM_HANDLER);
    }

    if (to & HS_UBER && is_uber(my_conf.badge_id)) {
        unlock_camo(LEG_ANIM_UBER);
    }

    if (to == HS_HUMAN && my_conf.hat_claimed && my_conf.hat_holder && my_conf.hat_id < HAT_UBER) {
        // I just put on my human hat!
        unlock_camo(LEG_ANIM_HUMAN_HAT);
    }
}

void temp_band_change(uint8_t from, uint8_t to) {
    minutes_in_temp_band = 0;
}

void light_band_change(uint8_t from, uint8_t to) {
    minutes_in_light_band = 0;
}


void start_button_longpressed() {
    if (being_inked || waking_up) return; // nope!

    if (mate_state == MS_IDLE && (my_conf.gilded & GILD_AVAIL)) {
        // gild on for ourselves
        my_conf.gilded ^= GILD_ON;
        my_conf_write_crc();
        if (my_conf.gilded & GILD_ON)
            eye_twinkle_on();
        else
            eye_twinkle_off();
    } else if (mate_state == MS_PAIRED && is_uber(my_conf.badge_id) && !my_conf.uber_hat_given) {
        mate_send_uber_hat_bestow();
    } else if (mate_state == MS_PAIRED && (is_gilder(my_conf.badge_id))) {
        mate_send_basic(0, 0, 1);
    } else if (mate_state == MS_PIPE_PAIRED && my_conf.hat_holder) {
        mate_send_flags(M_REPRINT_HAT);
    }
}

void select_button_longpressed() {
    radio_ink_received(my_conf.camo_id, LEG_INK_INDEX, my_conf.badge_id);

    ink_cooldown = INK_OUT_COOLDOWN_SECS;
    face_start_anim(FACE_ANIM_CUTESY);
    eye_twinkle_on();
    face_set_ambient_temp_direct(INKING_EYES);
}

void start_button_clicked() {
    button_press_window = button_press_window << 1;
    // start is 1.
    button_press_window |= 0x0000000000000001;
    if (buttons_pressed < 64) buttons_pressed++;
    check_button_presses();

    if (ink_cooldown) {
        tentacle_start_anim(LEG_ANIM_META_WAKEUP, 1, 0, 0); // blinky.
        return;
    }

    if (face_current_animation == FACE_ANIM_KONAMI && (face_curr_anim_frame == face_all_animations[FACE_ANIM_KONAMI]->len-1)) {
        // start was pressed at end of konami code.
        tentacle_start_anim(LEG_ANIM_GAMER, 2, 2, 0);
        unlock_camo(LEG_ANIM_GAMER);
        make_eligible_for_pull_hat(HAT_KONAMI);
        return;
    }

    if (being_inked || waking_up) return; // nope!

    switch (mate_state) {
    case MS_IDLE:
        send_ink();
        break;
    case MS_INK_WAIT:
        tentacle_send_meta_mating(0, 0);
        if (super_ink_waits_on_me) { // waiting on me:
            mate_send_basic(1,0,0);
            enter_super_inking();
        }
        // otherwise ignore it... we're waiting on the other badge.
        break;
    case MS_PAIRED:
        tentacle_send_meta_mating(0, 0);
        mate_send_basic(1,0,0);
        maybe_enter_ink_wait(1);
        break;
    default:
        // ignore it.
        __no_operation();
    }
}

void select_button_clicked() {
    button_press_window = button_press_window << 1;
    // select is 0.
    button_press_window &= ~0x0000000000000001;
    if (buttons_pressed < 64) buttons_pressed++;
    check_button_presses();

    if (being_inked || waking_up || mate_state == MS_SUPER_INK || ink_cooldown) return; // nope!

    static uint8_t new_camo = 0;

    new_camo = my_conf.camo_id;

    do {
        new_camo = (new_camo+1) % LEG_ANIM_COUNT;
    } while (!is_camo_avail(new_camo));

    my_conf.camo_id = new_camo;
    my_conf_write_crc();
    tentacle_start_anim(LEG_ANIM_META_WAKEUP, 1, 0, 0); // interrupt with a blinky.
    tentacle_start_anim(my_conf.camo_id, LEG_CAMO_INDEX, 1, 1);
}

void leg_anim_done(uint8_t tentacle_anim_id) {
    if (waking_up) waking_up = 0;

    being_inked = 0;
    if (mate_state == MS_SUPER_INK && just_sent_superink) {
        tentacle_send_meta_mating(2, 13); // 2 is the pewpew
        send_super_ink();
        just_sent_superink = 0;
    } else if (mate_state == MS_SUPER_INK) {
        // just finished the pewpew.
        // Stop twinkling if need be.
        if (!(my_conf.gilded & GILD_ON))
            eye_twinkle_off();
    }
}

void not_lonely() {
}

void new_badge_seen(uint8_t deferred) {
    // I think I skip this if I'm paired or being inked...
    if (mate_state || waking_up || being_inked || !tentacle_is_ambient) {
        if (!deferred) deferred_new_badges++;
        return;
    }

    do_brightness_correction(light_order+2, 1);
    tentacle_start_anim(LEG_ANIM_META_SOCIAL, 0, 1, 0);
}

void new_badge_mated() {
    // The animation is handled elsewhere.
}

void radio_beacon_interval() {
    uint8_t next_neighbor_count = 0;
    for (uint8_t i=0; i<BADGES_IN_SYSTEM; i++) {
        if (neighbor_badges[i]) {
            next_neighbor_count++;
            neighbor_badges[i]--;
        }
    }

    if (next_neighbor_count && !neighbor_count) {
        //  gone from alone to not alone.
        not_lonely();
    }

    neighbor_count = next_neighbor_count;

    // Now do our beacon:
    send_beacon();
}

void radio_beacon_received(uint8_t from_id, uint8_t on_duty) {
    if (neighbor_badges[from_id] == RECEIVE_WINDOW)
        return; // Already seen this time around.
    neighbor_badges[from_id] = RECEIVE_WINDOW;

    if (on_duty && is_handler(from_id)) {
        neighbors_on_duty[from_id] = 1;
    } else if (from_id <= HANDLER_MAX_INCLUSIVE) {
        neighbors_on_duty[from_id] = 0;
    }

    set_badge_seen(from_id, on_duty);
}

void radio_basic_base_received(uint8_t base_id) {
    // base_id is validated by our main routine.
    event_checkin(base_id);
}

void radio_ink_received(uint8_t ink_id, uint8_t ink_type, uint8_t from_addr) {
    if (being_inked || waking_up || ink_cooldown || mate_state != MS_IDLE)
        return; // we ignore inks if we're mated, or already being inked.
    being_inked = ink_type; // 1 for regular, 2 for double.
    if (being_inked == 1)
        do_brightness_correction(light_order+2, 1);
    else
        do_brightness_correction(light_order+8, 1);
    tentacle_start_anim(ink_id, ink_type, legs_all_anim_sets[ink_id][ink_type]->ink_loops, 0);
    face_start_anim(FACE_ANIM_META_INKED);
    if (ink_id >= LEG_ANIM_ZFLAG_BEAR && ink_id <= LEG_ANIM_ZFLAG_TRANS) {
        unlock_camo(ink_id);
    }
    if (from_addr != my_conf.badge_id) {
        // not a self-ink:
        my_conf.ink_margin--;
        save_inks_and_check();
    }

}

void radio_broadcast_received(rfbcpayload *payload) {
    // There are three possibilities for a broadcast.
    // They're NOT mutually exclusive.
    //     ==Broadcast==
    //  # Beacon: Just the normal gaydar beacon
    if (payload->flags & RFBC_BEACON) {
        radio_beacon_received(payload->badge_addr, payload->flags & RFBC_HANDLER_ON_DUTY);
    }
    //  # Basic event: A basic check-in broadcast from the base station
    if (payload->flags & RFBC_EVENT) {
        radio_basic_base_received(payload->base_addr);
    }
    //  # Ink or super ink
    if (payload->flags & RFBC_INK) {
        radio_ink_received(payload->ink_id, ((payload->flags & RFBC_DINK) ? 2 : 1), payload->badge_addr);
    }

    // # PUSH hat award:
    if (payload->flags & RFBC_HATOFFER) {
        award_push_hat(payload->ink_id);
        if (my_conf.hat_id == payload->ink_id && my_conf.hat_holder) {
            // reply with a HATACK.
            // Otherwise let it time out.
            out_payload.ink_id = my_conf.hat_id;
            out_payload.flags = RFBC_HATACK;
            complete_rfbc_payload(&out_payload, 3);
            rfm75_tx();
        }
    }
}

void radio_transmit_done() {
}

void mate_plug() {
    face_set_ambient_temp_direct(CLOSED_EYES);
}

void mate_start(uint8_t badge_id, uint8_t handler_on_duty) {
    face_set_ambient_temp_direct(GIGGITY_EYES);
    if (badges_mated[badge_id]) {
        // We've mated before
        tentacle_start_anim(LEG_ANIM_META_SOCIAL, 1, 0, 0);
    } else {
        tentacle_start_anim(LEG_ANIM_META_SOCIAL, 2, 0, 0);
    }
    set_badge_mated(badge_id, handler_on_duty);
}

void mate_end(uint8_t badge_id) {
    face_restore_ambient();
    mate_over_cleanup();
}

void borrowing_hat() {
    make_eligible_for_pull_hat(HAT_BORROWER);
    radio_ink_received(LEG_ANIM_SHUTDOWN, 2, my_conf.badge_id);
}
