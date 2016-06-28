#include "../../led_display.h"
#include "leg_anims.h"

///////////////// DEF:
// frames for camo
const rgbcolor_t def_camo_frames[][8] = {
    {{0x0, 0x0, 0x0}, {0xff0, 0x7f8, 0xff0}, {0xff0, 0x7f8, 0xff0}, {0x0, 0x0, 0x0}, {0xff0, 0x7f8, 0xff0}, {0xff0, 0x7f8, 0xff0}, {0xff0, 0x7f8, 0xff0}, {0xff0, 0x7f8, 0xff0}},
    {{0x0, 0x0, 0x0}, {0xff0, 0x7f8, 0xff0}, {0x0, 0x0, 0x0}, {0xff0, 0x7f8, 0xff0}, {0xff0, 0x7f8, 0xff0}, {0xff0, 0x7f8, 0xff0}, {0xff0, 0x7f8, 0xff0}, {0xff0, 0x7f8, 0xff0}},
    {{0xff0, 0x7f8, 0xff0}, {0x0, 0x0, 0x0}, {0x0, 0x0, 0x0}, {0xff0, 0x7f8, 0xff0}, {0xff0, 0x7f8, 0xff0}, {0xff0, 0x7f8, 0xff0}, {0xff0, 0x7f8, 0xff0}, {0xff0, 0x7f8, 0xff0}},
    {{0xff0, 0x7f8, 0xff0}, {0x0, 0x0, 0x0}, {0xff0, 0x7f8, 0xff0}, {0xff0, 0x7f8, 0xff0}, {0xff0, 0x7f8, 0xff0}, {0xff0, 0x7f8, 0xff0}, {0xff0, 0x7f8, 0xff0}, {0xff0, 0x7f8, 0xff0}},
    {{0xff0, 0x7f8, 0xff0}, {0x0, 0x0, 0x0}, {0xff0, 0x7f8, 0xff0}, {0x0, 0x0, 0x0}, {0xff0, 0x7f8, 0xff0}, {0xff0, 0x7f8, 0xff0}, {0xff0, 0x7f8, 0xff0}, {0xff0, 0x7f8, 0xff0}},
};
uint16_t def_camo_durations[] = {800, 800, 800, 800, 800};
uint16_t def_camo_fade_durs[] = {0, 0, 0, 0, 0};
// the animation:
const tentacle_animation_t def_camo = {def_camo_frames, def_camo_durations, def_camo_fade_durs, 5, ANIM_TYPE_SOLID};
// frames for ink
const rgbcolor_t def_ink_frames[][8] = {
    {{0x0, 0x0, 0x0}, {0x0, 0x0, 0x0}, {0x0, 0x0, 0x0}, {0x0, 0x0, 0x0}, {0xff0, 0x7f8, 0xff0}, {0xff0, 0x7f8, 0xff0}, {0xff0, 0x7f8, 0xff0}, {0xff0, 0x7f8, 0xff0}},
};
uint16_t def_ink_durations[] = {6000};
uint16_t def_ink_fade_durs[] = {0};
// the animation:
const tentacle_animation_t def_ink = {def_ink_frames, def_ink_durations, def_ink_fade_durs, 1, ANIM_TYPE_SOLID};
// frames for doubleink
const rgbcolor_t def_doubleink_frames[][8] = {
    {{0x0, 0x0, 0x0}, {0x0, 0x0, 0x0}, {0x0, 0x0, 0x0}, {0x0, 0x0, 0x0}, {0xff0, 0x7f8, 0xff0}, {0xff0, 0x7f8, 0xff0}, {0xff0, 0x7f8, 0xff0}, {0xff0, 0x7f8, 0xff0}},
};
uint16_t def_doubleink_durations[] = {6000};
uint16_t def_doubleink_fade_durs[] = {0};
// the animation:
const tentacle_animation_t def_doubleink = {def_doubleink_frames, def_doubleink_durations, def_doubleink_fade_durs, 1, ANIM_TYPE_SOLID};

const tentacle_animation_t *def_anim_set[3] = {&def_camo, &def_ink, &def_doubleink};

