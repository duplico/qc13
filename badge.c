/*
 * badge.c
 *
 *  Created on: Jun 27, 2016
 *      Author: George
 */

#include "qc13.h"
#include "leg_anims.h"

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

void initial_animations() {
    face_set_ambient(0);
    tentacle_start_anim(my_conf.camo_id, LEG_CAMO_INDEX, 1, 1);
}

void time_loop() {

}

void start_button_clicked() {
    tentacle_start_anim(my_conf.camo_id, LEG_DOUBLEINK_INDEX, 0, 0);
}

void select_button_clicked() {
    if ((my_conf.camo_id+1) % LEG_ANIM_COUNT == my_conf.camo_id) {
        // not allowed to change camo
    } else {
        my_conf.camo_id = (my_conf.camo_id+1) % LEG_ANIM_COUNT;
        tentacle_start_anim(my_conf.camo_id, LEG_CAMO_INDEX, 1, 1);
    }
}
