/*
 * badge.h
 *
 *  Created on: Jun 27, 2016
 *      Author: George
 */

#ifndef BADGE_H_
#define BADGE_H_

void initial_animations(); // Called when it's time to start the first ambients
void time_loop(); // Called every time_loop_interval
void send_super_ink(); // Called when time to do a double ink.
void radio_beacon_interval(); // Called when it's time for the radio to beacon.
void new_badge_seen(uint8_t deferred); // Called when we've seen a new badge
void new_badge_mated(); // Called when we've mated to a new badge.
void start_button_clicked(); // The start button was pressed and released
void start_button_longpressed(); // Start button press & hold (fired before release)
void select_button_longpressed(); // Select button press & hold (fired before release)
void hat_change(uint8_t from, uint8_t to); // The hat we're wearing has changed.
void temp_band_change(uint8_t from, uint8_t to); // Temp band has changed
void light_band_change(uint8_t from, uint8_t to); // Temp band has changed
void select_button_clicked(); // The select button was pressed and released
void radio_ink_received(uint8_t ink_id, uint8_t ink_type, uint8_t from_addr);
void radio_broadcast_received(rfbcpayload *payload); // Received a radio message
void radio_transmit_done(); // Finished transmitting a radio message
void leg_anim_done(uint8_t tentacle_anim_id); // Leg animation finished
void face_animation_done(); // Eye animation finished
void second(); // Called once per second (ish)
void minute(); // Called once per 2ish seconds
void mate_plug(); // Called when badges connected
void mate_start(uint8_t badge_id, uint8_t handler_on_duty); // Called when badges mated (pair)
void mate_end(uint8_t badge_id); // Called when badges unmated
void borrowing_hat();

extern uint8_t just_sent_superink;
extern uint8_t being_inked;
extern uint8_t waking_up;
extern uint64_t button_press_window;
extern uint8_t buttons_pressed;

#endif /* BADGE_H_ */
