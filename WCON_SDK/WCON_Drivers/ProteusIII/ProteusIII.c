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
 * @brief ProteusIII driver source file.
 */

#include "ProteusIII.h"

#include "stdio.h"
#include "string.h"

#include "../global/global.h"

typedef enum ProteusIII_Pin_t
{
	ProteusIII_Pin_Reset,
	ProteusIII_Pin_SleepWakeUp,
	ProteusIII_Pin_Boot,
	ProteusIII_Pin_Mode,
	ProteusIII_Pin_Count
} ProteusIII_Pin_t;

#define CMD_WAIT_TIME 500
#define CNFINVALID 255

#define LENGTH_CMD_OVERHEAD             (uint16_t)5
#define LENGTH_CMD_OVERHEAD_WITHOUT_CRC (uint16_t)(LENGTH_CMD_OVERHEAD - 1)
#define MAX_PAYLOAD_LENGTH              (uint16_t)964
#define MAX_CMD_LENGTH                  (uint16_t)(MAX_PAYLOAD_LENGTH + LENGTH_CMD_OVERHEAD)

#define CMD_POSITION_STX        (uint8_t)0
#define CMD_POSITION_CMD        (uint8_t)1
#define CMD_POSITION_LENGTH_LSB (uint8_t)2
#define CMD_POSITION_LENGTH_MSB (uint8_t)3
#define CMD_POSITION_DATA       (uint8_t)4

#define CMD_STX 0x02

#define ProteusIII_CMD_TYPE_REQ (uint8_t)(0 << 6)
#define ProteusIII_CMD_TYPE_CNF (uint8_t)(1 << 6)
#define ProteusIII_CMD_TYPE_IND (uint8_t)(2 << 6)
#define ProteusIII_CMD_TYPE_RSP (uint8_t)(3 << 6)

#define ProteusIII_CMD_RESET (uint8_t)0x00
#define ProteusIII_CMD_RESET_REQ (ProteusIII_CMD_RESET | ProteusIII_CMD_TYPE_REQ)
#define ProteusIII_CMD_RESET_CNF (ProteusIII_CMD_RESET | ProteusIII_CMD_TYPE_CNF)

#define ProteusIII_CMD_GETSTATE (uint8_t)0x01
#define ProteusIII_CMD_GETSTATE_REQ (ProteusIII_CMD_GETSTATE | ProteusIII_CMD_TYPE_REQ)
#define ProteusIII_CMD_GETSTATE_CNF (ProteusIII_CMD_GETSTATE | ProteusIII_CMD_TYPE_CNF)
#define ProteusIII_CMD_GETSTATE_IND (ProteusIII_CMD_GETSTATE | ProteusIII_CMD_TYPE_IND)

#define ProteusIII_CMD_SLEEP (uint8_t)0x02
#define ProteusIII_CMD_SLEEP_REQ (ProteusIII_CMD_SLEEP | ProteusIII_CMD_TYPE_REQ)
#define ProteusIII_CMD_SLEEP_CNF (ProteusIII_CMD_SLEEP | ProteusIII_CMD_TYPE_CNF)
#define ProteusIII_CMD_SLEEP_IND (ProteusIII_CMD_SLEEP | ProteusIII_CMD_TYPE_IND)

#define ProteusIII_CMD_DATA (uint8_t)0x04
#define ProteusIII_CMD_DATA_REQ (ProteusIII_CMD_DATA | ProteusIII_CMD_TYPE_REQ)
#define ProteusIII_CMD_DATA_CNF (ProteusIII_CMD_DATA | ProteusIII_CMD_TYPE_CNF)
#define ProteusIII_CMD_DATA_IND (ProteusIII_CMD_DATA | ProteusIII_CMD_TYPE_IND)
#define ProteusIII_CMD_TXCOMPLETE_RSP (ProteusIII_CMD_DATA | ProteusIII_CMD_TYPE_RSP)

#define ProteusIII_CMD_CONNECT (uint8_t)0x06
#define ProteusIII_CMD_CONNECT_REQ (ProteusIII_CMD_CONNECT | ProteusIII_CMD_TYPE_REQ)
#define ProteusIII_CMD_CONNECT_CNF (ProteusIII_CMD_CONNECT | ProteusIII_CMD_TYPE_CNF)
#define ProteusIII_CMD_CONNECT_IND (ProteusIII_CMD_CONNECT | ProteusIII_CMD_TYPE_IND)
#define ProteusIII_CMD_CHANNELOPEN_RSP (ProteusIII_CMD_CONNECT | ProteusIII_CMD_TYPE_RSP)

#define ProteusIII_CMD_DISCONNECT (uint8_t)0x07
#define ProteusIII_CMD_DISCONNECT_REQ (ProteusIII_CMD_DISCONNECT | ProteusIII_CMD_TYPE_REQ)
#define ProteusIII_CMD_DISCONNECT_CNF (ProteusIII_CMD_DISCONNECT | ProteusIII_CMD_TYPE_CNF)
#define ProteusIII_CMD_DISCONNECT_IND (ProteusIII_CMD_DISCONNECT | ProteusIII_CMD_TYPE_IND)

#define ProteusIII_CMD_SECURITY_IND (uint8_t)0x88

#define ProteusIII_CMD_SCANSTART (uint8_t)0x09
#define ProteusIII_CMD_SCANSTART_REQ (ProteusIII_CMD_SCANSTART  | ProteusIII_CMD_TYPE_REQ)
#define ProteusIII_CMD_SCANSTART_CNF (ProteusIII_CMD_SCANSTART  | ProteusIII_CMD_TYPE_CNF)
#define ProteusIII_CMD_SCANSTART_IND (ProteusIII_CMD_SCANSTART  | ProteusIII_CMD_TYPE_IND)

#define ProteusIII_CMD_SCANSTOP (uint8_t)0x0A
#define ProteusIII_CMD_SCANSTOP_REQ (ProteusIII_CMD_SCANSTOP  | ProteusIII_CMD_TYPE_REQ)
#define ProteusIII_CMD_SCANSTOP_CNF (ProteusIII_CMD_SCANSTOP  | ProteusIII_CMD_TYPE_CNF)
#define ProteusIII_CMD_SCANSTOP_IND (ProteusIII_CMD_SCANSTOP  | ProteusIII_CMD_TYPE_IND)

#define ProteusIII_CMD_GETDEVICES (uint8_t)0x0B
#define ProteusIII_CMD_GETDEVICES_REQ (ProteusIII_CMD_GETDEVICES  | ProteusIII_CMD_TYPE_REQ)
#define ProteusIII_CMD_GETDEVICES_CNF (ProteusIII_CMD_GETDEVICES  | ProteusIII_CMD_TYPE_CNF)
#define ProteusIII_CMD_GETDEVICES_IND (ProteusIII_CMD_GETDEVICES  | ProteusIII_CMD_TYPE_IND)

#define ProteusIII_CMD_PASSKEY (uint8_t)0x0D
#define ProteusIII_CMD_PASSKEY_REQ (ProteusIII_CMD_PASSKEY  | ProteusIII_CMD_TYPE_REQ)
#define ProteusIII_CMD_PASSKEY_CNF (ProteusIII_CMD_PASSKEY  | ProteusIII_CMD_TYPE_CNF)
#define ProteusIII_CMD_PASSKEY_IND (ProteusIII_CMD_PASSKEY  | ProteusIII_CMD_TYPE_IND)

#define ProteusIII_CMD_GET (uint8_t)0x10
#define ProteusIII_CMD_GET_REQ (ProteusIII_CMD_GET | ProteusIII_CMD_TYPE_REQ)
#define ProteusIII_CMD_GET_CNF (ProteusIII_CMD_GET | ProteusIII_CMD_TYPE_CNF)

#define ProteusIII_CMD_SET (uint8_t)0x11
#define ProteusIII_CMD_SET_REQ (ProteusIII_CMD_SET | ProteusIII_CMD_TYPE_REQ)
#define ProteusIII_CMD_SET_CNF (ProteusIII_CMD_SET | ProteusIII_CMD_TYPE_CNF)

#define ProteusIII_CMD_PHYUPDATE (uint8_t)0x1A
#define ProteusIII_CMD_PHYUPDATE_REQ (ProteusIII_CMD_PHYUPDATE | ProteusIII_CMD_TYPE_REQ)
#define ProteusIII_CMD_PHYUPDATE_CNF (ProteusIII_CMD_PHYUPDATE | ProteusIII_CMD_TYPE_CNF)
#define ProteusIII_CMD_PHYUPDATE_IND (ProteusIII_CMD_PHYUPDATE | ProteusIII_CMD_TYPE_IND)

#define ProteusIII_CMD_FACTORYRESET (uint8_t)0x1C
#define ProteusIII_CMD_FACTORYRESET_REQ (ProteusIII_CMD_FACTORYRESET | ProteusIII_CMD_TYPE_REQ)
#define ProteusIII_CMD_FACTORYRESET_CNF (ProteusIII_CMD_FACTORYRESET | ProteusIII_CMD_TYPE_CNF)

#define ProteusIII_CMD_NUMERIC_COMP (uint8_t)0x24
#define ProteusIII_CMD_NUMERIC_COMP_REQ    (ProteusIII_CMD_NUMERIC_COMP | ProteusIII_CMD_TYPE_REQ)
#define ProteusIII_CMD_NUMERIC_COMP_CNF    (ProteusIII_CMD_NUMERIC_COMP | ProteusIII_CMD_TYPE_CNF)
#define ProteusIII_CMD_DISPLAY_PASSKEY_IND (ProteusIII_CMD_NUMERIC_COMP | ProteusIII_CMD_TYPE_CNF)

#define ProteusIII_CMD_GPIO_LOCAL_WRITECONFIG (uint8_t)0x25
#define ProteusIII_CMD_GPIO_LOCAL_WRITECONFIG_REQ (ProteusIII_CMD_GPIO_LOCAL_WRITECONFIG | ProteusIII_CMD_TYPE_REQ)
#define ProteusIII_CMD_GPIO_LOCAL_WRITECONFIG_CNF (ProteusIII_CMD_GPIO_LOCAL_WRITECONFIG | ProteusIII_CMD_TYPE_CNF)

#define ProteusIII_CMD_GPIO_LOCAL_READCONFIG (uint8_t)0x2B
#define ProteusIII_CMD_GPIO_LOCAL_READCONFIG_REQ (ProteusIII_CMD_GPIO_LOCAL_READCONFIG | ProteusIII_CMD_TYPE_REQ)
#define ProteusIII_CMD_GPIO_LOCAL_READCONFIG_CNF (ProteusIII_CMD_GPIO_LOCAL_READCONFIG | ProteusIII_CMD_TYPE_CNF)

