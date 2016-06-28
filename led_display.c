/*
 * led_display.c
 *
 * Queercon 13 LED application logic
 *
 * (c) 2016 George Louthan
 * 3-clause BSD license; see license.md.
 */

#include <stdlib.h>

#include "qc13.h"
#include "led_display.h"
#include "tlc5948a.h"

#include "etc/eyes/eye_anims.h"
#include "etc/tentacles/leg_anims.h"

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

rgbcolor_t leg_colors_curr[8] = {
        {0xee00, 0x0100, 0x0100}, // Red
        {0xff00, 0x6c00, 0x0000}, // Orange
        {0xff00, 0xed00, 0x0000}, // Yellow
        {0xff00, 0xff00, 0x0000}, // Yellower
        {0, 0xff00, 0}, //Greener
        {0x0000, 0x8000, 0x2600}, // Green
        {0x0000, 0x4d00, 0xff00}, // Blue
        {0x7500, 0x0700, 0x8700}, // Purple
};

rgbcolor_t leg_colors_next[8] = {
        {0, 0, 0},
        {0, 0, 0},
        {0, 0, 0},
        {0, 0, 0},
        {0, 0, 0},
        {0, 0, 0},
        {0, 0, 0},
        {0, 0, 0},
};

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
uint16_t face_ambient_brightness = 0x1f00;

uint16_t face_frame_dur = 0;
uint64_t curr_frame = 0;

uint8_t face_state = FACESTATE_AMBIENT;

#define FACE_DUR_STEP 3

uint8_t tentacle_first_frame = 0;
uint8_t tentacle_anim_index = 0;
uint8_t tentacle_is_ambient = 1;
uint8_t tentacle_anim_looping = 0;
uint8_t tentacle_anim_length = 0;
uint16_t tentacle_hold_steps = 0;
uint16_t tentacle_hold_index = 0;
uint16_t tentacle_transition_steps = 0;
uint16_t tentacle_transition_index = 0;
rgbcolor_t (*tentacle_curr_frames)[8]; // pointer to arrays of colors
const tentacle_animation_t *tentacle_current_anim;

void set_face(uint64_t frame) {
    for (uint8_t i=0; i<64; i++) {
        if (frame & ((uint64_t) 1 << i)) {
            tlc_bank_gs[i/16][i%16] = face_ambient_brightness;
        } else {
            tlc_bank_gs[i/16][i%16] = 0x00;
        }
    }
}

void stage_color(rgbcolor_t *dest_color_frame, rgbcolor_t *src_color_frame) {
    memcpy(dest_color_frame, src_color_frame, sizeof(rgbcolor_t));
}

// Each time we hit a new frame, we hit this.
// If we're fading, it will set up the fades.
void leg_load_colors() {
    // leg_colors_curr <- tentacle_current_anim[tentacle_anim_index]

    // Stage in the current color:
    // TODO:
//    memcpy(&leg_colors_curr[0], &tentacle_curr_frames[tentacle_anim_index][0], sizeof(rgbcolor_t)*8);
    for (uint8_t i=0; i<8; i++) {
        leg_colors_curr[i].red = tentacle_current_anim->colors[tentacle_anim_index][i].red;
        leg_colors_curr[i].green = tentacle_current_anim->colors[tentacle_anim_index][i].green;
        leg_colors_curr[i].blue = tentacle_current_anim->colors[tentacle_anim_index][i].blue;

        // Stage in next color.
        // If we're looping, it's modded. If not looping, back to black.
        if (tentacle_anim_index == tentacle_current_anim->len-1 && !tentacle_anim_looping) { // last frame:
            // We're at the last frame, and we are NOT looping. So our NEXT
            // color will be OFF.
            stage_color(&leg_colors_next[i], &legs_off[i]);
        } else {
            // We're either looping or not at the end, so it's safe to say:
//            stage_color(&leg_colors_next[i], tentacle_current_anim->colors[(tentacle_anim_index+1) % tentacle_current_anim->len]);
            uint8_t next_id = (tentacle_anim_index+1) % tentacle_current_anim->len;
            leg_colors_next[i].red = tentacle_current_anim->colors[next_id][i].red;
            leg_colors_next[i].green = tentacle_current_anim->colors[next_id][i].green;
            leg_colors_next[i].blue = tentacle_current_anim->colors[next_id][i].blue;
        }

        leg_colors_step[i].red = ((int_fast32_t) leg_colors_next[i].red - leg_colors_curr[i].red) / tentacle_transition_steps;
        leg_colors_step[i].green = ((int_fast32_t) leg_colors_next[i].green - leg_colors_curr[i].green) / tentacle_transition_steps;
        leg_colors_step[i].blue = ((int_fast32_t) leg_colors_next[i].blue - leg_colors_curr[i].blue) / tentacle_transition_steps;
    }
}

inline void leg_fade_colors() {
    // If this is the very last transition step,
    if (tentacle_transition_steps && tentacle_transition_index == tentacle_transition_steps-1) {
        // hit the destination:
        for (uint8_t i=0; i<8; i++) {
            memcpy(&leg_colors_curr[i], &leg_colors_next[i], sizeof(rgbcolor_t)); // TODO: contiguous
        }
    } else {
        for (uint8_t i=0; i<8; i++) {
            leg_colors_curr[i].red += leg_colors_step[i].red;
            leg_colors_curr[i].green += leg_colors_step[i].green;
            leg_colors_curr[i].blue += leg_colors_step[i].blue;
        }
    }
}

