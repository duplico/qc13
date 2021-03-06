/*
 * mating.h
 *
 *  Created on: Jul 13, 2016
 *      Author: George
 */

#ifndef MATING_H_
#define MATING_H_

#define MATE_NUM_SYNC_BYTES 4
extern const uint8_t mate_sync_bytes[MATE_NUM_SYNC_BYTES];

#define MATE_PROTO_ACHIEVEMENTS BIT7 //1=achievements, 0=badge
#define MATE_PROTO_BADGE BIT6 // Version2
#define MATE_CRC_SEED 0x0000

#define MS_IDLE 0
#define MS_PLUG 1
#define MS_HALF_PAIR 2
#define MS_PAIRED 3
#define MS_INK_WAIT 4
#define MS_SUPER_INK 5
#define MS_PIPE_PAIRED 6
#define MS_PIPE_DONE 7
#define MS_UBER_HAT_OFFER 8

extern uint8_t mate_state;
extern uint16_t mate_ink_wait;
extern uint8_t super_ink_waits_on_me;

void init_mating();
void mate_send_basic(uint8_t click, uint8_t rst, uint8_t gild);
void mate_send_hat_response(uint8_t ack);
void mate_send_uber_hat_bestow();
void mate_send_flags(uint16_t flags);
uint8_t award_push_hat(uint8_t id);
void mate_deferred_rx_interrupt();
void maybe_enter_ink_wait(uint8_t local);
void mate_over_cleanup();
void enter_super_inking();
void ink_wait_timeout();
void super_ink_timeout();

#endif /* MATING_H_ */
