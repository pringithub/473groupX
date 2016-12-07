/*
 * Copyright (c) 2015, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
/** ============================================================================
 *  @file       CC2640.h
 *
 *  @brief      CC2640 FlexZone Board Specific header file.
 *
 *
 *  ============================================================================
 */
#ifndef __CC2640_BOARD_H__
#define __CC2640_BOARD_H__

#ifdef __cplusplus
extern "C" {
#endif

/** ============================================================================
 *  Includes
 *  ==========================================================================*/
#include <ti/drivers/PIN.h>
#include <driverlib/ioc.h>
#include "FlexZoneGlobals.h"
/** ============================================================================
 *  Externs
 *  ==========================================================================*/
extern const PIN_Config BoardGpioInitTable[];

/** ============================================================================
 *  Defines
 *  ==========================================================================*/

/* Identify as CC2650 LaunchPad */
//#define CC2650LP

/* Same RF Configuration as 7x7 EM */
//#define CC2650EM_7ID
#define CC2650EM_5XD

 /* External flash manufacturer and device ID */
#define EXT_FLASH_MAN_ID            0xEF
#define EXT_FLASH_DEV_ID            0x12

/* Mapping of pins to board signals using general board aliases
 *      <board signal alias>        <pin mapping>
 */

/* UART Board */
#if defined(USE_UART)
#define Board_UART_RX               IOID_2          /* RXD  */
#define Board_UART_TX               IOID_3          /* TXD  */
#define Board_UART_CTS              IOID_19         /* CTS  */
#define Board_UART_RTS              IOID_18         /* RTS */
#else
#define Board_UART_RX               PIN_UNASSIGNED          /* RXD  */
#define Board_UART_TX               PIN_UNASSIGNED          /* TXD  */
#define Board_UART_CTS              PIN_UNASSIGNED         /* CTS  */
#define Board_UART_RTS              PIN_UNASSIGNED         /* RTS */
#endif // USE_UART

/* SPI Board */
#define Board_SPI0_MISO             IOID_8          /* RF1.20 */
#define Board_SPI0_MOSI             IOID_9          /* RF1.18 */
#define Board_SPI0_CLK              IOID_10         /* RF1.16 */
#define Board_SPI0_CSN              PIN_UNASSIGNED
#define Board_SPI1_MISO             IOID_14
#define Board_SPI1_MOSI             IOID_12
#define Board_SPI1_CLK              IOID_11
#define Board_SPI1_CSN              PIN_UNASSIGNED

/* I2C */
#define Board_I2C0_SCL0             IOID_10
#define Board_I2C0_SDA0             IOID_9

//FlexZone Board Custom
#define Board_ANALOG_EN				IOID_1
#if defined(USE_UART)
#define Board_VIBE_MOTOR			IOID_5
#else
#define Board_VIBE_MOTOR			IOID_3
#endif

#if defined(USE_UART)
#define Board_CH1_IN				IOID_29
#define Board_CH0_IN				IOID_23
#else
#define Board_CH1_IN				IOID_13
#define Board_CH0_IN				IOID_7
#endif //USE_UART

#define BOARD_CH1_AUX				ADC_COMPB_IN_AUXIO1
#define BOARD_CH0_AUX				ADC_COMPB_IN_AUXIO7

#define DIGIPOT_1_CS				IOID_0
#define DIGIPOT_0_CS				IOID_2

/** ============================================================================
 *  Instance identifiers
 *  ==========================================================================*/
/* Generic I2C instance identifiers */
#define Board_I2C                   CC2640_I2C0
/* Generic SPI instance identifiers */
#define Board_SPI0                  CC2640_SPI0
#define Board_SPI1                  CC2640_SPI1
/* Generic UART instance identifiers */
#define Board_UART                  CC2640_UART0


/** ============================================================================
 *  Number of peripherals and their names
 *  ==========================================================================*/

/*!
 *  @def    CC2640_I2CName
 *  @brief  Enum of I2C names on the CC2650 dev board
 */
typedef enum CC2640_I2CName {
    CC2640_I2C0 = 0,

    CC2640_I2CCOUNT
} CC2640_I2CName;


/*!
 *  @def    CC2640_SPIName
 *  @brief  Enum of SPI names on the CC2650 dev board
 */
typedef enum CC2640_SPIName {
    CC2640_SPI0 = 0,
    CC2640_SPI1,

    CC2640_SPICOUNT
} CC2640_SPIName;

/*!
 *  @def    CC2640_UARTName
 *  @brief  Enum of UARTs on the CC2650 dev board
 */
typedef enum CC2640_UARTName {
    CC2640_UART0 = 0,

    CC2640_UARTCOUNT
} CC2640_UARTName;

/*!
 *  @def    CC2640_UdmaName
 *  @brief  Enum of DMA buffers
 */
typedef enum CC2640_UdmaName {
    CC2640_UDMA0 = 0,

    CC2640_UDMACOUNT
} CC2640_UdmaName;

#ifdef __cplusplus
}
#endif

#endif /* __CC2640_BOARD_H__ */
