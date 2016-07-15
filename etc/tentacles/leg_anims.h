#ifndef LEGANIMS_H_
#define LEGANIMS_H_

#include "../../led_display.h"

#define LEG_CAMO_INDEX 0
#define LEG_INK_INDEX 1
#define LEG_DOUBLEINK_INDEX 2
#define LEG_ANIM_TYPE_NONE 201
#define LEG_ANIM_NONE 211

// frames for camo
extern const rgbcolor_t def_camo_frames[][8];
extern uint16_t def_camo_durations[];
extern uint16_t def_camo_fade_durs[];
extern const tentacle_animation_t def_camo;
// frames for ink
extern const rgbcolor_t def_ink_frames[][8];
extern uint16_t def_ink_durations[];
extern uint16_t def_ink_fade_durs[];
extern const tentacle_animation_t def_ink;
// frames for doubleink
extern const rgbcolor_t def_doubleink_frames[][8];
extern uint16_t def_doubleink_durations[];
extern uint16_t def_doubleink_fade_durs[];
extern const tentacle_animation_t def_doubleink;
// frames for camo
extern const rgbcolor_t handler_camo_frames[][8];
extern uint16_t handler_camo_durations[];
extern uint16_t handler_camo_fade_durs[];
extern const tentacle_animation_t handler_camo;
// frames for ink
extern const rgbcolor_t handler_ink_frames[][8];
extern uint16_t handler_ink_durations[];
extern uint16_t handler_ink_fade_durs[];
extern const tentacle_animation_t handler_ink;
// frames for doubleink
extern const rgbcolor_t handler_doubleink_frames[][8];
extern uint16_t handler_doubleink_durations[];
extern uint16_t handler_doubleink_fade_durs[];
extern const tentacle_animation_t handler_doubleink;
// frames for camo
extern const rgbcolor_t lush_camo_frames[][8];
extern uint16_t lush_camo_durations[];
extern uint16_t lush_camo_fade_durs[];
extern const tentacle_animation_t lush_camo;
// frames for ink
extern const rgbcolor_t lush_ink_frames[][8];
extern uint16_t lush_ink_durations[];
extern uint16_t lush_ink_fade_durs[];
extern const tentacle_animation_t lush_ink;
// frames for doubleink
extern const rgbcolor_t lush_doubleink_frames[][8];
extern uint16_t lush_doubleink_durations[];
extern uint16_t lush_doubleink_fade_durs[];
extern const tentacle_animation_t lush_doubleink;
// frames for camo
extern const rgbcolor_t mixologist_camo_frames[][8];
extern uint16_t mixologist_camo_durations[];
extern uint16_t mixologist_camo_fade_durs[];
extern const tentacle_animation_t mixologist_camo;
// frames for ink
extern const rgbcolor_t mixologist_ink_frames[][8];
extern uint16_t mixologist_ink_durations[];
extern uint16_t mixologist_ink_fade_durs[];
extern const tentacle_animation_t mixologist_ink;
// frames for doubleink
extern const rgbcolor_t mixologist_doubleink_frames[][8];
extern uint16_t mixologist_doubleink_durations[];
extern uint16_t mixologist_doubleink_fade_durs[];
extern const tentacle_animation_t mixologist_doubleink;
// frames for camo
extern const rgbcolor_t rainbow_camo_frames[][8];
extern uint16_t rainbow_camo_durations[];
extern uint16_t rainbow_camo_fade_durs[];
extern const tentacle_animation_t rainbow_camo;
// frames for ink
extern const rgbcolor_t rainbow_ink_frames[][8];
extern uint16_t rainbow_ink_durations[];
extern uint16_t rainbow_ink_fade_durs[];
extern const tentacle_animation_t rainbow_ink;
// frames for doubleink
extern const rgbcolor_t rainbow_doubleink_frames[][8];
extern uint16_t rainbow_doubleink_durations[];
extern uint16_t rainbow_doubleink_fade_durs[];
extern const tentacle_animation_t rainbow_doubleink;
// frames for camo
extern const rgbcolor_t splish_splash_camo_frames[][8];
extern uint16_t splish_splash_camo_durations[];
extern uint16_t splish_splash_camo_fade_durs[];
extern const tentacle_animation_t splish_splash_camo;
// frames for ink
extern const rgbcolor_t splish_splash_ink_frames[][8];
extern uint16_t splish_splash_ink_durations[];
extern uint16_t splish_splash_ink_fade_durs[];
extern const tentacle_animation_t splish_splash_ink;
// frames for doubleink
extern const rgbcolor_t splish_splash_doubleink_frames[][8];
extern uint16_t splish_splash_doubleink_durations[];
extern uint16_t splish_splash_doubleink_fade_durs[];
extern const tentacle_animation_t splish_splash_doubleink;
#define LEG_ANIM_COUNT 6
#define LEG_ANIM_DEF 0
#define LEG_ANIM_HANDLER 1
#define LEG_ANIM_LUSH 2
#define LEG_ANIM_MIXOLOGIST 3
#define LEG_ANIM_RAINBOW 4
#define LEG_ANIM_SPLISH_SPLASH 5
#define ANIM_TYPE_SOLID 0
#define ANIM_TYPE_FAST_TWINKLE 1
#define LEG_ANIM_TYPE_COUNT 2
extern const tentacle_animation_t **legs_all_anim_sets[];
#endif // _H_
