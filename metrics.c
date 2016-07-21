/*
 * metrics.c
 *
 *  Created on: Jul 15, 2016
 *      Author: George
 */

#include "qc13.h"
#include "metrics.h"
#include "badge.h"

void set_badge_seen(uint8_t id, uint8_t handler_on_duty) {
    if (badges_seen[id])
        return; // already seen.

    // It's new! :-D

    badges_seen[id] = 1;
    my_conf.seen_count++;

    if (id < UBER_COUNT && my_conf.uber_seen_count < UBER_COUNT) {
        // it's uber:
        my_conf.uber_seen_count++;
    }

    if (handler_on_duty && my_conf.odh_seen_count < HANDLER_COUNT) {
        // On-duty handler:
        my_conf.odh_seen_count++;
    }

    new_badge_seen(0);

    // TODO: Check for achievements.
}

void tick_badge_seen(uint8_t id, uint8_t handler_on_duty) {
    // TODO: Handle incrementing ubers' and handlers' timers.
}

void set_badge_mated(uint8_t id, uint8_t handler_on_duty) {
    if (badges_mated[id])
        return; // already seen.

    // It's new! :-D

    badges_mated[id] = 1;
    my_conf.seen_count++;

    if (id < UBER_COUNT && my_conf.uber_seen_count < UBER_COUNT) {
        // it's uber:
        my_conf.uber_seen_count++;
    }

    if (handler_on_duty && my_conf.odh_seen_count < HANDLER_COUNT) {
        // On-duty handler:
        my_conf.odh_seen_count++;
    }

    new_badge_mated();

    // TODO: Check for achievements.
}
