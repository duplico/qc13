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
#include "badge.h"

#include "eye_anims.h"
#include "leg_anims.h"

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

///////////////// leg_meta_mate:
// frames for camo
rgbcolor_t leg_meta_mate_camo_frames[][8] = {
    {{0x0, 0x0, 0x0}, {0x0, 0x0, 0x0}, {0x0, 0x0, 0x0}, {0x0, 0x0, 0x0}, {0x0, 0x0, 0x0}, {0x0, 0x0, 0x0}, {0x0, 0x0, 0x0}, {0x0, 0x0, 0x0}},
    {{0x0, 0x0, 0x0}, {0x0, 0x0, 0x0}, {0x0, 0x0, 0x0}, {0x0, 0x0, 0x0}, {0x0, 0x0, 0x0}, {0x0, 0x0, 0x0}, {0x78, 0x2d0, 0x216}, {0x0, 0x0, 0x0}},
    {{0x0, 0x0, 0x0}, {0x0, 0x0, 0x0}, {0x78, 0x2d0, 0x216}, {0x0, 0x0, 0x0}, {0x0, 0x0, 0x0}, {0x0, 0x0, 0x0}, {0x78, 0x2d0, 0x216}, {0x0, 0x0, 0x0}},
    {{0x0, 0x0, 0x0}, {0x0, 0x0, 0x0}, {0x78, 0x2d0, 0x216}, {0x0, 0x0, 0x0}, {0x0, 0x0, 0x0}, {0x0, 0x0, 0x0}, {0x0, 0x0, 0x0}, {0x0, 0x0, 0x0}},
    {{0x0, 0x0, 0x0}, {0x0, 0x0, 0x0}, {0x0, 0x0, 0x0}, {0x0, 0x0, 0x0}, {0x0, 0x0, 0x0}, {0x0, 0x0, 0x0}, {0x0, 0x0, 0x0}, {0x0, 0x0, 0x0}},
    {{0x0, 0x0, 0x0}, {0x0, 0x0, 0x0}, {0x0, 0x0, 0x0}, {0x0, 0x0, 0x0}, {0x0, 0x0, 0x0}, {0x0, 0x0, 0x0}, {0x0, 0x0, 0x0}, {0x0, 0x0, 0x0}},
    {{0x0, 0x0, 0x0}, {0x0, 0x0, 0x0}, {0x0, 0x0, 0x0}, {0x0, 0x0, 0x0}, {0x0, 0x0, 0x0}, {0x0, 0x0, 0x0}, {0x0, 0x0, 0x0}, {0x0, 0x0, 0x0}},
};
uint16_t leg_meta_mate_camo_durations[] = {50, 100, 100, 100, 100, 100, 100};
uint16_t leg_meta_mate_camo_fade_durs[] = {100, 100, 100, 100, 100, 100, 100};
// the animation:
const tentacle_animation_t leg_meta_mate_camo = {(const rgbcolor_t (*)[8])leg_meta_mate_camo_frames, leg_meta_mate_camo_durations, leg_meta_mate_camo_fade_durs, 7, ANIM_TYPE_SOLID, 0, 4};
// frames for ink
rgbcolor_t leg_meta_mate_ink_frames[][8] = {
    {{0x0, 0x0, 0x0}, {0x0, 0x0, 0x0}, {0x0, 0x0, 0x0}, {0x0, 0x0, 0x0}, {0x0, 0x0, 0x0}, {0x0, 0x0, 0x0}, {0x0, 0x0, 0x0}, {0x0, 0x0, 0x0}},
    {{0x0, 0x0, 0x0}, {0x0, 0x0, 0x0}, {0x0, 0x0, 0x0}, {0x0, 0x0, 0x0}, {0x0, 0x0, 0x0}, {0x0, 0x0, 0x0}, {0x0, 0x0, 0x0}, {0x0, 0x0, 0x0}},
    {{0x0, 0x0, 0x0}, {0x0, 0x0, 0x0}, {0x0, 0x0, 0x0}, {0x0, 0x0, 0x0}, {0x0, 0x0, 0x0}, {0x0, 0x0, 0x0}, {0x0, 0x0, 0x0}, {0x0, 0x0, 0x0}},
    {{0x0, 0x0, 0x0}, {0x78, 0x2d0, 0x216}, {0x0, 0x0, 0x0}, {0x0, 0x0, 0x0}, {0x0, 0x0, 0x0}, {0x0, 0x0, 0x0}, {0x0, 0x0, 0x0}, {0x0, 0x0, 0x0}},
    {{0x0, 0x0, 0x0}, {0x78, 0x2d0, 0x216}, {0x0, 0x0, 0x0}, {0x0, 0x0, 0x0}, {0x0, 0x0, 0x0}, {0x78, 0x2d0, 0x216}, {0x0, 0x0, 0x0}, {0x0, 0x0, 0x0}},
    {{0x0, 0x0, 0x0}, {0x0, 0x0, 0x0}, {0x0, 0x0, 0x0}, {0x0, 0x0, 0x0}, {0x0, 0x0, 0x0}, {0x78, 0x2d0, 0x216}, {0x0, 0x0, 0x0}, {0x0, 0x0, 0x0}},
    {{0x0, 0x0, 0x0}, {0x0, 0x0, 0x0}, {0x0, 0x0, 0x0}, {0x0, 0x0, 0x0}, {0x0, 0x0, 0x0}, {0x0, 0x0, 0x0}, {0x0, 0x0, 0x0}, {0x0, 0x0, 0x0}},
};
uint16_t leg_meta_mate_ink_durations[] = {50, 100, 100, 100, 100, 100, 100};
uint16_t leg_meta_mate_ink_fade_durs[] = {100, 100, 100, 100, 100, 100, 100};
// the animation:
const tentacle_animation_t leg_meta_mate_ink = {(const rgbcolor_t (*)[8])leg_meta_mate_ink_frames, leg_meta_mate_ink_durations, leg_meta_mate_ink_fade_durs, 7, ANIM_TYPE_SOLID, 0, 4};
// frames for doubleink
rgbcolor_t leg_meta_mate_doubleink_frames[][8] = {
    {{0x0, 0x0, 0x0}, {0x0, 0x0, 0x0}, {0x0, 0x0, 0x0}, {0x0, 0x0, 0x0}, {0x0, 0x0, 0x0}, {0x0, 0x0, 0x0}, {0x0, 0x0, 0x0}, {0x0, 0x0, 0x0}},
    {{0x0, 0x0, 0x0}, {0x0, 0x0, 0x0}, {0x0, 0x0, 0x0}, {0x0, 0x0, 0x0}, {0x78, 0x2d0, 0x216}, {0x0, 0x0, 0x0}, {0x0, 0x0, 0x0}, {0x78, 0x2d0, 0x216}},
    {{0x78, 0x2d0, 0x216}, {0x0, 0x0, 0x0}, {0x0, 0x0, 0x0}, {0x78, 0x2d0, 0x216}, {0x78, 0x2d0, 0x216}, {0x0, 0x0, 0x0}, {0x0, 0x0, 0x0}, {0x78, 0x2d0, 0x216}},
    {{0x78, 0x2d0, 0x216}, {0x0, 0x0, 0x0}, {0x0, 0x0, 0x0}, {0x78, 0x2d0, 0x216}, {0x0, 0x0, 0x0}, {0x0, 0x0, 0x0}, {0x0, 0x0, 0x0}, {0x0, 0x0, 0x0}},
    {{0x0, 0x0, 0x0}, {0x0, 0x0, 0x0}, {0x0, 0x0, 0x0}, {0x0, 0x0, 0x0}, {0x0, 0x0, 0x0}, {0x0, 0x0, 0x0}, {0x0, 0x0, 0x0}, {0x0, 0x0, 0x0}},
};
uint16_t leg_meta_mate_doubleink_durations[] = {50, 50, 50, 50, 0};
uint16_t leg_meta_mate_doubleink_fade_durs[] = {50, 50, 50, 50, 50};
// the animation:
const tentacle_animation_t leg_meta_mate_doubleink = {(const rgbcolor_t (*)[8])leg_meta_mate_doubleink_frames, leg_meta_mate_doubleink_durations, leg_meta_mate_doubleink_fade_durs, 5, ANIM_TYPE_SOLID, 0, 13};

