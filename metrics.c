/*
 * metrics.c
 *
 *  Created on: Jul 15, 2016
 *      Author: George
 */

#include "qc13.h"
#include "metrics.h"
#include "badge.h"
#include "leg_anims.h"

uint8_t is_uber(uint8_t badge_id) {
    return badge_id >= UBER_MIN_INCLUSIVE && badge_id <= UBER_MAX_INCLUSIVE;
}

uint8_t is_handler(uint8_t badge_id) {
    return (badge_id >= HANDLER_MIN_INCLUSIVE)
            && (badge_id <= HANDLER_MAX_INCLUSIVE);
}

uint8_t is_donor(uint8_t badge_id) {
    return (badge_id >= DONOR_MIN_INCLUSIVE) && (badge_id <= DONOR_MAX_INCLUSIVE);
}

uint8_t is_gilder(uint8_t badge_id) {
    return badge_id == EVAN_ID || badge_id == GEORGE_ID
            || badge_id == JONATHAN_ID || badge_id == JASON_ID;
}

// Returns 1 if accepted.
uint8_t award_push_hat(uint8_t id) {
    if (my_conf.hat_holder)
        return 0;
    if (id == HAT_UBER || id == HAT_HANDLER) {
        my_conf.hat_claimed = 1;
    }
    my_conf.achievements |= ((uint64_t) 0x01 << id);
    my_conf.hat_holder = 1;
    my_conf.hat_id = id;
    my_conf_write_crc();
    eyes_spinning = my_conf.hat_holder && !my_conf.hat_claimed;
    return 1;
}

uint8_t make_eligible_for_pull_hat(uint8_t id) {
    // This awards the achievement, REGARDLESS of whether we already have a hat
    //  Later on, this same hat MIGHT be awarded by the pipe.
    //  But we'll have to wait for that.
    if (my_conf.achievements & ((uint64_t) 0x01 << id)) {
        // NO THANKS. WE'VE ALREADY GOT ONE, YOU SEE.
        return 0;
    }

    my_conf.achievements |= ((uint64_t) 0x01 << id);
    my_conf_write_crc();

    return 1;
}

void claim_hat(uint8_t id) {
    my_conf.hat_claimed = 1;
    my_conf_write_crc();
    eyes_spinning = my_conf.hat_holder && !my_conf.hat_claimed;
}

uint8_t event_checkin(uint8_t event_id) {
    /*
     * Possible events:
     * QC Trans Meetup - awards camo found
     * Thursday party  - awards camo giver
     * Thursday Mixer  - awards camo mixologist, ticks mixers
     * Badge talk      - awards camo learned
     * Friday mixer    - awards camo mixologist, ticks mixers
     * Pool party      - awards camo poolparty
     * Women of QC     - awards camo geek_girl
     * Saturday Mixer  - awards camo mixologist, ticks mixers
     * Karaoke/Sat     - awards camo partytime
     * Sunday mixer    - Awards one of the wrap-ups, PULL mixtacular if attended all events
     *
     */

    if (my_conf.event_checkins[event_id]) {
        // Already checked in.
        return 0;
    }


    switch(event_id) {
    case BASE_TRANS:
        unlock_camo(LEG_ANIM_ZFLAG_TRANS);
        unlock_camo(LEG_ANIM_FOUND);
        break;
    case BASE_BTHUPARTY:
        unlock_camo(LEG_ANIM_GIVER);
        break;
    case BASE_BTHUMIX:
        unlock_camo(LEG_ANIM_MIXOLOGIST);
        break;
    case BASE_BTALK:
        unlock_camo(LEG_ANIM_LEARNED);
        break;
    case BASE_BFRIMIX:
        unlock_camo(LEG_ANIM_MIXOLOGIST);
        break;
    case BASE_BPOOL:
        unlock_camo(LEG_ANIM_POOLPARTY);
        break;
    case BASE_BWOMEN:
        unlock_camo(LEG_ANIM_GEEK_GIRL);
        break;
    case BASE_BSATMIX:
        unlock_camo(LEG_ANIM_MIXOLOGIST);
        break;
    case BASE_BKARAOKE:
        unlock_camo(LEG_ANIM_PARTYTIME);
        break;
    case BASE_BSUNMIX:
        unlock_camo(LEG_ANIM_MIXOLOGIST);
        if (my_conf.event_checkins[BASE_BTHUMIX] && my_conf.event_checkins[BASE_BFRIMIX] && my_conf.event_checkins[BASE_BSATMIX]) {
            // If we attended all the mixers:
            make_eligible_for_pull_hat(HAT_ALL_MIXERS);
            if (!is_camo_avail(LEG_ANIM_WRAPUP_1) && !is_camo_avail(LEG_ANIM_WRAPUP_2) && !is_camo_avail(LEG_ANIM_WRAPUP_3) && !is_camo_avail(LEG_ANIM_WRAPUP_4)) {
                // haven't wrapped up yet, so...
                uint8_t ach_total = 0;
                for (uint8_t i=0; i<=HAT_HANDLER; i++) {
                    if (my_conf.achievements & ((uint64_t) 0x01 << i)) {
                        ach_total++;
                    }
                }

                if (ach_total > 19) {
                    unlock_camo(LEG_ANIM_WRAPUP_4);
                } else if (ach_total > 15) {
                    unlock_camo(LEG_ANIM_WRAPUP_3);
                } else if (ach_total > 7) {
                    unlock_camo(LEG_ANIM_WRAPUP_2);
                } else {
                    unlock_camo(LEG_ANIM_WRAPUP_1);
                }
            }
        }
        break;
    default:
        return 0;
    }

    my_conf.event_checkins[event_id] = 1;
    my_conf_write_crc();
    return 1;
}

