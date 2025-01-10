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
 * @brief Main file for DaphnisI driver examples.
 *
 */
#include <global/global.h>
#include <DaphnisI/DaphnisI.h>
#include <DaphnisI/ATCommands/ATGPIO.h>
#include <stdio.h>
#include <DaphnisI/DaphnisI_Examples.h>
#include <DaphnisI/DaphnisI_Local_GPIO_Example.h>

#if DAPHNISI_MIN_FW_VER >= FW(1,4,0)

/**
 * @brief In this example the module, that is already registered at TTN network, is joining the network
 * with the provided application EUI and application key. After joining, the module transmits a test string and is able
 * to receive data from the TTN server.
 */
void DaphnisI_Local_GPIO_Example()
{
	if (!DaphnisI_Init(&DaphnisI_uart, &DaphnisI_pins, NULL))
	{
		WE_DEBUG_PRINT("Initialization error\r\n");
		return;
	}

	DaphnisI_GPIOConfigBlock_t pin_configurations_set[] = {
		{
			.GPIO_ID = DaphnisI_GPIO_0,
			.function = DaphnisI_GPIO_IO_Output,
			.value = {
					.output = DaphnisI_GPIO_Output_High
			}
		},
		{
			.GPIO_ID = DaphnisI_GPIO_1,
			.function = DaphnisI_GPIO_IO_Input,
			.value = {
					.input = DaphnisI_GPIO_Input_PullUp
			}
		},
	};

	if(!DaphnisI_GPIO_Local_Configuration_Set(pin_configurations_set, sizeof(pin_configurations_set) / sizeof(DaphnisI_GPIOConfigBlock_t), NULL))
	{
		WE_DEBUG_PRINT("Setting GPIO configuration failed\r\n");
		return;
	}

	DaphnisI_GPIOConfigBlock_t pin_configurations_get[DaphnisI_GPIO_Count];

	uint8_t pin_config_count;

	if(!DaphnisI_GPIO_Local_Configuration_Get(pin_configurations_get, &pin_config_count))
	{
		WE_DEBUG_PRINT("Getting GPIO configuration failed\r\n");
		return;
	}

	DaphnisI_GPIOValueSetBlock_t gpio0_setControlBlock = {
			.GPIO_ID = DaphnisI_GPIO_0 ,
			.value = DaphnisI_GPIO_Output_Low
	};

	DaphnisI_GPIOStatusBlock_t gpio0_statusBlock;

	if(!DaphnisI_GPIO_Local_Value_Set(&gpio0_setControlBlock, 1, &gpio0_statusBlock))
	{
		WE_DEBUG_PRINT("Setting GPIO value failed\r\n");
		return;
	}

	DaphnisI_GPIO_t gpio_read = DaphnisI_GPIO_1;

	DaphnisI_GPIOValueGetBlock_t gpio1_readControlBlock;

	if(!DaphnisI_GPIO_Local_Value_Get(&gpio_read, 1, &gpio1_readControlBlock))
	{
		WE_DEBUG_PRINT("Getting GPIO value failed\r\n");
		return;
	}

	WE_DEBUG_PRINT("GPIO %d value is %d\r\n", gpio_read, gpio1_readControlBlock.value);

}

#endif