const tentacle_animation_t *leg_meta_mate_anim_set[3] = {&leg_meta_mate_camo, &leg_meta_mate_ink, &leg_meta_mate_doubleink};



face_animation_t face_animations[0];

uint64_t face_curr;
uint64_t face_ambient = 0xffffffffffffffff;
uint8_t face_current_animation = FACE_ANIM_NONE;
uint8_t face_curr_anim_frame = 0;
uint16_t face_curr_dur = 0;
uint32_t face_ambient_brightness = FACE_DIM_BRIGHTNESS;

uint16_t face_frame_dur = 0;
uint64_t curr_frame = 0;

uint8_t face_state = FACESTATE_AMBIENT;

#define FACE_DUR_STEP 2
#define LEGS_DUR_STEP 2

uint8_t tentacle_anim_frame = 0;
uint8_t tentacle_anim_id = 0;
uint8_t tentacle_anim_type = 0;
uint8_t tentacle_saved_anim_id = 0;
uint8_t tentacle_saved_anim_type = 0;
uint8_t tentacle_is_ambient = 1;
uint8_t tentacle_anim_looping = 0;
uint8_t tentacle_anim_length = 0;
uint16_t tentacle_hold_steps = 0;
uint16_t tentacle_hold_index = 0;
uint16_t tentacle_transition_steps = 0;
uint16_t tentacle_transition_index = 0;
const tentacle_animation_t *tentacle_current_anim;

