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
#include <ThyoneE/ThyoneE.h>
#include <global/global.h>
#include <stdio.h>
#include <string.h>

/**
 * @brief Definition of the pins
 */
static ThyoneE_Pins_t ThyoneE_pins;

/**
 * @brief Definition of the uart
 */
static WE_UART_t ThyoneE_uart;

/**
 * @brief Callback called when data has been received via radio
 */
static void RxCallback(uint8_t *payload, uint16_t payload_length, uint32_t sourceAddress, int8_t rssi)
{
	uint16_t i = 0;
	printf("Received data from address 0x%02lx with %d dBm:\n-> ", sourceAddress, rssi);
	printf("0x");
	for (i = 0; i < payload_length; i++)
	{
		printf("%02x", *(payload + i));
	}
	printf(" (");
	for (i = 0; i < payload_length; i++)
	{
		printf("%c", *(payload + i));
	}
	printf(")\r\n");
	fflush(stdout);
}

/**
 * @brief Prints the supplied string, prefixed with OK or NOK (depending on the success parameter).
 *
 * @param str String to print
 * @param success Variable indicating if action was ok
 */
static void Examples_Print(char *str, bool success)
{
	printf("%s%s\r\n", success ? "OK    " : "NOK   ", str);
}

/**
 * @brief Command mode example repeatedly transmitting data via radio
 */
static void Example_CommandMode_DataTransmission()
{
	bool ret = false;

	if (false == ThyoneE_Init(&ThyoneE_uart, &ThyoneE_pins, ThyoneE_OperationMode_CommandMode, RxCallback))
	{
		printf("Initialization error\r\n");
		return;
	}

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

	uint8_t data[224];
	for (uint16_t i = 0; i < sizeof(data); i++)
	{
		data[i] = (uint8_t) i;
	}

	while (1)
	{
		data[0]++;
		ret = ThyoneE_TransmitBroadcast(data, sizeof(data));
		Examples_Print("Broadcast transmission", ret);
		WE_Delay(500);
	}
}

static void Example_CommandMode_Throughput()
{
	bool ret = false;

	uint8_t data[224];
	for (uint16_t i = 0; i < sizeof(data); i++)
	{
		data[i] = (uint8_t) i;
	}

#if 0
	/* set Thyone to 1MBaud */
	if(false == ThyoneE_Init(&ThyoneE_uart, &ThyoneE_pins, ThyoneE_OperationMode_CommandMode, RxCallback))
	{
		printf("Initialization error\r\n");
		return;
	}

	ret = ThyoneE_SetBaudrateIndex(ThyoneE_BaudRateIndex_1000000,ThyoneE_UartParity_None, true);
	Examples_Print("Thyone-e set baudrate", ret);
	return;
#elif 1
	/* run the test */
#pragma message("Thyone-e must be set to 1MBaud first")
	ThyoneE_uart.baudrate = 1000000;
	ThyoneE_uart.flowControl = WE_FlowControl_RTSAndCTS;
	if (false == ThyoneE_Init(&ThyoneE_uart, &ThyoneE_pins, ThyoneE_OperationMode_CommandMode, RxCallback))
	{
		printf("Initialization error\r\n");
		return;
	}

#elif 0
	/* set Thyone to default */
	ThyoneE_uart.baudrate = 1000000;
	ThyoneE_uart.flowControl = WE_FlowControl_RTSAndCTS;
	if(false == ThyoneE_Init(&ThyoneE_uart, &ThyoneE_pins, ThyoneE_OperationMode_CommandMode, RxCallback))
	{
		printf("Initialization error\r\n");
		return;
	}

	ret = ThyoneE_FactoryReset();
	Examples_Print("Thyone-e factory reset", ret);

	return;
#endif

	while (1)
	{
		data[0]++;

		ret = ThyoneE_TransmitBroadcast(data, sizeof(data));
		Examples_Print("Broadcast transmission", ret);
	}
}

/**
 * @brief Transparent mode example repeatedly transmitting data via radio
 */
static void Example_TransparentMode_DataTransmission()
{
	if (false == ThyoneE_Init(&ThyoneE_uart, &ThyoneE_pins, ThyoneE_OperationMode_TransparentMode, RxCallback))
	{
		printf("Initialization error\r\n");
		return;
	}

	uint8_t data[224];
	for (uint16_t i = 0; i < sizeof(data); i++)
	{
		data[i] = (uint8_t) i;
	}

	while (1)
	{
		/* Check state of busy pin and print message on state change */
		if (true == ThyoneE_IsTransparentModeBusy())
		{
			printf("Module busy\r\n");
			WE_Delay(15);
		}
		else if (ThyoneE_Transparent_Transmit(data, sizeof(data)))
		{
			uint8_t count = 0;
			/* transmission success */
			while (count < 20)
			{
				/* wait for high */
				WE_Delay(2);
				count++;
				if (true == ThyoneE_IsTransparentModeBusy())
				{
					/* high detected */
					break;
				}
			}
			count = 0;
			while (count < 128)
			{
				/* wait for low */
				WE_Delay(1);
				count++;
				if (false == ThyoneE_IsTransparentModeBusy())
				{
					/* low detected */
					break;
				}
			}
		}
		else
		{
			printf("Transmission failed\r\n");
		}
		WE_Delay(100);
	}
}

/**
 * @brief The application's main function.
 */
void ThyoneE_Examples(void)
{
	ThyoneE_pins.ThyoneE_Pin_Reset.port = (void*) GPIOA;
	ThyoneE_pins.ThyoneE_Pin_Reset.pin = GPIO_PIN_10;
	ThyoneE_pins.ThyoneE_Pin_Busy.port = (void*) GPIOA;
	ThyoneE_pins.ThyoneE_Pin_Busy.pin = GPIO_PIN_6;
	ThyoneE_pins.ThyoneE_Pin_Mode.port = (void*) GPIOA;
	ThyoneE_pins.ThyoneE_Pin_Mode.pin = GPIO_PIN_8;

	ThyoneE_uart.baudrate = THYONEE_DEFAULT_BAUDRATE;
	ThyoneE_uart.flowControl = WE_FlowControl_NoFlowControl;
	ThyoneE_uart.parity = WE_Parity_None;
	ThyoneE_uart.uartInit = WE_UART1_Init;
	ThyoneE_uart.uartDeinit = WE_UART1_DeInit;
	ThyoneE_uart.uartTransmit = WE_UART1_Transmit;

	//Example_CommandMode_Throughput();
	Example_CommandMode_DataTransmission();
	//Example_TransparentMode_DataTransmission();

	return;
}
