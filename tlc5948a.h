/*
 * leds.h
 * (c) 2014 George Louthan
 * 3-clause BSD license; see license.md.
 */

#ifndef TLC5948A_H_
#define TLC5948A_H_

#include <stdint.h>

// Defines for pins:

// 1-origined because fuck you
#define LED_BANK1_PORT GPIO_PORT_PJ
#define LED_BANK2_PORT GPIO_PORT_PJ
#define LED_BANK3_PORT GPIO_PORT_PJ
#define LED_BANK4_PORT GPIO_PORT_PJ
#define LED_BANK5_PORT GPIO_PORT_P3
#define LED_BANK6_PORT GPIO_PORT_P3

#define LED_BANK1_OUT PJOUT
#define LED_BANK2_OUT PJOUT
#define LED_BANK3_OUT PJOUT
#define LED_BANK4_OUT PJOUT
#define LED_BANK5_OUT P3OUT
#define LED_BANK6_OUT P3OUT

#define LED_BANK1_PIN GPIO_PIN0
#define LED_BANK2_PIN GPIO_PIN1
#define LED_BANK3_PIN GPIO_PIN2
#define LED_BANK4_PIN GPIO_PIN3
#define LED_BANK5_PIN GPIO_PIN7
#define LED_BANK6_PIN GPIO_PIN6

// Defines for the TLC:
#define TLC_LATPORT GPIO_PORT_P1
#define TLC_LATPIN  GPIO_PIN4

void tlc_init();

uint8_t tlc_test_loopback(uint8_t);

void tlc_set_gs();
void tlc_set_fun();
void tlc_stage_blank(uint8_t);

extern uint8_t tlc_send_type;
extern uint16_t tlc_bank_gs[6][16];

#endif /* TLC5948A_H_ */
