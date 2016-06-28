/*
 * badge.h
 *
 *  Created on: Jun 27, 2016
 *      Author: George
 */

#ifndef BADGE_H_
#define BADGE_H_

void initial_animations();
void time_loop();
void start_button_clicked();
void select_button_clicked();
void radio_received(uint8_t *payload);
void radio_transmit_done();

#endif /* BADGE_H_ */
