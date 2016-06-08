/*
 *  This file is automatically generated and does not require a license
 *
 *  ==== WARNING: CHANGES TO THIS GENERATED FILE WILL BE OVERWRITTEN ====
 *
 *  To make changes to the generated code, use the space between existing
 *      "USER CODE START (section: <name>)"
 *  and
 *      "USER CODE END (section: <name>)"
 *  comments, where <name> is a single word identifying the section.
 *  Only these sections will be preserved.
 *
 *  Do not move these sections within this file or change the START and
 *  END comments in any way.
 *  ==== ALL OTHER CHANGES WILL BE OVERWRITTEN WHEN IT IS REGENERATED ====
 *
 *  This file was generated from
 *      D:/ti/grace_3_10_00_82/packages/ti/mcu/msp430/csl2/communication/EUSCI_init.xdt
 */
#include <stdint.h>
#include "_Grace.h"
#include <driverlib/MSP430FR5xx_6xx/inc/hw_memmap.h>
#include <driverlib/MSP430FR5xx_6xx/eusci_b_spi.h>

/* USER CODE START (section: EUSCI_B0_init_c_prologue) */
/* User defined includes, defines, global variables and functions */
/* USER CODE END (section: EUSCI_B0_init_c_prologue) */

/*
 *  ======== EUSCI_B0_graceInit ========
 *  Initialize Config for the MSP430 EUSCI_B0
 */
void EUSCI_B0_graceInit(void)
{
    /* Struct to pass to EUSCI_B_SPI_initMaster */
    EUSCI_B_SPI_initMasterParam initSPIMasterParam = {0};

    /* USER CODE START (section: EUSCI_B0_graceInit_prologue) */
    /* User initialization code */
    /* USER CODE END (section: EUSCI_B0_graceInit_prologue) */

    /* initialize eUSCI SPI master mode */
    initSPIMasterParam.selectClockSource = EUSCI_B_SPI_CLOCKSOURCE_SMCLK;
    initSPIMasterParam.clockSourceFrequency = 8000000;
    initSPIMasterParam.desiredSpiClock = 500000;
    initSPIMasterParam.msbFirst = EUSCI_B_SPI_MSB_FIRST;
    initSPIMasterParam.clockPhase = EUSCI_B_SPI_PHASE_DATA_CAPTURED_ONFIRST_CHANGED_ON_NEXT;
    initSPIMasterParam.clockPolarity = EUSCI_B_SPI_CLOCKPOLARITY_INACTIVITY_LOW;
    initSPIMasterParam.spiMode = EUSCI_B_SPI_4PIN_UCxSTE_ACTIVE_LOW;
    EUSCI_B_SPI_initMaster(EUSCI_B0_BASE, &initSPIMasterParam);

    /* select eUSCI SPI STE mode */
    EUSCI_B_SPI_select4PinFunctionality(EUSCI_B0_BASE, EUSCI_B_SPI_ENABLE_SIGNAL_FOR_4WIRE_SLAVE);

    /* enable eUSCI SPI */
    EUSCI_B_SPI_enable(EUSCI_B0_BASE);

    /* disable eUSCI SPI transmit interrupt */
    EUSCI_B_SPI_disableInterrupt(EUSCI_B0_BASE, EUSCI_B_SPI_TRANSMIT_INTERRUPT);

    /* disable eUSCI SPI receive interrupt */
    EUSCI_B_SPI_disableInterrupt(EUSCI_B0_BASE, EUSCI_B_SPI_RECEIVE_INTERRUPT);

    /* USER CODE START (section: EUSCI_B0_graceInit_epilogue) */
    /* User code */
    /* USER CODE END (section: EUSCI_B0_graceInit_epilogue) */

}