#define ProteusIII_CMD_GPIO_LOCAL_WRITE (uint8_t)0x26
#define ProteusIII_CMD_GPIO_LOCAL_WRITE_REQ (ProteusIII_CMD_GPIO_LOCAL_WRITE | ProteusIII_CMD_TYPE_REQ)
#define ProteusIII_CMD_GPIO_LOCAL_WRITE_CNF (ProteusIII_CMD_GPIO_LOCAL_WRITE | ProteusIII_CMD_TYPE_CNF)
#define ProteusIII_CMD_GPIO_LOCAL_WRITE_IND (ProteusIII_CMD_GPIO_LOCAL_WRITE | ProteusIII_CMD_TYPE_IND)

#define ProteusIII_CMD_GPIO_LOCAL_READ (uint8_t)0x27
#define ProteusIII_CMD_GPIO_LOCAL_READ_REQ (ProteusIII_CMD_GPIO_LOCAL_READ | ProteusIII_CMD_TYPE_REQ)
#define ProteusIII_CMD_GPIO_LOCAL_READ_CNF (ProteusIII_CMD_GPIO_LOCAL_READ | ProteusIII_CMD_TYPE_CNF)

#define ProteusIII_CMD_GPIO_REMOTE_WRITECONFIG (uint8_t)0x28
#define ProteusIII_CMD_GPIO_REMOTE_WRITECONFIG_REQ (ProteusIII_CMD_GPIO_REMOTE_WRITECONFIG | ProteusIII_CMD_TYPE_REQ)
#define ProteusIII_CMD_GPIO_REMOTE_WRITECONFIG_CNF (ProteusIII_CMD_GPIO_REMOTE_WRITECONFIG | ProteusIII_CMD_TYPE_CNF)
#define ProteusIII_CMD_GPIO_REMOTE_WRITECONFIG_IND (ProteusIII_CMD_GPIO_REMOTE_WRITECONFIG | ProteusIII_CMD_TYPE_IND)

#define ProteusIII_CMD_GPIO_REMOTE_READCONFIG (uint8_t)0x2C
#define ProteusIII_CMD_GPIO_REMOTE_READCONFIG_REQ (ProteusIII_CMD_GPIO_REMOTE_READCONFIG | ProteusIII_CMD_TYPE_REQ)
#define ProteusIII_CMD_GPIO_REMOTE_READCONFIG_CNF (ProteusIII_CMD_GPIO_REMOTE_READCONFIG | ProteusIII_CMD_TYPE_CNF)

#define ProteusIII_CMD_GPIO_REMOTE_WRITE (uint8_t)0x29
#define ProteusIII_CMD_GPIO_REMOTE_WRITE_REQ (ProteusIII_CMD_GPIO_REMOTE_WRITE | ProteusIII_CMD_TYPE_REQ)
#define ProteusIII_CMD_GPIO_REMOTE_WRITE_CNF (ProteusIII_CMD_GPIO_REMOTE_WRITE | ProteusIII_CMD_TYPE_CNF)
#define ProteusIII_CMD_GPIO_REMOTE_WRITE_IND (ProteusIII_CMD_GPIO_REMOTE_WRITE | ProteusIII_CMD_TYPE_IND)

#define ProteusIII_CMD_GPIO_REMOTE_READ (uint8_t)0x2A
#define ProteusIII_CMD_GPIO_REMOTE_READ_REQ (ProteusIII_CMD_GPIO_REMOTE_READ | ProteusIII_CMD_TYPE_REQ)
#define ProteusIII_CMD_GPIO_REMOTE_READ_CNF (ProteusIII_CMD_GPIO_REMOTE_READ | ProteusIII_CMD_TYPE_CNF)

#define ProteusIII_CMD_ALLOWUNBONDEDCONNECTIONS        (uint8_t)0x2D
#define ProteusIII_CMD_ALLOWUNBONDEDCONNECTIONS_REQ    (ProteusIII_CMD_ALLOWUNBONDEDCONNECTIONS | ProteusIII_CMD_TYPE_REQ)
#define ProteusIII_CMD_ALLOWUNBONDEDCONNECTIONS_CNF    (ProteusIII_CMD_ALLOWUNBONDEDCONNECTIONS | ProteusIII_CMD_TYPE_CNF)

#define CMD_ARRAY_SIZE() ((((uint16_t)CMD_Array[CMD_POSITION_LENGTH_LSB] << 0) | ((uint16_t)CMD_Array[CMD_POSITION_LENGTH_MSB] << 8)) + LENGTH_CMD_OVERHEAD)


/**
 * @brief Type used to check the response, when a command was sent to the ProteusIII
 */
typedef enum ProteusIII_CMD_Status_t
{
    CMD_Status_Success = (uint8_t)0x00,
    CMD_Status_Failed = (uint8_t)0x01,
    CMD_Status_Invalid,
    CMD_Status_Reset,
    CMD_Status_NoStatus,
} ProteusIII_CMD_Status_t;

/**
 * @brief Command confirmation.
 */
typedef struct
{
    uint8_t cmd;            /**< Variable to check if correct CMD has been confirmed */
    ProteusIII_CMD_Status_t status;    /**< Variable used to check the response (*_CNF), when a request (*_REQ) was sent to the ProteusIII */
} ProteusIII_CMD_Confirmation_t;

/**************************************
 *          Static variables          *
 **************************************/
static uint8_t CMD_Array[MAX_CMD_LENGTH]; /* for UART TX to module*/
static uint8_t RxPacket[MAX_CMD_LENGTH];

#define CMDCONFIRMATIONARRAY_LENGTH 2
static ProteusIII_CMD_Confirmation_t cmdConfirmation_array[CMDCONFIRMATIONARRAY_LENGTH];
static ProteusIII_GetDevices_t* ProteusIII_GetDevicesP = NULL;
static ProteusIII_States_t ble_state;
static bool askedForState;
static WE_Pin_t ProteusIII_pins[ProteusIII_Pin_Count] = {0};
static ProteusIII_CallbackConfig_t callbacks;
static uint8_t checksum = 0;
static uint16_t RxByteCounter = 0;
static uint16_t BytesToReceive = 0;
static uint8_t RxBuffer[MAX_CMD_LENGTH]; /* For UART RX from module */

/**************************************
 *         Static functions           *
 **************************************/


static void HandleRxPacket(uint8_t *pRxBuffer)
{
    ProteusIII_CMD_Confirmation_t cmdConfirmation;
    cmdConfirmation.cmd = CNFINVALID;
    cmdConfirmation.status = CMD_Status_Invalid;

    uint16_t cmd_length = (uint16_t)(pRxBuffer[CMD_POSITION_LENGTH_LSB]+(pRxBuffer[CMD_POSITION_LENGTH_MSB]<<8));
    memcpy(&RxPacket[0], pRxBuffer, cmd_length + LENGTH_CMD_OVERHEAD);

    switch (RxPacket[CMD_POSITION_CMD])
    {
    case ProteusIII_CMD_GETDEVICES_CNF:
    {
        cmdConfirmation.cmd = RxPacket[CMD_POSITION_CMD];
        cmdConfirmation.status = RxPacket[CMD_POSITION_DATA];
        if((cmdConfirmation.status == CMD_Status_Success)&&(ProteusIII_GetDevicesP != NULL))
        {
            uint8_t size = RxPacket[CMD_POSITION_DATA+1];

            if (size >= MAX_NUMBER_OF_DEVICES)
            {
                size = MAX_NUMBER_OF_DEVICES;
            }
            ProteusIII_GetDevicesP->numberofdevices = size;

            int i;
            int len = CMD_POSITION_DATA+2;
            for(i=0; i<ProteusIII_GetDevicesP->numberofdevices; i++)
            {
                memcpy(&ProteusIII_GetDevicesP->devices[i].btmac[0],&RxPacket[len],6);
                ProteusIII_GetDevicesP->devices[i].rssi = RxPacket[len+6];
                ProteusIII_GetDevicesP->devices[i].txpower = RxPacket[len+7];
                ProteusIII_GetDevicesP->devices[i].devicenamelength = RxPacket[len+8];
                memcpy(&ProteusIII_GetDevicesP->devices[i].devicename[0],&RxPacket[len+9],ProteusIII_GetDevicesP->devices[i].devicenamelength);
                len += (9+ProteusIII_GetDevicesP->devices[i].devicenamelength);
            }
        }
        break;
    }
    case ProteusIII_CMD_RESET_CNF:
    case ProteusIII_CMD_SCANSTART_CNF:
    case ProteusIII_CMD_SCANSTOP_CNF:
    case ProteusIII_CMD_GET_CNF:
    case ProteusIII_CMD_SET_CNF:
    case ProteusIII_CMD_PASSKEY_CNF:
    case ProteusIII_CMD_PHYUPDATE_CNF:
    case ProteusIII_CMD_CONNECT_CNF:
    case ProteusIII_CMD_DATA_CNF:
    case ProteusIII_CMD_DISCONNECT_CNF:
    case ProteusIII_CMD_FACTORYRESET_CNF:
    case ProteusIII_CMD_SLEEP_CNF:
    case ProteusIII_CMD_GPIO_LOCAL_WRITECONFIG_CNF:
    case ProteusIII_CMD_GPIO_LOCAL_READCONFIG_CNF:
    case ProteusIII_CMD_GPIO_LOCAL_WRITE_CNF:
    case ProteusIII_CMD_GPIO_LOCAL_READ_CNF:
    case ProteusIII_CMD_GPIO_REMOTE_WRITECONFIG_CNF:
    case ProteusIII_CMD_GPIO_REMOTE_READCONFIG_CNF:
    case ProteusIII_CMD_GPIO_REMOTE_WRITE_CNF:
    case ProteusIII_CMD_GPIO_REMOTE_READ_CNF:
    case ProteusIII_CMD_ALLOWUNBONDEDCONNECTIONS_CNF:
    case ProteusIII_CMD_TXCOMPLETE_RSP:
    {
        cmdConfirmation.cmd = RxPacket[CMD_POSITION_CMD];
        cmdConfirmation.status = RxPacket[CMD_POSITION_DATA];
        break;
    }

    case ProteusIII_CMD_GETSTATE_CNF:
    {
        cmdConfirmation.cmd = RxPacket[CMD_POSITION_CMD];
        /* GETSTATE_CNF has no status field*/
        cmdConfirmation.status = CMD_Status_NoStatus;
        break;
    }

    case ProteusIII_CMD_CHANNELOPEN_RSP:
    {
        /* Payload of CHANNELOPEN_RSP: Status (1 byte), BTMAC (6 byte), Max Payload (1byte)*/
        ble_state = ProteusIII_State_BLE_Channel_Open;
        if(callbacks.channelOpenCb != NULL)
        {
            callbacks.channelOpenCb(&RxPacket[CMD_POSITION_DATA+1], (uint16_t)RxPacket[CMD_POSITION_DATA + 7]);
        }
        break;
    }

    case ProteusIII_CMD_CONNECT_IND:
    {
        ble_state = ProteusIII_State_BLE_Connected;
        if(callbacks.connectCb != NULL)
        {
            callbacks.connectCb(&RxPacket[CMD_POSITION_DATA+1]);
        }
        break;
    }

    case ProteusIII_CMD_DISCONNECT_IND:
    {
        ble_state = ProteusIII_State_BLE_Invalid;
        if(callbacks.disconnectCb != NULL)
        {
            callbacks.disconnectCb();
        }
        break;
    }

    case ProteusIII_CMD_DATA_IND:
    {
        if(callbacks.rxCb != NULL)
        {
            uint16_t payload_length = ((((uint16_t) RxPacket[CMD_POSITION_LENGTH_LSB] << 0) | ((uint16_t) RxPacket[CMD_POSITION_LENGTH_MSB] << 8))) - 7;
            callbacks.rxCb(&RxPacket[CMD_POSITION_DATA + 7], payload_length, &RxPacket[CMD_POSITION_DATA], RxPacket[CMD_POSITION_DATA + 6]);
        }
        break;
    }

    case ProteusIII_CMD_SECURITY_IND:
    {
        if(callbacks.securityCb != NULL)
        {
            callbacks.securityCb(&RxPacket[CMD_POSITION_DATA+1],RxPacket[CMD_POSITION_DATA]);
        }
        break;
    }

    case ProteusIII_CMD_PASSKEY_IND:
    {
        if(callbacks.passkeyCb != NULL)
        {
            callbacks.passkeyCb(&RxPacket[CMD_POSITION_DATA+1]);
        }
        break;
    }

    case ProteusIII_CMD_DISPLAY_PASSKEY_IND:
    {
        if(callbacks.displayPasskeyCb != NULL)
        {
            callbacks.displayPasskeyCb((ProteusIII_DisplayPasskeyAction_t)RxPacket[CMD_POSITION_DATA],&RxPacket[CMD_POSITION_DATA+1],&RxPacket[CMD_POSITION_DATA+7]);
        }
        break;
    }

    case ProteusIII_CMD_PHYUPDATE_IND:
    {
        if(callbacks.phyUpdateCb != NULL)
        {
            callbacks.phyUpdateCb(&RxPacket[CMD_POSITION_DATA+3],(ProteusIII_Phy_t)RxPacket[CMD_POSITION_DATA+1],(ProteusIII_Phy_t)RxPacket[CMD_POSITION_DATA+2]);
        }
        break;
    }

    default:
    {
        /* invalid*/
        break;
    }
    }

    int i = 0;
    for(i=0; i<CMDCONFIRMATIONARRAY_LENGTH; i++)
    {
        if(cmdConfirmation_array[i].cmd == CNFINVALID)
        {
            cmdConfirmation_array[i].cmd = cmdConfirmation.cmd;
            cmdConfirmation_array[i].status = cmdConfirmation.status;
            break;
        }
    }
}

