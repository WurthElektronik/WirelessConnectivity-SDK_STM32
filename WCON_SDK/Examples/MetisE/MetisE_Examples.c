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
 * @brief Metis-e example.
 *
 */

#include <MetisE/MetisE.h>
#include <global/global.h>
#include <stdio.h>
#include <string.h>
#if defined(STM32L073xx)
#include <global_L0xx.h>
#elif defined(STM32F401xE)
#include <global_F4xx.h>
#endif

#define METIS_E_CLOCK_KHZ 4000
/**
 * @brief Definition of the pins
 */
static MetisE_Pins_t MetisE_pins;

/**
 * @brief Definition of the uart
 */
static WE_UART_t MetisE_uart;

/* Pick the example to be executed in the main function. */
static void MeterExample();
static void GatewayExample();

static char* modeString[] = {"S", "T", "T", "C+T", "C", "C", "Undefined"};
static char* frameFormatString[] = {"A","B"};

/**
 * @brief Prints the supplied string, prefixed with OK or NOK (depending on the success parameter).
 *
 * @param str String to print
 * @param success Variable indicating if action was ok
 */
static void Examples_Print(char *str, bool success)
{
	WE_DEBUG_PRINT("%s%s\r\n", success ? "OK    " : "NOK   ", str);
}

/**
 * @brief Callback called when data has been received via radio
 */
static void RxCallback(MetisE_ReceivedData_t receivedData)
{
	uint16_t i = 0;
	WE_DEBUG_PRINT("Received wmBus frame in mode %s with frame format %s", modeString[receivedData.wmBusModeRx], frameFormatString[receivedData.wmBusModeFrameFormat-1]);
	if(receivedData.rssi != METIS_E_RSSIINVALID)
	{
		WE_DEBUG_PRINT(" with %d dBm", receivedData.rssi);
	}

	if(receivedData.timestamp != 0)
	{
		WE_DEBUG_PRINT(" at %lu ms", receivedData.timestamp/METIS_E_CLOCK_KHZ);
	}

	WE_DEBUG_PRINT(":\r\n0x");
	for (i = 0; i < receivedData.payloadLength; i++)
	{
		WE_DEBUG_PRINT("%02x ", *(receivedData.pPayload + i));
	}
}

void MetisE_Examples(void)
{
	MetisE_pins.MetisE_Pin_Reset.port = (void*) GPIOA;
	MetisE_pins.MetisE_Pin_Reset.pin = GPIO_PIN_10;
	MetisE_pins.MetisE_Pin_SleepWakeUp.port = (void*) GPIOA;
	MetisE_pins.MetisE_Pin_SleepWakeUp.pin = GPIO_PIN_9;
	MetisE_pins.MetisE_Pin_Boot.port = (void*) GPIOA;
	MetisE_pins.MetisE_Pin_Boot.pin = GPIO_PIN_7;

	MetisE_uart.baudrate = METIS_E_DEFAULT_BAUDRATE;
	MetisE_uart.flowControl = WE_FlowControl_NoFlowControl;
	MetisE_uart.parity = WE_Parity_None;
	MetisE_uart.uartInit = WE_UART1_Init;
	MetisE_uart.uartDeinit = WE_UART1_DeInit;
	MetisE_uart.uartTransmit = WE_UART1_Transmit;

	if (false == MetisE_Init(&MetisE_uart, &MetisE_pins, RxCallback))
	{
		WE_DEBUG_PRINT("Initialization error\r\n");
		return;
	}
	WE_Delay(10);

	/* reset module */
	if (!MetisE_PinReset())
	{
		WE_DEBUG_PRINT("Pin reset failed\n");
		MetisE_Deinit();
		return;
	}
	WE_Delay(300);

	uint8_t serialNr[4];
	Examples_Print("Read serial number", MetisE_GetSerialNumber(serialNr));
	WE_DEBUG_PRINT("Serial number is 0x%02x%02x%02x%02x\r\n", serialNr[0], serialNr[1], serialNr[2], serialNr[3]);
	WE_Delay(500);

	uint8_t fwVersion[3];
	Examples_Print("Read firmware version", MetisE_GetFirmwareVersion(fwVersion));
	WE_DEBUG_PRINT("Firmware version is %u.%u.%u\r\n", fwVersion[0], fwVersion[1], fwVersion[2]);
	WE_Delay(500);

	//MeterExample();
	GatewayExample();
}

/**
 * @brief Command mode example repeatedly transmitting data via radio
 */
void MeterExample(void)
{
	uint8_t role = MetisE_wmBusRole_Meter;
	Examples_Print("Set role", MetisE_SetVolatile_wmBusRole(role));
	WE_Delay(500);

	int8_t txPower = 4;
	Examples_Print("Set default txPower", MetisE_SetVolatile_TXPower(txPower));
	WE_Delay(500);

	/* example wM-Bus compliant data taken from OMS specification Annex N 5.2*/
	uint8_t data[] = {0x47, 0x93, 0x44, 0x44, 0x33, 0x22, 0x11, 0x55, 0x37,0x8C, 0x20, 0x75, 0x8B, 0x88, 0x77, 0x66, 0x55, 0x93, 0x44, 0x55, 0x08, 0xFF, 0x04, 0x00, 0x00};

	while (1)
	{
		if (false == MetisE_Transmit(MetisE_wmBusMode_T_Meter, MetisE_FrameFormt_A, data, sizeof(data)))
		{
			WE_DEBUG_PRINT("Transmission error\r\n");
		}
		WE_Delay(5000);
	}

	return;
}


static void GatewayExample()
{
	uint8_t role = MetisE_wmBusRole_Gateway;
	Examples_Print("Set role", MetisE_SetVolatile_wmBusRole(role));
	WE_Delay(500);

	uint8_t mode = MetisE_wmBusMode_CT_Other;
	Examples_Print("Set mode", MetisE_SetVolatile_wmBusRxMode(mode));
	WE_Delay(500);

	uint8_t cfgFlags = METIS_E_CFG_RSSI_ENABLE | METIS_E_CFG_TIMESTAMP_ENABLE;
	Examples_Print("Enable timestamp and rssi output", MetisE_SetVolatile_CFGFlags(cfgFlags));
	WE_Delay(500);

	if(false == MetisE_ReceiveStart())
	{
		WE_DEBUG_PRINT("Could not enter receive mode\r\n");
	}

	while(1)
	{
		WE_Delay(1000);
	}
}
