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
extern const rgbcolor_t fire_camo_frames[][8];
extern uint16_t fire_camo_durations[];
extern uint16_t fire_camo_fade_durs[];
extern const tentacle_animation_t fire_camo;
// frames for ink
extern const rgbcolor_t fire_ink_frames[][8];
extern uint16_t fire_ink_durations[];
extern uint16_t fire_ink_fade_durs[];
extern const tentacle_animation_t fire_ink;
// frames for doubleink
extern const rgbcolor_t fire_doubleink_frames[][8];
extern uint16_t fire_doubleink_durations[];
extern uint16_t fire_doubleink_fade_durs[];
extern const tentacle_animation_t fire_doubleink;
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
extern const rgbcolor_t rainbow2_camo_frames[][8];
extern uint16_t rainbow2_camo_durations[];
extern uint16_t rainbow2_camo_fade_durs[];
extern const tentacle_animation_t rainbow2_camo;
// frames for ink
extern const rgbcolor_t rainbow2_ink_frames[][8];
extern uint16_t rainbow2_ink_durations[];
extern uint16_t rainbow2_ink_fade_durs[];
extern const tentacle_animation_t rainbow2_ink;
// frames for doubleink
extern const rgbcolor_t rainbow2_doubleink_frames[][8];
extern uint16_t rainbow2_doubleink_durations[];
extern uint16_t rainbow2_doubleink_fade_durs[];
extern const tentacle_animation_t rainbow2_doubleink;
#define LEG_ANIM_COUNT 4
#define LEG_ANIM_DEF 0
#define LEG_ANIM_FIRE 1
#define LEG_ANIM_RAINBOW 2
#define LEG_ANIM_RAINBOW2 3
#define ANIM_TYPE_SOLID 0
#define ANIM_TYPE_FAST_TWINKLE 1
#define LEG_ANIM_TYPE_COUNT 2
extern const tentacle_animation_t **legs_all_anim_sets[];
#endif // _H_
