#ifndef LEGANIMS_H_
#define LEGANIMS_H_

#define LEG_CAMO_INDEX 0
#define LEG_INK_INDEX 0
#define LEG_DOUBLEINK_INDEX 1

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
#define LEG_ANIM_COUNT 2
#define LEG_ANIM_DEF 0
#define LEG_ANIM_FIRE 1
#define ANIM_TYPE_SOLID 0
#define ANIM_TYPE_FAST_TWINKLE 1
extern const tentacle_animation_t **legs_all_anim_sets[];
#endif // _H_
