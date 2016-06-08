/*
 * leds.c
 * (c) 2014 George Louthan
 * 3-clause BSD license; see license.md.
 */

#include "qc12.h"
#include "leds.h"
#include <string.h>

/*
 *   LED controller (TLC5948A)
 *        (write on rise, change on fall,
 *         clock inactive low, MSB first)
 *        eUSCI_A0 - LEDs  (shared)
 *        somi, miso, clk
 *        GSCLK     P1.2 (timer TA1.1)
 *        LAT       P1.4
 */

volatile uint8_t f_time_loop = 0; // TODO

void usci_a0_send_sync(uint8_t data) {
    EUSCI_A_SPI_transmitData(EUSCI_A0_BASE, data);
    while (!EUSCI_A_SPI_getInterruptStatus(EUSCI_A0_BASE,
              EUSCI_A_SPI_TRANSMIT_INTERRUPT));
    while (!EUSCI_A_SPI_getInterruptStatus(EUSCI_A0_BASE,
            EUSCI_A_SPI_RECEIVE_INTERRUPT));
    EUSCI_A_SPI_receiveData(EUSCI_A0_BASE); // Throw away the stale garbage we got while sending.
}

uint8_t usci_a0_recv_sync(uint8_t data) {
    EUSCI_A_SPI_transmitData(EUSCI_A0_BASE, data);
    while (!EUSCI_A_SPI_getInterruptStatus(EUSCI_A0_BASE,
              EUSCI_A_SPI_TRANSMIT_INTERRUPT));
    while (!EUSCI_A_SPI_getInterruptStatus(EUSCI_A0_BASE,
            EUSCI_A_SPI_RECEIVE_INTERRUPT));
    return EUSCI_A_SPI_receiveData(EUSCI_A0_BASE);
}

uint16_t gs_data[16] = {
		0xffff,
		0xffff,
		0xffff,
		0xffff,
		0xffff,
		0xffff,
		0xffff,
		0xffff,
		0xffff,
		0xffff,
		0xffff,
		0xffff,
		0xffff,
		0xffff,
		0xffff,
		0xffff,
};

void tlc_set_gs(uint8_t shift) {
    while (!EUSCI_A_SPI_getInterruptStatus(EUSCI_A0_BASE,
            EUSCI_A_SPI_TRANSMIT_INTERRUPT));

    // We need a 0 to show it's GS:
    usci_a0_send_sync(0x00);
    // Now the GS data itself.

    // 5 RGB LEDs:
    for (uint8_t channel=0; channel<16; channel++) {
    	usci_a0_send_sync((uint8_t) (gs_data[(channel) % 16] >> 8));
    	usci_a0_send_sync((uint8_t) (gs_data[(channel) % 16] & 0x00ff));
    }

    // LATCH:
    GPIO_pulse(GPIO_PORT_P1, GPIO_PIN4);
}

void tlc_set_fun(uint8_t blank) {
    while (!EUSCI_A_SPI_getInterruptStatus(EUSCI_A0_BASE,
            EUSCI_A_SPI_TRANSMIT_INTERRUPT));

    usci_a0_send_sync(0x01); // 1 for Function

    for (uint8_t i=0; i<14; i++) {
        usci_a0_send_sync(0x00);
    }

    usci_a0_send_sync(0x00); // LSB of this is PSM(D2)

    // B135 / PSM(D1)
    // B134 / PSM(D0)
    // B133 / OLDENA
    // B132 / IDMCUR(D1)
    // B131 / IDMCUR(D0)
    // B130 / IDMRPT(D0)
    // B129 / IDMENA
    // B128 / LATTMG(D1)

    usci_a0_send_sync(0x01);

    // B127 / LATTMG(D0)
    // B126 / LSDVLT(D1)
    // B125 / LSDVLT(D0)
    // B124 / LODVLT(D1)
    // B123 / LODVLT(D0)
    // B122 / ESPWM
    // B121 / TMGRST
    // B120 / DSPRPT

    usci_a0_send_sync(0x85);

    // B119 / BLANK
    // MSB is BLANK; remainder are BC:
    usci_a0_send_sync(0x08 + (blank << 7));


    UCA0CTLW0 |= UCSWRST;
    UCA0CTLW0 |= UC7BIT;
    UCA0CTLW0 &= ~UCSWRST;

    // 16 dot-correct 7-tets:
    for (uint8_t i=0; i<16; i++) {
    	usci_a0_send_sync(0x7F);
    }

    // LATCH:
    GPIO_pulse(GPIO_PORT_P1, GPIO_PIN4);

    UCA0CTLW0 |= UCSWRST;
    UCA0CTLW0 &= ~UC7BIT;
    UCA0CTLW0 &= ~UCSWRST;
}

uint8_t led_loopback(uint8_t test_pattern) {
	volatile uint8_t tlc_loopback_data_in = 0;
	for (uint8_t i=0; i<35; i++) {
		tlc_loopback_data_in = usci_a0_recv_sync(test_pattern);
	}

	__no_operation();

	return tlc_loopback_data_in != (uint8_t) ((test_pattern << 7) | (test_pattern >> 1));
}

void init_leds() {

    UCA0CTLW0 |= UCSWRST;
    UCA0CTLW0 &= ~UC7BIT;
    UCA0CTLW0 &= ~UCSWRST;

    led_loopback(0b10011101);

    tlc_set_fun(1);
    tlc_set_gs(0);
    tlc_set_fun(0);
}

void led_enable(uint16_t duty_cycle) {
}

void led_disable( void )
{
}
