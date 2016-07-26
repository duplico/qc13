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
    // Have we seen it at all? If not, increment count and maybe uber count.
    // Is it ODH, and have we seen it as ODH? If not, increment.

    if (!badges_seen[id]) {
        // Haven't seen it at all.
        badges_seen[id] = BADGE_SEEN_BITS;
    }

    // TODO: Check for achievements.
}

void tick_badge_seen(uint8_t id, uint8_t handler_on_duty) {
    // TODO: Handle incrementing ubers' and handlers' timers.
}
