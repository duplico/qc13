#include "eye_anims.h"

#include "../../led_display.h"

// Frames for angry
uint64_t angry_frames[] = {0b0111111000100000000001111111111001111110001000000000011111111110, 0b0000011111110000000000000000000000000111111100000000000000000000, 0b0111111000100000000001111111111001111110001000000000011111111110, 0b0000011111110000000000000000000000000111111100000000000000000000, 0b1000010000100000111111111111111010000100001000001111111111111110, 0b0111111000100000000001111111111001111110001000000000011111111110, 0b0000011111110000000000000000000000000111111100000000000000000000, 0b0111111000100000000001111111111001111110001000000000011111111110, 0b0000011111110000000000000000000000000111111100000000000000000000, 0b0111111000100000000001111111111001111110001000000000011111111110, 0b0000011111110000000000000000000000000111111100000000000000000000, 0b0111111000100000000001111111111001111110001000000000011111111110, 0b0000011111110000000000000000000000000111111100000000000000000000};
uint16_t angry_lengths[] = {800, 400, 2800, 400, 1700, 1100, 400, 2600, 400, 400, 400, 400, 400};

// Animation struct for angry
face_animation_t angry = {angry_frames, angry_lengths, 13};

// Frames for blinking
uint64_t blinking_frames[] = {0b0000011111110000000000000000000000000111111100000000000000000000, 0b1000010000100000111111111111111010000100001000001111111111111110};
uint16_t blinking_lengths[] = {400, 150};

// Animation struct for blinking
face_animation_t blinking = {blinking_frames, blinking_lengths, 2};

// Frames for cats_left
uint64_t cats_left_frames[] = {0b0000011111110000000000000000000000000111111100000000000000000000, 0b1111111000000000111110000000000011000000001111100000000000111110, 0b0000011111110000000000000000000000000111111100000000000000000000};
uint16_t cats_left_lengths[] = {400, 2500, 400};

// Animation struct for cats_left
face_animation_t cats_left = {cats_left_frames, cats_left_lengths, 3};

// Frames for cats_right
uint64_t cats_right_frames[] = {0b0000011111110000000000000000000000000111111100000000000000000000, 0b1100000000111110000000000011111011111110000000001111100000000000, 0b0000011111110000000000000000000000000111111100000000000000000000};
uint16_t cats_right_lengths[] = {400, 2500, 400};

// Animation struct for cats_right
face_animation_t cats_right = {cats_right_frames, cats_right_lengths, 3};

// Frames for crosseyedcat
uint64_t crosseyedcat_frames[] = {0b0000011111110000000000000000000000000111111100000000000000000000, 0b1111111000000000111110000000000011111110000000001111100000000000, 0b0000011111110000000000000000000000000111111100000000000000000000};
uint16_t crosseyedcat_lengths[] = {400, 2500, 400};

// Animation struct for crosseyedcat
face_animation_t crosseyedcat = {crosseyedcat_frames, crosseyedcat_lengths, 3};

// Frames for cutesy
uint64_t cutesy_frames[] = {0b1000011111110000111110000011111010000111111100001111100000111110, 0b0000011111110000000000000000000000000111111100000000000000000000, 0b1000011111110000111110000011111010000111111100001111100000111110, 0b0000011111110000000000000000000000000111111100000000000000000000, 0b1000011111110000111110000011111010000111111100001111100000111110, 0b0000011111110000000000000000000000000111111100000000000000000000};
uint16_t cutesy_lengths[] = {1800, 400, 400, 400, 400, 400};

// Animation struct for cutesy
face_animation_t cutesy = {cutesy_frames, cutesy_lengths, 6};

