﻿/*
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
 * @brief Thyone-e driver source file.
 */

#include "ThyoneE.h"

#include <stdio.h>
#include <string.h>

#include "../global/global.h"

typedef enum ThyoneE_Pin_t
{
	ThyoneE_Pin_Reset,
	ThyoneE_Pin_Mode,
	ThyoneE_Pin_Busy,
	ThyoneE_Pin_Count
} ThyoneE_Pin_t;

#define CMD_WAIT_TIME 1500
#define CMD_WAIT_TIME_STEP_MS 0
#define CNFINVALID 255

/* Normal overhead: Start signal + Command + Length + CS = 1+1+2+1=5 bytes */
#define LENGTH_CMD_OVERHEAD             (uint16_t)5
#define LENGTH_CMD_OVERHEAD_WITHOUT_CRC (uint16_t)(LENGTH_CMD_OVERHEAD - 1)
/* Max. overhead (used by CMD_SNIFFER_IND):
 * Start signal + Command + Length + Src Addr + DATA_IND + RSSI + CS = 1+1+2+4+1+1+1=11 bytes */
#define LENGTH_CMD_OVERHEAD_MAX         (uint16_t)11
#define MAX_PAYLOAD_LENGTH              (uint16_t)224
#define MAX_PAYLOAD_LENGTH_MULTICAST_EX (uint16_t)223
#define MAX_PAYLOAD_LENGTH_UNICAST_EX   (uint16_t)220
#define MAX_CMD_LENGTH                  (uint16_t)(MAX_PAYLOAD_LENGTH + LENGTH_CMD_OVERHEAD_MAX)

typedef struct
{
	uint8_t Stx;
	uint8_t Cmd;
	uint16_t Length;
	uint8_t Data[MAX_CMD_LENGTH + 1]; /* +1 from CS */

} ThyoneE_CMD_Frame_t;

#define CMD_STX 0x02

#define THYONEE_CMD_TYPE_REQ (uint8_t)(0 << 6)
#define THYONEE_CMD_TYPE_CNF (uint8_t)(1 << 6)
#define THYONEE_CMD_TYPE_IND (uint8_t)(2 << 6)
#define THYONEE_CMD_TYPE_RSP (uint8_t)(3 << 6)

#define THYONEE_CMD_RESET (uint8_t)0x00
#define THYONEE_CMD_RESET_REQ (THYONEE_CMD_RESET | THYONEE_CMD_TYPE_REQ)
#define THYONEE_CMD_RESET_CNF (THYONEE_CMD_RESET | THYONEE_CMD_TYPE_CNF)

#define THYONEE_CMD_GETSTATE (uint8_t)0x01
#define THYONEE_CMD_GETSTATE_REQ (THYONEE_CMD_GETSTATE | THYONEE_CMD_TYPE_REQ)
#define THYONEE_CMD_GETSTATE_CNF (THYONEE_CMD_GETSTATE | THYONEE_CMD_TYPE_CNF)

#define THYONEE_CMD_SLEEP (uint8_t)0x02
#define THYONEE_CMD_SLEEP_REQ (THYONEE_CMD_SLEEP | THYONEE_CMD_TYPE_REQ)
#define THYONEE_CMD_SLEEP_CNF (THYONEE_CMD_SLEEP | THYONEE_CMD_TYPE_CNF)
#define THYONEE_CMD_SLEEP_IND (THYONEE_CMD_SLEEP | THYONEE_CMD_TYPE_IND)

#define THYONEE_CMD_START_IND (uint8_t)0x73

#define THYONEE_CMD_UNICAST_DATA (uint8_t)0x04
#define THYONEE_CMD_UNICAST_DATA_REQ (THYONEE_CMD_UNICAST_DATA | THYONEE_CMD_TYPE_REQ)

/* Transmissions of any kind will be confirmed and indicated by the same message CMD_DATA_CNF or CMD_DATA_IND */
#define THYONEE_CMD_DATA_CNF (THYONEE_CMD_UNICAST_DATA | THYONEE_CMD_TYPE_CNF)
#define THYONEE_CMD_DATA_IND (THYONEE_CMD_UNICAST_DATA | THYONEE_CMD_TYPE_IND)
#define THYONEE_CMD_TXCOMPLETE_RSP (THYONEE_CMD_UNICAST_DATA | THYONEE_CMD_TYPE_RSP)

#define THYONEE_CMD_MULTICAST_DATA (uint8_t)0x05
#define THYONEE_CMD_MULTICAST_DATA_REQ (THYONEE_CMD_MULTICAST_DATA | THYONEE_CMD_TYPE_REQ)

#define THYONEE_CMD_BROADCAST_DATA (uint8_t)0x06
#define THYONEE_CMD_BROADCAST_DATA_REQ (THYONEE_CMD_BROADCAST_DATA | THYONEE_CMD_TYPE_REQ)

#define THYONEE_CMD_UNICAST_DATA_EX (uint8_t)0x07
#define THYONEE_CMD_UNICAST_DATA_EX_REQ (THYONEE_CMD_UNICAST_DATA_EX | THYONEE_CMD_TYPE_REQ)

#define THYONEE_CMD_MULTICAST_DATA_EX (uint8_t)0x08
#define THYONEE_CMD_MULTICAST_DATA_EX_REQ (THYONEE_CMD_MULTICAST_DATA_EX | THYONEE_CMD_TYPE_REQ)

#define THYONEE_CMD_SNIFFER_IND (uint8_t)0x99

#define THYONEE_CMD_SETCHANNEL (uint8_t)0x09
#define THYONEE_CMD_SETCHANNEL_REQ (THYONEE_CMD_SETCHANNEL | THYONEE_CMD_TYPE_REQ)
#define THYONEE_CMD_SETCHANNEL_CNF (THYONEE_CMD_SETCHANNEL | THYONEE_CMD_TYPE_CNF)

#define THYONEE_CMD_GET (uint8_t)0x10
#define THYONEE_CMD_GET_REQ (THYONEE_CMD_GET | THYONEE_CMD_TYPE_REQ)
#define THYONEE_CMD_GET_CNF (THYONEE_CMD_GET | THYONEE_CMD_TYPE_CNF)

#define THYONEE_CMD_SET (uint8_t)0x11
#define THYONEE_CMD_SET_REQ (THYONEE_CMD_SET | THYONEE_CMD_TYPE_REQ)
#define THYONEE_CMD_SET_CNF (THYONEE_CMD_SET | THYONEE_CMD_TYPE_CNF)

#define THYONEE_CMD_FACTORYRESET (uint8_t)0x1C
#define THYONEE_CMD_FACTORYRESET_REQ (THYONEE_CMD_FACTORYRESET | THYONEE_CMD_TYPE_REQ)
#define THYONEE_CMD_FACTORYRESET_CNF (THYONEE_CMD_FACTORYRESET | THYONEE_CMD_TYPE_CNF)

#define THYONEE_CMD_GPIO_LOCAL_SETCONFIG (uint8_t)0x25
#define THYONEE_CMD_GPIO_LOCAL_SETCONFIG_REQ (THYONEE_CMD_GPIO_LOCAL_SETCONFIG | THYONEE_CMD_TYPE_REQ)
#define THYONEE_CMD_GPIO_LOCAL_SETCONFIG_CNF (THYONEE_CMD_GPIO_LOCAL_SETCONFIG | THYONEE_CMD_TYPE_CNF)

#define THYONEE_CMD_GPIO_LOCAL_GETCONFIG (uint8_t)0x26
#define THYONEE_CMD_GPIO_LOCAL_GETCONFIG_REQ (THYONEE_CMD_GPIO_LOCAL_GETCONFIG | THYONEE_CMD_TYPE_REQ)
#define THYONEE_CMD_GPIO_LOCAL_GETCONFIG_CNF (THYONEE_CMD_GPIO_LOCAL_GETCONFIG | THYONEE_CMD_TYPE_CNF)

