/*
 * qcxi.h
 *
 * (c) 2014 George Louthan
 * 3-clause BSD license; see license.md.
 */

#ifndef QC13_H_
#define QC13_H_

#include <msp430fr5949.h>
#include <stdint.h>
//#include <driverlib/MSP430FR5xx_6xx/driverlib.h>
#include <driverlib.h>

void usci_a_send(uint16_t base, uint8_t data);

// Let's try to avoid using this too much please:
void delay_millis(unsigned long);

#define BADGE_TARGET 1
#define BADGE_ID 163

/////////////////////////////////////////////////////////////////////
// Badge & system configuration /////////////////////////////////////

#define BADGES_IN_SYSTEM 250
#define EVENTS_IN_SYSTEM 1
#define SLEEP_BITS LPM1_bits // We need SMCLK at all times.

// Special badge system setup:

#define UBER_MIN_INCLUSIVE 0
#define UBER_MAX_INCLUSIVE 12
#define HANDLER_MIN_INCLUSIVE 7
#define HANDLER_MAX_INCLUSIVE 14
#define DONOR_MIN_INCLUSIVE 15
#define DONOR_MAX_INCLUSIVE 18
#define EVAN_ID 1
#define GEORGE_ID 0
#define JASON_ID 7
#define JONATHAN_ID 9

#define UBER_COUNT UBER_MAX_INCLUSIVE + 1 - UBER_MIN_INCLUSIVE
#define HANDLER_COUNT HANDLER_MAX_INCLUSIVE + 1 - HANDLER_MIN_INCLUSIVE


#define RF_RESEND_COUNT 3

#define FACE_DIM_BRIGHTNESS 0x08f0

#define TIME_LOOP_PERIOD 50
#define LOOPS_PER_SECOND 697

#define RECEIVE_WINDOW 10
#define BEACON_INTERVAL_SECS 10 // TODO

#define ADC_WINDOW 32

#define SUPER_INK_WINDOW_SECS 1
#define SUPER_INK_DECAY_SECS 10

#define BADGE_SEEN_BITS BIT7
#define ODH_SEEN_BITS BIT6
#define BADGE_MATED_BITS BIT5
#define ODH_MATED_BITS BIT4

/////////////////////////////////////////////////////////////////////
// Hardware related defines /////////////////////////////////////////

#define GPIO_pulse(port, pin) do { GPIO_setOutputHighOnPin(port, pin); GPIO_setOutputLowOnPin(port, pin); } while (0)

/////////////////////////////////////////////////////////////////////
// State constants //////////////////////////////////////////////////

#define DEDICATED_BASE_ID 254
#define NOT_A_BASE 253

// Button events:
#define BUTTON_PRESS 1
#define BUTTON_RELEASE 2

// LED sending types:
#define TLC_SEND_IDLE     0
#define TLC_SEND_TYPE_GS  1
#define TLC_SEND_TYPE_FUN 2
#define TLC_SEND_TYPE_LB  3

// Important structs:

typedef struct {
    uint8_t proto_version;
    uint8_t from_addr, base_addr;
    uint8_t ttl;
    uint8_t ink_id;
    uint8_t flags;
    uint16_t crc16;
} rfbcpayload;

#define RFBC_BEACON BIT0
#define RFBC_EVENT BIT1
#define RFBC_INK BIT2
#define RFBC_DINK BIT3
#define RFBC_HAT_ON BIT4
#define RFBC_HATHOLDER BIT5
#define RFBC_PUSH_HAT_ELIGIBLE BIT6
#define RFBC_HANDLER_ON_DUTY BIT7

typedef struct {
    uint8_t proto_version;
    uint8_t to_addr; // redundant
    uint8_t hat_id;
    uint16_t crc16;
} rfucpayload;

typedef struct {
    uint8_t proto_version;
    uint8_t from_addr;
    uint8_t hat_award_id;
    uint8_t camo_id;
    uint16_t flags;
    uint64_t achievements;
    uint16_t crc16;
} matepayload;

#define MATE_VERSION 1

#define M_ACHIEVEMENT BIT0 // 0=mate; 1=achievement
#define M_HAT_AWARD BIT1
#define M_HAT_AWARD_ACK BIT2
#define M_HAT_AWARD_NACK BIT3
#define M_INK BIT4
#define M_HAT_HOLDER BIT5
#define M_RST BIT6
#define M_PUSH_HAT_ELIGIBLE BIT7
#define M_HANDLER_ON_DUTY BIT8
#define M_PIPE BIT9 // 0=badge; 1=pipe
#define M_BESTOW_HAT BITA
#define M_RQ_ACHIEVEMENTS BITB
#define M_BESTOW_ACH_0 BITC
#define M_BESTOW_ACH_1 BITD

typedef struct {
    uint8_t badge_id;
    uint8_t seen_count, uber_seen_count, odh_seen_count;
    uint8_t mate_count, uber_mate_count, odh_mate_count;
    uint16_t event_checkins;
    uint32_t camo_unlocks;
    uint8_t camo_id;
    uint64_t achievements;
    uint16_t crc16;
} qc13conf;

/////////////////////////////////////////////////////////////////////
// Global declarations //////////////////////////////////////////////

extern volatile uint8_t f_time_loop;
extern volatile uint8_t f_rfm75_interrupt;
extern volatile uint8_t f_mate_interrupt;

extern uint8_t s_face_anim_done;

extern uint16_t light;
extern uint16_t light_tot;
extern uint8_t light_order;
extern uint16_t temp;
extern qc13conf my_conf;
extern const qc13conf default_conf;
extern rfbcpayload in_payload, out_payload;

extern uint8_t badges_seen[BADGES_IN_SYSTEM];
extern uint8_t badges_mated[BADGES_IN_SYSTEM];
extern uint8_t neighbor_badges[BADGES_IN_SYSTEM];
extern uint8_t neighbor_count;

#endif /* QC13_H_ */
