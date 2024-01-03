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
 * @brief Themisto-I example.
 */
#include <ThemistoI/ThemistoI.h>
#include <global/global.h>
#include <stdio.h>
#include <string.h>

/**
 * @brief Definition of the pins
 */
static ThemistoI_Pins_t ThemistoI_pins;

/**
 * @brief Definition of the uart
 */
static WE_UART_t ThemistoI_uart;

/* Pick the example to be executed in the main function. */
static void CommandModeExample();

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
 * @brief Callback called when data has been received via radio
 */
static void RxCallback(uint8_t *payload, uint8_t payload_length, uint8_t dest_network_id, uint8_t dest_address_lsb, uint8_t dest_address_msb, int8_t rssi)
{
	uint8_t i = 0;
	printf("Received data from address (NetID:0x%02x,Addr:0x%02x%02x) with %d dBm:\n-> ", dest_network_id, dest_address_lsb, dest_address_msb, rssi);
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
 * @brief Command mode example repeatedly transmitting data via radio
 */
void ThemistoI_Examples(void)
{
	ThemistoI_pins.ThemistoI_Pin_Reset.port = (void*) GPIOA;
	ThemistoI_pins.ThemistoI_Pin_Reset.pin = GPIO_PIN_10;
	ThemistoI_pins.ThemistoI_Pin_SleepWakeUp.port = (void*) GPIOA;
	ThemistoI_pins.ThemistoI_Pin_SleepWakeUp.pin = GPIO_PIN_9;
	ThemistoI_pins.ThemistoI_Pin_Boot.port = (void*) GPIOA;
	ThemistoI_pins.ThemistoI_Pin_Boot.pin = GPIO_PIN_7;
	ThemistoI_pins.ThemistoI_Pin_Mode.port = (void*) GPIOA;
	ThemistoI_pins.ThemistoI_Pin_Mode.pin = GPIO_PIN_8;

	ThemistoI_uart.baudrate = THEMISTOI_DEFAULT_BAUDRATE;
	ThemistoI_uart.flowControl = WE_FlowControl_NoFlowControl;
	ThemistoI_uart.parity = WE_Parity_None;
	ThemistoI_uart.uartInit = WE_UART1_Init;
	ThemistoI_uart.uartDeinit = WE_UART1_DeInit;
	ThemistoI_uart.uartTransmit = WE_UART1_Transmit;

	CommandModeExample();
}

/**
 * @brief Command mode example repeatedly transmitting data via radio
 */
void CommandModeExample(void)
{

	if (!ThemistoI_Init(&ThemistoI_uart, &ThemistoI_pins, ThemistoI_AddressMode_0, RxCallback))
	{
		printf("Initialization error\r\n");
		return;
	}

	uint8_t serialNr[4];
	Examples_Print("Read serial number", ThemistoI_GetSerialNumber(serialNr));
	printf("Serial number is 0x%02x%02x%02x%02x\r\n", serialNr[0], serialNr[1], serialNr[2], serialNr[3]);
	WE_Delay(500);

	uint8_t fwVersion[3];
	Examples_Print("Read firmware version", ThemistoI_GetFirmwareVersion(fwVersion));
	printf("Firmware version is %u.%u.%u\r\n", fwVersion[0], fwVersion[1], fwVersion[2]);
	WE_Delay(500);

	uint8_t data[4 * 16];
	for (uint16_t i = 0; i < sizeof(data); i++)
	{
		data[i] = (uint8_t) i;
	}

	while (1)
	{
		if (false == ThemistoI_Transmit(data, sizeof(data)))
		{
			printf("Transmission error\r\n");
		}
		WE_Delay(500);
	}

	return;
}