#define THYONEE_CMD_GPIO_LOCAL_WRITE (uint8_t)0x27
#define THYONEE_CMD_GPIO_LOCAL_WRITE_REQ (THYONEE_CMD_GPIO_LOCAL_WRITE | THYONEE_CMD_TYPE_REQ)
#define THYONEE_CMD_GPIO_LOCAL_WRITE_CNF (THYONEE_CMD_GPIO_LOCAL_WRITE | THYONEE_CMD_TYPE_CNF)

#define THYONEE_CMD_GPIO_LOCAL_READ (uint8_t)0x28
#define THYONEE_CMD_GPIO_LOCAL_READ_REQ (THYONEE_CMD_GPIO_LOCAL_READ | THYONEE_CMD_TYPE_REQ)
#define THYONEE_CMD_GPIO_LOCAL_READ_CNF (THYONEE_CMD_GPIO_LOCAL_READ | THYONEE_CMD_TYPE_CNF)

#define THYONEE_CMD_GPIO_REMOTE_SETCONFIG (uint8_t)0x29
#define THYONEE_CMD_GPIO_REMOTE_SETCONFIG_REQ (THYONEE_CMD_GPIO_REMOTE_SETCONFIG | THYONEE_CMD_TYPE_REQ)
#define THYONEE_CMD_GPIO_REMOTE_SETCONFIG_CNF (THYONEE_CMD_GPIO_REMOTE_SETCONFIG | THYONEE_CMD_TYPE_CNF)

#define THYONEE_CMD_GPIO_REMOTE_GETCONFIG (uint8_t)0x2A
#define THYONEE_CMD_GPIO_REMOTE_GETCONFIG_REQ (THYONEE_CMD_GPIO_REMOTE_GETCONFIG | THYONEE_CMD_TYPE_REQ)
#define THYONEE_CMD_GPIO_REMOTE_GETCONFIG_CNF (THYONEE_CMD_GPIO_REMOTE_GETCONFIG | THYONEE_CMD_TYPE_CNF)
#define THYONEE_CMD_GPIO_REMOTE_GETCONFIG_RSP (THYONEE_CMD_GPIO_REMOTE_GETCONFIG | THYONEE_CMD_TYPE_RSP)

#define THYONEE_CMD_GPIO_REMOTE_WRITE (uint8_t)0x2B
#define THYONEE_CMD_GPIO_REMOTE_WRITE_REQ (THYONEE_CMD_GPIO_REMOTE_WRITE | THYONEE_CMD_TYPE_REQ)
#define THYONEE_CMD_GPIO_REMOTE_WRITE_CNF (THYONEE_CMD_GPIO_REMOTE_WRITE | THYONEE_CMD_TYPE_CNF)

#define THYONEE_CMD_GPIO_REMOTE_READ (uint8_t)0x2C
#define THYONEE_CMD_GPIO_REMOTE_READ_REQ (THYONEE_CMD_GPIO_REMOTE_READ | THYONEE_CMD_TYPE_REQ)
#define THYONEE_CMD_GPIO_REMOTE_READ_CNF (THYONEE_CMD_GPIO_REMOTE_READ | THYONEE_CMD_TYPE_CNF)
#define THYONEE_CMD_GPIO_REMOTE_READ_RSP (THYONEE_CMD_GPIO_REMOTE_READ | THYONEE_CMD_TYPE_RSP)

/**
 * @brief Type used to check the response, when a command was sent to the ThyoneE
 */
typedef enum ThyoneE_CMD_Status_t
{
	CMD_Status_Success = (uint8_t) 0x00,
	CMD_Status_Failed = (uint8_t) 0x01,
	CMD_Status_Invalid,
	CMD_Status_Reset,
	CMD_Status_NoStatus,
} ThyoneE_CMD_Status_t;

typedef struct
{
	uint8_t cmd; /* variable to check if correct CMD has been confirmed */
	ThyoneE_CMD_Status_t status; /* variable used to check the response (*_CNF), when a request (*_REQ) was sent to the ThyoneE */
} ThyoneE_CMD_Confirmation_t;

/**************************************
 *          Static variables          *
 **************************************/
static ThyoneE_CMD_Frame_t txPacket = {
		.Stx = CMD_STX,
		.Length = 0 }; /* request to be sent to the module */
static ThyoneE_CMD_Frame_t rxPacket = {
		.Stx = CMD_STX,
		.Length = 0 }; /* received packet that has been sent by the module */
;

#define CMDCONFIRMATIONARRAY_LENGTH 3
static ThyoneE_CMD_Confirmation_t cmdConfirmation_array[CMDCONFIRMATIONARRAY_LENGTH];
static WE_Pin_t ThyoneE_pins[ThyoneE_Pin_Count] = {
		0 };
static uint8_t checksum = 0;
static uint16_t rxByteCounter = 0;
static uint16_t bytesToReceive = 0;
static uint8_t rxBuffer[sizeof(ThyoneE_CMD_Frame_t)]; /* For UART RX from module */
void (*RxCallback)(uint8_t*, uint16_t, uint32_t, int8_t); /* callback function */
static ThyoneE_OperationMode_t operationMode = ThyoneE_OperationMode_CommandMode;

/**************************************
 *         Static functions           *
 **************************************/

static void HandleRxPacket(uint8_t *prxBuffer)
{
	ThyoneE_CMD_Confirmation_t cmdConfirmation;
	cmdConfirmation.cmd = CNFINVALID;
	cmdConfirmation.status = CMD_Status_Invalid;

	uint16_t cmdLength = ((ThyoneE_CMD_Frame_t*) prxBuffer)->Length;
	memcpy(&rxPacket, prxBuffer, cmdLength + LENGTH_CMD_OVERHEAD);

	switch (rxPacket.Cmd)
	{
	case THYONEE_CMD_RESET_CNF:
	case THYONEE_CMD_START_IND :
	case THYONEE_CMD_GPIO_REMOTE_GETCONFIG_RSP:
	case THYONEE_CMD_GPIO_REMOTE_READ_RSP:
	{
		cmdConfirmation.cmd = rxPacket.Cmd;
		cmdConfirmation.status = CMD_Status_NoStatus;
		break;
	}

	case THYONEE_CMD_DATA_CNF:
	case THYONEE_CMD_GET_CNF:
	case THYONEE_CMD_SET_CNF:
	case THYONEE_CMD_SETCHANNEL_CNF:
	case THYONEE_CMD_FACTORYRESET_CNF:
	case THYONEE_CMD_SLEEP_CNF:
	case THYONEE_CMD_GPIO_LOCAL_SETCONFIG_CNF:
	case THYONEE_CMD_GPIO_LOCAL_GETCONFIG_CNF:
	case THYONEE_CMD_GPIO_LOCAL_WRITE_CNF:
	case THYONEE_CMD_GPIO_LOCAL_READ_CNF:
	case THYONEE_CMD_GPIO_REMOTE_SETCONFIG_CNF:
	case THYONEE_CMD_GPIO_REMOTE_GETCONFIG_CNF:
	case THYONEE_CMD_GPIO_REMOTE_WRITE_CNF:
	case THYONEE_CMD_TXCOMPLETE_RSP:
	case THYONEE_CMD_GETSTATE_CNF:
	{
		cmdConfirmation.cmd = rxPacket.Cmd;
		cmdConfirmation.status = rxPacket.Data[0];
		break;
	}

	case THYONEE_CMD_GPIO_REMOTE_READ_CNF:
	{
		cmdConfirmation.cmd = rxPacket.Cmd;
		cmdConfirmation.status = CMD_Status_Invalid;

		break;
	}

	case THYONEE_CMD_DATA_IND:
	{
		if (RxCallback != NULL)
		{
			uint32_t src_address;
			memcpy(&src_address, &rxPacket.Data[0], 4);
			RxCallback(&rxPacket.Data[5], rxPacket.Length - 5, src_address, rxPacket.Data[4]);
		}
		break;
	}

	case THYONEE_CMD_SNIFFER_IND :
	{
		if (RxCallback != NULL)
		{
			uint32_t src_address;
			memcpy(&src_address, &rxPacket.Data[0], 4);
			RxCallback(&rxPacket.Data[6], rxPacket.Length - 6, src_address, rxPacket.Data[4]);
		}
		break;
	}

	default:
	{
		/* invalid*/
		break;
	}
	}

	for (uint16_t i = 0; i < CMDCONFIRMATIONARRAY_LENGTH; i++)
	{
		if (cmdConfirmation_array[i].cmd == CNFINVALID)
		{
			cmdConfirmation_array[i].cmd = cmdConfirmation.cmd;
			cmdConfirmation_array[i].status = cmdConfirmation.status;
			break;
		}
	}
}