uint8_t wiggle_mask = 0xff;

uint8_t current_ambient_correct = 0;
uint8_t previous_ambient_correct = 0;

uint64_t eye_twinkle_bits = 0xffffffffffffffff;
uint8_t eyes_twinkling = 0;
uint8_t tentacle_twinkle_bits = 0xea;
uint16_t leg_anim_adj_index = 0;
uint16_t face_anim_adj_index = 0;

// See https://graphics.stanford.edu/~seander/bithacks.html#IntegerLogObvious
inline uint8_t log2(uint16_t v) {
    int8_t r = 0; // r will be lg(v)
    while (v >>= 1)
    {
        r++;
    }

    return r;
}

void do_light_step() {
    // Do the light averaging:
    // This can go up to 12 bits:
    light = light_tot * 0.8 / ADC_WINDOW;

    // We're going to get the order of magnitude (log2) of light:
    // 0 .. 12
    light_order = log2(light);

    // Do some correction:
    if (light_order <=2) {
        light_order = 0;
    } else {
        light_order-=2;
    }
    // 0 .. 10
}

void do_brightness_correction(uint8_t order, uint8_t immediate) {
    if (immediate)
        current_ambient_correct = order;
    else if (current_ambient_correct < order)
        current_ambient_correct++;
    else if (current_ambient_correct > order)
        current_ambient_correct--;

    face_ambient_brightness = ((uint32_t)FACE_DIM_BRIGHTNESS) << current_ambient_correct;
    if (face_ambient_brightness > UINT16_MAX) {
        face_ambient_brightness = UINT16_MAX;
    }

}

void set_face(uint64_t frame) {
    static uint64_t eye_mask = 0;
    for (uint8_t i=0; i<64; i++) {
        eye_mask = ((uint64_t) 1 << i);
        if (frame & eye_mask) {
            if (!eyes_twinkling || (eye_twinkle_bits & eye_mask)) // not twinkling, or twinkle full
                tlc_bank_gs[i/16][i%16] = face_ambient_brightness;
            else
                tlc_bank_gs[i/16][i%16] = face_ambient_brightness << 2;
        } else {
            tlc_bank_gs[i/16][i%16] = 0x00;
        }
    }
}

// Each time we hit a new frame, we hit this.
// If we're fading, it will set up the fades.
void leg_load_colors() {
    // leg_colors_curr <- tentacle_current_anim[tentacle_anim_index]

    // Stage in the current color:
    memcpy(leg_colors_curr, tentacle_current_anim->colors[tentacle_anim_frame], sizeof(rgbcolor_t)*8);

    // Stage in the next color:
    // If we're looping, it's modded. If not looping, back to black.
    if (tentacle_anim_frame == tentacle_current_anim->len-1 && !(tentacle_anim_looping || tentacle_is_ambient)) { // last frame:
        // We're at the last frame, and we are NOT looping. So our NEXT
        // color will be OFF.
        memcpy(leg_colors_next, legs_off, sizeof(rgbcolor_t)*8);
    } else {
        // We're either looping or not at the end, so it's probably safe to skip this:
        uint8_t next_id = (tentacle_anim_frame+1) % tentacle_current_anim->len;
        memcpy(leg_colors_next, tentacle_current_anim->colors[next_id], sizeof(rgbcolor_t)*8);
    }

    // Stage in the step color:
    for (uint8_t i=0; i<8; i++) {
        leg_colors_step[i].red = ((int_fast32_t) leg_colors_next[i].red - leg_colors_curr[i].red) / tentacle_transition_steps;
        leg_colors_step[i].green = ((int_fast32_t) leg_colors_next[i].green - leg_colors_curr[i].green) / tentacle_transition_steps;
        leg_colors_step[i].blue = ((int_fast32_t) leg_colors_next[i].blue - leg_colors_curr[i].blue) / tentacle_transition_steps;
    }
}

