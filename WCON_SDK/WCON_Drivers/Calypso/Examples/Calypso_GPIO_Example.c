/*
 ***************************************************************************************************
 * This file is part of WIRELESS CONNECTIVITY SDK for STM32:
 *
 *
 * THE SOFTWARE INCLUDING THE SOURCE CODE IS PROVIDED “AS IS”. YOU ACKNOWLEDGE THAT WÜRTH ELEKTRONIK
 * EISOS MAKES NO REPRESENTATIONS AND WARRANTIES OF ANY KIND RELATED TO, BUT NOT LIMITED
 * TO THE NON-INFRINGEMENT OF THIRD PARTIES’ INTELLECTUAL PROPERTY RIGHTS OR THE
 * MERCHANTABILITY OR FITNESS FOR YOUR INTENDED PURPOSE OR USAGE. WÜRTH ELEKTRONIK EISOS DOES NOT
 * WARRANT OR REPRESENT THAT ANY LICENSE, EITHER EXPRESS OR IMPLIED, IS GRANTED UNDER ANY PATENT
 * RIGHT, COPYRIGHT, MASK WORK RIGHT, OR OTHER INTELLECTUAL PROPERTY RIGHT RELATING TO ANY
 * COMBINATION, MACHINE, OR PROCESS IN WHICH THE PRODUCT IS USED. INFORMATION PUBLISHED BY
 * WÜRTH ELEKTRONIK EISOS REGARDING THIRD-PARTY PRODUCTS OR SERVICES DOES NOT CONSTITUTE A LICENSE
 * FROM WÜRTH ELEKTRONIK EISOS TO USE SUCH PRODUCTS OR SERVICES OR A WARRANTY OR ENDORSEMENT
 * THEREOF
 *
 * THIS SOURCE CODE IS PROTECTED BY A LICENSE.
 * FOR MORE INFORMATION PLEASE CAREFULLY READ THE LICENSE AGREEMENT FILE LOCATED
 * IN THE ROOT DIRECTORY OF THIS DRIVER PACKAGE.
 *
 * COPYRIGHT (c) 2022 Würth Elektronik eiSos GmbH & Co. KG
 *
 ***************************************************************************************************
 */

/**
 * @file
 * @brief Calypso GPIO example.
 */

#include "Calypso_GPIO_Example.h"

#include <stdio.h>

#include <Calypso/ATCommands/ATDevice.h>
#include <Calypso/ATCommands/ATGPIO.h>

#include "Calypso_Examples.h"

void Calypso_GPIO_Example(void)
{
    printf("*** Start of Calypso GPIO example ***\r\n");

    if (!Calypso_Init(Calypso_Examples_baudRate, Calypso_Examples_flowControl, Calypso_Examples_parity, &Calypso_Examples_EventCallback, NULL))
    {
		return;
	}
	
	Calypso_PinReset();

	Calypso_Examples_WaitForStartup(5000);

	WE_Delay(1000);

	ATGPIO_GPIO_t gpio;
	bool ret;

	/* Get version info. This retrieves Calypso's firmware version (amongst other version info) and
     * stores the firmware version in Calypso_firmwareVersionMajor, Calypso_firmwareVersionMinor and
     * Calypso_firmwareVersionPatch for later use. */
    ATDevice_Value_t deviceValue;
    ret = ATDevice_Get(ATDevice_GetId_General, ATDevice_GetGeneral_Version, &deviceValue);
    Calypso_Examples_Print("Get device version", ret);

	/* Configure GPIO0 as output (and store configuration in flash) */
	memset(&gpio, 0, sizeof(gpio));
	gpio.id = ATGPIO_GPIOId_0;
	gpio.type = ATGPIO_GPIOType_Output;
	gpio.parameters.input.state = ATGPIO_GPIOState_High;
	ret = ATGPIO_Set(&gpio, true);
	Calypso_Examples_Print("Set GPIO0 = output", ret);

	/* Configure GPIO1 as input (and store configuration in flash) */
	memset(&gpio, 0, sizeof(gpio));
	gpio.id = ATGPIO_GPIOId_1;
	gpio.type = ATGPIO_GPIOType_Input;
	gpio.parameters.input.pullType = ATGPIO_PullType_PullUp;
	ret = ATGPIO_Set(&gpio, true);
	Calypso_Examples_Print("Set GPIO1 = input", ret);

	/* Toggle GPIO0 and read GPIO1 every second */
	for (uint8_t i = 0; i < 10; i++)
	{
		memset(&gpio, 0, sizeof(gpio));
		gpio.id = ATGPIO_GPIOId_0;
		gpio.type = ATGPIO_GPIOType_Output;
		gpio.parameters.input.state = i % 2 ? ATGPIO_GPIOState_High : ATGPIO_GPIOState_Low;
		ret = ATGPIO_Set(&gpio, false);
		Calypso_Examples_Print("Set GPIO0 level", ret);
		if (ret)
		{
			printf("GPIO0 level set to %s\r\n", gpio.parameters.output.state == ATGPIO_GPIOState_High ? "high" : "low");
		}

		memset(&gpio, 0, sizeof(gpio));
		gpio.id = ATGPIO_GPIOId_1;
		gpio.type = ATGPIO_GPIOType_Input;
		ret = ATGPIO_Get(gpio.id, false, &gpio);
		Calypso_Examples_Print("Get GPIO1 level", ret);
		if (ret)
		{
			printf("GPIO1 level is %s\r\n", gpio.parameters.input.state == ATGPIO_GPIOState_High ? "high" : "low");
		}

		WE_Delay(1000);
	}

	/* Configure GPIO2 as PWM output with 100 ms period (and store configuration
	 * in flash). Initial ratio = 0%. */
	uint16_t pwmPeriodMs = 100;
	memset(&gpio, 0, sizeof(gpio));
	gpio.id = ATGPIO_GPIOId_2;
	gpio.type = ATGPIO_GPIOType_PWM;
	gpio.parameters.pwm.period = pwmPeriodMs;
	gpio.parameters.pwm.ratio = 0;
	ret = ATGPIO_Set(&gpio, true);
	Calypso_Examples_Print("Set GPIO2 = PWM", ret);
	WE_Delay(1000);

	/* Increase ratio by 10% every second */
	for (uint8_t ratio = 10; ratio <= 100; ratio += 10)
	{
		memset(&gpio, 0, sizeof(gpio));
		gpio.id = ATGPIO_GPIOId_2;
		gpio.type = ATGPIO_GPIOType_PWM;
		gpio.parameters.pwm.period = pwmPeriodMs;
		gpio.parameters.pwm.ratio = ratio;
		ret = ATGPIO_Set(&gpio, false);
		Calypso_Examples_Print("Increase GPIO2 PWM ratio", ret);
		if (ret)
		{
			printf("GPIO2 PWM set to period=%d ms, ratio=%d%%\r\n",
				   gpio.parameters.pwm.period,
				   gpio.parameters.pwm.ratio);
		}
		WE_Delay(1000);
	}

	/* Disable all GPIOs */
	for (uint8_t id = 0; id < ATGPIO_GPIOId_NumberOfValues; id++)
	{
		memset(&gpio, 0, sizeof(gpio));
		gpio.id = id;
		gpio.type = ATGPIO_GPIOType_Unused;
		ret = ATGPIO_Set(&gpio, true);
		Calypso_Examples_Print("Set GPIO unused", ret);
	}

    Calypso_Deinit();
}
