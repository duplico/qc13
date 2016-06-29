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
    const rgbcolor_t (*colors)[8];
    const uint16_t *durations;
    const uint16_t *fade_durs;
    uint8_t len;
    uint8_t anim_type;
} tentacle_animation_t;

typedef struct {
    uint64_t * frames; // Pointer to an array of face frames.
    uint16_t * frame_durations;
    uint8_t len;
} face_animation_t;

#define FACE_ANIM_NONE 0xFF

#define FACESTATE_AMBIENT 0
#define FACESTATE_ANIMATION 1

void led_post();

void face_set_ambient(uint8_t amb_index);
void face_start_anim(uint8_t anim_index);
void tentacle_start_anim(uint8_t anim_id, uint8_t anim_type, uint8_t loop, uint8_t ambient);
void leds_timestep();

extern uint16_t face_banks[4];
extern rgbcolor_t leg_colors_curr[8];
extern uint16_t face_ambient_brightness;
extern uint8_t face_state;

#endif /* LED_DISPLAY_H_ */
