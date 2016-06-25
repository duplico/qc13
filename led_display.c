/*
 * led_display.c
 *
 * Queercon 13 LED application logic
 *
 * (c) 2016 George Louthan
 * 3-clause BSD license; see license.md.
 */

#include "qc13.h"
#include "led_display.h"
#include "tlc5948a.h"

#include "etc/eyes/eye_anims.h"

const rgbcolor_t rainbow_colors[] = {
        {0xe400, 0x0300, 0x0300}, // Red
        {0xff00, 0x8c00, 0x0000}, // Orange
        {0xff00, 0xed00, 0x0000}, // Yellow
        {0x0000, 0x8000, 0x2600}, // Green
        {0x0000, 0x4d00, 0xff00}, // Blue
        {0x7500, 0x0700, 0x8700}, // Purple
};

const rgbcolor_t rainbow_legs[] = {
        {0xee00, 0x0100, 0x0100}, // Red
        {0xff00, 0x6c00, 0x0000}, // Orange
        {0xff00, 0xed00, 0x0000}, // Yellow
        {0xff00, 0xff00, 0x0000}, // Yellower
        {0, 0xff00, 0}, //Greener
        {0x0000, 0x8000, 0x2600}, // Green
        {0x0000, 0x4d00, 0xff00}, // Blue
        {0x7500, 0x0700, 0x8700}, // Purple
};

const rgbcolor_t legs_off[8] = {
        {0, 0, 0},
        {0, 0, 0},
        {0, 0, 0},
        {0, 0, 0},
        {0, 0, 0},
        {0, 0, 0},
        {0, 0, 0},
        {0, 0, 0},
};

rgbcolor_t* leg_colors_curr = rainbow_legs;

rgbcolor_t* leg_colors_next = legs_off;

rgbdelta_t leg_colors_step[8] = {
        {0, 0, 0},
        {0, 0, 0},
        {0, 0, 0},
        {0, 0, 0},
        {0, 0, 0},
        {0, 0, 0},
        {0, 0, 0},
        {0, 0, 0},
};

face_animation_t face_animations[0];

uint64_t face_curr;
uint64_t face_ambient = 0xffffffffffffffff;
uint8_t face_current_animation = FACE_ANIM_NONE;
uint8_t face_curr_anim_frame = 0;
uint16_t face_curr_dur = 0;
uint16_t ambient_brightness = 0x1f00;

uint16_t face_frame_dur = 0;
uint64_t curr_frame = 0;

uint8_t face_state = FACESTATE_AMBIENT;

#define FACE_DUR_STEP 3

void set_face(uint64_t frame) {
    for (uint8_t i=0; i<64; i++) {
        if (frame & ((uint64_t) 1 << i)) {
            tlc_bank_gs[i/16][i%16] = ambient_brightness;
        } else {
            tlc_bank_gs[i/16][i%16] = 0x00;
        }
    }
}

void set_tentacles(rgbcolor_t* leg_colors) {
    for (uint8_t tent=0; tent<8; tent++) {
        tlc_bank_gs[4+(tent/4)][4+((tent*3)%12)] = leg_colors[tent].red;
        tlc_bank_gs[4+(tent/4)][4+((tent*3)%12)+1] = leg_colors[tent].green;
        tlc_bank_gs[4+(tent/4)][4+((tent*3)%12)+2] = leg_colors[tent].blue;
    }
}

void face_set_ambient(uint8_t amb_index) {
    face_ambient = 0b1000010000100000111111111111111010000100001000001111111111111110;
    if (face_state == FACESTATE_AMBIENT) {
        set_face(face_ambient);
    }
}

void face_start_anim(uint8_t anim_index) {
    face_curr_anim_frame = 0;
    face_current_animation = anim_index;
    face_curr = face_all_animations[face_current_animation]->frames[0];
    face_curr_dur = face_all_animations[face_current_animation]->frame_durations[0];
    face_state = FACESTATE_ANIMATION;
}

void led_post() {
    uint64_t chase = 0;
    set_face(chase);
    tlc_stage_blank(0);
    tlc_set_fun();
    chase = 1;

    for (uint8_t t=4; t<6; t++) {
        for (uint8_t i=4; i<16; i++) {
            tlc_bank_gs[t][i] = ambient_brightness;
            delay_millis(45);
            tlc_bank_gs[t][i] = 0;
        }
    }

    for (uint8_t i=0; i<64; i++) {
        chase = chase << 1;
        set_face(chase);
        delay_millis(7);
    }

    uint16_t old_ab = ambient_brightness;
    for (ambient_brightness=0x1000; ambient_brightness;ambient_brightness-=32) {
        set_face(0xffffffffffffffff);
        for (uint8_t t=4; t<6; t++) {
            for (uint8_t i=4; i<16; i++) {
                tlc_bank_gs[t][i] = ambient_brightness;
            }
        }
    }
    ambient_brightness = old_ab;
    for (uint8_t t=4; t<6; t++) {
        for (uint8_t i=4; i<16; i++) {
            tlc_bank_gs[t][i] = 0;
        }
    }

}

void leds_timestep() {
    // Face:
    //  Check whether we need to change the brightness because of:
    //  * twinkle
    //  * ambient light level change
    //  Check whether we need to change the face.
    //
    // If either, make it happen, captain.

    if (face_state == FACESTATE_ANIMATION) {
        if (face_curr_dur < FACE_DUR_STEP) { // Time for next frame?
            face_curr_anim_frame++;
            if (face_curr_anim_frame == face_all_animations[face_current_animation]->len) { // done?
                face_state = FACESTATE_AMBIENT;
                face_curr = face_ambient;
                // done
            } else {
                face_curr = face_all_animations[face_current_animation]->frames[face_curr_anim_frame];
                face_curr_dur = face_all_animations[face_current_animation]->frame_durations[face_curr_anim_frame];
                set_face(face_curr);
            }
        } else {
            // tick towards next frame.
            face_curr_dur -= FACE_DUR_STEP;
            // TODO: Check to see if we need to change anything.
        }
    }

    if (face_state == FACESTATE_AMBIENT) {
        // TODO: Check to see if we need to change anything.
        set_face(face_ambient);
    }


    // Tentacles:
    //  Apply our current delta animation timestep.
    // TODO: Check to see if we need to change anything.
    set_tentacles(leg_colors_curr);


//    // For now:
//    for (uint8_t bank=4; bank<6; bank++) {
//        for (uint8_t channel=0; channel<12; channel++) {
//            tlc_bank_gs[bank][channel+4] = 0x0f00;
//        }
//    }

}
