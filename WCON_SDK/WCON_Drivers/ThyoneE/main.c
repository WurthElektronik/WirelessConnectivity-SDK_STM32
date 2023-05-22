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
 * COPYRIGHT (c) 2023 Würth Elektronik eiSos GmbH & Co. KG
 *
 ***************************************************************************************************
 */

/**
 * @file
 * @brief Thyone-e example.
 */

#include <stdio.h>
#include <string.h>

#include "../../WCON_Drivers/ThyoneE/ThyoneE.h"
#include "../../WCON_Drivers/global/global.h"

/* callback for data reception */
static void RxCallback(uint8_t *payload, uint16_t payload_length, uint32_t sourceAddress, int8_t rssi)
{
	int i = 0;
	printf("Received data from address 0x%02lx with %d dBm:\n-> ", sourceAddress, rssi);
	printf("0x ");
	for (i = 0; i < payload_length; i++)
	{
		printf("%02x ", *(payload + i));
	}
	printf("\n-> ");
	for (i = 0; i < payload_length; i++)
	{
		printf("%c", *(payload + i));
	}
	printf("\n");
	fflush(stdout);
}

/**
 * @brief Prints the supplied string, prefixed with OK or NOK (depending on the success parameter).
 *
 * @param str String to print
 * @param success Variable indicating if action was ok
 */
void Examples_Print(char *str, bool success)
{
	printf("%s%s\r\n", success ? "OK    " : "NOK   ", str);
}

void Example_CommandMode_GetFirmwareVersion()
{
	bool ret = false;

	ret = ThyoneE_Init(THYONEE_DEFAULT_BAUDRATE, WE_FlowControl_NoFlowControl, ThyoneE_OperationMode_CommandMode, RxCallback);
	Examples_Print("Thyone-e init", ret);

	while (1)
	{
		uint8_t fwVersion[3];
		memset(fwVersion, 0, sizeof(fwVersion));
		ret = ThyoneE_GetFWVersion(fwVersion);
		Examples_Print("Get FW version", ret);
		printf("Firmware version is %u.%u.%u\r\n", fwVersion[2], fwVersion[1], fwVersion[0]);
		WE_Delay(500);

		uint8_t serialNr[4];
		memset(serialNr, 0, sizeof(serialNr));
		ret = ThyoneE_GetSerialNumber(serialNr);
		Examples_Print("Get serial number", ret);
		printf("Serial number is 0x%02x%02x%02x%02x\r\n", serialNr[3], serialNr[2], serialNr[1], serialNr[0]);
		WE_Delay(500);

		ret = ThyoneE_PinReset();
		Examples_Print("Reset", ret);
		WE_Delay(500);
	}
}

void Example_CommandMode_DataTransmission()
{
	bool ret = false;

	uint8_t data[224];
	for (uint16_t i = 0; i < sizeof(data); i++)
	{
		data[i] = (uint8_t)i;
	}

	ret = ThyoneE_Init(THYONEE_DEFAULT_BAUDRATE, WE_FlowControl_NoFlowControl, ThyoneE_OperationMode_CommandMode, RxCallback);
	Examples_Print("Thyone-e init", ret);

	while (1)
	{
		data[0]++;

		ret = ThyoneE_TransmitBroadcast(data, sizeof(data));
		Examples_Print("Broadcast transmission", ret);
	}
}

void Example_TransparentMode_DataTransmission()
{
	bool ret = false;
	bool busy = true;

	uint8_t data[224];
	for (uint16_t i = 0; i < sizeof(data); i++)
	{
		data[i] = (uint8_t)i;
	}

	ret = ThyoneE_Init(THYONEE_DEFAULT_BAUDRATE, WE_FlowControl_NoFlowControl, ThyoneE_OperationMode_TransparentMode, RxCallback);
	Examples_Print("Thyone-e init", ret);

	while (1)
	{
		/* Check state of busy pin and print message on state change */
		bool b = ThyoneE_IsTransparentModeBusy();

		if ((busy == true) && (b == false))
		{
			/* falling edge detected, module not busy anymore */
			data[0]++;
			WE_UART_Transmit(data, sizeof(data));
		}
		busy = b;
	}
}

/**
 * @brief The application's main function.
 */
int main(void)
{
	/* Initialize platform (peripherals, flash interface, Systick, system clock) */
	WE_Platform_Init();

#ifdef WE_DEBUG
	WE_Debug_Init();
#endif

	uint8_t driverVersion[3];
	WE_GetDriverVersion(driverVersion);
	printf("Wuerth Elektronik eiSos Wireless Connectivity SDK version %d.%d.%d\r\n", driverVersion[0], driverVersion[1], driverVersion[2]);

	Example_CommandMode_GetFirmwareVersion();
	//Example_CommandMode_DataTransmission();
	//Example_TransparentMode_DataTransmission();
}
