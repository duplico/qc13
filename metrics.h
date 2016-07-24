/*
 * metrics.h
 *
 *  Created on: Jul 15, 2016
 *      Author: George
 */

uint8_t award_hat(uint8_t id);
void claim_hat(uint8_t id);
void set_badge_seen(uint8_t id, uint8_t handler_on_duty);
void set_badge_mated(uint8_t id, uint8_t handler_on_duty);
void tick_badge_seen(uint8_t id, uint8_t handler_on_duty);
void tick_badge_mated(uint8_t id, uint8_t handler_on_duty);
uint8_t is_uber(uint8_t badge_id);
uint8_t is_handler(uint8_t badge_id);
uint8_t is_donor(uint8_t badge_id);
uint8_t is_gilder(uint8_t badge_id);
uint8_t is_camo_avail(uint8_t camo_id);
void unlock_camo(uint8_t camo_id);
void check_button_presses();
