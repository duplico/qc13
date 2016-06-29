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
void radio_received(qcpayload *payload);
void radio_transmit_done();
void leg_anim_done(uint8_t tentacle_anim_id);
void second();

#endif /* BADGE_H_ */
