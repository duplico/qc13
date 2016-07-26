/*
 * qcxi.h
 *
 * (c) 2014 George Louthan
 * 3-clause BSD license; see license.md.
 */

#ifndef QC12_H_
#define QC12_H_

#include <stdint.h>
#include <driverlib/MSP430FR5xx_6xx/driverlib.h>
#include <msp430fr5949.h>
#include <grlib.h>
#include "qc13.h"

// CONFIGURATION SETTINGS ///////////////////////////////////////////

// From our hacked-together build script:

// System configuration
#define BASES_IN_SYSTEM 7
#define UBERS_IN_SYSTEM 12
#define NUM_ACHIEVEMENTS 28
#define FAVORITE_COUNT 3
#define RADIO_TX_POWER_DB 6 // [-18 .. 13]
#define FLAG_OUT_COOLDOWN_MINUTES 10
#define FLAG_IN_COOLDOWN_SECONDS 10
#define FLAG_SEND_TRIES 3

// Name entry configuration parameters:
#define NAME_SEL_CHAR '*'
#define NAME_MAX_LEN 14
// this will break everything if it's 1 or possibly 2:
#define NAME_COMMIT_LOOPS 120

// Character name & title font:
#define NAME_FONT_HEIGHT 14
#define NAME_FONT g_sFontCmss12b

// Main system font:
#define SYS_FONT g_sFontCmss12b
#define SYS_FONT_HEIGHT 12

// Softkey label font:
#define SOFTKEY_LABEL_FONT SYS_FONT
#define SOFTKEY_FONT_HEIGHT 12

//////////////////////////
// Derived definitions ///

#define NAME_Y_OFFSET 10+SYS_FONT_HEIGHT*7 // Top of the name entry field.
#define SPRITE_Y 64 - SOFTKEY_FONT_HEIGHT - 3 // Top of the char sprite.

/////////////////////////////////////////////////////////////////////
// State constants //////////////////////////////////////////////////

// Button events:
#define BUTTON_PRESS 1
#define BUTTON_RELEASE 2

// Overall operating modes:
#define OP_MODE_IDLE 0
#define OP_MODE_UNLOCK 1
#define OP_MODE_TXT 2

// Softkey options:
// Softkey options:
#define SK_SEL_UNLOCK 0
#define SK_SEL_LOCK 1
#define SK_SEL_BOFF 2
#define SK_SEL_BTRANS 3
#define SK_SEL_BTHUMIX 4
#define SK_SEL_BTALK 5
#define SK_SEL_BFRIMIX 6
#define SK_SEL_BPOOL 7
#define SK_SEL_BWOMEN 8
#define SK_SEL_BSATMIX 9
#define SK_SEL_BKARAOKE 10
#define SK_SEL_BSUNMIX 11
#define SK_SEL_TRIGGER_START 12
#define SK_SEL_TRIGGER_END 13

#define SK_MASK_STARTABLE 0x0FFF
#define SK_MASK_ENDABLE   0x17FF
#define SK_MASK_NOPUSH    0x07FF

#define SK_SEL_MAX SK_SEL_TRIGGER_END

#define SK_BIT_UNLOCK BIT0
#define SK_BIT_LOCK BIT1
#define SK_BIT_BOFF BIT2

#define SK_BIT_MAX BITD

extern const char sk_labels[SK_SEL_MAX+1][12];
extern uint16_t softkey_en;

// Radio bidness:
#define BASE_SUITE 0
#define BASE_POOL  1
#define BASE_KICKOFF 2
#define BASE_MIXER 3
#define BASE_TALK  4

extern uint8_t idle_mode_softkey_sel;
extern uint8_t idle_mode_softkey_dis;
extern uint8_t op_mode;
extern const char base_labels[][12];

void delay(unsigned int i);

#endif /* QC12_H_ */