/**
 * @brief Function that waits for the return value of ThyoneE (*_CNF), when a command (*_REQ) was sent before
 */
static bool Wait4CNF(int max_time_ms, uint8_t expectedCmdConfirmation, ThyoneE_CMD_Status_t expectedStatus, bool reset_confirmstate)
{
	uint32_t t0 = WE_GetTick();

	if (reset_confirmstate)
	{
		for (uint16_t i = 0; i < CMDCONFIRMATIONARRAY_LENGTH; i++)
		{
			cmdConfirmation_array[i].cmd = CNFINVALID;
		}
	}
	while (1)
	{
		for (uint16_t i = 0; i < CMDCONFIRMATIONARRAY_LENGTH; i++)
		{
			if (expectedCmdConfirmation == cmdConfirmation_array[i].cmd)
			{
				return (cmdConfirmation_array[i].status == expectedStatus);
			}
		}

		uint32_t now = WE_GetTick();
		if (now - t0 > max_time_ms)
		{
			/* received no correct response within timeout */
			return false;
		}

		if (CMD_WAIT_TIME_STEP_MS > 0)
		{
			/* wait */
			WE_Delay(CMD_WAIT_TIME_STEP_MS);
		}
	}
	return true;
}

/**
 * @brief Function to add the checksum at the end of the data packet.
 */
static bool FillChecksum(ThyoneE_CMD_Frame_t *cmd)
{
	if ((cmd->Length >= 0) && (cmd->Stx == CMD_STX))
	{
		uint8_t checksum = (uint8_t) 0;
		uint8_t *pArray = (uint8_t*) cmd;
		uint16_t i = 0;
		for (i = 0; i < (cmd->Length + LENGTH_CMD_OVERHEAD_WITHOUT_CRC ); i++)
		{
			checksum ^= pArray[i];
		}
		cmd->Data[cmd->Length] = checksum;
		return true;
	}
	return false;
}

/**************************************
 *         Global functions           *
 **************************************/

void WE_UART_HandleRxByte(uint8_t received_byte)
{
	rxBuffer[rxByteCounter] = received_byte;

	switch (rxByteCounter)
	{
	case 0:
		/* wait for start byte of frame */
		if (rxBuffer[rxByteCounter] == CMD_STX)
		{
			bytesToReceive = 0;
			rxByteCounter = 1;
		}
		break;

	case 1:
		/* CMD */
		rxByteCounter++;
		break;

	case 2:
		/* length field LSB */
		rxByteCounter++;
		bytesToReceive = (uint16_t) (rxBuffer[rxByteCounter - 1]);
		break;

	case 3:
		/* length field MSB */
		rxByteCounter++;
		bytesToReceive += (((uint16_t) rxBuffer[rxByteCounter - 1] << 8) + LENGTH_CMD_OVERHEAD ); /* len_msb + len_lsb + crc + sfd + cmd */
		break;

	default:
		/* data field */
		rxByteCounter++;
		if (rxByteCounter == bytesToReceive)
		{
			/* check CRC */
			checksum = 0;
			for (uint16_t i = 0; i < (bytesToReceive - 1); i++)
			{
				checksum ^= rxBuffer[i];
			}

			if (checksum == rxBuffer[bytesToReceive - 1])
			{
				/* received frame ok, interpret it now */
				HandleRxPacket(rxBuffer);
			}

			rxByteCounter = 0;
			bytesToReceive = 0;
		}
		break;
	}
}

/**
 * @brief Initialize the ThyoneE interface for serial interface
 *
 * Caution: The parameter baudrate must match the configured UserSettings of the ThyoneE.
 *          The baudrate parameter must match to perform a successful FTDI communication.
 *          Updating this parameter during runtime may lead to communication errors.
 *
 * @param[in] baudrate:       baudrate of the interface
 * @param[in] flow_control:   enable/disable flowcontrol
 * @param[in] opMode:   	  operation mode
 * @param[in] RXcb:           RX callback function
 *
 * @return true if initialization succeeded,
 *         false otherwise
 */
bool ThyoneE_Init(uint32_t baudrate, WE_FlowControl_t flow_control, ThyoneE_OperationMode_t opMode, void (*RXcb)(uint8_t*, uint16_t, uint32_t, int8_t))
{
	/* set RX callback function */
	RxCallback = RXcb;

	operationMode = opMode;

	/* initialize the pins */
	ThyoneE_pins[ThyoneE_Pin_Reset].port = GPIOA;
	ThyoneE_pins[ThyoneE_Pin_Reset].pin = GPIO_PIN_10;
	ThyoneE_pins[ThyoneE_Pin_Reset].type = WE_Pin_Type_Output;
	ThyoneE_pins[ThyoneE_Pin_Mode].port = GPIOA;
	ThyoneE_pins[ThyoneE_Pin_Mode].pin = GPIO_PIN_8;
	ThyoneE_pins[ThyoneE_Pin_Mode].type = WE_Pin_Type_Output;
	ThyoneE_pins[ThyoneE_Pin_Busy].port = GPIOA;
	ThyoneE_pins[ThyoneE_Pin_Busy].pin = GPIO_PIN_9;
	ThyoneE_pins[ThyoneE_Pin_Busy].type = WE_Pin_Type_Input;
	if (false == WE_InitPins(ThyoneE_pins, ThyoneE_Pin_Count))
	{
		/* error */
		return false;
	}
	WE_SetPin(ThyoneE_pins[ThyoneE_Pin_Reset], WE_Pin_Level_High);
	WE_SetPin(ThyoneE_pins[ThyoneE_Pin_Mode], (operationMode == ThyoneE_OperationMode_TransparentMode) ? WE_Pin_Level_High : WE_Pin_Level_Low);


	WE_UART_Init(baudrate, flow_control, WE_Parity_None, true);
	WE_Delay(10);

	/* reset module */
	if (ThyoneE_PinReset())
	{
		WE_Delay(THYONEE_BOOT_DURATION);
	}
	else
	{
		fprintf(stdout, "Pin reset failed\n");
		ThyoneE_Deinit();
		return false;
	}

	uint8_t driverVersion[3];
	if (WE_GetDriverVersion(driverVersion))
	{
		fprintf(stdout, "ThyoneE driver version %d.%d.%d\n", driverVersion[0], driverVersion[1], driverVersion[2]);
	}
	WE_Delay(100);

	return true;
}

/**
 * @brief Deinitialize the ThyoneE interface
 *
 * @return true if deinitialization succeeded,
 *         false otherwise
 */
bool ThyoneE_Deinit()
{
	/* close the communication interface to the module */
	WE_UART_DeInit();

	/* deinit pins */
	WE_DeinitPin(ThyoneE_pins[ThyoneE_Pin_Reset]);
	WE_DeinitPin(ThyoneE_pins[ThyoneE_Pin_Mode]);

	RxCallback = NULL;

	return true;
}

