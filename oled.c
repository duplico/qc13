/*
 * oled.c
 *
 *  Created on: Jul 6, 2015
 *      Author: George
 */

/*
 *   OLED (OLED_0.96)
 *        (write on rise, change on fall,
 *         CS active low, MSB first)
 *        eUSCI_A1
 *        ste, miso, clk
 *        DC        P2.6
 *        RES       P2.7
 */

#include <oled.h>
#include <grlib.h>
#include <qc12_oled.h>
#include <qc12.h>
#include "qc13.h"
#include <string.h>
#include <stdlib.h>

uint8_t oled_anim_state = OLED_ANIM_DONE;
uint8_t anim_index = 0;
uint8_t anim_loops = 0;
uint8_t anim_frame_skip = 0;
const qc12_anim_t *anim_data;

int8_t char_pos_x = 0;
int8_t char_pos_y = 0;

void init_oled() {

    // Init USCI_A1 peripheral:
    EUSCI_A_SPI_initMasterParam ini = {0};
    ini.clockPhase = EUSCI_A_SPI_PHASE_DATA_CAPTURED_ONFIRST_CHANGED_ON_NEXT;
    ini.clockPolarity = EUSCI_A_SPI_CLOCKPOLARITY_INACTIVITY_LOW;
    ini.clockSourceFrequency = SMCLK_RATE_HZ;
    ini.desiredSpiClock = 1000000;
    ini.msbFirst = EUSCI_A_SPI_MSB_FIRST;
    ini.selectClockSource = EUSCI_A_SPI_CLOCKSOURCE_SMCLK;
    ini.spiMode = EUSCI_A_SPI_4PIN_UCxSTE_ACTIVE_LOW;
    EUSCI_A_SPI_initMaster(EUSCI_A1_BASE, &ini);
    EUSCI_A_SPI_select4PinFunctionality(EUSCI_A1_BASE, EUSCI_A_SPI_ENABLE_SIGNAL_FOR_4WIRE_SLAVE);

    EUSCI_A_SPI_enable(EUSCI_A1_BASE);
    EUSCI_A_SPI_clearInterrupt(EUSCI_A1_BASE, EUSCI_A_SPI_TRANSMIT_INTERRUPT);
    EUSCI_A_SPI_enableInterrupt(EUSCI_A1_BASE, EUSCI_A_SPI_TRANSMIT_INTERRUPT);

    GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P2, GPIO_PIN5, GPIO_SECONDARY_MODULE_FUNCTION); // TX
    GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P2, GPIO_PIN3, GPIO_SECONDARY_MODULE_FUNCTION); // STE
    GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P2, GPIO_PIN4, GPIO_SECONDARY_MODULE_FUNCTION); // CLK

    GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN7);
    GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN7);
    // This would be RX, but we aren't listening:
    GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN6);
    GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN6);

    qc12_oledInit(0);
    GrContextInit(&g_sContext, &g_sqc12_oled);
    GrContextBackgroundSet(&g_sContext, ClrBlack);
    GrContextForegroundSet(&g_sContext, ClrWhite);
    GrContextFontSet(&g_sContext, &SYS_FONT);
    GrClearDisplay(&g_sContext);
    GrFlush(&g_sContext);
}

void oled_draw_pane_and_flush(uint8_t softkey_sel) {
    GrClearDisplay(&g_sContext);
    static tRectangle erase_rect_top = {0, 0, 64, 2*NAME_FONT_HEIGHT+1};
    static tRectangle erase_rect_btm = {0, SPRITE_Y + 64, 64, 127};

    GrContextForegroundSet(&g_sContext, ClrBlack);
    GrRectFill(&g_sContext, &erase_rect_btm);
    GrRectFill(&g_sContext, &erase_rect_top);
    GrContextForegroundSet(&g_sContext, ClrWhite);

    GrContextFontSet(&g_sContext, &NAME_FONT);
    GrStringDrawCentered(&g_sContext, "qc13event", -1, 32, 6, 0);
    if (my_conf.base_id != NOT_A_BASE) {
        GrStringDrawCentered(&g_sContext, base_labels[my_conf.base_id], -1, 32, 16, 0);
    }
    GrImageDraw(&g_sContext, &fingerprint_1BPP_UNCOMP, 0, 21);

    GrContextFontSet(&g_sContext, &SOFTKEY_LABEL_FONT);
    GrStringDrawCentered(&g_sContext, sk_labels[softkey_sel], -2, 32,  SPRITE_Y + 64 + SOFTKEY_FONT_HEIGHT/2, 0);
    GrLineDrawH(&g_sContext, 0, 64, SPRITE_Y + 64);
    GrFlush(&g_sContext);
}

void oled_print(uint8_t x, uint8_t y, const char str[], uint8_t opaque, uint8_t centered) {
    GrContextFontSet(&g_sContext, &SYS_FONT);
    uint8_t curr_y = y;
    uint8_t str_start = 0;
    uint8_t str_end = 0;
    uint8_t str_end_candidate = 0;
    uint8_t str_len = strlen(str);
    // Note: This could spin forever if we use long words.
    while (str_end < str_len) {
        // find the next space, call it str_end_candidate
        while (str_end_candidate < str_len && str[str_end_candidate] != ' ') {
            str_end_candidate++;
        }
        // If the string, from start to end, fits, we continue (find next space).
        // str_end becomes str_end_candidate.
        if (x + GrStringWidthGet(&g_sContext, &str[str_start], str_end_candidate - str_start) < 63) {
            str_end = str_end_candidate;
            str_end_candidate++; // Skip the space.
        } else {
            // If it doesn't fit, we halt and catch fire. Make sure it fits.
            if (centered) {
                GrStringDrawCentered(&g_sContext, &str[str_start], str_end - str_start, (64+x)/2, curr_y+SYS_FONT_HEIGHT/2, opaque);
            } else {
                GrStringDraw(&g_sContext, &str[str_start], str_end - str_start, x, curr_y, opaque);
            }
            curr_y += SYS_FONT_HEIGHT+1;
            str_start = str_end+1; // Skip the space.
        }
    }

    // If there's still something to draw:
    if (str_start < str_len) {
        if (centered) {
            GrStringDrawCentered(&g_sContext, &str[str_start], -1, (64+x)/2, curr_y+SYS_FONT_HEIGHT/2, opaque);
        }
        else {
            GrStringDraw(&g_sContext, &str[str_start], -1, x, curr_y, opaque);
        }
    }
}

