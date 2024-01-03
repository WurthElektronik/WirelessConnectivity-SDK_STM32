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
 * @brief MultiModule_ProteusIII_TarvosIII_Examples example.
 */

#include <ProteusIII/ProteusIII.h>
#include <TarvosIII/TarvosIII.h>
#include <global/global.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define BUFFERSIZE 10
typedef struct DataStorage_t
{
	uint8_t *dataP;
	uint16_t data_len;
} DataStorage_t;
DataStorage_t Proteus2TarvosBuffer[BUFFERSIZE] = {
		{
				NULL,
				0 } };
DataStorage_t Tarvos2ProteusBuffer[BUFFERSIZE] = {
		{
				NULL,
				0 } };

/* Callback functions for various indications sent by the Proteus-III. */
static void ProteusIII_RxCallback(uint8_t *payload, uint16_t payloadLength, uint8_t *btMac, int8_t rssi);
static void ProteusIII_ConnectCallback(bool success, uint8_t *btMac);
static void ProteusIII_DisconnectCallback(ProteusIII_DisconnectReason_t reason);
static void ProteusIII_ChannelOpenCallback(uint8_t *btMac, uint16_t maxPayload);

static void TarvosIII_RxCallback(uint8_t *payload, uint8_t payload_length, uint8_t dest_network_id, uint8_t dest_address_lsb, uint8_t dest_address_msb, int8_t rssi);

/**
 * @brief Definition of the uart and pins of Proteus-III
 */
static ProteusIII_Pins_t ProteusIII_pins;
static WE_UART_t ProteusIII_uart;

/**
 * @brief Definition of the uart and pins of Tarvos-III
 */
static TarvosIII_Pins_t TarvosIII_pins;
static WE_UART_t TarvosIII_uart;

/**
 * @brief Function to init data storage to buffer data between the two modules
 */
static bool DataStorage_Init(DataStorage_t *dataStorage)
{
	for (uint8_t i = 0; i < BUFFERSIZE; i++)
	{
		dataStorage[i].dataP = NULL;
		dataStorage[i].data_len = 0;
	}
	return true;
}

/**
 * @brief Function to get next entry from data storage
 */
static bool DataStorage_GetNextEntry(DataStorage_t *dataStorage, DataStorage_t **entryPP)
{
	for (uint8_t i = 0; i < BUFFERSIZE; i++)
	{
		if (dataStorage[i].dataP != NULL)
		{
			*entryPP = &dataStorage[i];
			printf("Get entry %d OK\r\n", i);
			return true;
		}
	}
	*entryPP = NULL;
	return false;
}

/**
 * @brief Function to create new entry in data storage
 */
static bool DataStorage_Create_Entry(DataStorage_t *dataStorage, uint8_t *data, uint16_t data_len)
{
	for (uint8_t i = 0; i < BUFFERSIZE; i++)
	{
		if (dataStorage[i].dataP == NULL)
		{
			dataStorage[i].dataP = malloc(data_len);
			if (dataStorage[i].dataP != NULL)
			{
				/* copy data */
				memcpy(dataStorage[i].dataP, data, data_len);
				dataStorage[i].data_len = data_len;
				printf("Create entry %d OK\r\n", i);
				return true;
			}

			printf("Create entry memory alloc failed\r\n");
			return false;
		}
	}
	printf("Create entry failed\r\n");
	return false;
}

/**
 * @brief Function to remove entry from data storage
 */