void set_badge_seen(uint8_t id, uint8_t handler_on_duty) {
    // Have we seen it at all? If not, increment count and maybe uber count.
    // Is it ODH, and have we seen it as ODH? If not, increment.

    if (!badges_seen[id]) {
        // Haven't seen it at all.

        badges_seen[id] = BADGE_SEEN_BITS;
        my_conf.seen_count++;

        if (is_uber(id) && my_conf.uber_seen_count < UBER_COUNT) {
            // it's uber:
            my_conf.uber_seen_count++;
        }

        if (id != my_conf.badge_id)
            new_badge_seen(0);
    }

    if (handler_on_duty && is_handler(id) && !(badges_seen[id] & ODH_SEEN_BITS) && my_conf.odh_seen_count < HANDLER_COUNT) {
        // It's an on-duty handler, and we haven't seen it on duty before.
        my_conf.odh_seen_count++;
        badges_seen[id] |= ODH_SEEN_BITS;

        if (my_conf.odh_seen_count == HANDLER_COUNT) {
            make_eligible_for_pull_hat(HAT_NEAR_HANDLERS);
        }
    }

    my_conf_write_crc();
}

void tick_badge_seen(uint8_t id, uint8_t handler_on_duty) {
    if (is_uber(id)) {
        uber_badges_ticks[id]++;
        if (uber_badges_ticks[id] > 8) {
            uber_badges_ticks[id] = 8;
            make_eligible_for_pull_hat(HAT_TIME_NEAR_UBERS);
        }
    }

    if (is_handler(id) && handler_on_duty) {
        odh_badges_ticks[id]++;
        if (odh_badges_ticks[id] > 8) {
            odh_badges_ticks[id] = 8;
            make_eligible_for_pull_hat(HAT_TIME_NEAR_HANDLERS);
        }
    }
}

