/*
 * qcxi.h
 *
 * (c) 2014 George Louthan
 * 3-clause BSD license; see license.md.
 */

#ifndef QC13_H_
#define QC13_H_

#include <stdint.h>
#include <driverlib/MSP430FR5xx_6xx/driverlib.h>

void usci_a_send(uint16_t base, uint8_t data);

// Let's try to avoid using this too much please:
void delay_millis(unsigned long);

// Configuration flags
#define BADGE_TARGET 1
#define BADGES_IN_SYSTEM 240

// Configuration of pins for the badge and launchpad
#if BADGE_TARGET
	// Target is the actual badge:
	#include <msp430fr5949.h>

#else
	// Target is the Launchpad+shield:
	#include <msp430fr5969.h>

#endif

// Useful defines:
#define GPIO_pulse(port, pin) do { GPIO_setOutputHighOnPin(port, pin); GPIO_setOutputLowOnPin(port, pin); } while (0)

extern uint16_t light;
extern uint16_t temp;

typedef struct {
	uint8_t to_addr, from_addr, base_id, clock_authority;
	uint8_t prop_from;
	uint8_t prop_id;
	uint16_t prop_time_loops_before_start;
	uint8_t beacon;
} qcpayload;

extern qcpayload in_payload, out_payload;

#endif /* QC13_H_ */
