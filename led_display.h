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
    uint8_t wiggle;
    uint8_t ink_loops;
} tentacle_animation_t;

typedef struct {
    uint64_t * frames; // Pointer to an array of face frames.
    uint16_t * frame_durations;
    uint8_t len;
} face_animation_t;

#define FACE_ANIM_NONE 0xFF

#define FACESTATE_AMBIENT 0
#define FACESTATE_ANIMATION 1
#define FACESTATE_INKOUT 2
#define FACESTATE_INKIN 3

void led_post();

void set_face(uint64_t frame);
void face_set_ambient_direct(uint64_t amb);
void face_set_baseline_ambient_direct(uint64_t amb);
void face_restore_ambient();
void face_set_ambient_temp_direct(uint64_t amb);
void face_start_anim(uint8_t anim_index);
void tentacle_start_anim_direct(const tentacle_animation_t *animation, uint8_t loop, uint8_t ambient);
void tentacle_start_anim(uint8_t anim_id, uint8_t anim_type, uint8_t loop, uint8_t ambient);
void tentacle_send_meta_mating(uint8_t index, uint8_t loop);
void tentacle_wiggle();
void eye_twinkle_off();
void eye_twinkle_on();

void leds_timestep();
void do_brightness_correction(uint8_t order, uint8_t immediate);
void do_light_step();

extern uint16_t face_banks[4];
extern rgbcolor_t leg_colors_curr[8];
extern uint32_t face_ambient_brightness;
extern uint8_t face_current_animation;
extern uint8_t face_curr_anim_frame;
extern uint16_t face_curr_dur;
extern uint8_t face_state;
extern uint64_t face_ambient;
extern uint8_t wiggle_mask;
extern const tentacle_animation_t *tentacle_current_anim;
extern uint8_t tentacle_is_ambient;
extern uint8_t current_ambient_correct;
extern uint8_t eyes_spinning;

#endif /* LED_DISPLAY_H_ */
