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
#define BADGE_ID 0xAA

#define COLLINSCODE 0b011000110110111101101111011010110110100101100101

/////////////////////////////////////////////////////////////////////
// Badge & system configuration /////////////////////////////////////

#define BADGES_IN_SYSTEM 250
#define EVENTS_IN_SYSTEM 10
#define SLEEP_BITS LPM1_bits // We need SMCLK at all times.
#define POWER_CYCLES_FOR_HAT 100
#define LIGHT_ORDER_MAX 7
#define INKS_PER_MINUTE 3

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

#define UBER_COUNT (UBER_MAX_INCLUSIVE + 1 - UBER_MIN_INCLUSIVE)
#define HANDLER_COUNT (HANDLER_MAX_INCLUSIVE + 1 - HANDLER_MIN_INCLUSIVE)

#define HAT_STATE_WAIT_SECS 3
#define INK_OUT_COOLDOWN_SECS 6

#define RF_RESEND_COUNT 2

#define FACE_DIM_BRIGHTNESS 0x08f0

#define TIME_LOOP_PERIOD 50
#define LOOPS_PER_SECOND 697
#define SECONDS_PER_MINUTE 45 // because fuck you, that's why.

#define LED_DUR_LOOPS 10

#define FACE_DUR_STEP (2 * LED_DUR_LOOPS)
#define LEGS_DUR_STEP (2 * LED_DUR_LOOPS)

#define RECEIVE_WINDOW 10
#define BEACON_INTERVAL_SECS 10

#define LONG_PRESS_THRESH 2

#define ADC_WINDOW 32

#define TEMP_THRESH_HOT  1100
#define TEMP_THRESH_COLD  800

#define SUPER_INK_WINDOW_SECS 1
#define SUPER_INK_DECAY_SECS 10

#define BADGE_SEEN_BITS BIT7
#define ODH_SEEN_BITS BIT6
#define BADGE_MATED_BITS BIT5
#define ODH_MATED_BITS BIT4

#define DEFAULT_EYES    0b1000010000100000111111111111111010000100001000001111111111111110
#define ANGRY_EYES      0b0111111000100000000001111111111001111110001000000000011111111110
#define CLOSED_EYES     0b0000011111110000000000000000000000000111111100000000000000000000
#define GIGGITY_EYES    0b1000011111110000111110000011111010000111111100001111100000111110
#define INKING_EYES     0b1000011111110000111110000011111010000111111100001111100000111110

/////////////////////////////////////////////////////////////////////
// Hardware related defines /////////////////////////////////////////

#define GPIO_pulse(port, pin) do { GPIO_setOutputHighOnPin(port, pin); GPIO_setOutputLowOnPin(port, pin); } while (0)

#define SMCLK_RATE_KHZ 16000
#define SMCLK_RATE_HZ  16000000

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
    uint8_t badge_addr, base_addr; // base_addr is event_id, basically.
    uint8_t ttl;
    uint8_t ink_id;
    uint8_t flags;
    uint32_t seqnum;
    uint16_t crc16;
} rfbcpayload;

#define RFBC_BEACON BIT0
#define RFBC_EVENT BIT1
#define RFBC_INK BIT2
#define RFBC_DINK BIT3
#define RFBC_HATACK BIT4
#define RFBC_HATHOLDER BIT5
#define RFBC_HATOFFER BIT6
#define RFBC_HANDLER_ON_DUTY BIT7

typedef struct {
    uint8_t proto_version;
    uint8_t from_addr;
    uint8_t hat_award_id;
    uint8_t camo_id;
    uint16_t flags;
    uint64_t achievements;
    uint8_t seen_count, uber_seen_count, odh_seen_count;
    uint8_t mate_count, uber_mate_count, odh_mate_count;
    uint16_t crc16;
} matepayload;

#define MATE_VERSION 2

#define M_HAT_AWARD BIT1 // Award = PUSH hat. From badge.
#define M_HAT_AWARD_ACK BIT2
#define M_HAT_AWARD_NACK BIT3
#define M_INK BIT4
#define M_HAT_HOLDER BIT5
#define M_RST BIT6
#define M_BADGE_HAS_CLAIMED_HAT BIT7 // comes from badge
#define M_HANDLER_ON_DUTY BIT8
#define M_PIPE BIT9 // 0=badge; 1=pipe
#define M_HAT_CLAIM_FROM_PIPE BITA // comes from pipe
#define M_REPRINT_HAT BITB
#define M_BESTOW_GILD BITF

