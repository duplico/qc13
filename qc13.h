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
#include <driverlib/MSP430FR5xx_6xx/driverlib.h>

void usci_a_send(uint16_t base, uint8_t data);

// Let's try to avoid using this too much please:
void delay_millis(unsigned long);

#define BADGE_TARGET 1

/////////////////////////////////////////////////////////////////////
// Badge & system configuration /////////////////////////////////////

#define BADGES_IN_SYSTEM 250
#define SLEEP_BITS LPM1_bits // We need SMCLK at all times.

/////////////////////////////////////////////////////////////////////
// Hardware related defines /////////////////////////////////////////

#define GPIO_pulse(port, pin) do { GPIO_setOutputHighOnPin(port, pin); GPIO_setOutputLowOnPin(port, pin); } while (0)

/////////////////////////////////////////////////////////////////////
// State constants //////////////////////////////////////////////////

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
    uint8_t to_addr, from_addr, base_id, clock_authority;
    uint8_t prop_from;
    uint8_t prop_id;
    uint16_t prop_time_loops_before_start;
    uint8_t beacon;
} qcpayload;

typedef struct {
    uint8_t badge_id;
    uint8_t seen_count;
    uint8_t uber_seen_count;
    uint8_t mate_count;
    uint8_t uber_mate_count;
    uint16_t bases_seen;
    uint16_t camo_unlocks;
    uint_fast32_t uptime;
    uint8_t suite_minutes;
    uint16_t crc16;
} qc13conf;

/////////////////////////////////////////////////////////////////////
// Global declarations //////////////////////////////////////////////

extern volatile uint8_t f_time_loop;
extern volatile uint8_t f_tentacle_anim_done;


extern volatile uint16_t light;
extern volatile uint16_t temp;
extern qc13conf my_conf;
extern const qc13conf default_conf;
extern qcpayload in_payload, out_payload;

extern uint8_t badges_seen[BADGES_IN_SYSTEM];
extern uint8_t neighbor_badges[BADGES_IN_SYSTEM];
extern uint8_t neighbor_count;

#endif /* QC13_H_ */
