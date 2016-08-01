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