/**
 * @brief Reset the ThyoneE by pin
 *
 * @return true if reset succeeded,
 *         false otherwise
 */
bool ThyoneE_PinReset()
{
	WE_SetPin(ThyoneE_pins[ThyoneE_Pin_Reset], WE_Pin_Level_Low);
	WE_Delay(5);
	WE_SetPin(ThyoneE_pins[ThyoneE_Pin_Reset], WE_Pin_Level_High);

	if (operationMode == ThyoneE_OperationMode_TransparentMode)
	{
		/* transparent mode is ready (the module doesn't send a "ready for operation" message in transparent mode) */
		return true;
	}

	/* wait for cnf */
	return Wait4CNF(CMD_WAIT_TIME, THYONEE_CMD_START_IND, CMD_Status_NoStatus, true);
}

/**
 * @brief Reset the ThyoneE by command
 *
 * @return true if reset succeeded,
 *         false otherwise
 */
bool ThyoneE_Reset()
{
	bool ret = false;

	/* fill CMD_ARRAY packet */

	txPacket.Cmd = THYONEE_CMD_RESET_REQ;
	txPacket.Length = 0;

	if (FillChecksum(&txPacket))
	{

		WE_UART_Transmit((uint8_t*) &txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD);

		/* wait for cnf */
		return Wait4CNF(CMD_WAIT_TIME, THYONEE_CMD_START_IND, CMD_Status_NoStatus, true);
	}
	return ret;
}

/**
 * @brief Put the ThyoneE into sleep mode
 *
 * @return true if succeeded,
 *         false otherwise
 */
bool ThyoneE_Sleep()
{
	bool ret = false;
	/* fill CMD_ARRAY packet */

	txPacket.Cmd = THYONEE_CMD_SLEEP_REQ;
	txPacket.Length = 0;

	if (FillChecksum(&txPacket))
	{

		WE_UART_Transmit((uint8_t*) &txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD);

		/* wait for cnf */
		ret = Wait4CNF(CMD_WAIT_TIME, THYONEE_CMD_SLEEP_CNF, CMD_Status_Success, true);
	}
	return ret;
}

/**
 * @brief Transmit data as broadcast
 *
 * @param[in] PayloadP: pointer to the data to transmit
 * @param[in] length:   length of the data to transmit
 *
 * @return true if succeeded,
 *         false otherwise
 */
bool ThyoneE_TransmitBroadcast(uint8_t *payloadP, uint16_t length)
{
	bool ret = false;
	if (length <= MAX_PAYLOAD_LENGTH)
	{

		txPacket.Cmd = THYONEE_CMD_BROADCAST_DATA_REQ;
		txPacket.Length = length;

		memcpy(&txPacket.Data[0], payloadP, length);

		if (FillChecksum(&txPacket))
		{
			WE_UART_Transmit((uint8_t*) &txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD);
			ret = Wait4CNF(CMD_WAIT_TIME, THYONEE_CMD_TXCOMPLETE_RSP, CMD_Status_Success, true);
		}
	}

	return ret;
}

/**
 * @brief Transmit data as multicast
 *
 * @param[in] PayloadP: pointer to the data to transmit
 * @param[in] length:   length of the data to transmit
 *
 * @return true if succeeded,
 *         false otherwise
 */
bool ThyoneE_TransmitMulticast(uint8_t *payloadP, uint16_t length)
{
	bool ret = false;
	if (length <= MAX_PAYLOAD_LENGTH)
	{

		txPacket.Cmd = THYONEE_CMD_MULTICAST_DATA_REQ;
		txPacket.Length = length;

		memcpy(&txPacket.Data[0], payloadP, length);

		if (FillChecksum(&txPacket))
		{
			WE_UART_Transmit((uint8_t*) &txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD);
			ret = Wait4CNF(CMD_WAIT_TIME, THYONEE_CMD_TXCOMPLETE_RSP, CMD_Status_Success, true);
		}
	}

	return ret;
}

/**
 * @brief Transmit data as unicast
 *
 * @param[in] PayloadP: pointer to the data to transmit
 * @param[in] length:   length of the data to transmit
 *
 * @return true if succeeded,
 *         false otherwise
 */
bool ThyoneE_TransmitUnicast(uint8_t *payloadP, uint16_t length)
{
	bool ret = false;
	if (length <= MAX_PAYLOAD_LENGTH)
	{

		txPacket.Cmd = THYONEE_CMD_UNICAST_DATA_REQ;
		txPacket.Length = length;

		memcpy(&txPacket.Data[0], payloadP, length);

		if (FillChecksum(&txPacket))
		{
			WE_UART_Transmit((uint8_t*) &txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD);
			ret = Wait4CNF(CMD_WAIT_TIME, THYONEE_CMD_TXCOMPLETE_RSP, CMD_Status_Success, true);
		}
	}

	return ret;
}

/**
 * @brief Transmit data as multicast
 *
 * @param[in] groupID : groupID to multicast
 * @param[in] PayloadP: pointer to the data to transmit
 * @param[in] length:   length of the data to transmit
 *
 * @return true if succeeded,
 *         false otherwise
 */
bool ThyoneE_TransmitMulticastExtended(uint8_t groupID, uint8_t *payloadP, uint16_t length)
{
	bool ret = false;
	if (length <= MAX_PAYLOAD_LENGTH_MULTICAST_EX)
	{
		txPacket.Cmd = THYONEE_CMD_MULTICAST_DATA_EX_REQ;
		txPacket.Length = length + 1;
		txPacket.Data[0] = groupID;

		memcpy(&txPacket.Data[1], payloadP, length);

		if (FillChecksum(&txPacket))
		{
			WE_UART_Transmit((uint8_t*) &txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD);
			ret = Wait4CNF(CMD_WAIT_TIME, THYONEE_CMD_TXCOMPLETE_RSP, CMD_Status_Success, true);
		}
	}

	return ret;
}

/**
 * @brief Transmit data as unicast
 *
 * @param[in] address: address to sent to
 * @param[in] payloadP: pointer to the data to transmit
 * @param[in] length:   length of the data to transmit
 *
 * @return true if succeeded,
 *         false otherwise
 */
bool ThyoneE_TransmitUnicastExtended(uint32_t address, uint8_t *payloadP, uint16_t length)
{
	bool ret = false;
	if (length <= MAX_PAYLOAD_LENGTH_UNICAST_EX)
	{
		txPacket.Cmd = THYONEE_CMD_UNICAST_DATA_EX_REQ;
		txPacket.Length = length + 4;

		memcpy(&txPacket.Data[0], &address, 4);
		memcpy(&txPacket.Data[4], payloadP, length);

		if (FillChecksum(&txPacket))
		{
			WE_UART_Transmit((uint8_t*) &txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD);
			ret = Wait4CNF(CMD_WAIT_TIME, THYONEE_CMD_TXCOMPLETE_RSP, CMD_Status_Success, true);
		}
	}

	return ret;
}

/**
 * @brief Factory reset the module
 *
 * @return true if succeeded,
 *         false otherwise
 */
bool ThyoneE_FactoryReset()
{
	bool ret = false;
	/* fill CMD_ARRAY packet */

	txPacket.Cmd = THYONEE_CMD_FACTORYRESET_REQ;
	txPacket.Length = 0;

	if (FillChecksum(&txPacket))
	{

		WE_UART_Transmit((uint8_t*) &txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD);

		/* wait for reset after factory reset */
		return Wait4CNF(CMD_WAIT_TIME, THYONEE_CMD_START_IND, CMD_Status_NoStatus, true);
	}
	return ret;

}