// Frames for cutesy_left
uint64_t cutesy_left_frames[] = {0b0000011111110000000000000000000000000111111100000000000000000000, 0b0100010000111110111111111100000001111110001000000000011111111110, 0b0000011111110000000000000000000000000111111100000000000000000000, 0b0100010000111110111111111100000001111110001000000000011111111110, 0b0000011111110000000000000000000000000111111100000000000000000000, 0b0100010000111110111111111100000001111110001000000000011111111110, 0b0000011111110000000000000000000000000111111100000000000000000000, 0b1000010000100000111111111111111010000100001000001111111111111110, 0b1000011111110000111110000011111010000111111100001111100000111110, 0b1000010000100000111111111111111010000100001000001111111111111110, 0b0000011111110000000000000000000000000111111100000000000000000000};
uint16_t cutesy_left_lengths[] = {400, 2200, 400, 3200, 400, 900, 400, 2200, 1100, 800, 400};

// Animation struct for cutesy_left
face_animation_t cutesy_left = {cutesy_left_frames, cutesy_left_lengths, 11};

// Frames for cutesy_right
uint64_t cutesy_right_frames[] = {0b0000011111110000000000000000000000000111111100000000000000000000, 0b0111111000100000000001111111111001000100001111101111111111000000, 0b0000011111110000000000000000000000000111111100000000000000000000, 0b0111111000100000000001111111111001000100001111101111111111000000, 0b0000011111110000000000000000000000000111111100000000000000000000, 0b0111111000100000000001111111111001000100001111101111111111000000, 0b0000011111110000000000000000000000000111111100000000000000000000, 0b1000010000100000111111111111111010000100001000001111111111111110, 0b0111101111011110000000000000000001111011110111100000000000000000, 0b1000010000100000111111111111111010000100001000001111111111111110, 0b0000011111110000000000000000000000000111111100000000000000000000};
uint16_t cutesy_right_lengths[] = {400, 2200, 400, 2200, 400, 2200, 400, 2200, 1100, 800, 400};

// Animation struct for cutesy_right
face_animation_t cutesy_right = {cutesy_right_frames, cutesy_right_lengths, 11};

// Frames for cute_lookaround
uint64_t cute_lookaround_frames[] = {0b0000011111110000000000000000000000000111111100000000000000000000, 0b1000010000100000111111111111111010000100001000001111111111111110, 0b0111111000100000000001111111111001000100001111101111111111000000, 0b1000010000100000111111111111111010000100001000001111111111111110, 0b0100010000111110111111111100000001111110001000000000011111111110, 0b1000010000100000111111111111111010000100001000001111111111111110, 0b0111111000100000000001111111111001000100001111101111111111000000, 0b0000011111110000000000000000000000000111111100000000000000000000, 0b1000010000100000111111111111111010000100001000001111111111111110, 0b0100010000111110111111111100000001111110001000000000011111111110, 0b1000010000100000111111111111111010000100001000001111111111111110, 0b0111111000100000000001111111111001000100001111101111111111000000, 0b0000011111110000000000000000000000000111111100000000000000000000};
uint16_t cute_lookaround_lengths[] = {400, 800, 1200, 1200, 1200, 1200, 1200, 400, 600, 2100, 400, 800, 400};

// Animation struct for cute_lookaround
face_animation_t cute_lookaround = {cute_lookaround_frames, cute_lookaround_lengths, 13};

// Frames for eyeroll
uint64_t eyeroll_frames[] = {0b0000011111110000000000000000000000000111111100000000000000000000, 0b1000010000100000111111111111111010000100001000001111111111111110, 0b1000000000000000111000000000111010000000000000001110000000001110, 0b0000010000000000111111100000000000000000001000000000000011111110, 0b0000010000100000000001111100000000000100001000000000011111000000, 0b1000010000100000111111111111111010000100001000001111111111111110, 0b0000011111110000000000000000000000000111111100000000000000000000, 0b1100010000111110111111111100000001111110001000000000011111111110};
uint16_t eyeroll_lengths[] = {400, 1900, 700, 700, 700, 2100, 400, 1700};

// Animation struct for eyeroll
face_animation_t eyeroll = {eyeroll_frames, eyeroll_lengths, 8};

