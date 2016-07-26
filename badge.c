/*
 * badge.c
 *
 *  Created on: Jun 27, 2016
 *      Author: George
 */

#include "qc13.h"
#include "qc12.h"
#include <stdlib.h>
#include "rfm75.h"
#include "metrics.h"
#include "badge.h"

uint8_t badges_seen[BADGES_IN_SYSTEM] = {0};
uint8_t badges_mated[BADGES_IN_SYSTEM] = {0};
uint8_t neighbor_badges[BADGES_IN_SYSTEM] = {0};
uint8_t neighbor_count = 0;

uint8_t blink_repeat_count = 0;

uint8_t ink_cooldown = 0;

qc13conf my_conf = {0};

uint64_t button_press_window = 0;
uint8_t buttons_pressed = 0;

uint8_t hat_award_state = 0;
uint8_t hat_award_offered = 0;
uint8_t hat_award_to = DEDICATED_BASE_ID;
uint8_t hat_award_tries = 10;

/*
 * typedef struct {
    uint8_t badge_id;
    uint8_t seen_count, uber_seen_count, odh_seen_count;
    uint8_t hat_sent_talk, hat_sent_pool_start, hat_sent_pool_end, hat_sent_sat_start, hat_sent_sat_end;
    uint8_t locked;
    uint8_t base_id;
    uint16_t crc16;
} qc13conf;
 */

const qc13conf default_conf = {
        DEDICATED_BASE_ID, // badge_id
        0, 0, 0, // seen_count, uber_seen_count, odh_seen_count;
        0, 0, 0, 0, 0, // hat_sent_talk, hat_sent_pool_start, hat_sent_pool_end, hat_sent_sat_start, hat_sent_sat_end;
        1, // locked
        NOT_A_BASE, // base_id
        0 // crc16
};

rfbcpayload in_payload, out_payload;

void second() {
    if (hat_award_state == HAS_OFFER) {
        send_hat_award();
    }
}

void two_seconds() {
}

void face_animation_done() {
}

// IMPORTANT: Call this last.
//  DON'T change stuff after calling it.
void complete_rfbc_payload(rfbcpayload *payload) {
    // TODO: not if retx from someone else:
    payload->seqnum = rfm75_seqnum;
    payload->base_addr = my_conf.base_id;

    // CRC it.
    CRC_setSeed(CRC_BASE, RFM75_CRC_SEED);
    for (uint8_t i = 0; i < sizeof(rfbcpayload) - 2; i++) {
        CRC_set8BitData(CRC_BASE, ((uint8_t *) payload)[i]);
    }
    payload->crc16 = CRC_getResult(CRC_BASE);
}

// TODO: enter hat awarding state machine:

void award_hat(uint8_t hat_id) {
    if (hat_award_state != HAS_IDLE) return; // reject!
    hat_award_state = HAS_OFFER;
    hat_award_offered = hat_id;

    if (!neighbor_count) { // If nobody's around...
        hat_award_state = HAS_FAIL;
        return; // no neighbor badges.
    }

    // If we do have neighbors, give it to one of them.
    uint8_t target_badge = rand() % BADGES_IN_SYSTEM;
    while (!neighbor_badges[target_badge]) {
        target_badge = (target_badge+1) % BADGES_IN_SYSTEM;
    }

    hat_award_to = target_badge;
    hat_award_tries = 10; // TODO
    send_hat_award();
}

void send_hat_award() {
    if (!hat_award_tries || hat_award_state != HAS_OFFER) {
        // give up.
        hat_award_state = HAS_FAIL;
        return;
    }

    hat_award_tries--;
    out_payload.badge_addr = hat_award_to;
    out_payload.ink_id = hat_award_offered;
    out_payload.flags = RFBC_HATOFFER;
    complete_rfbc_payload(&out_payload);
    rfm75_tx();
}

void send_beacon() {
    out_payload.badge_addr = DEDICATED_BASE_ID;
    out_payload.ink_id = 211; // NOT_AN_INK
    out_payload.flags = RFBC_EVENT;
    complete_rfbc_payload(&out_payload);
    rfm75_tx();
}

void not_lonely() {
}

void new_badge_seen(uint8_t deferred) {
}

void new_badge_mated() {
    // The animation is handled elsewhere.
}

void radio_beacon_interval() {
    uint8_t next_neighbor_count = 0;
    for (uint8_t i=0; i<BADGES_IN_SYSTEM; i++) {
        if (neighbor_badges[i]) {
            next_neighbor_count++;
            neighbor_badges[i]--;
        }
    }

    if (next_neighbor_count && !neighbor_count) {
        //  gone from alone to not alone.
        not_lonely();
    }

    neighbor_count = next_neighbor_count;

    // Now do our beacon:
    send_beacon();
}

void radio_beacon_received(uint8_t from_id, uint8_t on_duty) {
    neighbor_badges[from_id] = RECEIVE_WINDOW;
    set_badge_seen(from_id, on_duty);
    tick_badge_seen(from_id, on_duty);
}

void radio_broadcast_received(rfbcpayload *payload) {
    //     ==Broadcast==
    //  # Beacon: Just the normal gaydar beacon
    //    We only care if they aren't a hatholder:
    if (payload->flags & RFBC_BEACON && !(payload->flags & RFBC_HATHOLDER)) {
        radio_beacon_received(payload->badge_addr, payload->flags & RFBC_HANDLER_ON_DUTY);
    }

    // TODO: there is a possible race condition here. It's not likely, though.
    //  only really happens if an uber gives one at the same time...
    if (hat_award_state == HAS_OFFER && (payload->flags & RFBC_HATACK || payload->flags & RFBC_HATHOLDER) && payload->badge_addr == hat_award_to) {
        // We had an offer out, and it was ACKed or we saw that the person we offered it to became a hat holder:
        hat_award_state = HAS_SUCCEED;
    }
}

void radio_transmit_done() {
}


void borrowing_hat() {
}