/**
 * @brief Set a special user setting
 *
 * Note: Reset the module after the adaption of the setting so that it can take effect.
 * Note: Use this function only in rare case, since flash can be updated only a limited number of times.
 *
 * @param[in] us:     user setting to be updated
 * @param[in] value:  pointer to the new settings value
 * @param[in] length: length of the value
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ThyoneE_Set(ThyoneE_UserSettings_t userSetting, uint8_t *ValueP, uint8_t length)
{
	bool ret = false;

	/* fill CMD_ARRAY packet */

	txPacket.Cmd = THYONEE_CMD_SET_REQ;
	txPacket.Length = 1 + length;
	txPacket.Data[0] = userSetting;
	memcpy(&txPacket.Data[1], ValueP, length);

	if (FillChecksum(&txPacket))
	{

		WE_UART_Transmit((uint8_t*) &txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD);

		/* wait for cnf */
		return Wait4CNF(CMD_WAIT_TIME, THYONEE_CMD_SET_CNF, CMD_Status_Success, true);
	}
	return ret;
}

/**
 * @brief Set the tx power
 *
 * @param[in] txPower: transmit power
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ThyoneE_SetTXPower(ThyoneE_TXPower_t txPower)
{
	return ThyoneE_Set(ThyoneE_USERSETTING_INDEX_RF_TX_POWER, (uint8_t*) &txPower, 1);
}

/**
 * @brief Set the UART baudrate index
 *
 * Note: Reset the module after the adaption of the setting so that it can take effect.
 * Note: Use this function only in rare case, since flash can be updated only a limited number of times.
 *
 * @param[in] baudrateindex: UART baudrate index
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ThyoneE_SetBaudrateIndex(ThyoneE_BaudRateIndex_t baudrate, ThyoneE_UartParity_t parity, bool flowcontrolEnable)
{
	uint8_t baudrateIndex = (uint8_t) baudrate;

	/* If flowcontrol is to be enabled UART index has to be increased by one in regard to base value */
	if (flowcontrolEnable)
	{
		baudrateIndex++;
	}

	/* If parity bit is even, UART index has to be increased by 64 in regard of base value*/
	if (ThyoneE_UartParity_Even == parity)
	{
		baudrateIndex += 64;
	}

	return ThyoneE_Set(ThyoneE_USERSETTING_INDEX_UART_CONFIG, (uint8_t*) &baudrateIndex, 1);
}

/**
 * @brief Set the RF channel
 *
 * @param[in] channel: Radio channel
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ThyoneE_SetRFChannel(uint8_t channel)
{
	/* permissible value for channel: 0-38 */
	if (channel <= 38)
	{
		return ThyoneE_Set(ThyoneE_USERSETTING_INDEX_RF_CHANNEL, (uint8_t*) &channel, 1);
	}
	else
	{
		return false;
	}
}

/**
 * @brief Set the RF channel on-the-fly without changing the user settings (volatile,
 * channel is reverted to the value stored in flash after a reset of the module).
 *
 * @param[in] channel: Radio channel
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ThyoneE_SetRFChannelRuntime(uint8_t channel)
{
	/* permissible value for channel: 0-38 */
	if (channel <= 38)
	{

		txPacket.Cmd = THYONEE_CMD_SETCHANNEL_REQ;
		txPacket.Length = 1;
		txPacket.Data[0] = channel;

		if (!FillChecksum(&txPacket))
		{
			return false;
		}
		WE_UART_Transmit((uint8_t*) &txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD);
		return Wait4CNF(CMD_WAIT_TIME, THYONEE_CMD_SETCHANNEL_CNF, CMD_Status_Success, true);
	}
	else
	{
		return false;
	}
}

/**
 * @brief Set the encryption mode
 *
 * @param[in] encryptionMode: encryptionMode
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ThyoneE_SetEncryptionMode(ThyoneE_EncryptionMode_t encryptionMode)
{
	return ThyoneE_Set(ThyoneE_USERSETTING_INDEX_ENCRYPTION_MODE, (uint8_t*) &encryptionMode, 1);
}

/**
 * @brief Set the rf profile
 *
 * @param[in] profile: rf profile
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ThyoneE_SetRfProfile(ThyoneE_Profile_t profile)
{
	return ThyoneE_Set(ThyoneE_USERSETTING_INDEX_RF_PROFILE, (uint8_t*) &profile, 1);
}

/**
 * @brief Set the number of retries
 *
 * @param[in] numRetries: number of retries
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ThyoneE_SetNumRetries(uint8_t numRetries)
{
	return ThyoneE_Set(ThyoneE_USERSETTING_INDEX_RF_NUM_RETRIES, (uint8_t*) &numRetries, 1);
}

/**
 * @brief Set the number of time slots
 *
 * @param[in] numSlots: number of time slots
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ThyoneE_SetRpNumSlots(uint8_t numSlots)
{
	return ThyoneE_Set(ThyoneE_USERSETTING_INDEX_RF_RP_NUM_SLOTS, (uint8_t*) &numSlots, 1);
}

/**
 * @brief Set the source address
 *
 * @param[in] sourceAddress: source address
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ThyoneE_SetSourceAddress(uint32_t sourceAddress)
{
	return ThyoneE_Set(ThyoneE_USERSETTING_INDEX_MAC_SOURCE_ADDRESS, (uint8_t*) &sourceAddress, 4);
}

/**
 * @brief Set the destination address
 *
 * @param[in] destinationAddress: destination address
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ThyoneE_SetDestinationAddress(uint32_t destinationAddress)
{
	return ThyoneE_Set(ThyoneE_USERSETTING_INDEX_MAC_DESTINATION_ADDRESS, (uint8_t*) &destinationAddress, 4);
}

/**
 * @brief Set the group id
 *
 * @param[in] groupid: groupID
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ThyoneE_SetGroupID(uint8_t groupId)
{
	return ThyoneE_Set(ThyoneE_USERSETTING_INDEX_MAC_GROUP_ID, (uint8_t*) &groupId, 1);
}

/**
 * @brief Set the encryption key
 *
 * @param[in] keyP: pointer to 16-byte key
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ThyoneE_SetEncryptionKey(uint8_t *keyP)
{
	return ThyoneE_Set(ThyoneE_USERSETTING_INDEX_MAC_ENCRYPTION_KEY, keyP, 16);
}

/**
 * @brief Set the time-to-live defining the maximum of hops if repeating
 *
 * @param[in] ttl: time-to-live value
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ThyoneE_SetTimeToLive(uint8_t ttl)
{
	return ThyoneE_Set(ThyoneE_USERSETTING_INDEX_MAC_TLL, &ttl, 1);
}

/**
 * @brief Set remote gpio config
 *
 * @param[in] remoteConfig: remote gpio config
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ThyoneE_SetGPIOBlockRemoteConfig(uint8_t remoteConfig)
{
	return ThyoneE_Set(ThyoneE_USERSETTING_INDEX_REMOTE_GPIO_CONFIG, &remoteConfig, 1);
}

/**
 * @brief Set module mode
 *
 * @param[in] moduleMode: operation mode of the module
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ThyoneE_SetModuleMode(ThyoneE_ModuleMode_t moduleMode)
{
	return ThyoneE_Set(ThyoneE_USERSETTING_INDEX_MODULE_MODE, (uint8_t*) &moduleMode, 1);
}

/**
 * @brief Request the current user settings
 *
 * @param[in] userSetting: user setting to be requested
 * @param[out] responseP: pointer of the memory to put the requested content
 * @param[out] response_lengthP: length of the requested content
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ThyoneE_Get(ThyoneE_UserSettings_t userSetting, uint8_t *ResponseP, uint16_t *Response_LengthP)
{
	bool ret = false;

	/* fill CMD_ARRAY packet */

	txPacket.Cmd = THYONEE_CMD_GET_REQ;
	txPacket.Length = 1;
	txPacket.Data[0] = userSetting;

	if (FillChecksum(&txPacket))
	{

		WE_UART_Transmit((uint8_t*) &txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD);

		/* wait for cnf */
		if (Wait4CNF(CMD_WAIT_TIME, THYONEE_CMD_GET_CNF, CMD_Status_Success, true))
		{
			uint16_t length = rxPacket.Length;
			memcpy(ResponseP, &rxPacket.Data[1], length - 1); /* First Data byte is status, following bytes response*/
			*Response_LengthP = length - 1;
			ret = true;
		}
	}
	return ret;
}