///////////////// FIRE:
// frames for camo
const rgbcolor_t fire_camo_frames[][8] = {
    {{0x8b0, 0x0, 0x0}, {0xff0, 0x0, 0x0}, {0xff0, 0x228, 0x0}, {0x8b0, 0x0, 0x0}, {0xff0, 0x0, 0x0}, {0xff0, 0x228, 0x0}, {0x8b0, 0x0, 0x0}, {0xff0, 0x0, 0x0}},
    {{0xff0, 0x0, 0x0}, {0xff0, 0x228, 0x0}, {0x8b0, 0x0, 0x0}, {0xff0, 0x0, 0x0}, {0xff0, 0x228, 0x0}, {0x8b0, 0x0, 0x0}, {0xff0, 0x228, 0x0}, {0x8b0, 0x0, 0x0}},
    {{0xff0, 0x228, 0x0}, {0x8b0, 0x0, 0x0}, {0xff0, 0x228, 0x0}, {0x8b0, 0x0, 0x0}, {0x8b0, 0x0, 0x0}, {0xff0, 0x0, 0x0}, {0x8b0, 0x0, 0x0}, {0xff0, 0x228, 0x0}},
    {{0x8b0, 0x0, 0x0}, {0xff0, 0x0, 0x0}, {0x8b0, 0x0, 0x0}, {0xff0, 0x228, 0x0}, {0xff0, 0x0, 0x0}, {0x8b0, 0x0, 0x0}, {0xff0, 0x228, 0x0}, {0x8b0, 0x0, 0x0}},
    {{0xff0, 0x228, 0x0}, {0x8b0, 0x0, 0x0}, {0xff0, 0x0, 0x0}, {0xff0, 0x0, 0x0}, {0x8b0, 0x0, 0x0}, {0xff0, 0x228, 0x0}, {0xff0, 0x0, 0x0}, {0xff0, 0x228, 0x0}},
};
uint16_t fire_camo_durations[] = {100, 100, 100, 100, 100};
uint16_t fire_camo_fade_durs[] = {50, 50, 50, 50, 50};
// the animation:
const tentacle_animation_t fire_camo = {fire_camo_frames, fire_camo_durations, fire_camo_fade_durs, 5, ANIM_TYPE_FAST_TWINKLE};
// frames for ink
const rgbcolor_t fire_ink_frames[][8] = {
    {{0xff0, 0x228, 0x0}, {0xff0, 0x228, 0x0}, {0xff0, 0x228, 0x0}, {0xff0, 0x228, 0x0}, {0x8b0, 0x0, 0x0}, {0x8b0, 0x0, 0x0}, {0x8b0, 0x0, 0x0}, {0x8b0, 0x0, 0x0}},
};
uint16_t fire_ink_durations[] = {6000};
uint16_t fire_ink_fade_durs[] = {0};
// the animation:
const tentacle_animation_t fire_ink = {fire_ink_frames, fire_ink_durations, fire_ink_fade_durs, 1, ANIM_TYPE_FAST_TWINKLE};
// frames for doubleink
const rgbcolor_t fire_doubleink_frames[][8] = {
    {{0xff0, 0x0, 0x0}, {0x8b0, 0x0, 0x0}, {0xff0, 0x228, 0x0}, {0xff0, 0x7f8, 0xe00}, {0x8b0, 0x0, 0x0}, {0xff0, 0x228, 0x0}, {0xff0, 0x7f8, 0xe00}, {0xff0, 0x0, 0x0}},
    {{0xff0, 0x7f8, 0xe00}, {0xff0, 0x0, 0x0}, {0x8b0, 0x0, 0x0}, {0xff0, 0x228, 0x0}, {0xff0, 0x0, 0x0}, {0x8b0, 0x0, 0x0}, {0xff0, 0x228, 0x0}, {0xff0, 0x7f8, 0xe00}},
    {{0xff0, 0x228, 0x0}, {0xff0, 0x7f8, 0xe00}, {0xff0, 0x0, 0x0}, {0x8b0, 0x0, 0x0}, {0xff0, 0x7f8, 0xe00}, {0xff0, 0x0, 0x0}, {0x8b0, 0x0, 0x0}, {0xff0, 0x228, 0x0}},
    {{0x8b0, 0x0, 0x0}, {0xff0, 0x228, 0x0}, {0xff0, 0x7f8, 0xe00}, {0xff0, 0x0, 0x0}, {0xff0, 0x228, 0x0}, {0xff0, 0x7f8, 0xe00}, {0xff0, 0x0, 0x0}, {0x8b0, 0x0, 0x0}},
};
uint16_t fire_doubleink_durations[] = {50, 50, 50, 50};
uint16_t fire_doubleink_fade_durs[] = {50, 50, 50, 50};
// the animation:
const tentacle_animation_t fire_doubleink = {fire_doubleink_frames, fire_doubleink_durations, fire_doubleink_fade_durs, 4, ANIM_TYPE_FAST_TWINKLE};

const tentacle_animation_t *fire_anim_set[3] = {&fire_camo, &fire_ink, &fire_doubleink};

const tentacle_animation_t **legs_all_anim_sets[] = {def_anim_set, fire_anim_set};
