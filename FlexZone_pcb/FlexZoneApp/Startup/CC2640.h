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
#define Board_UART_RX               PIN_UNASSIGNED          /* RXD  */
#define Board_UART_TX               PIN_UNASSIGNED          /* TXD  */
#define Board_UART_CTS              PIN_UNASSIGNED         /* CTS  */
#define Board_UART_RTS              PIN_UNASSIGNED         /* RTS */

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

/* SPI */
//#define Board_SPI_FLASH_CS          IOID_20
#define Board_FLASH_CS_ON           0
#define Board_FLASH_CS_OFF          1

/* Booster pack generic */
#define Board_DIO0                  IOID_0
#define Board_DIO1_RFSW             IOID_1
#define Board_DIO12                 IOID_12
#define Board_DIO15                 IOID_15
#define Board_DIO16_TDO             IOID_16
#define Board_DIO17_TDI             IOID_17
#define Board_DIO21                 IOID_21
#define Board_DIO22                 IOID_22

#define Board_DIO23_ANALOG          IOID_23
#define Board_DIO24_ANALOG          IOID_24
#define Board_DIO25_ANALOG          IOID_25
#define Board_DIO26_ANALOG          IOID_26
#define Board_DIO27_ANALOG          IOID_27
#define Board_DIO28_ANALOG          IOID_28
#define Board_DIO29_ANALOG          IOID_29
#define Board_DIO30_ANALOG          IOID_30


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
 *  @def    CC2640_CryptoName
 *  @brief  Enum of Crypto names on the CC2650 dev board
 */
//typedef enum CC2640_CryptoName {
//    CC2640_CRYPTO0 = 0,
//
//    CC2640_CRYPTOCOUNT
//} CC2640_CryptoName;


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