inline void leg_fade_colors() {
    // If this is the very last transition step,
    if (tentacle_transition_steps && tentacle_transition_index == tentacle_transition_steps-1) {
        // hit the destination:
        memcpy(leg_colors_curr, leg_colors_next, sizeof(rgbcolor_t) * 8);
    } else {
        for (uint8_t i=0; i<8; i++) {
            leg_colors_curr[i].red += leg_colors_step[i].red;
            leg_colors_curr[i].green += leg_colors_step[i].green;
            leg_colors_curr[i].blue += leg_colors_step[i].blue;
        }
    }
}

void eye_twinkle_off() {
    eye_twinkle_bits = 0xffffffffffffffff;
    eyes_twinkling = 0;
    set_face(face_curr); // Dirty face.
}

void eye_twinkle_on() {
    eyes_twinkling = 1;
    face_anim_adj_index = 0;
}

void tentacle_wiggle() {
    uint8_t wiggle_mask_temp = 0xff;
    if (tentacle_current_anim->wiggle) {
        wiggle_mask_temp &= ~(1 << (rand() % 4));
        if (rand() % 2) wiggle_mask_temp &= ~(1 << (rand() % 4));
    }
    wiggle_mask = wiggle_mask_temp;
}

// This actually sets the colors of the tentacles/legs.
// Here is where we are going to do some fancy stuff:
//  1. brightness correction (ambient)
//  2. twinkling
void set_tentacles(const rgbcolor_t* leg_colors) {
    static uint_fast32_t r = 0;
    static uint_fast32_t g = 0;
    static uint_fast32_t b = 0;

    for (uint8_t tent=0; tent<8; tent++) {
        r = leg_colors[tent].red << 1;
        g = leg_colors[tent].green << 1;
        b = leg_colors[tent].blue << 1;

        // If it's <3 (meaning lower) and masked out by wiggling,
        if (tent < 4 && !(wiggle_mask & (1 << tent))) {
            // turn it off.
            r=0;
            g=0;
            b=0;
        } else if (tent >=4 && !(wiggle_mask & (1 << (tent-4)))) {
            r = leg_colors[tent-4].red;
            g = leg_colors[tent-4].green;
            b = leg_colors[tent-4].blue;
        }

        // Handle the particulars of the animation's
        //  sub-type. (twinkling, etc.)
        switch(tentacle_current_anim->anim_type) {
        case ANIM_TYPE_FASTTWINKLE:
            if (tentacle_twinkle_bits & (1 << tent)) {
                r = r >> 2;
                g = g >> 2;
                b = b >> 2;
            }
            break;
        case ANIM_TYPE_SLOWTWINKLE:
            if (tentacle_twinkle_bits & (1 << tent)) {
                r = r >> 2;
                g = g >> 2;
                b = b >> 2;
            }
            break;
        case ANIM_TYPE_HARDTWINKLE:
            if (tentacle_twinkle_bits & (1 << tent)) {
                r = 0;
                g = 0;
                b = 0;
            }
            break;
        default:
            break;
        }

        static int8_t temp_tent = 0;
        temp_tent = tent-4;

        // If we're upper, and our corresponding lower light is on:
        // EVEN IF WE'RE NOT "RETRACTED".
        if ((tent > 3) && (tlc_bank_gs[4+(temp_tent/4)][4+((temp_tent*3)%12)] || tlc_bank_gs[4+(temp_tent/4)][4+((temp_tent*3)%12)+1] || tlc_bank_gs[4+(temp_tent/4)][4+((temp_tent*3)%12)+2])) {
            // Dim it.
            r = r >> 3;
            g = g >> 3;
            b = b >> 3;
        }

        r = r << current_ambient_correct;
        g = g << current_ambient_correct;
        b = b << current_ambient_correct;

        if (r>UINT16_MAX) r=UINT16_MAX;
        if (g>UINT16_MAX) g=UINT16_MAX;
        if (b>UINT16_MAX) b=UINT16_MAX;

        tlc_bank_gs[4+(tent/4)][4+((tent*3)%12)] = b;
        tlc_bank_gs[4+(tent/4)][4+((tent*3)%12)+1] = g;
        tlc_bank_gs[4+(tent/4)][4+((tent*3)%12)+2] = r;
    }
}