// Frames for fall_asleep
uint64_t fall_asleep_frames[] = {0b0000011111110000000000000000000000000111111100000000000000000000, 0b1000010000100000111111111111111010000100001000001111111111111110, 0b0000011111110000000001111100000000000111111100000000011111000000, 0b1000010000100000111111111111111010000100001000001111111111111110, 0b0000011111110000000000000000000000000111111100000000000000000000, 0b1000010000100000111111111111111010000100001000001111111111111110, 0b0000011111110000000001111100000000000111111100000000011111000000, 0b0000010000100000000001111100000000000100001000000000011111000000, 0b0000011111110000000000000000000000000111111100000000000000000000, 0b1000010000100000111111111111111010000100001000001111111111111110, 0b0000011111110000000000000000000000000111111100000000000000000000, 0b1000010000100000111111111111111010000100001000001111111111111110, 0b0000011111110000000000000000000000000111111100000000000000000000, 0b1000010000100000111111111111111010000100001000001111111111111110, 0b0000011111110000000001111100000000000111111100000000011111000000, 0b0000010000100000000001111100000000000100001000000000011111000000, 0b0000011111110000000000000000000000000111111100000000000000000000, 0b1000010000100000111111111111111010000100001000001111111111111110, 0b0000011111110000000000000000000000000111111100000000000000000000, 0b1000010000100000111111111111111010000100001000001111111111111110, 0b0000011111110000000000000000000000000111111100000000000000000000, 0b1000010000100000111111111111111010000100001000001111111111111110, 0b0000011111110000000000000000000000000111111100000000000000000000, 0b1000010000100000111111111111111010000100001000001111111111111110};
uint16_t fall_asleep_lengths[] = {400, 1800, 1800, 1000, 400, 2100, 2000, 3300, 300, 1000, 400, 3100, 400, 2400, 1500, 7500, 300, 1100, 400, 500, 400, 500, 400, 500};

// Animation struct for fall_asleep
face_animation_t fall_asleep = {fall_asleep_frames, fall_asleep_lengths, 24};

// Frames for fastblinking
uint64_t fastblinking_frames[] = {0b0000011111110000000000000000000000000111111100000000000000000000, 0b1000010000100000111111111111111010000100001000001111111111111110};
uint16_t fastblinking_lengths[] = {150, 150};

// Animation struct for fastblinking
face_animation_t fastblinking = {fastblinking_frames, fastblinking_lengths, 2};

// Frames for konami
uint64_t konami_frames[] = {0b0111101111011110000000000000000001111011110111100000000000000000, 0b0000011111110000000000000000000000000111111100000000000000000000, 0b0000000000100000000000001111111000000100000000001111111000000000, 0b0000000000000000000000000000000000000000000000000000000000000000, 0b0000000000100000000000001111111000000100000000001111111000000000, 0b0000000000000000000000000000000000000000000000000000000000000000, 0b0000010000000000111111100000000000000000001000000000000011111110, 0b0000000000000000000000000000000000000000000000000000000000000000, 0b0000010000000000111111100000000000000000001000000000000011111110, 0b0000000000000000000000000000000000000000000000000000000000000000, 0b1000000000000000111000000000111010000000000000001110000000001110, 0b0000010000100000000001111100000000000100001000000000011111000000, 0b1000000000000000111000000000111010000000000000001110000000001110, 0b0000010000100000000001111100000000000100001000000000011111000000, 0b0000000000000000000000000000000000000000000000000000000000000000, 0b0111101111011110000000000000000001111011110111100000000000000000, 0b1000010000100000111111111111111010000100001000001111111111111110, 0b0000011111110000000000000000000000000111111100000000000000000000};
uint16_t konami_lengths[] = {1700, 400, 1800, 900, 1800, 900, 1800, 900, 1800, 900, 1800, 1800, 1800, 1800, 1800, 1700, 1400, 400};

// Animation struct for konami
face_animation_t konami = {konami_frames, konami_lengths, 18};

// Frames for left_right_squint
uint64_t left_right_squint_frames[] = {0b1000010000100000111111111111111000000111111100000000011111000000, 0b0000011111110000000001111100000010000100001000001111111111111110, 0b1000010000100000111111111111111000000111111100000000011111000000, 0b0000011111110000000001111100000010000100001000001111111111111110, 0b1000010000100000111111111111111000000111111100000000011111000000, 0b0000011111110000000001111100000010000100001000001111111111111110, 0b0000011111110000000000000000000000000111111100000000000000000000};
uint16_t left_right_squint_lengths[] = {900, 900, 900, 900, 900, 900, 400};