/**
 * @brief Function that waits for the return value of ProteusIII (*_CNF),
 * when a command (*_REQ) was sent before.
 */
static bool Wait4CNF(int max_time_ms, uint8_t expectedCmdConfirmation, ProteusIII_CMD_Status_t expectedStatus, bool reset_confirmstate)
{
    int count = 0;
    int time_step_ms = 5; /* 5ms */
    int max_count = max_time_ms / time_step_ms;
    int i = 0;

    if(reset_confirmstate)
    {
        for(i=0; i<CMDCONFIRMATIONARRAY_LENGTH; i++)
        {
            cmdConfirmation_array[i].cmd = CNFINVALID;
        }
    }
    while (1)
    {
        for(i=0; i<CMDCONFIRMATIONARRAY_LENGTH; i++)
        {
            if(expectedCmdConfirmation == cmdConfirmation_array[i].cmd)
            {
                return (cmdConfirmation_array[i].status == expectedStatus);
            }
        }

        if (count >= max_count)
        {
            /* received no correct response within timeout */
            return false;
        }

        /* wait */
        count++;
        WE_Delay(time_step_ms);
    }
    return true;
}

/**
 * @brief Function to add the checksum at the end of the data packet.
 */
static bool FillChecksum(uint8_t* pArray, uint16_t length)
{
    bool ret = false;

    if ((length >= LENGTH_CMD_OVERHEAD) && (pArray[0] == CMD_STX))
    {
        uint8_t checksum = (uint8_t)0;
        uint16_t payload_length = (uint16_t) (pArray[CMD_POSITION_LENGTH_MSB] << 8) + pArray[CMD_POSITION_LENGTH_LSB];
        uint16_t i = 0;
        for (i = 0;
                i < (payload_length + LENGTH_CMD_OVERHEAD_WITHOUT_CRC);
                i++)
        {
            checksum ^= pArray[i];
        }
        pArray[payload_length + LENGTH_CMD_OVERHEAD_WITHOUT_CRC] = checksum;
        ret = true;
    }
    return ret;
}

/**************************************
 *         Global functions           *
 **************************************/
 
void WE_UART_HandleRxByte(uint8_t received_byte)
{
	RxBuffer[RxByteCounter] = received_byte;

	switch (RxByteCounter)
	{
	case 0:
		/* wait for start byte of frame */
		if (RxBuffer[RxByteCounter] == CMD_STX)
		{
			BytesToReceive = 0;
			RxByteCounter = 1;
		}
		break;

	case 1:
		/* CMD */
		RxByteCounter++;
		break;

	case 2:
		/* length field lsb */
		RxByteCounter++;
		BytesToReceive = (uint16_t)(RxBuffer[RxByteCounter - 1]);
		break;

	case 3:
		/* length field msb */
		RxByteCounter++;
		BytesToReceive += (((uint16_t)RxBuffer[RxByteCounter - 1]<<8) + LENGTH_CMD_OVERHEAD); /* len_msb + len_lsb + crc + sfd + cmd */
		break;

	default:
		/* data field */
		RxByteCounter++;
		if (RxByteCounter == BytesToReceive)
		{
			/* check CRC */
			checksum = 0;
			int i = 0;
			for (i = 0; i < (BytesToReceive - 1); i++)
			{
				checksum ^= RxBuffer[i];
			}

			if (checksum == RxBuffer[BytesToReceive - 1])
			{
				/* received frame ok, interpret it now */
				HandleRxPacket(RxBuffer);
			}

			RxByteCounter = 0;
			BytesToReceive = 0;
		}
		break;
	}

}

/**
 * @brief Initialize the ProteusIII for serial interface.
 *
 * Caution: The parameter baudrate must match the configured UserSettings of the ProteusIII.
 *          The baudrate parameter must match to perform a successful FTDI communication.
 *          Updating this parameter during runtime may lead to communication errors.
 *
 * @param[in] baudrate:       baudrate of the interface
 * @param[in] flow_control:   enable/disable flowcontrol
 *
 * @return true if initialization succeeded,
 *         false otherwise
 */
bool ProteusIII_Init(uint32_t baudrate, WE_FlowControl_t flow_control, ProteusIII_CallbackConfig_t callbackConfig)
{
    /* initialize the pins */
	ProteusIII_pins[ProteusIII_Pin_Reset].port = GPIOA;
	ProteusIII_pins[ProteusIII_Pin_Reset].pin = GPIO_PIN_10;
	ProteusIII_pins[ProteusIII_Pin_Reset].type = WE_Pin_Type_Output;
	ProteusIII_pins[ProteusIII_Pin_SleepWakeUp].port = GPIOA;
	ProteusIII_pins[ProteusIII_Pin_SleepWakeUp].pin = GPIO_PIN_9;
	ProteusIII_pins[ProteusIII_Pin_SleepWakeUp].type = WE_Pin_Type_Output;
	ProteusIII_pins[ProteusIII_Pin_Boot].port = GPIOA;
	ProteusIII_pins[ProteusIII_Pin_Boot].pin = GPIO_PIN_7;
	ProteusIII_pins[ProteusIII_Pin_Boot].type = WE_Pin_Type_Output;
	ProteusIII_pins[ProteusIII_Pin_Mode].port = GPIOA;
	ProteusIII_pins[ProteusIII_Pin_Mode].pin = GPIO_PIN_8;
	ProteusIII_pins[ProteusIII_Pin_Mode].type = WE_Pin_Type_Output;
    if (false == WE_InitPins(ProteusIII_pins, ProteusIII_Pin_Count))
    {
        /* error */
        return false ;
    }
    WE_SetPin(ProteusIII_pins[ProteusIII_Pin_Boot], WE_Pin_Level_High);
    WE_SetPin(ProteusIII_pins[ProteusIII_Pin_SleepWakeUp], WE_Pin_Level_High);
    WE_SetPin(ProteusIII_pins[ProteusIII_Pin_Reset], WE_Pin_Level_High);
    WE_SetPin(ProteusIII_pins[ProteusIII_Pin_Mode], WE_Pin_Level_Low);

    /* set RX callback function */
    callbacks.rxCb = callbackConfig.rxCb;
	callbacks.connectCb = callbackConfig.connectCb;
	callbacks.securityCb = callbackConfig.securityCb;
	callbacks.passkeyCb = callbackConfig.passkeyCb;
    callbacks.displayPasskeyCb = callbackConfig.displayPasskeyCb;
	callbacks.disconnectCb = callbackConfig.disconnectCb;
	callbacks.channelOpenCb = callbackConfig.channelOpenCb;
	callbacks.phyUpdateCb = callbackConfig.phyUpdateCb;

	WE_UART_Init(baudrate, flow_control, WE_Parity_None, false);
    WE_Delay(10);

	/* reset module*/
    if(ProteusIII_PinReset())
    {
        WE_Delay(ProteusIII_BOOT_DURATION);
    }
    else
    {
        fprintf(stdout, "Pin reset failed\n");
        ProteusIII_Deinit();
        return false;
    }

	askedForState = false;
	ble_state = ProteusIII_State_BLE_Invalid;
	ProteusIII_GetDevicesP = NULL;

	uint8_t driver_version[3];
	if(WE_GetDriverVersion(driver_version))
	{
		fprintf(stdout, "ProteusIII driver version %d.%d.%d\n",driver_version[0],driver_version[1],driver_version[2]);
	}
	WE_Delay(100);

	return true;
}

/**
 * @brief Deinitialize the ProteusIII interface.
 *
 * @return true if deinitialization succeeded,
 *         false otherwise
 */