void face_set_ambient_direct(uint64_t amb) {
    face_ambient = amb;
    if (face_state == FACESTATE_AMBIENT) {
        face_curr = face_ambient;
        set_face(face_curr);
    }
}

void face_start_anim(uint8_t anim_index) {
    face_curr_anim_frame = 0;
    face_current_animation = anim_index;
    face_curr = face_all_animations[face_current_animation]->frames[0];
    face_curr_dur = face_all_animations[face_current_animation]->frame_durations[0];
    face_state = FACESTATE_ANIMATION;
    set_face(face_curr);
}

uint8_t tentacle_animation_state = 0;


void tentacle_setup_transitions_and_go() {
    tentacle_hold_steps = tentacle_current_anim->durations[tentacle_anim_frame] / LEGS_DUR_STEP;
    tentacle_hold_index = 0;
    tentacle_transition_steps = tentacle_current_anim->fade_durs[tentacle_anim_frame] / LEGS_DUR_STEP;
    tentacle_transition_index = 0;

    leg_load_colors();
    set_tentacles(tentacle_current_anim->colors[tentacle_anim_frame]);

}

// If ambient=1, then a bit of pre-work is needed.
void tentacle_start_anim_direct(const tentacle_animation_t *animation, uint8_t loop, uint8_t ambient) {
    // If we've been asked to do an interrupting animation, remember what our ambient anim
    //  was so we can go back to it.
    if (!ambient && tentacle_is_ambient) {
        tentacle_saved_anim_id = tentacle_anim_id;
        tentacle_saved_anim_type = tentacle_anim_type;
    }

    tentacle_is_ambient = ambient;

    tentacle_current_anim = animation;

    tentacle_anim_frame = 0; // This is our frame index in the animation.
    tentacle_animation_state = 1; // animating

    leg_anim_adj_index = 0;
    tentacle_anim_looping = loop;
    tentacle_anim_length = tentacle_current_anim->len;

    wiggle_mask = 0xff;

    tentacle_setup_transitions_and_go();
}

// pew pew pew
void tentacle_send_meta_mating(uint8_t index) {
    tentacle_start_anim_direct(leg_meta_mate_anim_set[index], 4, 0);
//    tentacle_start_anim(LEG_ANIM_META_MATING, 2, 4, 0);
}

void tentacle_start_anim(uint8_t anim_id, uint8_t anim_type, uint8_t loop, uint8_t ambient) {
    if (ambient && !tentacle_is_ambient) {
        // If we've been asked to switch our ambient animation, but we're currently in an
        //  interrupting animation, we need to change what we have saved so we go back to
        //  the new ambient animation.
        tentacle_saved_anim_id = anim_id;
        tentacle_saved_anim_type = anim_type;
        return;
    }

    if (ambient) {
        for (uint8_t at=0; at<3; at++) {
            for (uint16_t i=0; i< leg_meta_mate_anim_set[at]->len; i++) {
                for (uint8_t j=0; j<8; j++) {
                    if (leg_meta_mate_anim_set[at]->colors[i][j].red || leg_meta_mate_anim_set[at]->colors[i][j].green || leg_meta_mate_anim_set[at]->colors[i][j].blue) {
                        switch(at) {
                        case 0:
                            leg_meta_mate_camo_frames[i][j].red = sprays[anim_id].red;
                            leg_meta_mate_camo_frames[i][j].green = sprays[anim_id].green;
                            leg_meta_mate_camo_frames[i][j].blue = sprays[anim_id].blue;
                            break;
                        case 1:
                            leg_meta_mate_ink_frames[i][j].red = sprays[anim_id].red;
                            leg_meta_mate_ink_frames[i][j].green = sprays[anim_id].green;
                            leg_meta_mate_ink_frames[i][j].blue = sprays[anim_id].blue;
                            break;
                        case 2:
                            leg_meta_mate_doubleink_frames[i][j].red = sprays[anim_id].red;
                            leg_meta_mate_doubleink_frames[i][j].green = sprays[anim_id].green;
                            leg_meta_mate_doubleink_frames[i][j].blue = sprays[anim_id].blue;
                            break;
                        }
                    }
                }
            }
        }
    }

    tentacle_start_anim_direct(legs_all_anim_sets[anim_id][anim_type], loop, ambient);

    tentacle_anim_id = anim_id;
    tentacle_anim_type = anim_type;
}