/**
 * @brief Request the 4 byte serial number
 *
 * @param[out] versionP: pointer to the 4 byte serial number
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ThyoneE_GetSerialNumber(uint8_t *serialNumberP)
{
	uint16_t length;
	return ThyoneE_Get(ThyoneE_USERSETTING_INDEX_SERIAL_NUMBER, serialNumberP, &length);
}

/**
 * @brief Request the 3 byte firmware version
 *
 * @param[out] versionP: pointer to the 3 byte firmware version
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ThyoneE_GetFWVersion(uint8_t *versionP)
{
	uint16_t length;
	return ThyoneE_Get(ThyoneE_USERSETTING_INDEX_FW_VERSION, versionP, &length);
}

/**
 * @brief Request the TX power
 *
 * @param[out] txpowerP: pointer to the TX power
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ThyoneE_GetTXPower(ThyoneE_TXPower_t *txpowerP)
{
	uint16_t length;
	return ThyoneE_Get(ThyoneE_USERSETTING_INDEX_RF_TX_POWER, (uint8_t*) txpowerP, &length);
}

/**
 * @brief Request the UART baudrate index
 *
 * @param[out] baudrateIndexP: pointer to the UART baudrate index
 * @param[out] parityP: pointer to the UART parity
 * @param[out] flowcontrolEnableP: pointer to the UART flow control parameter
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ThyoneE_GetBaudrateIndex(ThyoneE_BaudRateIndex_t *baudrateP, ThyoneE_UartParity_t *parityP, bool *flowcontrolEnableP)
{
	bool ret = false;
	uint16_t length;
	uint8_t uartIndex;

	if (ThyoneE_Get(ThyoneE_USERSETTING_INDEX_UART_CONFIG, (uint8_t*) &uartIndex, &length))
	{
		/* if index is even, flow control is off.
		 * If flow control is on, decrease index by one to later determine the base baudrate */
		if (0x01 == (uartIndex & 0x01))
		{
			/* odd */
			*flowcontrolEnableP = true;
			uartIndex--;
		}
		else
		{
			/* even */
			*flowcontrolEnableP = false;
		}

		/* If baudrate index is greater than or equal to 64, parity bit is even*/
		if (uartIndex < 64)
		{
			*parityP = ThyoneE_UartParity_None;
		}
		else
		{
			*parityP = ThyoneE_UartParity_Even;
			uartIndex -= 64;
		}

		*baudrateP = (ThyoneE_BaudRateIndex_t) uartIndex;
		ret = true;
	}

	return ret;
}

/**
 * @brief Get the encryption mode
 *
 * @param[out] encryptionModeP: Pointer to encryptionMode
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ThyoneE_GetEncryptionMode(ThyoneE_EncryptionMode_t *encryptionModeP)
{
	uint16_t length;
	return ThyoneE_Get(ThyoneE_USERSETTING_INDEX_ENCRYPTION_MODE, (uint8_t*) encryptionModeP, &length);
}

/**
 * @brief Get the rf profile
 *
 * *output:
 * @param[out] profileP: Pointer to rf profile
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ThyoneE_GetRfProfile(ThyoneE_Profile_t *profileP)
{
	uint16_t length;
	return ThyoneE_Get(ThyoneE_USERSETTING_INDEX_RF_PROFILE, (uint8_t*) profileP, &length);
}

/**
 * @brief Get the rf channel
 *
 * @param[out] channelP: Pointer to rf channel
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ThyoneE_GetRFChannel(uint8_t *channelP)
{
	uint16_t length;
	return ThyoneE_Get(ThyoneE_USERSETTING_INDEX_RF_CHANNEL, (uint8_t*) channelP, &length);
}

/**
 * @brief Get the number of retries
 *
 * @param[out] numRetriesP: Pointer to number of retries
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ThyoneE_GetNumRetries(uint8_t *numRetriesP)
{
	uint16_t length;
	return ThyoneE_Get(ThyoneE_USERSETTING_INDEX_RF_NUM_RETRIES, numRetriesP, &length);
}

/**
 * @brief Get the number of time slots
 *
 * @param[out] numSlotsP: pointer of number of time slots
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ThyoneE_GetRpNumSlots(uint8_t *numSlotsP)
{
	uint16_t length;
	return ThyoneE_Get(ThyoneE_USERSETTING_INDEX_RF_RP_NUM_SLOTS, numSlotsP, &length);
}

/**
 * @brief Get the source address
 *
 * @param[out] sourceAddressP: pointer to source address
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ThyoneE_GetSourceAddress(uint32_t *sourceAddressP)
{
	uint16_t length;
	return ThyoneE_Get(ThyoneE_USERSETTING_INDEX_MAC_SOURCE_ADDRESS, (uint8_t*) sourceAddressP, &length);
}

/**
 * @brief Get the destination address
 *
 * @param[out] destinationAddressP: pointer to destination address
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ThyoneE_GetDestinationAddress(uint32_t *destinationAddressP)
{
	uint16_t length;
	return ThyoneE_Get(ThyoneE_USERSETTING_INDEX_MAC_DESTINATION_ADDRESS, (uint8_t*) destinationAddressP, &length);
}

/**
 * @brief Get the group id
 *
 * @param[out] groupIdP: pointer to groupID
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ThyoneE_GetGroupID(uint8_t *groupIdP)
{
	uint16_t length;
	return ThyoneE_Get(ThyoneE_USERSETTING_INDEX_MAC_GROUP_ID, groupIdP, &length);
}

/**
 * @brief Get the time-to-live defining the maximum of hops if repeating
 *
 * @param[out] ttlP: pointer to time-to-live
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ThyoneE_GetTimeToLive(uint8_t *ttlP)
{
	uint16_t length;
	return ThyoneE_Get(ThyoneE_USERSETTING_INDEX_MAC_TLL, ttlP, &length);
}

/**
 * @brief Get remote gpio config
 *
 * @param[out] remoteConfigP: pointer to remote gpio config
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ThyoneE_GetGPIOBlockRemoteConfig(uint8_t *remoteConfigP)
{
	uint16_t length;
	return ThyoneE_Get(ThyoneE_USERSETTING_INDEX_REMOTE_GPIO_CONFIG, remoteConfigP, &length);
}

/**
 * @brief Get module mode
 *
 * @param[out] moduleMode: operation mode of the module
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ThyoneE_GetModuleMode(ThyoneE_ModuleMode_t *moduleModeP)
{
	uint16_t length;
	return ThyoneE_Get(ThyoneE_USERSETTING_INDEX_MODULE_MODE, (uint8_t*) moduleModeP, &length);
}

/**
 * @brief Request the module state
 *
 * @param[out] state:   current state of the module
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ThyoneE_GetState(ThyoneE_States_t *state)
{
	bool ret = false;

	/* fill CMD_ARRAY packet */

	txPacket.Cmd = THYONEE_CMD_GETSTATE_REQ;
	txPacket.Length = 0;

	if (FillChecksum(&txPacket))
	{

		WE_UART_Transmit((uint8_t*) &txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD);
		/* wait for cnf */
		if (Wait4CNF(CMD_WAIT_TIME, THYONEE_CMD_GETSTATE_CNF, CMD_Status_Success, true))
		{
			*state = rxPacket.Data[1];
			ret = true;
		}
	}
	return ret;
}

