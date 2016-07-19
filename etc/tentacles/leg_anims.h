#ifndef LEGANIMS_H_
#define LEGANIMS_H_

#include "../../led_display.h"

#define LEG_CAMO_INDEX 0
#define LEG_INK_INDEX 1
#define LEG_DOUBLEINK_INDEX 2
#define LEG_ANIM_TYPE_NONE 201
#define LEG_ANIM_NONE 211

// frames for camo
extern const rgbcolor_t bookworm_camo_frames[][8];
extern uint16_t bookworm_camo_durations[];
extern uint16_t bookworm_camo_fade_durs[];
extern const tentacle_animation_t bookworm_camo;
// frames for ink
extern const rgbcolor_t bookworm_ink_frames[][8];
extern uint16_t bookworm_ink_durations[];
extern uint16_t bookworm_ink_fade_durs[];
extern const tentacle_animation_t bookworm_ink;
// frames for doubleink
extern const rgbcolor_t bookworm_doubleink_frames[][8];
extern uint16_t bookworm_doubleink_durations[];
extern uint16_t bookworm_doubleink_fade_durs[];
extern const tentacle_animation_t bookworm_doubleink;
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
extern const rgbcolor_t doubleink_camo_frames[][8];
extern uint16_t doubleink_camo_durations[];
extern uint16_t doubleink_camo_fade_durs[];
extern const tentacle_animation_t doubleink_camo;
// frames for ink
extern const rgbcolor_t doubleink_ink_frames[][8];
extern uint16_t doubleink_ink_durations[];
extern uint16_t doubleink_ink_fade_durs[];
extern const tentacle_animation_t doubleink_ink;
// frames for doubleink
extern const rgbcolor_t doubleink_doubleink_frames[][8];
extern uint16_t doubleink_doubleink_durations[];
extern uint16_t doubleink_doubleink_fade_durs[];
extern const tentacle_animation_t doubleink_doubleink;
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
extern const rgbcolor_t found_camo_frames[][8];
extern uint16_t found_camo_durations[];
extern uint16_t found_camo_fade_durs[];
extern const tentacle_animation_t found_camo;
// frames for ink
extern const rgbcolor_t found_ink_frames[][8];
extern uint16_t found_ink_durations[];
extern uint16_t found_ink_fade_durs[];
extern const tentacle_animation_t found_ink;
// frames for doubleink
extern const rgbcolor_t found_doubleink_frames[][8];
extern uint16_t found_doubleink_durations[];
extern uint16_t found_doubleink_fade_durs[];
extern const tentacle_animation_t found_doubleink;
// frames for camo
extern const rgbcolor_t gamer_camo_frames[][8];
extern uint16_t gamer_camo_durations[];
extern uint16_t gamer_camo_fade_durs[];
extern const tentacle_animation_t gamer_camo;
// frames for ink
extern const rgbcolor_t gamer_ink_frames[][8];
extern uint16_t gamer_ink_durations[];
extern uint16_t gamer_ink_fade_durs[];
extern const tentacle_animation_t gamer_ink;
// frames for doubleink
extern const rgbcolor_t gamer_doubleink_frames[][8];
extern uint16_t gamer_doubleink_durations[];
extern uint16_t gamer_doubleink_fade_durs[];
extern const tentacle_animation_t gamer_doubleink;
// frames for camo
extern const rgbcolor_t geek_girl_camo_frames[][8];
extern uint16_t geek_girl_camo_durations[];
extern uint16_t geek_girl_camo_fade_durs[];
extern const tentacle_animation_t geek_girl_camo;
// frames for ink
extern const rgbcolor_t geek_girl_ink_frames[][8];
extern uint16_t geek_girl_ink_durations[];
extern uint16_t geek_girl_ink_fade_durs[];
extern const tentacle_animation_t geek_girl_ink;
// frames for doubleink
extern const rgbcolor_t geek_girl_doubleink_frames[][8];
extern uint16_t geek_girl_doubleink_durations[];
extern uint16_t geek_girl_doubleink_fade_durs[];
extern const tentacle_animation_t geek_girl_doubleink;
// frames for camo
extern const rgbcolor_t glam_camo_frames[][8];
extern uint16_t glam_camo_durations[];
extern uint16_t glam_camo_fade_durs[];
extern const tentacle_animation_t glam_camo;
// frames for ink
extern const rgbcolor_t glam_ink_frames[][8];
extern uint16_t glam_ink_durations[];
extern uint16_t glam_ink_fade_durs[];
extern const tentacle_animation_t glam_ink;
// frames for doubleink
extern const rgbcolor_t glam_doubleink_frames[][8];
extern uint16_t glam_doubleink_durations[];
extern uint16_t glam_doubleink_fade_durs[];
extern const tentacle_animation_t glam_doubleink;
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
extern const rgbcolor_t karatekid_camo_frames[][8];
extern uint16_t karatekid_camo_durations[];
extern uint16_t karatekid_camo_fade_durs[];
extern const tentacle_animation_t karatekid_camo;
// frames for ink
extern const rgbcolor_t karatekid_ink_frames[][8];
extern uint16_t karatekid_ink_durations[];
extern uint16_t karatekid_ink_fade_durs[];
extern const tentacle_animation_t karatekid_ink;
// frames for doubleink
extern const rgbcolor_t karatekid_doubleink_frames[][8];
extern uint16_t karatekid_doubleink_durations[];
extern uint16_t karatekid_doubleink_fade_durs[];
extern const tentacle_animation_t karatekid_doubleink;
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
extern const rgbcolor_t partytime_camo_frames[][8];
extern uint16_t partytime_camo_durations[];
extern uint16_t partytime_camo_fade_durs[];
extern const tentacle_animation_t partytime_camo;
// frames for ink
extern const rgbcolor_t partytime_ink_frames[][8];
extern uint16_t partytime_ink_durations[];
extern uint16_t partytime_ink_fade_durs[];
extern const tentacle_animation_t partytime_ink;
// frames for doubleink
extern const rgbcolor_t partytime_doubleink_frames[][8];
extern uint16_t partytime_doubleink_durations[];
extern uint16_t partytime_doubleink_fade_durs[];
extern const tentacle_animation_t partytime_doubleink;
// frames for camo
extern const rgbcolor_t powerhungry_camo_frames[][8];
extern uint16_t powerhungry_camo_durations[];
extern uint16_t powerhungry_camo_fade_durs[];
extern const tentacle_animation_t powerhungry_camo;
// frames for ink
extern const rgbcolor_t powerhungry_ink_frames[][8];
extern uint16_t powerhungry_ink_durations[];
extern uint16_t powerhungry_ink_fade_durs[];
extern const tentacle_animation_t powerhungry_ink;
// frames for doubleink
extern const rgbcolor_t powerhungry_doubleink_frames[][8];
extern uint16_t powerhungry_doubleink_durations[];
extern uint16_t powerhungry_doubleink_fade_durs[];
extern const tentacle_animation_t powerhungry_doubleink;
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
// frames for camo
extern const rgbcolor_t uber_camo_frames[][8];
extern uint16_t uber_camo_durations[];
extern uint16_t uber_camo_fade_durs[];
extern const tentacle_animation_t uber_camo;
// frames for ink
extern const rgbcolor_t uber_ink_frames[][8];
extern uint16_t uber_ink_durations[];
extern uint16_t uber_ink_fade_durs[];
extern const tentacle_animation_t uber_ink;
// frames for doubleink
extern const rgbcolor_t uber_doubleink_frames[][8];
extern uint16_t uber_doubleink_durations[];
extern uint16_t uber_doubleink_fade_durs[];
extern const tentacle_animation_t uber_doubleink;
#define LEG_ANIM_COUNT 16
#define LEG_ANIM_BOOKWORM 0
#define LEG_ANIM_DEF 1
#define LEG_ANIM_DOUBLEINK 2
#define LEG_ANIM_FIRE 3
#define LEG_ANIM_FOUND 4
#define LEG_ANIM_GAMER 5
#define LEG_ANIM_GEEK_GIRL 6
#define LEG_ANIM_GLAM 7
#define LEG_ANIM_HANDLER 8
#define LEG_ANIM_KARATEKID 9
#define LEG_ANIM_LUSH 10
#define LEG_ANIM_MIXOLOGIST 11
#define LEG_ANIM_PARTYTIME 12
#define LEG_ANIM_POWERHUNGRY 13
#define LEG_ANIM_SPLISH_SPLASH 14
#define LEG_ANIM_UBER 15
#define ANIM_TYPE_SOLID 0
#define ANIM_TYPE_FASTTWINKLE 1
#define ANIM_TYPE_SLOWTWINKLE 2
#define ANIM_TYPE_SOFTTWINKLE 3
#define ANIM_TYPE_HARDTWINKLE 4
#define LEG_ANIM_TYPE_COUNT 5
extern const tentacle_animation_t **legs_all_anim_sets[];
#endif // _H_
