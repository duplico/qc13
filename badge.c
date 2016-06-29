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

uint8_t badges_seen[BADGES_IN_SYSTEM] = {0};
uint8_t neighbor_badges[BADGES_IN_SYSTEM] = {0};
uint8_t neighbor_count = 0;

qc13conf my_conf = {0};

const qc13conf default_conf = {
        BADGE_ID,
        0,
        0,
        0,
        0,
        0,
        0x01,
        1,
        0
};

qcpayload in_payload, out_payload;

uint8_t being_inked = 0;
uint8_t mated = 0;

uint8_t seconds_to_next_thing = 0;

void initial_animations() {
    face_set_ambient_direct(0b1000010000100000111111111111111010000100001000001111111111111110);
    tentacle_start_anim(my_conf.camo_id, LEG_CAMO_INDEX, 1, 1);
}

void second() {
    if (!seconds_to_next_thing) {
        if (face_state == FACESTATE_AMBIENT)
        face_start_anim(FACE_ANIM_BLINKING);
        seconds_to_next_thing = rand() % 6;
    } else {
        seconds_to_next_thing--;
    }
}


void time_loop() {
    static uint16_t second_loops = LOOPS_PER_SECOND;
    if (second_loops) {
        second_loops--;
    } else {
        second_loops = LOOPS_PER_SECOND;
        second();
    }
}

void start_button_clicked() {
    if (being_inked) return; // nope!
    out_payload.ink_id = my_conf.camo_id;
    out_payload.ink_type = LEG_INK_INDEX;
    rfm75_tx();
}

void select_button_clicked() {
    if ((my_conf.camo_id+1) % LEG_ANIM_COUNT == my_conf.camo_id) {
        // not allowed to change camo
    } else {
        my_conf.camo_id = (my_conf.camo_id+1) % LEG_ANIM_COUNT;
        tentacle_start_anim(my_conf.camo_id, LEG_CAMO_INDEX, 1, 1);
    }
}

void leg_anim_done(uint8_t tentacle_anim_id) {
    being_inked = 0;
}

void ink_received(uint8_t ink_id, uint8_t ink_type, uint8_t from_addr) {
    being_inked = 1;
    face_start_anim(3);
    tentacle_start_anim(ink_id, ink_type, 3, 0);
}

void radio_received(qcpayload *payload) {
    if (!being_inked && payload->ink_id != LEG_ANIM_NONE) { // it's an ink!
        ink_received(payload->ink_id, payload->ink_type, payload->from_addr);
    }
}

void radio_transmit_done() {
    face_start_anim(4);
    tentacle_start_anim(my_conf.camo_id, LEG_DOUBLEINK_INDEX, 0, 0);
}

uint64_t mate_old_ambient = 0b1000010000100000111111111111111010000100001000001111111111111110;

void mate_start(uint8_t badge_id) {
    mate_old_ambient = face_ambient;
    face_set_ambient_direct(0b1000000000000000111100000001111010000000000000001111000000011110);
}

void mate_end(uint8_t badge_id) {
    face_set_ambient(0);
}
