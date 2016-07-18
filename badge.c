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

uint8_t badges_seen[BADGES_IN_SYSTEM] = {0};
uint8_t neighbor_badges[BADGES_IN_SYSTEM] = {0};
uint8_t neighbor_count = 0;

uint8_t blink_repeat_count = 0;

qc13conf my_conf = {0};

const qc13conf default_conf = {
        BADGE_ID,
        0, 0, 0, // seen counts
        0, 0, 0, // mate counts
        0, // event check-ins
        0xffffffff, // camo_unlocks bitfield // TODO: should be 0x1
        LEG_ANIM_DEF, // camo_id
        0 // blank CRC.
};

rfbcpayload in_payload, out_payload;

uint8_t being_inked = 0;
uint8_t mated = 0;

uint8_t seconds_to_next_thing = 0;

void initial_animations() {
    face_set_ambient_direct(0b1000010000100000111111111111111010000100001000001111111111111110);
    tentacle_start_anim(my_conf.camo_id, LEG_CAMO_INDEX, 1, 1);
}

void second() {
    if (!seconds_to_next_thing) {
        if (face_state == FACESTATE_AMBIENT) {
            uint8_t to_blink = rand() % 5;
            uint8_t thing_to_do = 0;

            if (!to_blink) {
                thing_to_do = rand() % FACE_ANIM_COUNT;
                if (thing_to_do == FACE_ANIM_FASTBLINKING || thing_to_do == FACE_ANIM_BLINKING)
                    to_blink = 1;
                else
                    face_start_anim(thing_to_do);
            }

            if (to_blink) {
                if (neighbor_count >= 10)
                    blink_repeat_count = 3;
                else if (neighbor_count >= 5)
                    blink_repeat_count = 2;
                else if (neighbor_count)
                    blink_repeat_count = 1;
                else
                    blink_repeat_count = 0;

                face_start_anim(blink_repeat_count? FACE_ANIM_FASTBLINKING : FACE_ANIM_BLINKING);
            }
        }
        seconds_to_next_thing = 255;
    } else {
        seconds_to_next_thing--;
    }

    if (mate_state == MS_PLUG) {
        mate_send_basic(0, 1);
    }

    do_brightness_correction();
}

void two_seconds() {
    uint8_t wiggle_mask_temp = 0xff;
    if (tentacle_current_anim->wiggle) {
        wiggle_mask_temp &= ~(1 << (rand() % 4));
        if (rand() % 2) wiggle_mask_temp &= ~(1 << (rand() % 4));
    }
    wiggle_mask = wiggle_mask_temp;
}

void face_animation_done() {
    if (blink_repeat_count) {
        blink_repeat_count--;
        face_start_anim(FACE_ANIM_FASTBLINKING);
    } else {
        seconds_to_next_thing = rand() % 5; // TODO SSOT
    }
}

// TODO: move to main.
void time_loop() {
    static uint8_t interval_seconds_remaining = BEACON_INTERVAL_SECS;
    static uint16_t second_loops = LOOPS_PER_SECOND;
    static uint8_t loops = 0;
    if (second_loops) {
        second_loops--;
    } else {
        loops += 1;
        if (loops & 0x01)
            two_seconds();
        second_loops = LOOPS_PER_SECOND;
        second();
        if (interval_seconds_remaining) {
            interval_seconds_remaining--;
        } else {
            radio_beacon_interval();
            interval_seconds_remaining = BEACON_INTERVAL_SECS;
        }
    }
}

// IMPORTANT: Call this last.
//  DON'T change stuff after calling it.
void complete_rfbc_payload(rfbcpayload *payload) {
    payload->base_addr = NOT_A_BASE;
    payload->from_addr = my_conf.badge_id;
    // TODO: if handler and wearing a hat:
//    payload->flags |= RFBC_HANDLER_ON_DUTY;
    // TODO: if hatholder:
//    payload->flags |= RFBC_HATHOLDER
    // TODO: if hat on:
//    payload->flags |= RFBC_HAT_ON
    // TODO: if eligible for a push hat:
//    payload.flags |= RFBC_PUSH_HAT_ELIGIBLE

    // CRC it.
    CRC_setSeed(CRC_BASE, RFM75_CRC_SEED);
    for (uint8_t i = 0; i < sizeof(rfbcpayload) - 2; i++) {
        CRC_set8BitData(CRC_BASE, ((uint8_t *) payload)[i]);
    }
    payload->crc16 = CRC_getResult(CRC_BASE);
}

