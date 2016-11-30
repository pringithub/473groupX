/*
 * Copyright (c) 2015-2016, Texas Instruments Incorporated
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

/*
 *  ======== empty.c ========
 */

/* BIOS Header files */
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Task.h>

/* TI-RTOS Header files */
// #include <ti/drivers/I2C.h>
#include <ti/drivers/PIN.h>
#include <ti/drivers/SPI.h>
#include <ti/drivers/Power.h>
// #include <ti/drivers/UART.h>
// #include <ti/drivers/Watchdog.h>

//CC26XXWARE Header Files
#include <driverlib/aux_adc.h>
#include <driverlib/aux_wuc.h>

/* Board Header files */
#include "Board.h"
#include "Digipot.h"
#include "accelerometer.h"

#define TASKSTACKSIZE   1000

Task_Struct task0Struct;
Char task0Stack[TASKSTACKSIZE];

/* Pin driver handle */
static PIN_Handle ledPinHandle;
static PIN_State ledPinState;

/*
 * Application LED pin configuration table:
 *   - All LEDs board LEDs are off.
 */
PIN_Config ledPinTable[] = {
		IOID_3 | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL | PIN_DRVSTR_MAX,
		PIN_TERMINATE
};

//EMG Pins
PIN_Handle emgPinHandle;
PIN_State emgPinState;
const PIN_Config emgPins[] = {
	IOID_13 | PIN_INPUT_DIS | PIN_GPIO_OUTPUT_DIS,
	IOID_7 | PIN_INPUT_DIS | PIN_GPIO_OUTPUT_DIS,
PIN_TERMINATE };

//EMG Pins
PIN_Handle analogPinHandle;
PIN_State analogPinState;
const PIN_Config analogPinTable[] = {
		IOID_1 | PIN_GPIO_OUTPUT_EN | PIN_GPIO_HIGH | PIN_PUSHPULL | PIN_DRVSTR_MAX,
		PIN_TERMINATE
};

void adc_init(void);
uint32_t read_adc(void);
void analog_init();

/*
 *  ======== heartBeatFxn ========
 *  Toggle the Board_LED0. The Task_sleep is determined by arg0 which
 *  is configured for the heartBeat Task instance.
 */
Void heartBeatFxn(UArg arg0, UArg arg1)
{
	uint8_t counter = 10;
    while (1) {
    	uint8_t pot0, pot1;
        Task_sleep((UInt)arg0);
        PIN_setOutputValue(ledPinHandle, IOID_3, !PIN_getOutputValue(IOID_3));

//        System_printf("counter: %d\r\n", counter);
//        set_Wiper(counter, 0);
//        pot0 = read_ISL(0x00, 0);
//        System_printf("\tpot0: %d\r\n", pot0);
//
//        set_Wiper(counter, 1);
//        pot1 = read_ISL(0x00, 1);
//        System_printf("\tpot1: %d\r\n", pot1);

        System_printf("adc: %d\r\n", read_adc());
        System_flush();

        counter += 10;
        if (counter > 0x7F) {
        	counter = 0;
        }
    }
}

/*
 *  ======== main ========
 */
int main(void)
{
    Task_Params taskParams;

    /* Call board init functions */
    Board_initGeneral();
    Board_initSPI();
    // Board_initUART();
    // Board_initWatchdog();

//    digiPot_spi_init();
    adc_init();
    analog_init();

    /* Construct heartBeat Task  thread */
    Task_Params_init(&taskParams);
    taskParams.arg0 = 200 * 1000 / Clock_tickPeriod;
    taskParams.stackSize = TASKSTACKSIZE;
    taskParams.stack = &task0Stack;
    Task_construct(&task0Struct, (Task_FuncPtr)heartBeatFxn, &taskParams, NULL);

    /* Open LED pins */
    ledPinHandle = PIN_open(&ledPinState, ledPinTable);
    if(!ledPinHandle) {
        System_abort("Error initializing board LED pins\n");
    }

    System_printf("Starting the example\nSystem provider is set to SysMin. "
                  "Halt the target to view any SysMin contents in ROV.\n");
    /* SysMin will only print to the console when you call flush or exit */
    System_flush();

    accel_createTask();

    /* Start BIOS */
    BIOS_start();

    return (0);
}

//**********************************************************************************
// Low Level Functions
//**********************************************************************************
/**
 * Initialize ADC module
 *
 * @param 	none
 * @return	none
 */
void adc_init() {
	// Set up pins
	emgPinHandle = PIN_open(&emgPinState, emgPins);

	//Initialize AUX, ADI, and ADC Clocks
	AUXWUCClockEnable(AUX_WUC_MODCLKEN0_ANAIF_M | AUX_WUC_MODCLKEN0_AUX_ADI4_M);

	//Old clock stuff
//	AUXWUCClockEnable(AUX_WUC_SOC_CLOCK | AUX_WUC_ADI_CLOCK | AUX_WUC_ADC_CLOCK);
//
//	//Wait for ADC clock to be ready
//	while (AUX_WUC_CLOCK_READY != AUXWUCClockStatus(AUX_WUC_ADC_CLOCK));

	//Configure ADC to use DIO13 (AUXIO1) on manual trigger.
	AUXADCSelectInput(ADC_COMPB_IN_AUXIO7);
}

void analog_init() {
	analogPinHandle = PIN_open(&analogPinState, analogPinTable);
    PIN_setOutputValue(analogPinHandle, IOID_1, 0);
}

/**
 * Wrapper function to read ADC on configured Analog Pin.
 *
 * @param 	none
 * @return	none
 */
uint32_t read_adc() {
	uint32_t temp;

	//Enable ADC
	AUXADCEnableSync(AUXADC_REF_FIXED, AUXADC_SAMPLE_TIME_10P6_US, AUXADC_TRIGGER_MANUAL);

	//Disallow STANDBY mode while reading
	//Power_setConstraint(Power_SB_DISALLOW);
	AUXADCGenManualTrigger();
	temp = AUXADCReadFifo();
	//Power_releaseConstraint(Power_SB_DISALLOW);

	//Disable ADC when read complete
	AUXADCDisable();

	return temp;
}