static bool DataStorage_Delete_Entry(DataStorage_t *dataStorage, DataStorage_t *entryP)
{
	for (uint8_t i = 0; i < BUFFERSIZE; i++)
	{
		if (&dataStorage[i] == entryP)
		{
			free(dataStorage[i].dataP);
			dataStorage[i].dataP = NULL;
			dataStorage[i].data_len = 0;
			printf("Delete entry %d OK\r\n", i);
			return true;
		}
	}
	printf("Delete entry FAILED\r\n");
	return false;
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
 * @brief MultiModule ProteusIII TarvosIII example.
 *
 * This example transmits data (received by Proteus-III radio module) by Tarvos-III
 * and vice versa. If no Bluetooth connection is open, the received data packets are buffered.
 */
void MultiModule_ProteusIII_TarvosIII_Examples(void)
{
	bool ret = false;

	DataStorage_Init(Tarvos2ProteusBuffer);
	DataStorage_Init(Proteus2TarvosBuffer);

	ProteusIII_pins.ProteusIII_Pin_Reset.port = (void*) GPIOA;
	ProteusIII_pins.ProteusIII_Pin_Reset.pin = GPIO_PIN_10;
	ProteusIII_pins.ProteusIII_Pin_SleepWakeUp.port = (void*) GPIOA;
	ProteusIII_pins.ProteusIII_Pin_SleepWakeUp.pin = GPIO_PIN_9;
	ProteusIII_pins.ProteusIII_Pin_Boot.port = (void*) GPIOA;
	ProteusIII_pins.ProteusIII_Pin_Boot.pin = GPIO_PIN_7;
	ProteusIII_pins.ProteusIII_Pin_Mode.port = (void*) GPIOA;
	ProteusIII_pins.ProteusIII_Pin_Mode.pin = GPIO_PIN_8;
	ProteusIII_pins.ProteusIII_Pin_Busy.port = (void*) GPIOB;
	ProteusIII_pins.ProteusIII_Pin_Busy.pin = GPIO_PIN_8;
	ProteusIII_pins.ProteusIII_Pin_StatusLed2.port = (void*) GPIOB;
	ProteusIII_pins.ProteusIII_Pin_StatusLed2.pin = GPIO_PIN_9;

	ProteusIII_uart.baudrate = PROTEUSIII_DEFAULT_BAUDRATE;
	ProteusIII_uart.flowControl = WE_FlowControl_NoFlowControl;
	ProteusIII_uart.parity = WE_Parity_None;
	ProteusIII_uart.uartInit = WE_UART1_Init;
	ProteusIII_uart.uartDeinit = WE_UART1_DeInit;
	ProteusIII_uart.uartTransmit = WE_UART1_Transmit;

	ProteusIII_CallbackConfig_t callbackConfig = {
			0 };
	callbackConfig.rxCb = ProteusIII_RxCallback;
	callbackConfig.connectCb = ProteusIII_ConnectCallback;
	callbackConfig.disconnectCb = ProteusIII_DisconnectCallback;
	callbackConfig.channelOpenCb = ProteusIII_ChannelOpenCallback;

	TarvosIII_pins.TarvosIII_Pin_Reset.port = (void*) GPIOA;
	TarvosIII_pins.TarvosIII_Pin_Reset.pin = GPIO_PIN_11;
	TarvosIII_pins.TarvosIII_Pin_SleepWakeUp.port = (void*) GPIOA;
	TarvosIII_pins.TarvosIII_Pin_SleepWakeUp.pin = GPIO_PIN_12;
	TarvosIII_pins.TarvosIII_Pin_Boot.port = (void*) GPIOA;
	TarvosIII_pins.TarvosIII_Pin_Boot.pin = GPIO_PIN_1;
	TarvosIII_pins.TarvosIII_Pin_Mode.port = (void*) GPIOA;
	TarvosIII_pins.TarvosIII_Pin_Mode.pin = GPIO_PIN_4;

	TarvosIII_uart.baudrate = TARVOSIII_DEFAULT_BAUDRATE;
	TarvosIII_uart.flowControl = WE_FlowControl_NoFlowControl;
	TarvosIII_uart.parity = WE_Parity_None;

#if defined(STM32L073xx)
	TarvosIII_uart.uartInit = WE_UART4_Init;
	TarvosIII_uart.uartDeinit = WE_UART4_DeInit;
	TarvosIII_uart.uartTransmit = WE_UART4_Transmit;
#elif defined(STM32F401xE)
	TarvosIII_uart.uartInit = WE_UART6_Init;
	TarvosIII_uart.uartDeinit = WE_UART6_DeInit;
	TarvosIII_uart.uartTransmit = WE_UART6_Transmit;
#endif

	if (false == ProteusIII_Init(&ProteusIII_uart, &ProteusIII_pins, ProteusIII_OperationMode_CommandMode, callbackConfig))
	{
		printf("Initialization error\r\n");
		return;
	}
	if (false == TarvosIII_Init(&TarvosIII_uart, &TarvosIII_pins, TarvosIII_AddressMode_0, TarvosIII_RxCallback))
	{
		printf("Initialization error\r\n");
		return;
	}

	uint8_t ProteusIII_fwVersion[3];
	ret = ProteusIII_GetFWVersion(ProteusIII_fwVersion);
	Examples_Print("Get firmware version", ret);
	printf("Firmware version of Proteus-III is %u.%u.%u\r\n", ProteusIII_fwVersion[2], ProteusIII_fwVersion[1], ProteusIII_fwVersion[0]);
	uint8_t btMac[6];
	ret = ProteusIII_GetBTMAC(btMac);
	Examples_Print("Get BT MAC", ret);
	printf("Proteus-III BTMAC is 0x%02x%02x%02x%02x%02x%02x\r\n", btMac[0], btMac[1], btMac[2], btMac[3], btMac[4], btMac[5]);
	WE_Delay(500);

	uint8_t TarvosIII_fwVersion[3];
	ret = TarvosIII_GetFirmwareVersion(TarvosIII_fwVersion);
	Examples_Print("Get firmware version", ret);
	printf("Firmware version of Tarvos-III is %u.%u.%u\r\n", TarvosIII_fwVersion[0], TarvosIII_fwVersion[1], TarvosIII_fwVersion[2]);
	uint8_t SN[4];
	ret = TarvosIII_GetSerialNumber(SN);
	Examples_Print("Get Tarvos-III SN", ret);
	printf("Tarvos-III serial number is 0x%02x%02x%02x%02x\r\n", SN[0], SN[1], SN[2], SN[3]);
	WE_Delay(500);

	DataStorage_t *entryP;
	while (1)
	{
		if (ProteusIII_DriverState_BLE_ChannelOpen == ProteusIII_GetDriverState())
		{
			/* relay data to Proteus */
			uint8_t i = 0;
			while (i < 3)
			{
				if (true == DataStorage_GetNextEntry(Tarvos2ProteusBuffer, &entryP))
				{
					if (true == ProteusIII_Transmit(entryP->dataP, entryP->data_len))
					{
						printf("Relayed data (%d bytes) from Tarvos to Proteus\r\n", entryP->data_len);
						DataStorage_Delete_Entry(Tarvos2ProteusBuffer, entryP);
					}
					else
					{
						printf("Transmission failed\r\n");
					}
				}
				else
				{
					/* leave loop */
					break;
				}
			}
		}
		else
		{
			printf("Wait for channel open\r\n");
			WE_Delay(1000);
		}

		/* relay data to Tarvos */
		uint8_t i = 0;
		while (i < 3)
		{
			if (true == DataStorage_GetNextEntry(Proteus2TarvosBuffer, &entryP))
			{
				if (true == TarvosIII_Transmit(entryP->dataP, entryP->data_len))
				{
					printf("Relayed data (%d bytes) from Proteus to Tarvos\r\n", entryP->data_len);
					DataStorage_Delete_Entry(Proteus2TarvosBuffer, entryP);
				}
				else
				{
					printf("Transmission failed\r\n");
				}
			}
			else
			{
				/* leave loop */
				break;
			}
		}

		WE_Delay(10);
	}

	return;
}

/**
 * @brief Callback called when data has been received via Proteus-III radio
 */
static void ProteusIII_RxCallback(uint8_t *payload, uint16_t payloadLength, uint8_t *btMac, int8_t rssi)
{
	printf("Received data from device with BTMAC (0x%02x%02x%02x%02x%02x%02x) with RSSI = %d dBm:\r\n", btMac[0], btMac[1], btMac[2], btMac[3], btMac[4], btMac[5], rssi);
#if false
	uint16_t i = 0;
	printf("-> 0x ");
	for (i = 0; i < payloadLength; i++)
	{
		printf("%02x ", *(payload + i));
	}
	printf("\r\n-> ");
	for (i = 0; i < payloadLength; i++)
	{
		printf("%c", *(payload + i));
	}
	printf("\r\n");
	fflush(stdout);
#endif
	DataStorage_Create_Entry(Proteus2TarvosBuffer, payload, payloadLength);
}

/**
 * @brief Callback called when Proteus-III connection request is received
 */
static void ProteusIII_ConnectCallback(bool success, uint8_t *btMac)
{
	printf("%s to device with BTMAC (0x%02x%02x%02x%02x%02x%02x)\r\n", success ? "Connected" : "Failed to connect", btMac[0], btMac[1], btMac[2], btMac[3], btMac[4], btMac[5]);
	fflush(stdout);
}

/**
 * @brief Callback called when Proteus-III connection is dropped
 */
static void ProteusIII_DisconnectCallback(ProteusIII_DisconnectReason_t reason)
{
	static const char *reasonStrings[] = {
			"unknown",
			"connection timeout",
			"user terminated connection",
			"host terminated connection",
			"connection interval unacceptable",
			"MIC failure",
			"connection setup failed" };

	printf("Disconnected (reason: %s)\r\n", reasonStrings[reason]);
	fflush(stdout);
}

/**
 * @brief Callback called when Proteus-III Bluetooth channel has been opened
 */
static void ProteusIII_ChannelOpenCallback(uint8_t *btMac, uint16_t maxPayload)
{
	printf("Channel opened to BTMAC (0x%02x%02x%02x%02x%02x%02x) with maximum payload = %d\r\n", btMac[0], btMac[1], btMac[2], btMac[3], btMac[4], btMac[5], maxPayload);
	fflush(stdout);
}

/**
 * @brief Callback called when data has been received via Tarvos-III radio
 */
static void TarvosIII_RxCallback(uint8_t *payload, uint8_t payload_length, uint8_t dest_network_id, uint8_t dest_address_lsb, uint8_t dest_address_msb, int8_t rssi)
{
	printf("Received data from address (NetID:0x%02x, Addr:0x%02x%02x) with %d dBm:\r\n", dest_network_id, dest_address_lsb, dest_address_msb, rssi);
#if false
    uint8_t i = 0;
    printf("-> 0x ");
    for(i=0; i<payload_length; i++)
    {
        printf("%02x ", *(payload+i));
    }
    printf("\r\n-> ");
    for(i=0; i<payload_length; i++)
    {
        printf("%c", *(payload+i));
    }
    printf("\r\n");
    fflush(stdout);
#endif
	DataStorage_Create_Entry(Tarvos2ProteusBuffer, payload, payload_length);
}

