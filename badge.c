/*
 * badge.c
 *
 *  Created on: Jun 27, 2016
 *      Author: George
 */

#include "qc13.h"
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

const qc13conf default_conf = {
        BADGE_ID,
        0, 0, 0, // seen counts
        0, 0, 0, // mate counts
        0, 0, 0, // hats!
        {0}, // event check-ins
        0, // camo_unlocks bitfield
        0, // camo_id
        0, // Uber hat not given out
        0, // No achievements
        0, // Not gilded.
        1, // Locked
        NOT_A_BASE, // Base ID.
        0 // blank CRC.
};

rfbcpayload in_payload, out_payload;

void second() {

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

void send_hat_award(uint8_t to_id, uint8_t hat_id) {
    out_payload.badge_addr = to_id;
    out_payload.ink_id = hat_id;
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

//    if (payload->flags & RFBC_HATACK) // TODO: state machine
}

void radio_transmit_done() {
}


void borrowing_hat() {
}
