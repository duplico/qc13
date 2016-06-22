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

// It's a PLUGIN.
#include "etc/eyes/eye_anims.h"

const rgbcolor_t rainbow_colors[] = {
        {0xe400, 0x0300, 0x0300}, // Red
        {0xff00, 0x8c00, 0x0000}, // Orange
        {0xff00, 0xed00, 0x0000}, // Yellow
        {0x0000, 0x8000, 0x2600}, // Green
        {0x0000, 0x4d00, 0xff00}, // Blue
        {0x7500, 0x0700, 0x8700}, // Purple
};

rgbcolor_t leg_colors_curr[8] = {
        {0, 0, 0},
        {0, 0, 0},
        {0, 0, 0},
        {0, 0, 0},
        {0, 0, 0},
        {0, 0, 0},
        {0, 0, 0},
        {0, 0, 0},
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

uint16_t face_banks[4];
uint16_t face_banks_ambient[4];
uint8_t face_current_animation = FACE_ANIMATION_NONE;

void face_set() {

}

void face_start_anim() {

}

uint32_t curr_l_frame = 0;
uint32_t curr_r_frame = 0;

void leds_timestep() {
    // Face:
    //  Check whether we need to change the brightness because of:
    //  * twinkle
    //  * ambient light level change
    //  Check whether we need to change the face.
    //
    // If either, make it happen, captain.

    // Tentacles:
    //  Apply our current delta animation timestep.

    // For now:

    curr_l_frame = angry_l_frames[0];
    curr_r_frame = angry_l_frames[0];

    // bank 0:
    for (uint8_t channel=0; channel<15; channel++) {
        if (curr_l_frame & ((uint32_t) 1 << (17+channel)))
            tlc_bank_gs[0][channel+1] = 0x0f00;
        else
            tlc_bank_gs[0][channel+1] = 0;
    }
    // bank 1:
    for (uint8_t channel=0; channel<15; channel++) {
        if (curr_l_frame & ((uint32_t) 1 << (1+channel)))
            tlc_bank_gs[1][channel+1] = 0x0f00;
        else
            tlc_bank_gs[1][channel+1] = 0;
    }
    // bank 2:
    for (uint8_t channel=0; channel<15; channel++) {
        if (curr_r_frame & ((uint32_t) 1 << (17+channel)))
            tlc_bank_gs[2][channel+1] = 0x0f00;
        else
            tlc_bank_gs[2][channel+1] = 0;
    }
    // bank 3:
    for (uint8_t channel=0; channel<15; channel++) {
        if (curr_r_frame & ((uint32_t) 1 << (1+channel)))
            tlc_bank_gs[3][channel+1] = 0x0f00;
        else
            tlc_bank_gs[3][channel+1] = 0;
    }

    // For now:
    for (uint8_t bank=4; bank<6; bank++) {
        for (uint8_t channel=0; channel<12; channel++) {
            tlc_bank_gs[bank][channel+4] = 0x0f00;
        }
    }
}
