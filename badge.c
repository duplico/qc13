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
    out_payload.seqnum = rfm75_seqnum;
    payload->base_addr = NOT_A_BASE;
    payload->from_addr = my_conf.badge_id;
    if (is_handler(my_conf.badge_id) && (hat_state & HS_HANDLER))
        payload->flags |= RFBC_HANDLER_ON_DUTY;
    if (my_conf.hat_holder)
        payload->flags |= RFBC_HATHOLDER;

    // CRC it.
    CRC_setSeed(CRC_BASE, RFM75_CRC_SEED);
    for (uint8_t i = 0; i < sizeof(rfbcpayload) - 2; i++) {
        CRC_set8BitData(CRC_BASE, ((uint8_t *) payload)[i]);
    }
    payload->crc16 = CRC_getResult(CRC_BASE);
}

void send_ink() {
    if (ink_cooldown)
        return;
    ink_cooldown = INK_OUT_COOLDOWN_SECS;
    out_payload.ink_id = my_conf.camo_id;
    out_payload.flags = RFBC_INK;
    complete_rfbc_payload(&out_payload);
    rfm75_tx();
}

void send_super_ink() {
    out_payload.ink_id = my_conf.camo_id;
    out_payload.flags = RFBC_INK | RFBC_DINK;
    complete_rfbc_payload(&out_payload);
    rfm75_tx();
}

void send_beacon() {
    out_payload.ink_id = 211;
    out_payload.flags = RFBC_BEACON;
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

void radio_basic_base_received(uint8_t base_id) {
    if (base_id == 0xff) { // TODO: check for the event bases.
        achievement_get(base_id);
    }
}

void radio_ink_received(uint8_t ink_id, uint8_t ink_type, uint8_t from_addr) {
    // ignore.
}

void radio_broadcast_received(rfbcpayload *payload) {
    // There are three possibilities for a broadcast.
    // They're NOT mutually exclusive.
    //     ==Broadcast==
    //  # Beacon: Just the normal gaydar beacon
    if (payload->flags & RFBC_BEACON) {
        radio_beacon_received(payload->from_addr, payload->flags & RFBC_HANDLER_ON_DUTY);
    }
    //  # Basic event: A basic check-in broadcast from the base station
    if (payload->flags & RFBC_EVENT) {
        radio_basic_base_received(payload->base_addr);
    }
    //  # Ink or super ink
    if (payload->flags & RFBC_INK) {
        radio_ink_received(payload->ink_id, ((payload->flags & RFBC_DINK) ? 2 : 1), payload->from_addr);
    }
}

void radio_transmit_done() {
}


void borrowing_hat() {
    // TODO: shutdown
}
