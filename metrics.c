/*
 * metrics.c
 *
 *  Created on: Jul 15, 2016
 *      Author: George
 */

#include "qc13.h"
#include "metrics.h"
#include "badge.h"

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

	    new_badge_seen();
	}

	if (handler_on_duty && is_handler(id) && !(badges_seen[id] & ODH_SEEN_BITS) && my_conf.odh_seen_count < HANDLER_COUNT) {
		// It's an on-duty handler, and we haven't seen it on duty before.
        my_conf.odh_seen_count++;
        badges_seen[id] |= ODH_SEEN_BITS;
	}

    // TODO: Check for achievements.
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

	    new_badge_mated();
	}

	if (handler_on_duty && is_handler(id) && !(badges_mated[id] & ODH_MATED_BITS) && my_conf.odh_mate_count < HANDLER_COUNT) {
		// It's an on-duty handler, and we haven't mated w/ it on duty before.
        my_conf.odh_mate_count++;
        badges_mated[id] |= ODH_MATED_BITS;
	}
    // TODO: Check for achievements.
}