bool ProteusIII_Deinit()
{
	/* close the communication interface to the module */
	WE_UART_DeInit();

    /* deinit pins */
    WE_DeinitPin(ProteusIII_pins[ProteusIII_Pin_Reset]);
    WE_DeinitPin(ProteusIII_pins[ProteusIII_Pin_SleepWakeUp]);
    WE_DeinitPin(ProteusIII_pins[ProteusIII_Pin_Boot]);
    WE_DeinitPin(ProteusIII_pins[ProteusIII_Pin_Mode]);

    /* reset callbacks */
    callbacks.rxCb = NULL;
    callbacks.connectCb = NULL;
    callbacks.securityCb = NULL;
    callbacks.passkeyCb = NULL;
    callbacks.disconnectCb = NULL;
    callbacks.channelOpenCb = NULL;
    callbacks.phyUpdateCb = NULL;
    callbacks.displayPasskeyCb = NULL;

    return true;
}

/**
 * @brief Wakeup the ProteusIII from sleep by pin
 *
 * @return true if wakeup succeeded,
 *         false otherwise
 */
bool ProteusIII_PinWakeup()
{
    int i = 0;

    WE_SetPin(ProteusIII_pins[ProteusIII_Pin_SleepWakeUp], WE_Pin_Level_Low);
    WE_Delay (5);
    for(i=0; i<CMDCONFIRMATIONARRAY_LENGTH; i++)
    {
        cmdConfirmation_array[i].status = CMD_Status_Invalid;
        cmdConfirmation_array[i].cmd = CNFINVALID;
    }
    WE_SetPin(ProteusIII_pins[ProteusIII_Pin_SleepWakeUp], WE_Pin_Level_High);

    /* wait for cnf */
    return Wait4CNF(CMD_WAIT_TIME, ProteusIII_CMD_GETSTATE_CNF, CMD_Status_NoStatus, false);
}

/**
 * @brief Reset the ProteusIII by pin.
 *
 * @return true if reset succeeded,
 *         false otherwise
 */
bool ProteusIII_PinReset()
{
    /* set to output mode */
    WE_SetPin(ProteusIII_pins[ProteusIII_Pin_Reset], WE_Pin_Level_Low);
    WE_Delay(5);
    WE_SetPin(ProteusIII_pins[ProteusIII_Pin_Reset], WE_Pin_Level_High);

    /* wait for cnf */
    return Wait4CNF(CMD_WAIT_TIME, ProteusIII_CMD_GETSTATE_CNF, CMD_Status_NoStatus, true);
}

/**
 * @brief Reset the ProteusIII by command
 *
 * @return true if reset succeeded,
 *         false otherwise
 */
bool ProteusIII_Reset()
{
    bool ret = false;

    /* fill CMD_ARRAY packet */
    CMD_Array[CMD_POSITION_STX] = CMD_STX;
    CMD_Array[CMD_POSITION_CMD] = ProteusIII_CMD_RESET_REQ;
    CMD_Array[CMD_POSITION_LENGTH_LSB] = (uint8_t)0;
    CMD_Array[CMD_POSITION_LENGTH_MSB] = (uint8_t)0;

    if (FillChecksum(CMD_Array, CMD_ARRAY_SIZE()))
    {
        /* now send CMD_ARRAY */
        WE_UART_Transmit(CMD_Array, CMD_ARRAY_SIZE());

        /* wait for cnf */
        return Wait4CNF(CMD_WAIT_TIME, ProteusIII_CMD_GETSTATE_CNF, CMD_Status_NoStatus, true);
    }
    return ret;
}

/**
 * @brief Disconnect the ProteusIII connection if open.
 *
 * @return true if disconnect succeeded,
 *         false otherwise
 */
bool ProteusIII_Disconnect()
{
    bool ret = false;
    /* fill CMD_ARRAY packet */
    CMD_Array[CMD_POSITION_STX] = CMD_STX;
    CMD_Array[CMD_POSITION_CMD] = ProteusIII_CMD_DISCONNECT_REQ;
    CMD_Array[CMD_POSITION_LENGTH_LSB] = (uint8_t)0;
    CMD_Array[CMD_POSITION_LENGTH_MSB] = (uint8_t)0;

    if (FillChecksum(CMD_Array, CMD_ARRAY_SIZE()))
    {
        /* now send CMD_ARRAY */
        WE_UART_Transmit(CMD_Array, CMD_ARRAY_SIZE());

        /* Confirmation is sent before perfoming the disconnect. After disconnect, module sends dicsonnect indication */
        ret = Wait4CNF(CMD_WAIT_TIME, ProteusIII_CMD_DISCONNECT_CNF, CMD_Status_Success, true);
    }
    return ret;
}

/**
 * @brief Put the ProteusIII into sleep mode.
 *
 * @return true if succeeded,
 *         false otherwise
 */
bool ProteusIII_Sleep()
{
    bool ret = false;
    /* fill CMD_ARRAY packet */
    CMD_Array[CMD_POSITION_STX] = CMD_STX;
    CMD_Array[CMD_POSITION_CMD] = ProteusIII_CMD_SLEEP_REQ;
    CMD_Array[CMD_POSITION_LENGTH_LSB] = (uint8_t)0;
    CMD_Array[CMD_POSITION_LENGTH_MSB] = (uint8_t)0;

    if (FillChecksum(CMD_Array, CMD_ARRAY_SIZE()))
    {
        /* now send CMD_ARRAY */
        WE_UART_Transmit( CMD_Array, CMD_ARRAY_SIZE());

        /* wait for cnf */
        ret = Wait4CNF(CMD_WAIT_TIME, ProteusIII_CMD_SLEEP_CNF, CMD_Status_Success, true);
    }
    return ret;
}

/**
 * @brief Transmit data if a connection is open
 *
 * @brief PayloadP: pointer to the data to transmit
 * @brief length:   length of the data to transmit
 *
 * @return true if succeeded,
 *         false otherwise
 */
bool ProteusIII_Transmit(uint8_t *PayloadP, uint16_t length)
{
    bool ret = false;
    if ((length < MAX_PAYLOAD_LENGTH)&&(ProteusIII_State_BLE_Channel_Open == ProteusIII_GetDriverState()))
    {
        CMD_Array[CMD_POSITION_STX] = CMD_STX;
        CMD_Array[CMD_POSITION_CMD] = ProteusIII_CMD_DATA_REQ;
        CMD_Array[CMD_POSITION_LENGTH_LSB] = (uint8_t) (length >> 0);
        CMD_Array[CMD_POSITION_LENGTH_MSB] = (uint8_t) (length >> 8);

        memcpy(&CMD_Array[CMD_POSITION_DATA], PayloadP, length);

        if (FillChecksum(CMD_Array, CMD_ARRAY_SIZE()))
        {
            WE_UART_Transmit( CMD_Array, CMD_ARRAY_SIZE());
            ret = Wait4CNF(CMD_WAIT_TIME, ProteusIII_CMD_TXCOMPLETE_RSP, CMD_Status_Success, true);
        }
    }
    return ret;
}


/*
 * @brief Factory reset of the module.
 *
 * @return true if succeeded,
 *         false otherwise
 */
bool ProteusIII_FactoryReset()
{
    bool ret = false;
    /* fill CMD_ARRAY packet */
    CMD_Array[CMD_POSITION_STX] = CMD_STX;
    CMD_Array[CMD_POSITION_CMD] = ProteusIII_CMD_FACTORYRESET_REQ;
    CMD_Array[CMD_POSITION_LENGTH_LSB] = (uint8_t)0;
    CMD_Array[CMD_POSITION_LENGTH_MSB] = (uint8_t)0;

    if (FillChecksum(CMD_Array, CMD_ARRAY_SIZE()))
    {
        /* now send CMD_ARRAY */
        WE_UART_Transmit(CMD_Array, CMD_ARRAY_SIZE());

        /* wait for reset after factory reset */
        return Wait4CNF(CMD_WAIT_TIME, ProteusIII_CMD_GETSTATE_CNF, CMD_Status_NoStatus, true);
    }
    return ret;
}

/**
 * @brief Set a special user setting.
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
bool ProteusIII_Set(ProteusIII_UserSettings_t userSetting, uint8_t *ValueP, uint8_t length)
{
    bool ret = false;

    /* fill CMD_ARRAY packet */
    CMD_Array[CMD_POSITION_STX] = CMD_STX;
    CMD_Array[CMD_POSITION_CMD] = ProteusIII_CMD_SET_REQ;
    CMD_Array[CMD_POSITION_LENGTH_LSB] = (uint8_t) (1 + length);
    CMD_Array[CMD_POSITION_LENGTH_MSB] = (uint8_t)0;
    CMD_Array[CMD_POSITION_DATA] = userSetting;
    memcpy(&CMD_Array[CMD_POSITION_DATA + 1], ValueP, length);

    if (FillChecksum(CMD_Array, CMD_ARRAY_SIZE()))
    {
        /* now send CMD_ARRAY */
        WE_UART_Transmit(CMD_Array, CMD_ARRAY_SIZE());

        /* wait for cnf */
        return Wait4CNF(CMD_WAIT_TIME, ProteusIII_CMD_SET_CNF, CMD_Status_Success, true);
    }
    return ret;
}

/**
 * @brief Set the BLE device name.
 *
 * Note: Reset the module after the adaption of the setting so that it can take effect.
 * Note: Use this function only in rare case, since flash can be updated only a limited number of times.
 *
 * @param[in] deviceNameP: pointer to the device name
 * @param[in] nameLength:  length of the device name
 *
 *note: reset the module after the adaption of the setting such that it can take effect
 *note: use this function only in rare case, since flash can be updated only a limited number times
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ProteusIII_SetDeviceName(uint8_t *deviceNameP, uint8_t nameLength)
{
    return ProteusIII_Set(ProteusIII_USERSETTING_POSITION_RF_DEVICE_NAME, deviceNameP, nameLength);
}

/**
 * @brief Set the BLE advertising timeout.
 *
 * Note: Reset the module after the adaption of the setting so that it can take effect.
 * Note: Use this function only in rare case, since flash can be updated only a limited number of times.
 *
 * @param[in] advTimeout: advertising timeout
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ProteusIII_SetAdvertisingTimeout(uint16_t advTimeout)
{
    uint8_t help[2];
    memcpy(help,(uint8_t*)&advTimeout,2);
    return ProteusIII_Set(ProteusIII_USERSETTING_POSITION_RF_ADVERTISING_TIMEOUT, help, 2);
}

/**
 * @brief Set the CFG flags
 *
 * Note: Reset the module after the adaption of the setting so that it can take effect.
 * Note: Use this function only in rare case, since flash can be updated only a limited number of times.
 *
 * @param[in] cfgflags: CFG flags
 *
 * @return true if request succeeded
 *         false otherwise
 */