/**
 * @brief Configure the local GPIO of the module
 *
 * @param[in] configP: pointer to one or more pin configurations
 * @param[in] number_of_configs: number of entries in configP array
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ThyoneE_GPIOLocalSetConfig(ThyoneE_GPIOConfigBlock_t *configP, uint16_t number_of_configs)
{
	bool ret = false;
	uint16_t length = 0;

	for (uint16_t i = 0; i < number_of_configs; i++)
	{
		switch (configP->function)
		{
		case ThyoneE_GPIO_IO_Disconnected:
		{
			txPacket.Data[length] = 3;
			txPacket.Data[length + 1] = configP->GPIO_ID;
			txPacket.Data[length + 2] = configP->function;
			txPacket.Data[length + 3] = 0x00;
			length += 4;
		}
			break;
		case ThyoneE_GPIO_IO_Input:
		{
			txPacket.Data[length] = 3;
			txPacket.Data[length + 1] = configP->GPIO_ID;
			txPacket.Data[length + 2] = configP->function;
			txPacket.Data[length + 3] = configP->value.input;
			length += 4;
		}
			break;
		case ThyoneE_GPIO_IO_Output:
		{
			txPacket.Data[length] = 3;
			txPacket.Data[length + 1] = configP->GPIO_ID;
			txPacket.Data[length + 2] = configP->function;
			txPacket.Data[length + 3] = configP->value.output;
			length += 4;
		}
			break;
		default:
		{
		}
			break;
		}
		configP++;
	}

	txPacket.Cmd = THYONEE_CMD_GPIO_LOCAL_SETCONFIG_REQ;
	txPacket.Length = length;

	if (FillChecksum(&txPacket))
	{

		WE_UART_Transmit((uint8_t*) &txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD);

		/* wait for cnf */
		ret = Wait4CNF(CMD_WAIT_TIME, THYONEE_CMD_GPIO_LOCAL_SETCONFIG_CNF, CMD_Status_Success, true);
	}

	return ret;
}

/**
 * @brief Read the local GPIO configuration of the module
 *
 * @param[out] configP: pointer to one or more pin configurations
 * @param[out] number_of_configsP: pointer to number of entries in configP array
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ThyoneE_GPIOLocalGetConfig(ThyoneE_GPIOConfigBlock_t *configP, uint16_t *number_of_configsP)
{
	bool ret = false;

	txPacket.Cmd = THYONEE_CMD_GPIO_LOCAL_GETCONFIG_REQ;
	txPacket.Length = 0;

	if (FillChecksum(&txPacket))
	{

		WE_UART_Transmit((uint8_t*) &txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD);

		/* wait for cnf */
		ret = Wait4CNF(CMD_WAIT_TIME, THYONEE_CMD_GPIO_LOCAL_GETCONFIG_CNF, CMD_Status_Success, true);

		if (ret == true)
		{
			uint16_t length = rxPacket.Length;

			*number_of_configsP = 0;
			uint8_t *uartP = &rxPacket.Data[1];
			ThyoneE_GPIOConfigBlock_t *configP_running = configP;
			while (uartP < &rxPacket.Data[length])
			{
				switch (*(uartP + 2))
				{
				case ThyoneE_GPIO_IO_Disconnected:
				{
					if (*uartP == 3)
					{
						configP_running->GPIO_ID = *(uartP + 1);
						configP_running->function = *(uartP + 2);

						configP_running++;
						*number_of_configsP += 1;
					}
				}
					break;
				case ThyoneE_GPIO_IO_Input:
				{
					if (*uartP == 3)
					{
						configP_running->GPIO_ID = *(uartP + 1);
						configP_running->function = *(uartP + 2);
						configP_running->value.input = *(uartP + 3);

						configP_running++;
						*number_of_configsP += 1;
					}
				}
					break;
				case ThyoneE_GPIO_IO_Output:
				{
					if (*uartP == 3)
					{
						configP_running->GPIO_ID = *(uartP + 1);
						configP_running->function = *(uartP + 2);
						configP_running->value.output = *(uartP + 3);

						configP_running++;
						*number_of_configsP += 1;
					}
				}
					break;
				default:
				{

				}
					break;
				}
				uartP += *uartP + 1;
			}
		}
	}

	return ret;
}

/**
 * @brief Set the output value of the local pin. Pin has to be configured first.
 * See ThyoneE_GPIOLocalWriteConfig
 *
 * @param[in] controlP: pointer to one or more pin controls
 * @param[in] number_of_controls: number of entries in controlP array
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ThyoneE_GPIOLocalWrite(ThyoneE_GPIOControlBlock_t *controlP, uint16_t number_of_controls)
{
	bool ret = false;
	uint16_t length = 0;

	for (uint16_t i = 0; i < number_of_controls; i++)
	{
		txPacket.Data[length] = 2;
		txPacket.Data[length + 1] = controlP->GPIO_ID;
		txPacket.Data[length + 2] = controlP->value.output;
		length += 3;
		controlP++;
	}

	txPacket.Cmd = THYONEE_CMD_GPIO_LOCAL_WRITE_REQ;
	txPacket.Length = length;

	if (FillChecksum(&txPacket))
	{

		WE_UART_Transmit((uint8_t*) &txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD);

		/* wait for cnf */
		ret = Wait4CNF(CMD_WAIT_TIME, THYONEE_CMD_GPIO_LOCAL_WRITE_CNF, CMD_Status_Success, true);
	}

	return ret;
}

/**
 * @brief Read the input of the pin. Pin has to be configured first.
 * See ThyoneE_GPIOLocalWriteConfig
 *
 * @param[in] GPIOToReadP: One or more pins to read.
 * @param[in] amountGPIOToRead: amount of pins to read and therefore length of GPIOToRead
 * @param[out] controlP: Pointer to controlBlock
 * @param[out] number_of_controlsP: pointer to number of entries in controlP array
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ThyoneE_GPIOLocalRead(uint8_t *GPIOToReadP, uint8_t amountGPIOToRead, ThyoneE_GPIOControlBlock_t *controlP, uint16_t *number_of_controlsP)
{
	bool ret = false;

	txPacket.Cmd = THYONEE_CMD_GPIO_LOCAL_READ_REQ;
	txPacket.Length = amountGPIOToRead + 1;
	txPacket.Data[0] = amountGPIOToRead;
	memcpy(&txPacket.Data[1], GPIOToReadP, amountGPIOToRead);

	if (FillChecksum(&txPacket))
	{

		WE_UART_Transmit((uint8_t*) &txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD);

		/* wait for cnf */
		ret = Wait4CNF(CMD_WAIT_TIME, THYONEE_CMD_GPIO_LOCAL_READ_CNF, CMD_Status_Success, true);

		if (ret)
		{
			uint16_t length = rxPacket.Length;

			*number_of_controlsP = 0;
			uint8_t *uartP = &rxPacket.Data[1];
			ThyoneE_GPIOControlBlock_t *controlP_running = controlP;
			while (uartP < &rxPacket.Data[length])
			{

				if (*uartP == 2)
				{
					controlP_running->GPIO_ID = *(uartP + 1);
					controlP_running->value.output = *(uartP + 2);

					controlP_running++;
					*number_of_controlsP += 1;
				}
				uartP += *uartP + 1;
			}
		}
	}

	return ret;
}