// Animation struct for left_right_squint
face_animation_t left_right_squint = {left_right_squint_frames, left_right_squint_lengths, 7};

// Frames for look_around
uint64_t look_around_frames[] = {0b0000011111110000000000000000000000000111111100000000000000000000, 0b1000010000100000111111111111111010000100001000001111111111111110, 0b0000011111110000000000000000000000000111111100000000000000000000, 0b1000010000100000111111111111111010000100001000001111111111111110, 0b0000000000100000000000001111111000000100000000001111111000000000, 0b1000010000100000111111111111111010000100001000001111111111111110, 0b0000010000000000111111100000000000000000001000000000000011111110, 0b0000011111110000000000000000000000000111111100000000000000000000, 0b0000010000000000111111100000000000000000001000000000000011111110, 0b1000010000100000111111111111111010000100001000001111111111111110, 0b0000000000100000000000001111111000000100000000001111111000000000, 0b1000010000100000111111111111111010000100001000001111111111111110, 0b0000010000000000111111100000000000000000001000000000000011111110, 0b1000010000100000111111111111111010000100001000001111111111111110, 0b0000011111110000000000000000000000000111111100000000000000000000, 0b1000010000100000111111111111111010000100001000001111111111111110, 0b0000011111110000000000000000000000000111111100000000000000000000, 0b1000010000100000111111111111111010000100001000001111111111111110, 0b0000010000000000111111100000000000000000001000000000000011111110, 0b1000010000100000111111111111111010000100001000001111111111111110, 0b0000011111110000000000000000000000000111111100000000000000000000};
uint16_t look_around_lengths[] = {400, 400, 400, 400, 1200, 1200, 1200, 400, 1200, 1200, 1200, 1200, 1200, 1200, 400, 2200, 400, 400, 800, 400, 400};

// Animation struct for look_around
face_animation_t look_around = {look_around_frames, look_around_lengths, 21};

// Frames for meta_doubleinked
uint64_t meta_doubleinked_frames[] = {0b1000010000100000111111111111111010000100001000001111111111111110, 0b0000000000000000000000000000000000000000000000000000000000000000, 0b0000011111110000000000000000000000000111111100000000000000000000, 0b1000010000100000111111111111111010000100001000001111111111111110, 0b0000011111110000000000000000000000000111111100000000000000000000, 0b1000010000100000111111111111111010000100001000001111111111111110, 0b0000000000100000000000001111111000000100000000001111111000000000, 0b1000010000100000111111111111111010000100001000001111111111111110, 0b0000010000000000111111100000000000000000001000000000000011111110, 0b1000010000100000111111111111111010000100001000001111111111111110, 0b0000011111110000000000000000000000000111111100000000000000000000, 0b1000010000100000111111111111111010000100001000001111111111111110, 0b0000011111110000000000000000000000000111111100000000000000000000, 0b1000010000100000111111111111111010000100001000001111111111111110, 0b0111111000100000000001111111111001111110001000000000011111111110, 0b0000011111110000000000000000000000000111111100000000000000000000, 0b0111111000100000000001111111111001111110001000000000011111111110};
uint16_t meta_doubleinked_lengths[] = {200, 6100, 400, 1100, 400, 400, 800, 400, 800, 800, 200, 200, 200, 400, 800, 200, 400};

// Animation struct for meta_doubleinked
face_animation_t meta_doubleinked = {meta_doubleinked_frames, meta_doubleinked_lengths, 17};