bool ProteusIII_SetCFGFlags(uint16_t cfgflags)
{
    uint8_t help[2];
    memcpy(help,(uint8_t*)&cfgflags,2);
    return ProteusIII_Set(ProteusIII_USERSETTING_POSITION_RF_CFGFLAGS, help, 2);
}

/**
 * @brief Set the BLE connection timing.
 *
 * Note: Reset the module after the adaption of the setting so that it can take effect.
 * Note: Use this function only in rare case, since flash can be updated only a limited number of times.
 *
 * @param[in] connectionTiming: connection timing
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ProteusIII_SetConnectionTiming(ProteusIII_ConnectionTiming_t connectionTiming)
{
    return ProteusIII_Set(ProteusIII_USERSETTING_POSITION_RF_CONNECTION_TIMING, (uint8_t*)&connectionTiming, 1);
}

/**
 * @brief Set the BLE scan timing
 *
 * Note: Reset the module after the adaption of the setting so that it can take effect.
 * Note: Use this function only in rare case, since flash can be updated only a limited number of times.
 *
 * @param[in] scanTiming: scan timing
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ProteusIII_SetScanTiming(ProteusIII_ScanTiming_t scanTiming)
{
    return ProteusIII_Set(ProteusIII_USERSETTING_POSITION_RF_SCAN_TIMING, (uint8_t*)&scanTiming, 1);
}

/**
 * @brief Set the BLE TX power.
 *
 * Note: Reset the module after the adaption of the setting so that it can take effect.
 * Note: Use this function only in rare case, since flash can be updated only a limited number of times.
 *
 * @param[in] txpower: TX power
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ProteusIII_SetTXPower(ProteusIII_TXPower_t txpower)
{
    return ProteusIII_Set(ProteusIII_USERSETTING_POSITION_RF_TX_POWER, (uint8_t*)&txpower, 1);
}

/**
 * @brief Set the BLE security flags.
 *
 * Note: Reset the module after the adaption of the setting so that it can take effect.
 * Note: Use this function only in rare case, since flash can be updated only a limited number of times.
 *
 * @param[in] secflags: security flags
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ProteusIII_SetSecFlags(ProteusIII_SecFlags_t secflags)
{
    return ProteusIII_Set(ProteusIII_USERSETTING_POSITION_RF_SEC_FLAGS, (uint8_t*)&secflags, 1);
}

/**
 * @brief Set the UART baudrate index
 *
 * Note: Reset the module after the adaption of the setting so that it can take effect.
 * Note: Use this function only in rare case, since flash can be updated only a limited number of times.
 *
 * @param[in] baudrate: UART baudrate
 * @param[in] parity: parity bit
 * @param[in] flowcontrolEnable: enable/disable flow control
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ProteusIII_SetBaudrateIndex(ProteusIII_BaudRate_t baudrate, ProteusIII_UartParity_t parity, bool flowcontrolEnable)
{
    uint8_t baudrateIndex = (uint8_t)baudrate;

    /* If flowcontrol is to be enabled UART index has to be increased by one in regard to base value */
    if(flowcontrolEnable)
    {
        baudrateIndex++;
    }

    /* If parity bit is even, UART index has to be increased by 64 in regard of base value*/
    if(ProteusIII_UartParity_Even == parity)
    {
        baudrateIndex += 64;
    }

    return ProteusIII_Set(ProteusIII_USERSETTING_POSITION_UART_CONFIG_INDEX, (uint8_t*)&baudrateIndex, 1);
}

/**
 * @brief Set the BLE static passkey
 *
 * Note: Reset the module after the adaption of the setting so that it can take effect.
 * Note: Use this function only in rare case, since flash can be updated only a limited number of times.
 *
 * @param[in] staticPasskeyP: pointer to the static passkey (6 digits)
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ProteusIII_SetStaticPasskey(uint8_t *staticPasskeyP)
{
    return ProteusIII_Set(ProteusIII_USERSETTING_POSITION_RF_STATIC_PASSKEY, staticPasskeyP, 6);
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
bool ProteusIII_Get(ProteusIII_UserSettings_t userSetting, uint8_t *responseP, uint16_t *response_lengthP)
{
    bool ret = false;

    /* fill CMD_ARRAY packet */
    CMD_Array[CMD_POSITION_STX] = CMD_STX;
    CMD_Array[CMD_POSITION_CMD] = ProteusIII_CMD_GET_REQ;
    CMD_Array[CMD_POSITION_LENGTH_LSB] = (uint8_t)1;
    CMD_Array[CMD_POSITION_LENGTH_MSB] = (uint8_t)0;
    CMD_Array[CMD_POSITION_DATA] = userSetting;

    if (FillChecksum(CMD_Array, CMD_ARRAY_SIZE()))
    {
        /* now send CMD_ARRAY */
        WE_UART_Transmit(CMD_Array, CMD_ARRAY_SIZE());

        /* wait for cnf */
        if (Wait4CNF(CMD_WAIT_TIME, ProteusIII_CMD_GET_CNF, CMD_Status_Success, true))
        {
            uint16_t length = ((uint16_t) RxPacket[CMD_POSITION_LENGTH_LSB] << 0) + ((uint16_t) RxPacket[CMD_POSITION_LENGTH_MSB] << 8);
            memcpy(responseP, &RxPacket[CMD_POSITION_DATA + 1], length - 1); /* First Data byte is status, following bytes response*/
            *response_lengthP = length - 1;
            ret = true;
        }
    }
    return ret;
}

/**
 * @brief Request the 3 byte firmware version.
 *
 * @param[out] versionP: pointer to the 3 byte firmware version
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ProteusIII_GetFWVersion(uint8_t *versionP)
{
    uint16_t length;
    return ProteusIII_Get(ProteusIII_USERSETTING_POSITION_FS_FWVersion, versionP, &length);
}

/**
 * @brief Request the current BLE device name.
 *
 * @param[out] deviceNameP: pointer to device name
 * @param[out] nameLengthP: pointer to the length of the device name
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ProteusIII_GetDeviceName(uint8_t *deviceNameP, uint16_t *nameLengthP)
{
    return ProteusIII_Get(ProteusIII_USERSETTING_POSITION_RF_DEVICE_NAME, deviceNameP, nameLengthP);
}

/**
 * @brief Request the 8 digit MAC.
 *
 * @param[out] macP: pointer to the MAC
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ProteusIII_GetMAC(uint8_t *macP)
{
    uint16_t length;
    return ProteusIII_Get(ProteusIII_USERSETTING_POSITION_FS_MAC, macP, &length);
}

/**
 * @brief Request the 6 digit Bluetooth MAC.
 *
 * @param[out] BTMacP: pointer to the Bluetooth MAC
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ProteusIII_GetBTMAC(uint8_t *BTMacP)
{
    uint16_t length;
    return ProteusIII_Get(ProteusIII_USERSETTING_POSITION_FS_BTMAC, BTMacP, &length);
}

/**
 * @brief Request the advertising timeout
 *
 * @param[out] advTimeoutP: pointer to the advertising timeout
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ProteusIII_GetAdvertisingTimeout(uint16_t *advTimeoutP)
{
    uint16_t length;
    bool ret = false;
    uint8_t help[2];

    ret = ProteusIII_Get(ProteusIII_USERSETTING_POSITION_RF_ADVERTISING_TIMEOUT, help, &length);
    memcpy((uint8_t*)advTimeoutP,help,2);

    return ret;
}

/**
 * @brief Request the connection timing
 *
 * @param[out] connectionTimingP: pointer to the  connection timing
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ProteusIII_GetConnectionTiming(ProteusIII_ConnectionTiming_t *connectionTimingP)
{
    uint16_t length;
    return ProteusIII_Get(ProteusIII_USERSETTING_POSITION_RF_CONNECTION_TIMING, (uint8_t*)connectionTimingP, &length);
}

/**
 * @brief Request the scan timing
 *
 * @param[out] scanTimingP: pointer to the  scan timing
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ProteusIII_GetScanTiming(ProteusIII_ScanTiming_t *scanTimingP)
{
    uint16_t length;
    return ProteusIII_Get(ProteusIII_USERSETTING_POSITION_RF_SCAN_TIMING, (uint8_t*)scanTimingP, &length);
}

/**
 * @brief Request the TX power
 *
 * @param[out] txpowerP: pointer to the TX power
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ProteusIII_GetTXPower(ProteusIII_TXPower_t *txpowerP)
{
    uint16_t length;
    return ProteusIII_Get(ProteusIII_USERSETTING_POSITION_RF_TX_POWER, (uint8_t*)txpowerP, &length);
}

/**
 * @brief Request the security flags
 *
 * @param[out] secflagsP: pointer to the security flags
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ProteusIII_GetSecFlags(ProteusIII_SecFlags_t *secflagsP)
{
    uint16_t length;
    return ProteusIII_Get(ProteusIII_USERSETTING_POSITION_RF_SEC_FLAGS, (uint8_t*)secflagsP, &length);
}

/**
 * @brief Request the UART baudrate index
 *
 * @param[out] baudrateP: pointer to the UART baudrate index
 * @param[out] parityP: pointer to the UART parity
 * @param[out] flowcontrolEnableP: pointer to the UART flow control parameter
 *
 *return true if request succeeded
 *       false otherwise
 */
bool ProteusIII_GetBaudrateIndex(ProteusIII_BaudRate_t *baudrateP, ProteusIII_UartParity_t *parityP, bool *flowcontrolEnableP)
{
    bool ret = false;
    uint16_t length;
    uint8_t uartIndex;

    if(ProteusIII_Get(ProteusIII_USERSETTING_POSITION_UART_CONFIG_INDEX, (uint8_t*)&uartIndex, &length))
    {
        /* if index is even, flow control is off.
         * If flow control is on, decrease index by one to later determin the base baudrate */
        if(0x01 == (uartIndex & 0x01))
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
        if(uartIndex < 64)
        {
            *parityP = ProteusIII_UartParity_None;
        }
        else
        {

            *parityP = ProteusIII_UartParity_Even;
            uartIndex -= 64;
        }

        *baudrateP = (ProteusIII_BaudRate_t)uartIndex;
        ret = true;
    }

    return ret;
}

