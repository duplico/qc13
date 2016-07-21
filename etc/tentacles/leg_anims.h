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
extern const rgbcolor_t giver_camo_frames[][8];
extern uint16_t giver_camo_durations[];
extern uint16_t giver_camo_fade_durs[];
extern const tentacle_animation_t giver_camo;
// frames for ink
extern const rgbcolor_t giver_ink_frames[][8];
extern uint16_t giver_ink_durations[];
extern uint16_t giver_ink_fade_durs[];
extern const tentacle_animation_t giver_ink;
// frames for doubleink
extern const rgbcolor_t giver_doubleink_frames[][8];
extern uint16_t giver_doubleink_durations[];
extern uint16_t giver_doubleink_fade_durs[];
extern const tentacle_animation_t giver_doubleink;
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
extern const rgbcolor_t human_hat_camo_frames[][8];
extern uint16_t human_hat_camo_durations[];
extern uint16_t human_hat_camo_fade_durs[];
extern const tentacle_animation_t human_hat_camo;
// frames for ink
extern const rgbcolor_t human_hat_ink_frames[][8];
extern uint16_t human_hat_ink_durations[];
extern uint16_t human_hat_ink_fade_durs[];
extern const tentacle_animation_t human_hat_ink;
// frames for doubleink
extern const rgbcolor_t human_hat_doubleink_frames[][8];
extern uint16_t human_hat_doubleink_durations[];
extern uint16_t human_hat_doubleink_fade_durs[];
extern const tentacle_animation_t human_hat_doubleink;
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
extern const rgbcolor_t meta_mating_camo_frames[][8];
extern uint16_t meta_mating_camo_durations[];
extern uint16_t meta_mating_camo_fade_durs[];
extern const tentacle_animation_t meta_mating_camo;
// frames for ink
extern const rgbcolor_t meta_mating_ink_frames[][8];
extern uint16_t meta_mating_ink_durations[];
extern uint16_t meta_mating_ink_fade_durs[];
extern const tentacle_animation_t meta_mating_ink;
// frames for doubleink
extern const rgbcolor_t meta_mating_doubleink_frames[][8];
extern uint16_t meta_mating_doubleink_durations[];
extern uint16_t meta_mating_doubleink_fade_durs[];
extern const tentacle_animation_t meta_mating_doubleink;
// frames for camo
extern const rgbcolor_t meta_social_camo_frames[][8];
extern uint16_t meta_social_camo_durations[];
extern uint16_t meta_social_camo_fade_durs[];
extern const tentacle_animation_t meta_social_camo;
// frames for ink
extern const rgbcolor_t meta_social_ink_frames[][8];
extern uint16_t meta_social_ink_durations[];
extern uint16_t meta_social_ink_fade_durs[];
extern const tentacle_animation_t meta_social_ink;
// frames for doubleink
extern const rgbcolor_t meta_social_doubleink_frames[][8];
extern uint16_t meta_social_doubleink_durations[];
extern uint16_t meta_social_doubleink_fade_durs[];
extern const tentacle_animation_t meta_social_doubleink;
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
extern const rgbcolor_t poolparty_camo_frames[][8];
extern uint16_t poolparty_camo_durations[];
extern uint16_t poolparty_camo_fade_durs[];
extern const tentacle_animation_t poolparty_camo;
// frames for ink
extern const rgbcolor_t poolparty_ink_frames[][8];
extern uint16_t poolparty_ink_durations[];
extern uint16_t poolparty_ink_fade_durs[];
extern const tentacle_animation_t poolparty_ink;
// frames for doubleink
extern const rgbcolor_t poolparty_doubleink_frames[][8];
extern uint16_t poolparty_doubleink_durations[];
extern uint16_t poolparty_doubleink_fade_durs[];
extern const tentacle_animation_t poolparty_doubleink;
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
extern const rgbcolor_t pushover_camo_frames[][8];
extern uint16_t pushover_camo_durations[];
extern uint16_t pushover_camo_fade_durs[];
extern const tentacle_animation_t pushover_camo;
// frames for ink
extern const rgbcolor_t pushover_ink_frames[][8];
extern uint16_t pushover_ink_durations[];
extern uint16_t pushover_ink_fade_durs[];
extern const tentacle_animation_t pushover_ink;
// frames for doubleink
extern const rgbcolor_t pushover_doubleink_frames[][8];
extern uint16_t pushover_doubleink_durations[];
extern uint16_t pushover_doubleink_fade_durs[];
extern const tentacle_animation_t pushover_doubleink;
// frames for camo
extern const rgbcolor_t shutdown_camo_frames[][8];
extern uint16_t shutdown_camo_durations[];
extern uint16_t shutdown_camo_fade_durs[];
extern const tentacle_animation_t shutdown_camo;
// frames for ink
extern const rgbcolor_t shutdown_ink_frames[][8];
extern uint16_t shutdown_ink_durations[];
extern uint16_t shutdown_ink_fade_durs[];
extern const tentacle_animation_t shutdown_ink;
// frames for doubleink
extern const rgbcolor_t shutdown_doubleink_frames[][8];
extern uint16_t shutdown_doubleink_durations[];
extern uint16_t shutdown_doubleink_fade_durs[];
extern const tentacle_animation_t shutdown_doubleink;
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
// frames for camo
extern const rgbcolor_t wrapup_1_camo_frames[][8];
extern uint16_t wrapup_1_camo_durations[];
extern uint16_t wrapup_1_camo_fade_durs[];
extern const tentacle_animation_t wrapup_1_camo;
// frames for ink
extern const rgbcolor_t wrapup_1_ink_frames[][8];
extern uint16_t wrapup_1_ink_durations[];
extern uint16_t wrapup_1_ink_fade_durs[];
extern const tentacle_animation_t wrapup_1_ink;
// frames for doubleink
extern const rgbcolor_t wrapup_1_doubleink_frames[][8];
extern uint16_t wrapup_1_doubleink_durations[];
extern uint16_t wrapup_1_doubleink_fade_durs[];
extern const tentacle_animation_t wrapup_1_doubleink;
// frames for camo
extern const rgbcolor_t wrapup_2_camo_frames[][8];
extern uint16_t wrapup_2_camo_durations[];
extern uint16_t wrapup_2_camo_fade_durs[];
extern const tentacle_animation_t wrapup_2_camo;
// frames for ink
extern const rgbcolor_t wrapup_2_ink_frames[][8];
extern uint16_t wrapup_2_ink_durations[];
extern uint16_t wrapup_2_ink_fade_durs[];
extern const tentacle_animation_t wrapup_2_ink;
// frames for doubleink
extern const rgbcolor_t wrapup_2_doubleink_frames[][8];
extern uint16_t wrapup_2_doubleink_durations[];
extern uint16_t wrapup_2_doubleink_fade_durs[];
extern const tentacle_animation_t wrapup_2_doubleink;
// frames for camo
extern const rgbcolor_t wrapup_3_camo_frames[][8];
extern uint16_t wrapup_3_camo_durations[];
extern uint16_t wrapup_3_camo_fade_durs[];
extern const tentacle_animation_t wrapup_3_camo;
// frames for ink
extern const rgbcolor_t wrapup_3_ink_frames[][8];
extern uint16_t wrapup_3_ink_durations[];
extern uint16_t wrapup_3_ink_fade_durs[];
extern const tentacle_animation_t wrapup_3_ink;
// frames for doubleink
extern const rgbcolor_t wrapup_3_doubleink_frames[][8];
extern uint16_t wrapup_3_doubleink_durations[];
extern uint16_t wrapup_3_doubleink_fade_durs[];
extern const tentacle_animation_t wrapup_3_doubleink;
// frames for camo
extern const rgbcolor_t wrapup_4_camo_frames[][8];
extern uint16_t wrapup_4_camo_durations[];
extern uint16_t wrapup_4_camo_fade_durs[];
extern const tentacle_animation_t wrapup_4_camo;
// frames for ink
extern const rgbcolor_t wrapup_4_ink_frames[][8];
extern uint16_t wrapup_4_ink_durations[];
extern uint16_t wrapup_4_ink_fade_durs[];
extern const tentacle_animation_t wrapup_4_ink;
// frames for doubleink
extern const rgbcolor_t wrapup_4_doubleink_frames[][8];
extern uint16_t wrapup_4_doubleink_durations[];
extern uint16_t wrapup_4_doubleink_fade_durs[];
extern const tentacle_animation_t wrapup_4_doubleink;
#define LEG_ANIM_COUNT 27
#define LEG_ANIM_BOOKWORM 0
#define LEG_ANIM_DEF 1
#define LEG_ANIM_DOUBLEINK 2
#define LEG_ANIM_FIRE 3
#define LEG_ANIM_FOUND 4
#define LEG_ANIM_GAMER 5
#define LEG_ANIM_GEEK_GIRL 6
#define LEG_ANIM_GIVER 7
#define LEG_ANIM_GLAM 8
#define LEG_ANIM_HANDLER 9
#define LEG_ANIM_HUMAN_HAT 10
#define LEG_ANIM_KARATEKID 11
#define LEG_ANIM_LUSH 12
#define LEG_ANIM_META_MATING 13
#define LEG_ANIM_META_SOCIAL 14
#define LEG_ANIM_MIXOLOGIST 15
#define LEG_ANIM_PARTYTIME 16
#define LEG_ANIM_POOLPARTY 17
#define LEG_ANIM_POWERHUNGRY 18
#define LEG_ANIM_PUSHOVER 19
#define LEG_ANIM_SHUTDOWN 20
#define LEG_ANIM_SPLISH_SPLASH 21
#define LEG_ANIM_UBER 22
#define LEG_ANIM_WRAPUP_1 23
#define LEG_ANIM_WRAPUP_2 24
#define LEG_ANIM_WRAPUP_3 25
#define LEG_ANIM_WRAPUP_4 26
#define ANIM_TYPE_SOLID 0
#define ANIM_TYPE_FASTTWINKLE 1
#define ANIM_TYPE_HARDTWINKLE 2
#define ANIM_TYPE_SLOWTWINKLE 3
#define ANIM_TYPE_SOFTTWINKLE 4
#define LEG_ANIM_TYPE_COUNT 5
extern const tentacle_animation_t **legs_all_anim_sets[];
#endif // _H_