// Frames for meta_gotinked
uint64_t meta_gotinked_frames[] = {0b1000010000100000111111111111111010000100001000001111111111111110, 0b0000000000000000000000000000000000000000000000000000000000000000, 0b0000011111110000000000000000000000000111111100000000000000000000, 0b1000010000100000111111111111111010000100001000001111111111111110, 0b0000011111110000000000000000000000000111111100000000000000000000, 0b1000010000100000111111111111111010000100001000001111111111111110, 0b0000011111110000000000000000000000000111111100000000000000000000, 0b1000010000100000111111111111111010000100001000001111111111111110, 0b0000011111110000000000000000000000000111111100000000000000000000, 0b0000000000000000000000000000000000000000000000000000000000000000, 0b0000011111110000000000000000000000000111111100000000000000000000, 0b1000010000100000111111111111111010000100001000001111111111111110, 0b0000011111110000000000000000000000000111111100000000000000000000, 0b1000010000100000111111111111111010000100001000001111111111111110, 0b0000011111110000000000000000000000000111111100000000000000000000, 0b1000010000100000111111111111111010000100001000001111111111111110, 0b0000011111110000000000000000000000000111111100000000000000000000, 0b1000010000100000111111111111111010000100001000001111111111111110};
uint16_t meta_gotinked_lengths[] = {500, 1500, 150, 150, 150, 150, 150, 150, 150, 1500, 150, 150, 150, 150, 150, 150, 350, 150};

// Animation struct for meta_gotinked
face_animation_t meta_gotinked = {meta_gotinked_frames, meta_gotinked_lengths, 18};

// Frames for meta_inked
uint64_t meta_inked_frames[] = {0b0000000000000000000000000000000000000000000000000000000000000000, 0b0000011111110000000000000000000000000111111100000000000000000000, 0b1000010000100000111111111111111010000100001000001111111111111110, 0b0111111000100000000001111111111001111110001000000000011111111110, 0b0000011111110000000000000000000000000111111100000000000000000000, 0b0111111000100000000001111111111001111110001000000000011111111110, 0b0000011111110000000000000000000000000111111100000000000000000000, 0b0111111000100000000001111111111001111110001000000000011111111110, 0b0000011111110000000000000000000000000111111100000000000000000000, 0b0111111000100000000001111111111001111110001000000000011111111110, 0b1000010000100000111111111111111010000100001000001111111111111110, 0b0000011111110000000000000000000000000111111100000000000000000000};
uint16_t meta_inked_lengths[] = {3000, 400, 800, 1100, 200, 200, 200, 200, 200, 1100, 800, 400};

// Animation struct for meta_inked
face_animation_t meta_inked = {meta_inked_frames, meta_inked_lengths, 12};