/**
 * @brief Request the BLE static passkey
 *
 * @param[out] staticPasskeyP: pointer to the static passkey (6 digits)
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ProteusIII_GetStaticPasskey(uint8_t *staticPasskeyP)
{
    uint16_t length;
    return ProteusIII_Get(ProteusIII_USERSETTING_POSITION_RF_STATIC_PASSKEY, staticPasskeyP, &length);
}

/**
 * @brief Request the CFG flags
 *
 * @param[out] cfgflags: pointer to the CFG flags
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ProteusIII_GetCFGFlags(uint16_t *cfgflags)
{
    uint16_t length;
    bool ret = false;
    uint8_t help[2];

    ret = ProteusIII_Get(ProteusIII_USERSETTING_POSITION_RF_CFGFLAGS, help, &length);
    memcpy((uint8_t*)cfgflags,help,2);

    return ret;
}

/**
 * @brief Request the module state
 *
 * @param[out] BLE_roleP:   pointer to the BLE role of the module
 * @param[out] BLE_actionP: pointer to the BLE action of the module
 * @param[out] InfoP:       pointer to the state info of the module
 * @param[out] LengthP:     pointer to the length of the state info of the module
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ProteusIII_GetState(ProteusIII_BLE_Role_t *BLE_roleP, ProteusIII_BLE_Action_t *BLE_actionP, uint8_t *InfoP, uint8_t *LengthP)
{
    bool ret = false;

    /* fill CMD_ARRAY packet */
    CMD_Array[CMD_POSITION_STX] = CMD_STX;
    CMD_Array[CMD_POSITION_CMD] = ProteusIII_CMD_GETSTATE_REQ;
    CMD_Array[CMD_POSITION_LENGTH_LSB] = (uint8_t)0;
    CMD_Array[CMD_POSITION_LENGTH_MSB] = (uint8_t)0;

    if (FillChecksum(CMD_Array, CMD_ARRAY_SIZE()))
    {
        /* now send CMD_ARRAY */
        WE_UART_Transmit(CMD_Array, CMD_ARRAY_SIZE());
        askedForState = true;
        /* wait for cnf */
        if (Wait4CNF(CMD_WAIT_TIME, ProteusIII_CMD_GETSTATE_CNF, CMD_Status_NoStatus, true))
        {
            uint16_t length = ((uint16_t) RxPacket[CMD_POSITION_LENGTH_LSB] << 0) + ((uint16_t) RxPacket[CMD_POSITION_LENGTH_MSB] << 8);
            *BLE_roleP = RxPacket[CMD_POSITION_DATA];
            *BLE_actionP = RxPacket[CMD_POSITION_DATA + 1];
            memcpy(InfoP, &RxPacket[CMD_POSITION_DATA + 2], length - 2);

            *LengthP = length-2;
            ret = true;
        }
    }
    askedForState = false;
    return ret;
}

/**
 * @brief Request the current state of the driver
 *
 * @return driver state
 */
ProteusIII_States_t ProteusIII_GetDriverState()
{
    return ble_state;
}

/**
 * @brief Start scan
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ProteusIII_Scanstart()
{
    bool ret = false;

    /* fill CMD_ARRAY packet */
    CMD_Array[CMD_POSITION_STX] = CMD_STX;
    CMD_Array[CMD_POSITION_CMD] = ProteusIII_CMD_SCANSTART_REQ;
    CMD_Array[CMD_POSITION_LENGTH_LSB] = (uint8_t)0;
    CMD_Array[CMD_POSITION_LENGTH_MSB] = (uint8_t)0;

    if (FillChecksum(CMD_Array, CMD_ARRAY_SIZE()))
    {
        /* now send CMD_ARRAY */
        WE_UART_Transmit(CMD_Array, CMD_ARRAY_SIZE());

        /* wait for cnf */
        ret = Wait4CNF(CMD_WAIT_TIME, ProteusIII_CMD_SCANSTART_CNF, CMD_Status_Success, true);
    }
    return ret;
}

/**
 * @brief Stop a scan
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ProteusIII_Scanstop()
{
    bool ret = false;

    /* fill CMD_ARRAY packet */
    CMD_Array[CMD_POSITION_STX] = CMD_STX;
    CMD_Array[CMD_POSITION_CMD] = ProteusIII_CMD_SCANSTOP_REQ;
    CMD_Array[CMD_POSITION_LENGTH_LSB] = (uint8_t)0;
    CMD_Array[CMD_POSITION_LENGTH_MSB] = (uint8_t)0;

    if (FillChecksum(CMD_Array, CMD_ARRAY_SIZE()))
    {
        /* now send CMD_ARRAY */
        WE_UART_Transmit(CMD_Array, CMD_ARRAY_SIZE());

        /* wait for cnf */
        ret = Wait4CNF(CMD_WAIT_TIME, ProteusIII_CMD_SCANSTOP_CNF, CMD_Status_Success, true);
    }
    return ret;
}

/**
 * @brief Request the scan results
 *
 * @param[out] devicesP: pointer to scan result struct
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ProteusIII_GetDevices(ProteusIII_GetDevices_t* devicesP)
{
    bool ret = false;

    ProteusIII_GetDevicesP = devicesP;
    if (ProteusIII_GetDevicesP != NULL)
    {
        ProteusIII_GetDevicesP->numberofdevices = 0;
    }

    /* fill CMD_ARRAY packet */
    CMD_Array[CMD_POSITION_STX] = CMD_STX;
    CMD_Array[CMD_POSITION_CMD] = ProteusIII_CMD_GETDEVICES_REQ;
    CMD_Array[CMD_POSITION_LENGTH_LSB] = (uint8_t)0;
    CMD_Array[CMD_POSITION_LENGTH_MSB] = (uint8_t)0;

    if (FillChecksum(CMD_Array, CMD_ARRAY_SIZE()))
    {
        /* now send CMD_ARRAY */
        WE_UART_Transmit(CMD_Array, CMD_ARRAY_SIZE());

        /* wait for cnf */
        ret = Wait4CNF(CMD_WAIT_TIME, ProteusIII_CMD_GETDEVICES_CNF, CMD_Status_Success, true);
    }

    ProteusIII_GetDevicesP = NULL;

    return ret;
}

/**
 * @brief Connect to the BLE device with the corresponding BTMAC
 *
 * @param[in] btmac: pointer to btmac
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ProteusIII_Connect(uint8_t *btmac)
{
    bool ret = false;

    /* fill CMD_ARRAY packet */
    CMD_Array[CMD_POSITION_STX] = CMD_STX;
    CMD_Array[CMD_POSITION_CMD] = ProteusIII_CMD_CONNECT_REQ;
    CMD_Array[CMD_POSITION_LENGTH_LSB] = (uint8_t)6;
    CMD_Array[CMD_POSITION_LENGTH_MSB] = (uint8_t)0;
    memcpy(&CMD_Array[CMD_POSITION_DATA], btmac, 6);

    if (FillChecksum(CMD_Array, CMD_ARRAY_SIZE()))
    {
        /* now send CMD_ARRAY */
        WE_UART_Transmit(CMD_Array, CMD_ARRAY_SIZE());

        /* wait for cnf */
        ret = Wait4CNF(3000, ProteusIII_CMD_CONNECT_CNF, CMD_Status_Success, true);
    }
    return ret;
}

/**
 * @brief Answer on a passkey request with a passkey to setup a connection
 *
 * @param[in] passkey: pointer to passkey
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ProteusIII_Passkey(uint8_t* passkey)
{
    bool ret = false;

    /* fill CMD_ARRAY packet */
    CMD_Array[CMD_POSITION_STX] = CMD_STX;
    CMD_Array[CMD_POSITION_CMD] = ProteusIII_CMD_PASSKEY_REQ;
    CMD_Array[CMD_POSITION_LENGTH_LSB] = (uint8_t)6;
    CMD_Array[CMD_POSITION_LENGTH_MSB] = (uint8_t)0;
    memcpy(&CMD_Array[CMD_POSITION_DATA], passkey, 6);

    if (FillChecksum(CMD_Array, CMD_ARRAY_SIZE()))
    {
        /* now send CMD_ARRAY */
        WE_UART_Transmit(CMD_Array, CMD_ARRAY_SIZE());

        /* wait for cnf */
        ret = Wait4CNF(CMD_WAIT_TIME, ProteusIII_CMD_PASSKEY_CNF, CMD_Status_Success, true);
    }
    return ret;
}

/**
 * @brief Answer on a numeric comparison request
 *
 * @param[in] keyIsOk: boolean to confirm if the key shown is correct
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ProteusIII_NumericCompareConfirm(bool keyIsOk)
{
    bool ret = false;
    uint8_t lescStatus;

    /* If key is ok, status byte is "0x00"
        otherwise it's "0x01" */
    if(keyIsOk)
    {
        lescStatus = 0x00;
    }
    else
    {
        lescStatus = 0x01;
    }

    /* fill CMD_ARRAY packet */
    CMD_Array[CMD_POSITION_STX] = CMD_STX;
    CMD_Array[CMD_POSITION_CMD] = ProteusIII_CMD_NUMERIC_COMP_REQ;
    CMD_Array[CMD_POSITION_LENGTH_LSB] = (uint8_t)1;
    CMD_Array[CMD_POSITION_LENGTH_MSB] = (uint8_t)0;
    CMD_Array[CMD_POSITION_DATA] = lescStatus;

    if (FillChecksum(CMD_Array, CMD_ARRAY_SIZE()))
    {
        /* now send CMD_ARRAY */
        WE_UART_Transmit(CMD_Array, CMD_ARRAY_SIZE());

        /* wait for cnf */
        ret = Wait4CNF(CMD_WAIT_TIME, ProteusIII_CMD_NUMERIC_COMP_CNF, CMD_Status_Success, true);
    }
    return ret;
}

