/*
 * metrics.h
 *
 *  Created on: Jul 15, 2016
 *      Author: George
 */

void set_badge_seen(uint8_t id, uint8_t handler_on_duty);
void tick_badge_seen(uint8_t id, uint8_t handler_on_duty);
void set_badge_mated(uint8_t id, uint8_t handler_on_duty);
