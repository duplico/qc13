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
    for (uint8_t bank=0; bank<4; bank++) {
        for (uint8_t channel=0; channel<15; channel++) {
            tlc_bank_gs[bank][channel+1] = 0x0010;
        }
    }
}