void send_ink() {
    out_payload.ink_id = my_conf.camo_id;
    out_payload.flags = RFBC_INK;
    complete_rfbc_payload(&out_payload);
    rfm75_tx();
}

void send_super_ink() {
    out_payload.ink_id = my_conf.camo_id;
    out_payload.flags = RFBC_INK | RFBC_DINK;
    complete_rfbc_payload(&out_payload);
    rfm75_tx();
    tentacle_start_anim(LEG_ANIM_HANDLER, 0, 0, 0);
}

void send_beacon() {
    out_payload.ink_id = LEG_ANIM_NONE;
    out_payload.flags = RFBC_BEACON;
    complete_rfbc_payload(&out_payload);
    rfm75_tx();
}

void start_button_clicked() {
    if (being_inked) return; // nope!

    switch (mate_state) {
    case MS_IDLE:
        send_ink();
        break;
    case MS_INK_WAIT:
        if (super_ink_waits_on_me) { // waiting on me:
            mate_send_basic(1,0);
            enter_super_inking();
        }
        // otherwise ignore it... we're waiting on the other badge.
        break;
    case MS_PAIRED:
        mate_send_basic(1,0);
        maybe_enter_ink_wait(1);
        break;
    default:
        // ignore it.
        __no_operation();
    }

    // TODO: Remove when done.
    // The testing code to cycle through faces:
//    static uint8_t face_anim_no = 0;
//    face_anim_no = (face_anim_no+1) % FACE_ANIM_COUNT;
//    face_start_anim(face_anim_no);
}

void select_button_clicked() {
    if (being_inked) return; // nope!

    if ((my_conf.camo_id+1) % LEG_ANIM_COUNT == my_conf.camo_id) {
        // not allowed to change camo
    } else {
        my_conf.camo_id = (my_conf.camo_id+1) % LEG_ANIM_COUNT;
        tentacle_start_anim(my_conf.camo_id, LEG_CAMO_INDEX, 1, 1);
    }

    // The testing code to cycle through faces:
//    face_start_anim(face_anim_no);
}

void leg_anim_done(uint8_t tentacle_anim_id) {
    being_inked = 0;
}

void not_lonely() {
}

void new_badge() {
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
    neighbor_badges[from_id] = RECEIVE_WINDOW;
    set_badge_seen(from_id, on_duty);
    tick_badge_seen(from_id, on_duty);
}

void radio_basic_base_received(uint8_t base_id) {

}

void radio_ink_received(uint8_t ink_id, uint8_t ink_type, uint8_t from_addr) {
    if (mate_state != MS_IDLE)
        return; // we ignore inks if we're mated.
    being_inked = 1;
    tentacle_start_anim(ink_id, ink_type, 3, 0);
}

void radio_broadcast_received(rfbcpayload *payload) {
    // There are three possibilities for a broadcast.
    // They're NOT mutually exclusive.
    //     ==Broadcast==
    //  # Beacon: Just the normal gaydar beacon
    if (payload->flags & RFBC_BEACON) {
        radio_beacon_received(payload->from_addr, payload->flags & RFBC_HANDLER_ON_DUTY);
    }
    //  # Basic event: A basic check-in broadcast from the base station
    if (payload->flags & RFBC_EVENT) {
        radio_basic_base_received(payload->base_addr);
    }
    //  # Ink or super ink
    if (payload->flags & RFBC_INK) {
        radio_ink_received(payload->ink_id, ((payload->flags & RFBC_DINK) ? 2 : 1), payload->from_addr);
    }
}

void radio_transmit_done() {
}

uint64_t mate_old_ambient = 0b1000010000100000111111111111111010000100001000001111111111111110;

void mate_start(uint8_t badge_id) {
    mate_old_ambient = face_ambient;
    face_set_ambient_direct(0b1000000000000000111100000001111010000000000000001111000000011110);
}

void mate_end(uint8_t badge_id) {
    face_set_ambient_direct(mate_old_ambient);
    tentacle_start_anim(0, 1, 3, 0);
    mate_over_cleanup();
}
