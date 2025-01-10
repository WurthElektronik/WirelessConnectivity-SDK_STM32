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
 * COPYRIGHT (c) 2025 Würth Elektronik eiSos GmbH & Co. KG
 *
 ***************************************************************************************************
 */

/**
 * @file
 * @brief Calypso GPIO example.
 */
#include <Calypso/Calypso_GPIO_Example.h>
#include <stdio.h>
#include <Calypso/ATCommands/ATDevice.h>
#include <Calypso/ATCommands/ATGPIO.h>
#include <Calypso/Calypso_Examples.h>

void Calypso_GPIO_Example(void)
{
	WE_DEBUG_PRINT("*** Start of Calypso GPIO example ***\r\n");

	if (!Calypso_Init(&Calypso_uart, &Calypso_pins, &Calypso_Examples_EventCallback))
	{
		WE_DEBUG_PRINT("Initialization error\r\n");
		return;
	}

	Calypso_PinReset();

	Calypso_Examples_WaitForStartup(5000);

	WE_Delay(1000);

	Calypso_ATGPIO_GPIO_t gpio;
	bool ret;

	/* Get version info. This retrieves Calypso's firmware version (amongst other version info) and
	 * stores the firmware version in Calypso_firmwareVersionMajor, Calypso_firmwareVersionMinor and
	 * Calypso_firmwareVersionPatch for later use. */
	Calypso_ATDevice_Value_t deviceValue;
	ret = Calypso_ATDevice_Get(Calypso_ATDevice_GetId_General, Calypso_ATDevice_GetGeneral_Version, &deviceValue);
	Calypso_Examples_Print("Get device version", ret);

	/* Configure GPIO0 as output (and store configuration in flash) */
	memset(&gpio, 0, sizeof(gpio));
	gpio.id = Calypso_ATGPIO_GPIOId_0;
	gpio.type = Calypso_ATGPIO_GPIOType_Output;
	gpio.parameters.input.state = Calypso_ATGPIO_GPIOState_High;
	ret = Calypso_ATGPIO_Set(&gpio, true);
	Calypso_Examples_Print("Set GPIO0 = output", ret);

	/* Configure GPIO1 as input (and store configuration in flash) */
	memset(&gpio, 0, sizeof(gpio));
	gpio.id = Calypso_ATGPIO_GPIOId_1;
	gpio.type = Calypso_ATGPIO_GPIOType_Input;
	gpio.parameters.input.pullType = Calypso_ATGPIO_PullType_PullUp;
	ret = Calypso_ATGPIO_Set(&gpio, true);
	Calypso_Examples_Print("Set GPIO1 = input", ret);

	/* Toggle GPIO0 and read GPIO1 every second */
	for (uint8_t i = 0; i < 10; i++)
	{
		memset(&gpio, 0, sizeof(gpio));
		gpio.id = Calypso_ATGPIO_GPIOId_0;
		gpio.type = Calypso_ATGPIO_GPIOType_Output;
		gpio.parameters.input.state = i % 2 ? Calypso_ATGPIO_GPIOState_High : Calypso_ATGPIO_GPIOState_Low;
		ret = Calypso_ATGPIO_Set(&gpio, false);
		Calypso_Examples_Print("Set GPIO0 level", ret);
		if (ret)
		{
			WE_DEBUG_PRINT("GPIO0 level set to %s\r\n", gpio.parameters.output.state == Calypso_ATGPIO_GPIOState_High ? "high" : "low");
		}

		memset(&gpio, 0, sizeof(gpio));
		gpio.id = Calypso_ATGPIO_GPIOId_1;
		gpio.type = Calypso_ATGPIO_GPIOType_Input;
		ret = Calypso_ATGPIO_Get(gpio.id, false, &gpio);
		Calypso_Examples_Print("Get GPIO1 level", ret);
		if (ret)
		{
			WE_DEBUG_PRINT("GPIO1 level is %s\r\n", gpio.parameters.input.state == Calypso_ATGPIO_GPIOState_High ? "high" : "low");
		}

		WE_Delay(1000);
	}

	/* Configure GPIO2 as PWM output with 100 ms period (and store configuration
	 * in flash). Initial ratio = 0%. */
	uint16_t pwmPeriodMs = 100;
	memset(&gpio, 0, sizeof(gpio));
	gpio.id = Calypso_ATGPIO_GPIOId_2;
	gpio.type = Calypso_ATGPIO_GPIOType_PWM;
	gpio.parameters.pwm.period = pwmPeriodMs;
	gpio.parameters.pwm.ratio = 0;
	ret = Calypso_ATGPIO_Set(&gpio, true);
	Calypso_Examples_Print("Set GPIO2 = PWM", ret);
	WE_Delay(1000);

	/* Increase ratio by 10% every second */
	for (uint8_t ratio = 10; ratio <= 100; ratio += 10)
	{
		memset(&gpio, 0, sizeof(gpio));
		gpio.id = Calypso_ATGPIO_GPIOId_2;
		gpio.type = Calypso_ATGPIO_GPIOType_PWM;
		gpio.parameters.pwm.period = pwmPeriodMs;
		gpio.parameters.pwm.ratio = ratio;
		ret = Calypso_ATGPIO_Set(&gpio, false);
		Calypso_Examples_Print("Increase GPIO2 PWM ratio", ret);
		if (ret)
		{
			WE_DEBUG_PRINT("GPIO2 PWM set to period=%d ms, ratio=%d%%\r\n", gpio.parameters.pwm.period, gpio.parameters.pwm.ratio);
		}
		WE_Delay(1000);
	}

	/* Disable all GPIOs */
	for (uint8_t id = 0; id < Calypso_ATGPIO_GPIOId_NumberOfValues; id++)
	{
		memset(&gpio, 0, sizeof(gpio));
		gpio.id = id;
		gpio.type = Calypso_ATGPIO_GPIOType_Unused;
		ret = Calypso_ATGPIO_Set(&gpio, true);
		Calypso_Examples_Print("Set GPIO unused", ret);
	}

	Calypso_Deinit();
}