void set_badge_mated(uint8_t id, uint8_t handler_on_duty) {
    // Have we mated w/ it at all? If not, increment count and maybe uber count.
    // Is it ODH, and have we mated w/ it as ODH? If not, increment.

    if (!badges_mated[id]) {
        // Haven't mated w/ it at all.

        badges_mated[id] = BADGE_MATED_BITS;
        my_conf.mate_count++;

        if (my_conf.mate_count >= 10) {
            unlock_camo(LEG_ANIM_LUSH);
        }
        if (my_conf.mate_count >= 50) {
            make_eligible_for_pull_hat(HAT_MATE_50);
        }
        if (my_conf.mate_count >= 100) {
            make_eligible_for_pull_hat(HAT_MATE_100);
        }
        if (my_conf.mate_count >= 200) {
            make_eligible_for_pull_hat(HAT_MATE_200);
        }

        if (is_uber(id) && my_conf.uber_mate_count < UBER_COUNT) {
            // it's uber:
            my_conf.uber_mate_count++;
            if (my_conf.uber_mate_count == UBER_COUNT) {
                make_eligible_for_pull_hat(HAT_MATE_UBER);
            }

            if (id != my_conf.badge_id) {
                unlock_camo(LEG_ANIM_GLAM);
            }

        }

        if (id != my_conf.badge_id)
            new_badge_mated();
    }

    if (handler_on_duty && is_handler(id) && !(badges_mated[id] & ODH_MATED_BITS) && my_conf.odh_mate_count < HANDLER_COUNT) {
        // It's an on-duty handler, and we haven't mated w/ it on duty before.
        my_conf.odh_mate_count++;
        badges_mated[id] |= ODH_MATED_BITS;
        if (my_conf.odh_mate_count == HANDLER_COUNT) {
            make_eligible_for_pull_hat(HAT_MATE_HANDLER);
            unlock_camo(LEG_ANIM_POWERHUNGRY);
        }
    }
    my_conf_write_crc();
}

uint8_t is_camo_avail(uint8_t camo_id) {
    return (my_conf.camo_unlocks & ((uint32_t)1 << camo_id)) ? 1 : 0;
}

void unlock_camo(uint8_t camo_id) {
    if (is_camo_avail(camo_id))
        return;
    my_conf.camo_unlocks |= (uint32_t)1 << camo_id;
    my_conf.camo_id = camo_id;
    my_conf_write_crc();
    tentacle_start_anim(my_conf.camo_id, LEG_CAMO_INDEX, 1, 1);
}

void lock_camo(uint8_t camo_id) {
    if (!is_camo_avail(camo_id))
        return;
    my_conf.camo_unlocks &= ~((uint32_t)1 << camo_id);

    uint8_t new_camo = my_conf.camo_id;

    do {
        new_camo = (new_camo+LEG_ANIM_COUNT-1) % LEG_ANIM_COUNT;
    } while (!is_camo_avail(new_camo));

    my_conf.camo_id = new_camo;
    my_conf_write_crc();
    tentacle_start_anim(my_conf.camo_id, LEG_CAMO_INDEX, 1, 1);
}

void check_button_presses() {
    // Be sure to check buttons_pressed, and to reset it after accepting the code.
    if (buttons_pressed >= 48 && ((button_press_window & 0x0000ffffffffffff) == COLLINSCODE)) {
        buttons_pressed = 0;
        // Collin's code:
        do_brightness_correction(light_order+5, 1);
        make_eligible_for_pull_hat(HAT_CONTEST);
        tentacle_start_anim(LEG_ANIM_POOLPARTY, 2, 6, 0);
    }
}

void save_inks_and_check() {
    if (my_conf.ink_count == UINT16_MAX)
        my_conf.ink_count = 256;

    if (my_conf.ink_count >= 50) {
        unlock_camo(LEG_ANIM_KARATEKID);
    }
    if (my_conf.ink_count >= 20) {
        unlock_camo(LEG_ANIM_FIRE);
    }

    if (my_conf.ink_margin > 10000) my_conf.ink_margin = 10000;
    if (my_conf.ink_margin < -10000) my_conf.ink_margin = -10000;
    if (my_conf.ink_count > 200 && !(my_conf.ink_count % 128) && !my_conf.freeze_ink_margin) {
        // OK to check margin
        if (my_conf.ink_margin > 500) {
            my_conf.freeze_ink_margin = 1;
            make_eligible_for_pull_hat(HAT_MARGIN_HIGH);
        } else if (my_conf.ink_margin < -500) {
            my_conf.freeze_ink_margin = 1;
            make_eligible_for_pull_hat(HAT_MARGIN_LOW);
        } else if (my_conf.ink_margin < 50 && my_conf.ink_margin > -50) {
            my_conf.freeze_ink_margin = 1;
            make_eligible_for_pull_hat(HAT_LOW_MARGIN);
        }
    }

    if (my_conf.dink_count >= 50) {
        make_eligible_for_pull_hat(HAT_DINK_50);
    } else if (my_conf.dink_count) {
        make_eligible_for_pull_hat(HAT_SUPER_INK);
        unlock_camo(LEG_ANIM_DOUBLEINK);
    }

    my_conf_write_crc();
}