// Frames for meta_inked_flash
uint64_t meta_inked_flash_frames[] = {0b0000000000000000000000000000000000000000000000000000000000000000, 0b0111101111011110000000000000000001111011110111100000000000000000, 0b0000000000000000000000000000000000000000000000000000000000000000, 0b0111101111011110000000000000000001111011110111100000000000000000, 0b0000000000000000000000000000000000000000000000000000000000000000, 0b0111101111011110000000000000000001111011110111100000000000000000, 0b0000000000000000000000000000000000000000000000000000000000000000, 0b0111101111011110000000000000000001111011110111100000000000000000, 0b0000000000000000000000000000000000000000000000000000000000000000, 0b0111101111011110000000000000000001111011110111100000000000000000, 0b0000000000000000000000000000000000000000000000000000000000000000, 0b0111101111011110000000000000000001111011110111100000000000000000, 0b0000000000000000000000000000000000000000000000000000000000000000, 0b0111101111011110000000000000000001111011110111100000000000000000, 0b0000000000000000000000000000000000000000000000000000000000000000, 0b0111101111011110000000000000000001111011110111100000000000000000, 0b0000000000000000000000000000000000000000000000000000000000000000, 0b0111101111011110000000000000000001111011110111100000000000000000, 0b0000000000000000000000000000000000000000000000000000000000000000, 0b0111101111011110000000000000000001111011110111100000000000000000, 0b0000000000000000000000000000000000000000000000000000000000000000, 0b1000010000100000111111111111111010000100001000001111111111111110, 0b0000000000000000000000000000000000000000000000000000000000000000, 0b1000010000100000111111111111111010000100001000001111111111111110, 0b0000000000000000000000000000000000000000000000000000000000000000, 0b1000010000100000111111111111111010000100001000001111111111111110, 0b0000000000000000000000000000000000000000000000000000000000000000, 0b1000010000100000111111111111111010000100001000001111111111111110, 0b0000000000000000000000000000000000000000000000000000000000000000, 0b1000010000100000111111111111111010000100001000001111111111111110, 0b0000000000000000000000000000000000000000000000000000000000000000, 0b1000010000100000111111111111111010000100001000001111111111111110, 0b0000000000000000000000000000000000000000000000000000000000000000, 0b0111111000100000000001111111111001111110001000000000011111111110, 0b0000000000000000000000000000000000000000000000000000000000000000, 0b0111111000100000000001111111111001111110001000000000011111111110, 0b0000000000000000000000000000000000000000000000000000000000000000, 0b0111111000100000000001111111111001111110001000000000011111111110, 0b0000000000000000000000000000000000000000000000000000000000000000, 0b0111111000100000000001111111111001111110001000000000011111111110, 0b0000000000000000000000000000000000000000000000000000000000000000, 0b0111111000100000000001111111111001111110001000000000011111111110, 0b0000000000000000000000000000000000000000000000000000000000000000, 0b0111111000100000000001111111111001111110001000000000011111111110, 0b0000000000000000000000000000000000000000000000000000000000000000, 0b0111111000100000000001111111111001111110001000000000011111111110, 0b0000000000000000000000000000000000000000000000000000000000000000, 0b0111111000100000000001111111111001111110001000000000011111111110, 0b0000000000000000000000000000000000000000000000000000000000000000, 0b0111111000100000000001111111111001111110001000000000011111111110, 0b0000000000000000000000000000000000000000000000000000000000000000, 0b0111111000100000000001111111111001111110001000000000011111111110, 0b0000000000000000000000000000000000000000000000000000000000000000, 0b0111111000100000000001111111111001111110001000000000011111111110, 0b0000000000000000000000000000000000000000000000000000000000000000, 0b0111111000100000000001111111111001111110001000000000011111111110, 0b0000000000000000000000000000000000000000000000000000000000000000, 0b0111111000100000000001111111111001111110001000000000011111111110, 0b0000000000000000000000000000000000000000000000000000000000000000, 0b0111111000100000000001111111111001111110001000000000011111111110, 0b0000000000000000000000000000000000000000000000000000000000000000, 0b0111111000100000000001111111111001111110001000000000011111111110, 0b0000000000000000000000000000000000000000000000000000000000000000, 0b0111111000100000000001111111111001111110001000000000011111111110, 0b0000000000000000000000000000000000000000000000000000000000000000, 0b0111111000100000000001111111111001111110001000000000011111111110, 0b0000000000000000000000000000000000000000000000000000000000000000, 0b0111111000100000000001111111111001111110001000000000011111111110, 0b0000000000000000000000000000000000000000000000000000000000000000, 0b0111111000100000000001111111111001111110001000000000011111111110, 0b0000000000000000000000000000000000000000000000000000000000000000, 0b0111111000100000000001111111111001111110001000000000011111111110, 0b0000000000000000000000000000000000000000000000000000000000000000, 0b0111111000100000000001111111111001111110001000000000011111111110, 0b0000000000000000000000000000000000000000000000000000000000000000, 0b0111111000100000000001111111111001111110001000000000011111111110, 0b0000000000000000000000000000000000000000000000000000000000000000, 0b0111111000100000000001111111111001111110001000000000011111111110, 0b0000000000000000000000000000000000000000000000000000000000000000, 0b0111111000100000000001111111111001111110001000000000011111111110, 0b0000000000000000000000000000000000000000000000000000000000000000, 0b0111111000100000000001111111111001111110001000000000011111111110, 0b0000000000000000000000000000000000000000000000000000000000000000, 0b0000011111110000000000000000000000000111111100000000000000000000, 0b0000000000000000000000000000000000000000000000000000000000000000, 0b0000011111110000000000000000000000000111111100000000000000000000, 0b0000000000000000000000000000000000000000000000000000000000000000, 0b0000011111110000000000000000000000000111111100000000000000000000, 0b0000000000000000000000000000000000000000000000000000000000000000, 0b0000011111110000000000000000000000000111111100000000000000000000, 0b0111111000100000000001111111111001111110001000000000011111111110, 0b0000000000000000000000000000000000000000000000000000000000000000, 0b0111111000100000000001111111111001111110001000000000011111111110, 0b0000000000000000000000000000000000000000000000000000000000000000, 0b0111111000100000000001111111111001111110001000000000011111111110, 0b0000000000000000000000000000000000000000000000000000000000000000, 0b0111111000100000000001111111111001111110001000000000011111111110, 0b0000000000000000000000000000000000000000000000000000000000000000, 0b0111111000100000000001111111111001111110001000000000011111111110, 0b0000000000000000000000000000000000000000000000000000000000000000, 0b0111111000100000000001111111111001111110001000000000011111111110, 0b0000000000000000000000000000000000000000000000000000000000000000, 0b0111111000100000000001111111111001111110001000000000011111111110, 0b0000000000000000000000000000000000000000000000000000000000000000, 0b0111111000100000000001111111111001111110001000000000011111111110, 0b0000000000000000000000000000000000000000000000000000000000000000};
uint16_t meta_inked_flash_lengths[] = {50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50};

