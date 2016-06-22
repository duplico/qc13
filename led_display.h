/*
 * led_display.h
 *
 * Header for Queercon 13 LED application logic
 *
 * (c) 2016 George Louthan
 * 3-clause BSD license; see license.md.
 */

#ifndef LED_DISPLAY_H_
#define LED_DISPLAY_H_

#include <stdint.h>

typedef struct {
    uint16_t red;
    uint16_t green;
    uint16_t blue;
} rgbcolor_t;

typedef struct {
    int_fast32_t red;
    int_fast32_t green;
    int_fast32_t blue;
} rgbdelta_t;

typedef struct {
    const rgbcolor_t * colors;
    uint8_t len;
} tentacle_animation_t;

typedef struct {
    uint32_t * left_frames; // Pointer to an array of face frames.
    uint32_t * right_frames; // Pointer to an array of face frames.
    uint16_t * frame_durations;
    uint8_t len;
} face_animation_t;

#define FACE_ANIMATION_NONE 0

void face_set();
void face_start_anim();
void leds_timestep();

extern uint16_t face_banks[4];

#endif /* LED_DISPLAY_H_ */