/**
 * @brief Configure the remote GPIO of the module
 *
 * @param[in] destAddress: Destination address of the remote Thyone-e device
 * @param[in] configP: pointer to one or more pin configurations
 * @param[in] number_of_configs: number of entries in configP array
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ThyoneE_GPIORemoteSetConfig(uint32_t destAddress, ThyoneE_GPIOConfigBlock_t *configP, uint16_t number_of_configs)
{
	bool ret = false;
	uint16_t length = 4;

	for (uint16_t i = 0; i < number_of_configs; i++)
	{
		switch (configP->function)
		{
		case ThyoneE_GPIO_IO_Disconnected:
		{
			txPacket.Data[length] = 3;
			txPacket.Data[length + 1] = configP->GPIO_ID;
			txPacket.Data[length + 2] = configP->function;
			txPacket.Data[length + 3] = 0x00;
			length += 4;
		}
			break;
		case ThyoneE_GPIO_IO_Input:
		{
			txPacket.Data[length] = 3;
			txPacket.Data[length + 1] = configP->GPIO_ID;
			txPacket.Data[length + 2] = configP->function;
			txPacket.Data[length + 3] = configP->value.input;
			length += 4;
		}
			break;
		case ThyoneE_GPIO_IO_Output:
		{
			txPacket.Data[length] = 3;
			txPacket.Data[length + 1] = configP->GPIO_ID;
			txPacket.Data[length + 2] = configP->function;
			txPacket.Data[length + 3] = configP->value.output;
			length += 4;
		}
			break;
		default:
		{
		}
			break;
		}
		configP++;
	}

	txPacket.Cmd = THYONEE_CMD_GPIO_REMOTE_SETCONFIG_REQ;
	txPacket.Length = length;

	memcpy(&txPacket.Data[0], &destAddress, 4);

	if (FillChecksum(&txPacket))
	{

		WE_UART_Transmit((uint8_t*) &txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD);

		/* wait for cnf */
		ret = Wait4CNF(CMD_WAIT_TIME, THYONEE_CMD_GPIO_REMOTE_SETCONFIG_CNF, CMD_Status_Success, true);
	}

	return ret;
}

/**
 * @brief Read the remote GPIO configuration of the module
 *
 * @param[out] destAddress: Destination address of the remote Thyone-e device
 * @param[out] configP: pointer to one or more pin configurations
 * @param[out] number_of_configsP: pointer to number of entries in configP array
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ThyoneE_GPIORemoteGetConfig(uint32_t destAddress, ThyoneE_GPIOConfigBlock_t *configP, uint16_t *number_of_configsP)
{
	bool ret = false;

	txPacket.Cmd = THYONEE_CMD_GPIO_REMOTE_GETCONFIG_REQ;
	txPacket.Length = 4;
	memcpy(&txPacket.Data[0], &destAddress, 4);

	if (FillChecksum(&txPacket))
	{

		WE_UART_Transmit((uint8_t*) &txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD);

		/* wait for cnf */
		ret = Wait4CNF(CMD_WAIT_TIME, THYONEE_CMD_GPIO_REMOTE_GETCONFIG_RSP, CMD_Status_NoStatus, true);

		if (ret)
		{
			uint16_t length = rxPacket.Length;

			*number_of_configsP = 0;
			uint8_t *uartP = &rxPacket.Data[1 + 4];
			ThyoneE_GPIOConfigBlock_t *configP_running = configP;
			while (uartP < &rxPacket.Data[length])
			{
				switch (*(uartP + 2))
				{
				case ThyoneE_GPIO_IO_Disconnected:
				{
					if (*uartP == 3)
					{
						configP_running->GPIO_ID = *(uartP + 1);
						configP_running->function = *(uartP + 2);

						configP_running++;
						*number_of_configsP += 1;
					}
				}
					break;
				case ThyoneE_GPIO_IO_Input:
				{
					if (*uartP == 3)
					{
						configP_running->GPIO_ID = *(uartP + 1);
						configP_running->function = *(uartP + 2);
						configP_running->value.input = *(uartP + 3);

						configP_running++;
						*number_of_configsP += 1;
					}
				}
					break;
				case ThyoneE_GPIO_IO_Output:
				{
					if (*uartP == 3)
					{
						configP_running->GPIO_ID = *(uartP + 1);
						configP_running->function = *(uartP + 2);
						configP_running->value.output = *(uartP + 3);

						configP_running++;
						*number_of_configsP += 1;
					}
				}
					break;
				default:
				{

				}
					break;
				}
				uartP += *uartP + 1;
			}
		}

	}

	return ret;
}

/**
 * @brief Set the output value of the remote pin. Pin has to be configured first.
 * See ThyoneE_GPIORemoteWriteConfig
 *
 * @param[in] destAddress: Destination address of the remote Thyone-e device
 * @param[in] controlP: pointer to one or more pin controls
 * @param[in] number_of_controls: number of entries in controlP array
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ThyoneE_GPIORemoteWrite(uint32_t destAddress, ThyoneE_GPIOControlBlock_t *controlP, uint16_t number_of_controls)
{
	bool ret = false;
	uint16_t length = 4;

	for (uint16_t i = 0; i < number_of_controls; i++)
	{
		txPacket.Data[length] = 2;
		txPacket.Data[length + 1] = controlP->GPIO_ID;
		txPacket.Data[length + 2] = controlP->value.output;
		length += 3;
		controlP += sizeof(ThyoneE_GPIOControlBlock_t);
	}

	txPacket.Cmd = THYONEE_CMD_GPIO_REMOTE_WRITE_REQ;
	txPacket.Length = length;

	memcpy(&txPacket.Data[0], &destAddress, 4);

	if (FillChecksum(&txPacket))
	{

		WE_UART_Transmit((uint8_t*) &txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD);

		/* wait for cnf */
		ret = Wait4CNF(CMD_WAIT_TIME, THYONEE_CMD_GPIO_REMOTE_WRITE_CNF, CMD_Status_Success, true);
	}

	return ret;
}

/**
 * @brief Read the input of the pins. Pin has to be configured first.
 * See ThyoneE_GPIORemoteWriteConfig
 *
 * @param[in] destAddress: Destination address of the remote Thyone-e device
 * @param[in] GPIOToReadP: One or more pins to read.
 * @param[in] amountGPIOToRead: amount of pins to read and therefore length of GPIOToReadP
 * @param[out] controlP: Pointer to controlBlock
 * @param[out] number_of_controlsP: pointer to number of entries in controlP array
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ThyoneE_GPIORemoteRead(uint32_t destAddress, uint8_t *GPIOToReadP, uint8_t amountGPIOToRead, ThyoneE_GPIOControlBlock_t *controlP, uint16_t *number_of_controlsP)
{
	bool ret = false;

	/* payload is 4-byte destination address + pin configuration*/
	uint16_t commandLength = 1 + amountGPIOToRead + 4;

	txPacket.Cmd = THYONEE_CMD_GPIO_REMOTE_READ_REQ;
	txPacket.Length = commandLength;

	memcpy(&txPacket.Data[0], &destAddress, 4);
	txPacket.Data[4] = amountGPIOToRead;
	memcpy(&txPacket.Data[5], GPIOToReadP, amountGPIOToRead);

	if (FillChecksum(&txPacket))
	{

		WE_UART_Transmit((uint8_t*) &txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD);

		/* wait for cnf */
		ret = Wait4CNF(1000, THYONEE_CMD_GPIO_REMOTE_READ_RSP, CMD_Status_NoStatus, true);

		if (ret)
		{
			uint16_t length = rxPacket.Length;

			*number_of_controlsP = 0;
			uint8_t *uartP = &rxPacket.Data[1 + 4];
			ThyoneE_GPIOControlBlock_t *controlP_running = controlP;
			while (uartP < &rxPacket.Data[length])
			{

				if (*uartP == 2)
				{
					controlP_running->GPIO_ID = *(uartP + 1);
					controlP_running->value.output = *(uartP + 2);

					controlP_running++;
					*number_of_controlsP += 1;
				}
				uartP += *uartP + 1;
			}
		}
	}

	return ret;
}

/**
 * @brief Returns the current level of the BUSY pin.
 * @return true if level is HIGH, false otherwise.
 */
bool ThyoneE_IsTransparentModeBusy()
{
	return WE_Pin_Level_High == WE_GetPinLevel(ThyoneE_pins[ThyoneE_Pin_Busy]);
}