// Animation struct for meta_inked_flash
face_animation_t meta_inked_flash = {meta_inked_flash_frames, meta_inked_flash_lengths, 106};

// Frames for meta_wakeup
uint64_t meta_wakeup_frames[] = {0b0000011111110000000000000000000000000111111100000000000000000000, 0b0000011111110000000001111100000000000111111100000000011111000000, 0b0000011111110000000000000000000000000111111100000000000000000000, 0b0000011111110000000001111100000000000111111100000000011111000000, 0b1000010000100000111111111111111010000100001000001111111111111110, 0b0000011111110000000000000000000000000111111100000000000000000000, 0b1000010000100000111111111111111010000100001000001111111111111110, 0b0000011111110000000000000000000000000111111100000000000000000000};
uint16_t meta_wakeup_lengths[] = {1000, 600, 500, 500, 700, 100, 100, 100};

// Animation struct for meta_wakeup
face_animation_t meta_wakeup = {meta_wakeup_frames, meta_wakeup_lengths, 8};

// Frames for punched_in_eye
uint64_t punched_in_eye_frames[] = {0b1000010000100000111111111111111001111011110111100000000000000000, 0b1000010000100000111111111111111000000000000000000000000000000000, 0b0000011111110000000000000000000000000000000000000000000000000000, 0b1000010000100000111111111111111000000000000000000000000000000000, 0b1000010000100000111111111111111000000111111100000000011111000000, 0b1000010000100000111111111111111000000111111100000000011111000000, 0b0000011111110000000000000000000000000111111100000000011111000000, 0b1000010000100000111111111111111000000111111100000000011111000000, 0b0000011111110000000000000000000000000111111100000000000000000000};
uint16_t punched_in_eye_lengths[] = {800, 2200, 400, 1800, 600, 1200, 400, 1100, 400};

// Animation struct for punched_in_eye
face_animation_t punched_in_eye = {punched_in_eye_frames, punched_in_eye_lengths, 9};

// Frames for sad
uint64_t sad_frames[] = {0b0100010000111110111111111100000001000100001111101111111111000000, 0b0000011111110000000000000000000000000111111100000000000000000000, 0b0100010000111110111111111100000001000100001111101111111111000000, 0b0000011111110000000000000000000000000111111100000000000000000000, 0b0100010000111110111111111100000001000100001111101111111111000000, 0b0000010000100000000001111100000000000100001000000000011111000000, 0b0100010000111110111111111100000001000100001111101111111111000000, 0b0000011111110000000000000000000000000111111100000000000000000000, 0b0100010000111110111111111100000001000100001111101111111111000000, 0b0000011111110000000000000000000000000111111100000000000000000000, 0b0100010000111110111111111100000001000100001111101111111111000000};
uint16_t sad_lengths[] = {3000, 1100, 4000, 400, 1200, 1600, 900, 400, 2600, 400, 900};