/**
 * @brief Update the phy during an open connection
 *
 * @param[in] phy: new phy
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ProteusIII_PhyUpdate(ProteusIII_Phy_t phy)
{
    bool ret = false;

    if (ProteusIII_State_BLE_Channel_Open == ProteusIII_GetDriverState())
    {
        /* fill CMD_ARRAY packet */
        CMD_Array[CMD_POSITION_STX] = CMD_STX;
        CMD_Array[CMD_POSITION_CMD] = ProteusIII_CMD_PHYUPDATE_REQ;
        CMD_Array[CMD_POSITION_LENGTH_LSB] = (uint8_t)1;
        CMD_Array[CMD_POSITION_LENGTH_MSB] = (uint8_t)0;
        CMD_Array[CMD_POSITION_DATA] = (uint8_t)phy;

        if (FillChecksum(CMD_Array, CMD_ARRAY_SIZE()))
        {
            /* now send CMD_ARRAY */
            WE_UART_Transmit(CMD_Array, CMD_ARRAY_SIZE());

            /* wait for cnf */
            ret = Wait4CNF(CMD_WAIT_TIME, ProteusIII_CMD_PHYUPDATE_CNF, CMD_Status_Success, true);
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
bool ProteusIII_GPIOLocalWriteConfig(ProteusIII_GPIOConfigBlock_t* configP, uint16_t number_of_configs)
{
    bool ret = false;
	uint16_t length = 0;

	for (uint16_t i=0; i < number_of_configs; i++)
	{
		switch(configP->function)
		{
			case ProteusIII_GPIO_IO_Disconnected:
			{
				CMD_Array[CMD_POSITION_DATA + length] = 3;
				CMD_Array[CMD_POSITION_DATA + length + 1] = configP->GPIO_ID;
				CMD_Array[CMD_POSITION_DATA + length + 2] = configP->function;
				CMD_Array[CMD_POSITION_DATA + length + 3] = 0x00;
				length += 4;
			}
			break;
			case ProteusIII_GPIO_IO_Input:
			{
				CMD_Array[CMD_POSITION_DATA + length] = 3;
				CMD_Array[CMD_POSITION_DATA + length + 1] = configP->GPIO_ID;
				CMD_Array[CMD_POSITION_DATA + length + 2] = configP->function;
				CMD_Array[CMD_POSITION_DATA + length + 3] = configP->value.input;
				length += 4;
			}
			break;
			case ProteusIII_GPIO_IO_Output:
			{
				CMD_Array[CMD_POSITION_DATA + length] = 3;
				CMD_Array[CMD_POSITION_DATA + length + 1] = configP->GPIO_ID;
				CMD_Array[CMD_POSITION_DATA + length + 2] = configP->function;
				CMD_Array[CMD_POSITION_DATA + length + 3] = configP->value.output;
				length += 4;
				}
			break;
			case ProteusIII_GPIO_IO_PWM:
			{
				CMD_Array[CMD_POSITION_DATA + length] = 5;
				CMD_Array[CMD_POSITION_DATA + length + 1] = configP->GPIO_ID;
				CMD_Array[CMD_POSITION_DATA + length + 2] = configP->function;
				memcpy(&CMD_Array[CMD_POSITION_DATA + length + 3], &configP->value.pwm.period, 2);
				CMD_Array[CMD_POSITION_DATA + length + 5] = configP->value.pwm.ratio;
				length += 6;
			}
			break;
			default:
			{
			}
			break;
		}

		/* Move pointer to next element. configP is increased by sizeof(ProteusIII_GPIOConfigBlock_t)*/
		configP++;
	}

    CMD_Array[CMD_POSITION_STX] = CMD_STX;
    CMD_Array[CMD_POSITION_CMD] = ProteusIII_CMD_GPIO_LOCAL_WRITECONFIG_REQ;
    CMD_Array[CMD_POSITION_LENGTH_LSB]= (length & 0x00FF);
    CMD_Array[CMD_POSITION_LENGTH_MSB]= (length & 0xFF00) >> 8;

    if (FillChecksum(CMD_Array, CMD_ARRAY_SIZE()))
    {
        /* now send CMD_ARRAY */
        WE_UART_Transmit(CMD_Array, CMD_ARRAY_SIZE());

        /* wait for cnf */
        ret = Wait4CNF(CMD_WAIT_TIME, ProteusIII_CMD_GPIO_LOCAL_WRITECONFIG_CNF, CMD_Status_Success, true);
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
bool ProteusIII_GPIOLocalReadConfig(ProteusIII_GPIOConfigBlock_t* configP, uint16_t *number_of_configsP)
{
    bool ret = false;

    CMD_Array[CMD_POSITION_STX] = CMD_STX;
    CMD_Array[CMD_POSITION_CMD] = ProteusIII_CMD_GPIO_LOCAL_READCONFIG_REQ;
    CMD_Array[CMD_POSITION_LENGTH_LSB]= 0;
    CMD_Array[CMD_POSITION_LENGTH_MSB]= 0;

    if (FillChecksum(CMD_Array, CMD_ARRAY_SIZE()))
    {
        /* now send CMD_ARRAY */
        WE_UART_Transmit(CMD_Array, CMD_ARRAY_SIZE());

        /* wait for cnf */
        ret = Wait4CNF(CMD_WAIT_TIME, ProteusIII_CMD_GPIO_LOCAL_READCONFIG_CNF, CMD_Status_Success, true);

		if(ret == true)
		{
			uint16_t length = ((uint16_t) RxPacket[CMD_POSITION_LENGTH_LSB] << 0) + ((uint16_t) RxPacket[CMD_POSITION_LENGTH_MSB] << 8);

			*number_of_configsP = 0;
			uint8_t* uartP = &RxPacket[CMD_POSITION_DATA+1];
			ProteusIII_GPIOConfigBlock_t* configP_running = configP;
			while(uartP < &RxPacket[CMD_POSITION_DATA+length])
			{
				switch(*(uartP + 2))
				{
					case ProteusIII_GPIO_IO_Disconnected:
					{
						if(*uartP == 3)
						{
							configP_running->GPIO_ID = *(uartP + 1);
							configP_running->function = *(uartP + 2);

							configP_running++;
							*number_of_configsP += 1;
						}
					}
					break;
					case ProteusIII_GPIO_IO_Input:
					{
						if(*uartP == 3)
						{
							configP_running->GPIO_ID = *(uartP + 1);
							configP_running->function = *(uartP + 2);
							configP_running->value.input = *(uartP + 3);

							configP_running++;
							*number_of_configsP += 1;
						}
					}
					break;
					case ProteusIII_GPIO_IO_Output:
					{
						if(*uartP == 3)
						{
							configP_running->GPIO_ID = *(uartP + 1);
							configP_running->function = *(uartP + 2);
							configP_running->value.output = *(uartP + 3);

							configP_running++;
							*number_of_configsP += 1;
						}
					}
					break;
					case ProteusIII_GPIO_IO_PWM:
					{
						if(*uartP == 5)
						{
							configP_running->GPIO_ID = *(uartP + 1);
							configP_running->function = *(uartP + 2);
							memcpy(&configP_running->value.pwm.period, (uartP + 3), 2);
							configP_running->value.pwm.ratio = *(uartP + 5);

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
 * See ProteusIII_GPIOLocalWriteConfig
 *
 * @param[in] controlP: pointer to one or more pin controls
 * @param[in] number_of_controls: number of entries in controlP array
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ProteusIII_GPIOLocalWrite(ProteusIII_GPIOControlBlock_t* controlP, uint16_t number_of_controls)
{
    bool ret = false;
	uint16_t length = 0;

	for (uint16_t i=0; i < number_of_controls; i++)
	{
		CMD_Array[CMD_POSITION_DATA + length] = 2;
		CMD_Array[CMD_POSITION_DATA + length + 1] = controlP->GPIO_ID;
		CMD_Array[CMD_POSITION_DATA + length + 2] = controlP->value.output;
		length += 3;

        /* Move pointer to next element. configP is increased by sizeof(ProteusIII_GPIOControlBlock_t)*/
		controlP++;
	}

    CMD_Array[CMD_POSITION_STX] = CMD_STX;
    CMD_Array[CMD_POSITION_CMD] = ProteusIII_CMD_GPIO_LOCAL_WRITE_REQ;
    CMD_Array[CMD_POSITION_LENGTH_LSB]= (length & 0x00FF);
    CMD_Array[CMD_POSITION_LENGTH_MSB]= (length & 0xFF00) >> 8;

    if (FillChecksum(CMD_Array, CMD_ARRAY_SIZE()))
    {
        /* now send CMD_ARRAY */
        WE_UART_Transmit(CMD_Array, CMD_ARRAY_SIZE());

        /* wait for cnf */
        ret = Wait4CNF(CMD_WAIT_TIME, ProteusIII_CMD_GPIO_LOCAL_WRITE_CNF, CMD_Status_Success, true);
    }

    return ret;
}

/**
 * @brief Read the input of the pin. Pin has to be configured first.
 * See ProteusIII_GPIOLocalWriteConfig
 *
 * @param[in] GPIOToReadP: One or more pins to read.
 * @param[in] amountGPIOToRead: amount of pins to read and therefore length of GPIOToRead
 * @param[out] controlP: Pointer to controlBlock
 * @param[out] number_of_controlsP: pointer to number of entries in controlP array
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ProteusIII_GPIOLocalRead(uint8_t *GPIOToReadP, uint8_t amountGPIOToRead, ProteusIII_GPIOControlBlock_t* controlP, uint16_t* number_of_controlsP)
{
    bool ret = false;

    CMD_Array[CMD_POSITION_STX] = CMD_STX;
    CMD_Array[CMD_POSITION_CMD] = ProteusIII_CMD_GPIO_LOCAL_READ_REQ;
    CMD_Array[CMD_POSITION_LENGTH_LSB]= amountGPIOToRead + 1;
    CMD_Array[CMD_POSITION_LENGTH_MSB]= 0;
    CMD_Array[CMD_POSITION_DATA] = amountGPIOToRead;
    memcpy(&CMD_Array[CMD_POSITION_DATA + 1], GPIOToReadP, amountGPIOToRead);

    if (FillChecksum(CMD_Array, CMD_ARRAY_SIZE()))
    {
        /* now send CMD_ARRAY */
        WE_UART_Transmit(CMD_Array, CMD_ARRAY_SIZE());

        /* wait for cnf */
        ret = Wait4CNF(CMD_WAIT_TIME, ProteusIII_CMD_GPIO_LOCAL_READ_CNF, CMD_Status_Success, true);

		if(ret)
		{
			uint16_t length = ((uint16_t) RxPacket[CMD_POSITION_LENGTH_LSB] << 0) + ((uint16_t) RxPacket[CMD_POSITION_LENGTH_MSB] << 8);

			*number_of_controlsP = 0;
			uint8_t* uartP = &RxPacket[CMD_POSITION_DATA+1];
			ProteusIII_GPIOControlBlock_t* controlP_running = controlP;
			while(uartP < &RxPacket[CMD_POSITION_DATA+length])
			{
			    /* each ControlBlock starts with length field which is currently fixed to "2" */
				if(*uartP == 2)
				{
					controlP_running->GPIO_ID = *(uartP + 1);
					controlP_running->value.output = *(uartP + 2);

                    /* Move pointer to next element. configP is increased by sizeof(ProteusIII_GPIOControlBlock_t)*/
					controlP_running++;
					*number_of_controlsP += 1;
				}

				/* uartP points to length field of control block. So increase address by value of length + 1 */
				uartP += *uartP + 1;
			}
		}
    }

    return ret;
}

/**
 * @brief Configure the remote GPIO of the module
 *
 * @param[in] configP: pointer to one or more pin configurations
 * @param[in] number_of_configs: number of entries in configP array
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ProteusIII_GPIORemoteWriteConfig(ProteusIII_GPIOConfigBlock_t* configP, uint16_t number_of_configs)
{
    bool ret = false;
	uint16_t length = 0;

	for (uint16_t i=0; i < number_of_configs; i++)
	{
		switch(configP->function)
		{
			case ProteusIII_GPIO_IO_Disconnected:
			{
				CMD_Array[CMD_POSITION_DATA + length] = 3;
				CMD_Array[CMD_POSITION_DATA + length + 1] = configP->GPIO_ID;
				CMD_Array[CMD_POSITION_DATA + length + 2] = configP->function;
				CMD_Array[CMD_POSITION_DATA + length + 3] = 0x00;
				length += 4;
			}
			break;
			case ProteusIII_GPIO_IO_Input:
			{
				CMD_Array[CMD_POSITION_DATA + length] = 3;
				CMD_Array[CMD_POSITION_DATA + length + 1] = configP->GPIO_ID;
				CMD_Array[CMD_POSITION_DATA + length + 2] = configP->function;
				CMD_Array[CMD_POSITION_DATA + length + 3] = configP->value.input;
				length += 4;
			}
			break;
			case ProteusIII_GPIO_IO_Output:
			{
				CMD_Array[CMD_POSITION_DATA + length] = 3;
				CMD_Array[CMD_POSITION_DATA + length + 1] = configP->GPIO_ID;
				CMD_Array[CMD_POSITION_DATA + length + 2] = configP->function;
				CMD_Array[CMD_POSITION_DATA + length + 3] = configP->value.output;
				length += 4;
				}
			break;
			case ProteusIII_GPIO_IO_PWM:
			{
				CMD_Array[CMD_POSITION_DATA + length] = 5;
				CMD_Array[CMD_POSITION_DATA + length + 1] = configP->GPIO_ID;
				CMD_Array[CMD_POSITION_DATA + length + 2] = configP->function;
				memcpy(&CMD_Array[CMD_POSITION_DATA + length + 3], &configP->value.pwm.period, 2);
				CMD_Array[CMD_POSITION_DATA + length + 5] = configP->value.pwm.ratio;
				length += 6;
			}
			break;
			default:
			{
			}
			break;
		}
		configP++;
	}

    CMD_Array[CMD_POSITION_STX] = CMD_STX;
    CMD_Array[CMD_POSITION_CMD] = ProteusIII_CMD_GPIO_REMOTE_WRITECONFIG_REQ;
    CMD_Array[CMD_POSITION_LENGTH_LSB]= (length & 0x00FF);
    CMD_Array[CMD_POSITION_LENGTH_MSB]= (length & 0xFF00) >> 8;

    if (FillChecksum(CMD_Array, CMD_ARRAY_SIZE()))
    {
        /* now send CMD_ARRAY */
        WE_UART_Transmit(CMD_Array, CMD_ARRAY_SIZE());

        /* wait for cnf */
        ret = Wait4CNF(CMD_WAIT_TIME, ProteusIII_CMD_GPIO_REMOTE_WRITECONFIG_CNF, CMD_Status_Success, true);
    }

    return ret;
}

/**
 * @brief Read the remote GPIO configuration of the module
 *
 * @param[out] configP: pointer to one or more pin configurations
 * @param[out] number_of_configsP: pointer to number of entries in configP array
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ProteusIII_GPIORemoteReadConfig(ProteusIII_GPIOConfigBlock_t* configP, uint16_t *number_of_configsP)
{
    bool ret = false;

    CMD_Array[CMD_POSITION_STX] = CMD_STX;
    CMD_Array[CMD_POSITION_CMD] = ProteusIII_CMD_GPIO_REMOTE_READCONFIG_REQ;
    CMD_Array[CMD_POSITION_LENGTH_LSB]= 0;
    CMD_Array[CMD_POSITION_LENGTH_MSB]= 0;

    if (FillChecksum(CMD_Array, CMD_ARRAY_SIZE()))
    {
        /* now send CMD_ARRAY */
        WE_UART_Transmit(CMD_Array, CMD_ARRAY_SIZE());

        /* wait for cnf */
        ret = Wait4CNF(CMD_WAIT_TIME, ProteusIII_CMD_GPIO_REMOTE_READCONFIG_CNF, CMD_Status_Success, true);

		if(ret == true)
		{
			uint16_t length = ((uint16_t) RxPacket[CMD_POSITION_LENGTH_LSB] << 0) + ((uint16_t) RxPacket[CMD_POSITION_LENGTH_MSB] << 8);

			*number_of_configsP = 0;
			uint8_t* uartP = &RxPacket[CMD_POSITION_DATA+1];
			ProteusIII_GPIOConfigBlock_t* configP_running = configP;
			while(uartP < &RxPacket[CMD_POSITION_DATA+length])
			{
				switch(*(uartP + 2))
				{
					case ProteusIII_GPIO_IO_Disconnected:
					{
						if(*uartP == 3)
						{
							configP_running->GPIO_ID = *(uartP + 1);
							configP_running->function = *(uartP + 2);

							configP_running++;
							*number_of_configsP += 1;
						}
					}
					break;
					case ProteusIII_GPIO_IO_Input:
					{
						if(*uartP == 3)
						{
							configP_running->GPIO_ID = *(uartP + 1);
							configP_running->function = *(uartP + 2);
							configP_running->value.input = *(uartP + 3);

							configP_running++;
							*number_of_configsP += 1;
						}
					}
					break;
					case ProteusIII_GPIO_IO_Output:
					{
						if(*uartP == 3)
						{
							configP_running->GPIO_ID = *(uartP + 1);
							configP_running->function = *(uartP + 2);
							configP_running->value.output = *(uartP + 3);

							configP_running++;
							*number_of_configsP += 1;
						}
					}
					break;
					case ProteusIII_GPIO_IO_PWM:
					{
						if(*uartP == 5)
						{
							configP_running->GPIO_ID = *(uartP + 1);
							configP_running->function = *(uartP + 2);
							memcpy(&configP_running->value.pwm.period, (uartP + 3), 2);
							configP_running->value.pwm.ratio = *(uartP + 5);

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
 * See ProteusIII_GPIORemoteWriteConfig
 *
 * @param[in] controlP: pointer to one or more pin controls
 * @param[in] number_of_controls: number of entries in controlP array
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ProteusIII_GPIORemoteWrite(ProteusIII_GPIOControlBlock_t* controlP, uint16_t number_of_controls)
{
    bool ret = false;
	uint16_t length = 0;

	for (uint16_t i=0; i < number_of_controls; i++)
	{
		CMD_Array[CMD_POSITION_DATA + length] = 2;
		CMD_Array[CMD_POSITION_DATA + length + 1] = controlP->GPIO_ID;
		CMD_Array[CMD_POSITION_DATA + length + 2] = controlP->value.ratio;
		length += 3;
		controlP++;
	}

    CMD_Array[CMD_POSITION_STX] = CMD_STX;
    CMD_Array[CMD_POSITION_CMD] = ProteusIII_CMD_GPIO_REMOTE_WRITE_REQ;
    CMD_Array[CMD_POSITION_LENGTH_LSB]= (length & 0x00FF);
    CMD_Array[CMD_POSITION_LENGTH_MSB]= (length & 0xFF00) >> 8;

    if (FillChecksum(CMD_Array, CMD_ARRAY_SIZE()))
    {
        /* now send CMD_ARRAY */
        WE_UART_Transmit(CMD_Array, CMD_ARRAY_SIZE());

        /* wait for cnf */
        ret = Wait4CNF(CMD_WAIT_TIME, ProteusIII_CMD_GPIO_REMOTE_WRITE_CNF, CMD_Status_Success, true);
    }

    return ret;
}

/**
 * @brief Read the input of the pins. Pin has to be configured first.
 * See ProteusIII_GPIORemoteWriteConfig
 *
 * @param[in] GPIOToReadP: One or more pins to read.
 * @param[in] amountGPIOToRead: amount of pins to read and therefore length of GPIOToReadP
 * @param[out] controlP: Pointer to controlBlock
 * @param[out] number_of_controlsP: pointer to number of entries in controlP array
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ProteusIII_GPIORemoteRead(uint8_t *GPIOToReadP, uint8_t amountGPIOToRead, ProteusIII_GPIOControlBlock_t* controlP, uint16_t* number_of_controlsP)
{
    bool ret = false;

    CMD_Array[CMD_POSITION_STX] = CMD_STX;
    CMD_Array[CMD_POSITION_CMD] = ProteusIII_CMD_GPIO_REMOTE_READ_REQ;
    CMD_Array[CMD_POSITION_LENGTH_LSB]= amountGPIOToRead + 1;
    CMD_Array[CMD_POSITION_LENGTH_MSB]= 0;
    CMD_Array[CMD_POSITION_DATA] = amountGPIOToRead;
    memcpy(&CMD_Array[CMD_POSITION_DATA + 1], GPIOToReadP, amountGPIOToRead);

    if (FillChecksum(CMD_Array, CMD_ARRAY_SIZE()))
    {
        /* now send CMD_ARRAY */
        WE_UART_Transmit(CMD_Array, CMD_ARRAY_SIZE());

        /* wait for cnf */
        ret = Wait4CNF(CMD_WAIT_TIME, ProteusIII_CMD_GPIO_REMOTE_READ_CNF, CMD_Status_Success, true);

		if(ret)
		{
			uint16_t length = ((uint16_t) RxPacket[CMD_POSITION_LENGTH_LSB] << 0) + ((uint16_t) RxPacket[CMD_POSITION_LENGTH_MSB] << 8);

			*number_of_controlsP = 0;
			uint8_t* uartP = &RxPacket[CMD_POSITION_DATA+1];
			ProteusIII_GPIOControlBlock_t* controlP_running = controlP;
			while(uartP < &RxPacket[CMD_POSITION_DATA+length])
			{
				if(*uartP == 2)
				{
					controlP_running->GPIO_ID = *(uartP + 1);
					controlP_running->value.ratio   = *(uartP + 2);

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
 * @brief Temporarily allow unbonded connections, in case only bonded connections have been configured
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ProteusIII_Allowunbondedconnections()
{
    bool ret = false;

    /* fill CMD_ARRAY packet */
    CMD_Array[CMD_POSITION_STX] = CMD_STX;
    CMD_Array[CMD_POSITION_CMD] = ProteusIII_CMD_ALLOWUNBONDEDCONNECTIONS_REQ;
    CMD_Array[CMD_POSITION_LENGTH_LSB] = (uint8_t)0;
    CMD_Array[CMD_POSITION_LENGTH_MSB] = (uint8_t)0;

    if (FillChecksum(CMD_Array, CMD_ARRAY_SIZE()))
    {
        /* now send CMD_ARRAY */
        WE_UART_Transmit(CMD_Array, CMD_ARRAY_SIZE());

        /* wait for cnf */
        ret = Wait4CNF(CMD_WAIT_TIME, ProteusIII_CMD_ALLOWUNBONDEDCONNECTIONS_CNF, CMD_Status_Success, true);
    }
    return ret;
}