#define GILD_ON BIT0
#define GILD_AVAIL BIT1

typedef struct {
    uint8_t badge_id;
    uint8_t seen_count, uber_seen_count, odh_seen_count;
    uint8_t mate_count, uber_mate_count, odh_mate_count;
    uint8_t hat_holder, hat_claimed, hat_id;
    uint8_t event_checkins[EVENTS_IN_SYSTEM];
    uint32_t camo_unlocks;
    uint8_t camo_id;
    uint8_t uber_hat_given;
    uint64_t achievements;
    uint8_t gilded;
    int16_t ink_margin;
    uint16_t ink_count, dink_count;
    uint16_t power_cycles;
    uint8_t been_hot, been_cold;
    uint8_t been_bright, been_dark;
    uint8_t uptime;
    uint8_t freeze_ink_margin, freeze_minuteman;
    uint16_t crc16;
} qc13conf;

// Hats:
#define HAT_GEORGE 0
#define HAT_EVAN 1
#define HAT_TPROPHET 2
#define HAT_ZAC 3
#define HAT_ERIC 4
#define HAT_SHAUN 5
#define HAT_MARCUS 6
#define HAT_JASON 7
#define HAT_AARON 8
#define HAT_JONATHAN 9
#define HAT_JAKE 10
#define HAT_COLLIN 11
#define HAT_ALYSSA 12
#define HAT_DONOR_REED 13
#define HAT_DONOR_HEATHER 14
#define HAT_DONOR_ERIC 15
#define HAT_DONOR_CHRIS 16
#define HAT_BADGE_TALK 17
#define HAT_POOL_FIRST 18
#define HAT_POOL_LAST 19
#define HAT_MIXER_SAT_FIRST 20
#define HAT_KARAOKE_FIRST 21
#define HAT_KARAOKE_LAST 22
#define HAT_PEST 23
#define HAT_MORNING 24
#define HAT_HOT_COLD 25
#define HAT_BRIGHT_DARK 26
#define HAT_COLD 27
#define HAT_HOT 28
#define HAT_MATE_UBER 29
#define HAT_MATE_HANDLER 30
#define HAT_MATE_50 31
#define HAT_MATE_100 32
#define HAT_MATE_200 33
#define HAT_DINK_50 34
#define HAT_TIME_NEAR_UBERS 35
#define HAT_NEAR_HANDLERS 36
#define HAT_TIME_NEAR_HANDLERS 37
#define HAT_ALL_MIXERS 38
#define HAT_KONAMI 39
#define HAT_MINUTEMAN 40
#define HAT_POWER_CYCLES 41
#define HAT_SUPER_INK 42
#define HAT_MARGIN_HIGH 43
#define HAT_MARGIN_LOW 44
#define HAT_LOW_MARGIN 45
#define HAT_BORROWER 46

#define HAT_CONTEST 50

#define BASE_TRANS 0
#define BASE_BTHUMIX 1
#define BASE_BTALK 2
#define BASE_BFRIMIX 3
#define BASE_BPOOL 4
#define BASE_BWOMEN 5
#define BASE_BSATMIX 6
#define BASE_BKARAOKE 7
#define BASE_BSUNMIX 8
#define BASE_BTHUPARTY 9

#define HAT_UBER 55
#define HAT_HANDLER 56

#define HS_NONE 0
#define HS_WAIT BIT1
#define HS_UBER BIT2
#define HS_HUMAN BIT3
#define HS_HANDLER BIT4
#define HS_UBER_HANDLER BIT4 | BIT2

#define TEMP_COLD 0
#define TEMP_HOT 2
#define TEMP_NORMAL 1

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
extern uint8_t temp_band;
extern qc13conf my_conf;
extern qc13conf backup_conf;
extern const qc13conf default_conf;
extern rfbcpayload in_payload, out_payload, cascade_payload;
extern uint8_t payload_cascade;

extern uint8_t badges_seen[BADGES_IN_SYSTEM];
extern uint8_t odh_badges_ticks[HANDLER_MAX_INCLUSIVE+1];
extern uint8_t badges_mated[BADGES_IN_SYSTEM];
extern uint8_t neighbor_badges[BADGES_IN_SYSTEM];
extern uint8_t uber_badges_ticks[UBER_COUNT];
extern uint8_t neighbor_count;

extern uint8_t hat_state;

void my_conf_write_crc();

#endif /* QC13_H_ */