// Animation struct for sad
face_animation_t sad = {sad_frames, sad_lengths, 11};

// Frames for shake
uint64_t shake_frames[] = {0b0000000000100000000000001111111000000100000000001111111000000000, 0b0000010000000000111111100000000000000000001000000000000011111110, 0b0000000000100000000000001111111000000100000000001111111000000000, 0b0000010000000000111111100000000000000000001000000000000011111110, 0b0000000000100000000000001111111000000100000000001111111000000000, 0b0000010000000000111111100000000000000000001000000000000011111110, 0b0000000000100000000000001111111000000100000000001111111000000000, 0b0000010000000000111111100000000000000000001000000000000011111110, 0b1000010000100000111111111111111010000100001000001111111111111110, 0b0000011111110000000000000000000000000111111100000000000000000000};
uint16_t shake_lengths[] = {400, 400, 400, 400, 400, 400, 400, 400, 900, 400};

// Animation struct for shake
face_animation_t shake = {shake_frames, shake_lengths, 10};

// Frames for something_in_eye
uint64_t something_in_eye_frames[] = {0b0000011111110000000001111100000010000100001000001111111111111110, 0b1000010000100000111111111111111010000100001000001111111111111110, 0b0000011111110000000001111100000010000100001000001111111111111110, 0b0000011111110000000001111100000000000111111100000000000000000000, 0b0000011111110000000001111100000010000100001000001111111111111110, 0b0000011111110000000001111100000000000111111100000000000000000000, 0b0000011111110000000001111100000010000100001000001111111111111110, 0b0000011111110000000000000000000000000111111100000000000000000000};
uint16_t something_in_eye_lengths[] = {2200, 800, 1000, 600, 1400, 600, 1400, 600};

// Animation struct for something_in_eye
face_animation_t something_in_eye = {something_in_eye_frames, something_in_eye_lengths, 8};

// Frames for spazz_out
uint64_t spazz_out_frames[] = {0b0000011111110000000000000000000000000111111100000000000000000000, 0b0111101111011110000000000000000001111011110111100000000000000000, 0b0000011111110000000000000000000000000111111100000000000000000000};
uint16_t spazz_out_lengths[] = {400, 2100, 400};

// Animation struct for spazz_out
face_animation_t spazz_out = {spazz_out_frames, spazz_out_lengths, 3};

// Frames for triple_blink
uint64_t triple_blink_frames[] = {0b0000011111110000000000000000000000000111111100000000000000000000, 0b1000010000100000111111111111111010000100001000001111111111111110, 0b0000011111110000000000000000000000000111111100000000000000000000, 0b1000010000100000111111111111111010000100001000001111111111111110, 0b0000011111110000000000000000000000000111111100000000000000000000, 0b1000010000100000111111111111111010000100001000001111111111111110, 0b0000010000000000111111100000000000000000001000000000000011111110, 0b1000010000100000111111111111111010000100001000001111111111111110, 0b0000000000100000000000001111111000000100000000001111111000000000, 0b0000011111110000000000000000000000000111111100000000000000000000};
uint16_t triple_blink_lengths[] = {300, 300, 300, 300, 300, 2300, 700, 700, 700, 400};

// Animation struct for triple_blink
face_animation_t triple_blink = {triple_blink_frames, triple_blink_lengths, 10};

// All animations here:
face_animation_t *face_all_animations[FACE_ANIM_COUNT_INCL_META] = {&angry, &blinking, &cats_left, &cats_right, &crosseyedcat, &cutesy, &cutesy_left, &cutesy_right, &cute_lookaround, &eyeroll, &fall_asleep, &fastblinking, &konami, &left_right_squint, &look_around, &punched_in_eye, &sad, &shake, &something_in_eye, &spazz_out, &triple_blink, &meta_doubleinked, &meta_gotinked, &meta_inked, &meta_inked_flash, &meta_wakeup};