uint8_t twinkle_bits = 0xea;
uint16_t anim_adj_index = 0;

// This actually sets the colors of the tentacles/legs.
// Here is where we are going to do some fancy stuff:
//  1. brightness correction (ambient)
//  2. twinkling
void set_tentacles(rgbcolor_t* leg_colors) {
    static uint_fast32_t r = 0;
    static uint_fast32_t g = 0;
    static uint_fast32_t b = 0;

    for (uint8_t tent=0; tent<8; tent++) {
        r = leg_colors[tent].red;
        g = leg_colors[tent].green;
        b = leg_colors[tent].blue;

        // TODO: brightness correction.

        // Twinklies:
        switch(tentacle_current_anim->anim_type) {
        case ANIM_TYPE_FAST_TWINKLE:
            if (twinkle_bits & (1 << tent)) {
                r = r << 1;
                g = g << 1;
                b = b << 1;
            }
            break;
        default:
            break;
        }

        if (tent > 3) {
            r = r >> 2;
            g = g >> 2;
            b = b >> 2;
        }

        if (r>UINT16_MAX) r=UINT16_MAX;
        if (g>UINT16_MAX) b=UINT16_MAX;
        if (g>UINT16_MAX) b=UINT16_MAX;

        tlc_bank_gs[4+(tent/4)][4+((tent*3)%12)] = b;
        tlc_bank_gs[4+(tent/4)][4+((tent*3)%12)+1] = g;
        tlc_bank_gs[4+(tent/4)][4+((tent*3)%12)+2] = r;
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

uint8_t tentacle_animation_state = 0;

void tentacle_start_anim(uint8_t anim_id, uint8_t anim_type, uint8_t loop, uint8_t ambient) {
    tentacle_current_anim = legs_all_anim_sets[anim_id][anim_type];
    f_tentacle_anim_done = 0;
    tentacle_first_frame = 1;
    tentacle_anim_index = 0; // This is our index in the animation.
    tentacle_animation_state = 1; // animating
    anim_adj_index = 0;

    tentacle_is_ambient = ambient;
    // TODO: If not ambient, remember what IS ambient so we can go back.

    tentacle_anim_length = tentacle_current_anim->len;
    tentacle_curr_frames = (rgbcolor_t (*)[8]) tentacle_current_anim->colors;

    tentacle_transition_steps = tentacle_current_anim->fade_durs[0] / FACE_DUR_STEP; // TODO: divided by something probably.
    tentacle_transition_index = 0;
    tentacle_hold_steps = tentacle_current_anim->durations[0] / FACE_DUR_STEP; // TODO: divided by something probably.
    tentacle_hold_index = 0;

    tentacle_anim_looping = loop;
    leg_load_colors();
    set_tentacles(tentacle_curr_frames[tentacle_anim_index]);
}

void tentacle_next_anim_frame() {
    f_tentacle_anim_done = 0;
    tentacle_first_frame = 0;
    tentacle_anim_index++; // This is our index in the animation.

    if (tentacle_anim_index >= tentacle_anim_length) {
        // TODO: check loop
        // tentacle_animation_state = 0; // stopped
        // flag done
        // TODO: If not ambient, remember what IS ambient so we can go back.
        tentacle_anim_index = 0; // loop
    }

    tentacle_hold_steps = tentacle_current_anim->durations[tentacle_anim_index] / FACE_DUR_STEP;
    tentacle_hold_index = 0;
    tentacle_transition_steps = tentacle_current_anim->fade_durs[tentacle_anim_index] / FACE_DUR_STEP;
    tentacle_transition_index = 0;

    leg_load_colors();
    set_tentacles(tentacle_curr_frames[tentacle_anim_index]);
}

void led_post() {
    uint64_t chase = 0;
    set_face(chase);
    tlc_stage_blank(0);
    tlc_set_fun();
    chase = 1;

//    for (uint8_t t=4; t<6; t++) {
//        for (uint8_t i=4; i<16; i++) {
//            tlc_bank_gs[t][i] = ambient_brightness;
//            delay_millis(45);
//            tlc_bank_gs[t][i] = 0;
//        }
//    }
//
//    for (uint8_t i=0; i<64; i++) {
//        chase = chase << 1;
//        set_face(chase);
//        delay_millis(7);
//    }

    uint16_t old_ab = face_ambient_brightness;
    for (face_ambient_brightness=0x1000; face_ambient_brightness;face_ambient_brightness-=32) {
        set_face(0xffffffffffffffff);
        for (uint8_t t=4; t<6; t++) {
            for (uint8_t i=4; i<16; i++) {
                tlc_bank_gs[t][i] = face_ambient_brightness;
            }
        }
    }
    face_ambient_brightness = old_ab;
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
    switch(tentacle_current_anim->anim_type) {
    case ANIM_TYPE_FAST_TWINKLE:
        anim_adj_index++;
        if (anim_adj_index == 25) {
            twinkle_bits = rand() % 256;
            anim_adj_index = 0;
            // TODO: dirty bit
        }
        break;
    default:
        // SOLID
        break;
    }

    if (tentacle_animation_state) {
        if (tentacle_hold_index < tentacle_hold_steps) {
            // Hold the same color.
            tentacle_hold_index++;
        } else {
            // We're transitioning:
            tentacle_transition_index++;
            if (tentacle_transition_index >= tentacle_transition_steps) {
                // next frame plx.
                tentacle_next_anim_frame();
            } else {
                leg_fade_colors();
            }
        }
    }
    // TODO: if dirty
    set_tentacles(leg_colors_curr);

}
