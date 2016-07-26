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
    // TODO: validate id
    if (my_conf.hat_holder)
        return 0; // TODO: This can't work.
    my_conf.achievements |= ((uint64_t) 0x01 << id);
    my_conf.hat_holder = 1;
    my_conf.hat_id = id;
    my_conf_write_crc();
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
    // TODO: validation?
    my_conf.hat_claimed = 1;
    my_conf_write_crc();
}

uint8_t event_checkin(uint8_t event_id) {
    /*
     * Possible events:
     * QC Trans Meetup - awards camo found
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
        unlock_camo(LEG_ANIM_FOUND);
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
    }

    // TODO: Check for achievements.
    my_conf_write_crc();
}

void tick_badge_seen(uint8_t id, uint8_t handler_on_duty) {
    // TODO: Handle incrementing ubers' and handlers' timers.
}

void set_badge_mated(uint8_t id, uint8_t handler_on_duty) {
    // Have we mated w/ it at all? If not, increment count and maybe uber count.
    // Is it ODH, and have we mated w/ it as ODH? If not, increment.

    if (!badges_mated[id]) {
        // Haven't mated w/ it at all.

        badges_mated[id] = BADGE_MATED_BITS;
        my_conf.mate_count++;

        if (is_uber(id) && my_conf.uber_mate_count < UBER_COUNT) {
            // it's uber:
            my_conf.uber_mate_count++;
        }

        if (id != my_conf.badge_id)
            new_badge_mated();
    }

    if (handler_on_duty && is_handler(id) && !(badges_mated[id] & ODH_MATED_BITS) && my_conf.odh_mate_count < HANDLER_COUNT) {
        // It's an on-duty handler, and we haven't mated w/ it on duty before.
        my_conf.odh_mate_count++;
        badges_mated[id] |= ODH_MATED_BITS;
    }
    // TODO: Check for achievements.
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
        // TODO: do a thing.
        buttons_pressed = 0;
    }
}