void tentacle_next_anim_frame() {
    tentacle_anim_frame++; // This is our index in the animation.

    if (tentacle_anim_frame >= tentacle_anim_length) { // rolled over.
        if (tentacle_is_ambient) { // we're ambient. (loop is dontcare)
            tentacle_anim_frame = 0; // start over from beginning.
        } else if (tentacle_anim_looping) { // not ambient; loops remaining
            tentacle_anim_frame = 0;
            tentacle_anim_looping--;
        } else { // not ambient, no loops remaining
            tentacle_is_ambient = 1; // Now we're back to being ambient...
            tentacle_start_anim(tentacle_saved_anim_id, tentacle_saved_anim_type, 0, 1);
            leg_anim_done(tentacle_anim_id);
            return; // skip the transitions_and_go because that's called in start_anim.
        }
    }

    tentacle_setup_transitions_and_go();
}

void led_post() {
    uint64_t chase = 0;
    set_face(chase);
    tlc_stage_blank(0);
    tlc_set_fun();
    chase = 1;

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
    static uint8_t face_dirty = 1;
    static uint8_t legs_dirty = 1;
    if (current_ambient_correct != previous_ambient_correct) {
        previous_ambient_correct = current_ambient_correct;
        face_dirty = 1;
        legs_dirty = 1;
    }

    if (eyes_twinkling) {
        face_anim_adj_index++;
        if (face_anim_adj_index == 50) {
            face_anim_adj_index = 0;
            eye_twinkle_bits = 0;
            for (uint8_t i=0; i<64; i+=8) {
                eye_twinkle_bits |= ((uint64_t)(rand() % 256) << i);
            }
            face_dirty = 1;
        }

    }

    if (face_state == FACESTATE_ANIMATION) {
        if (face_curr_dur < FACE_DUR_STEP) { // Time for next frame?
            face_curr_anim_frame++;
            if (face_curr_anim_frame == face_all_animations[face_current_animation]->len) { // done?
                face_state = FACESTATE_AMBIENT;
                face_curr = face_ambient;
                face_dirty = 1;
                s_face_anim_done = 1;
                // done
            } else {
                face_curr = face_all_animations[face_current_animation]->frames[face_curr_anim_frame];
                face_curr_dur = face_all_animations[face_current_animation]->frame_durations[face_curr_anim_frame];
                face_dirty = 1;
            }
        } else {
            // tick towards next frame.
            face_curr_dur -= FACE_DUR_STEP;
        }
    }

    if (face_dirty) {
        set_face(face_curr);
        face_dirty = 0;
    }

    // Tentacles:
    //  Apply our current delta animation timestep.
    switch(tentacle_current_anim->anim_type) {
    case ANIM_TYPE_FASTTWINKLE:
        leg_anim_adj_index++;
        if (leg_anim_adj_index == 50) {
            tentacle_twinkle_bits = rand() % 256;
            leg_anim_adj_index = 0;
            legs_dirty = 1;
        }
        break;
    case ANIM_TYPE_SLOWTWINKLE:
        leg_anim_adj_index++;
        if (leg_anim_adj_index == 400) {
            tentacle_twinkle_bits = rand() % 256;
            leg_anim_adj_index = 0;
            legs_dirty = 1;
        }
        break;
    case ANIM_TYPE_HARDTWINKLE:
        leg_anim_adj_index++;
        if (leg_anim_adj_index == 40) {
            tentacle_twinkle_bits = rand() % 256;
            leg_anim_adj_index = 0;
            legs_dirty = 1;
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
                legs_dirty = 1;
            } else {
                leg_fade_colors();
                legs_dirty = 1;
            }
        }
    }
    if (legs_dirty) {
        set_tentacles(leg_colors_curr);
        legs_dirty = 0;
    }

}
