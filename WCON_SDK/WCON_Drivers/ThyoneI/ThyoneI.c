/**
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
 * COPYRIGHT (c) 2021 Würth Elektronik eiSos GmbH & Co. KG
 *
 ***************************************************************************************************
 **/

#include "string.h"

#include "ThyoneI.h"
#include "../global/global.h"

typedef struct ThyoneI_Pins_t {
	Pin_t reset;
	Pin_t sleep_wake_up;
	Pin_t boot;
	Pin_t mode;
} ThyoneI_Pins_t;

#define CMD_WAIT_TIME 1500
#define CNFINVALID 255

#define LENGTH_CMD_OVERHEAD             (uint16_t)5
#define LENGTH_CMD_OVERHEAD_WITHOUT_CRC (uint16_t)(LENGTH_CMD_OVERHEAD - 1)
#define MAX_PAYLOAD_LENGTH              (uint16_t)224
#define MAX_PAYLOAD_LENGTH_MULTICAST_EX (uint16_t)223
#define MAX_PAYLOAD_LENGTH_UNICAST_EX   (uint16_t)220
#define MAX_CMD_LENGTH                  (uint16_t)(MAX_PAYLOAD_LENGTH + LENGTH_CMD_OVERHEAD)

#define CMD_POSITION_STX        (uint8_t)0
#define CMD_POSITION_CMD        (uint8_t)1
#define CMD_POSITION_LENGTH_LSB (uint8_t)2
#define CMD_POSITION_LENGTH_MSB (uint8_t)3
#define CMD_POSITION_DATA       (uint8_t)4

#define CMD_STX 0x02

#define ThyoneI_CMD_TYPE_REQ (uint8_t)(0 << 6)
#define ThyoneI_CMD_TYPE_CNF (uint8_t)(1 << 6)
#define ThyoneI_CMD_TYPE_IND (uint8_t)(2 << 6)
#define ThyoneI_CMD_TYPE_RSP (uint8_t)(3 << 6)


#define ThyoneI_CMD_RESET (uint8_t)0x00
#define ThyoneI_CMD_RESET_REQ (ThyoneI_CMD_RESET | ThyoneI_CMD_TYPE_REQ)
#define ThyoneI_CMD_RESET_CNF (ThyoneI_CMD_RESET | ThyoneI_CMD_TYPE_CNF)

#define ThyoneI_CMD_GETSTATE (uint8_t)0x01
#define ThyoneI_CMD_GETSTATE_REQ (ThyoneI_CMD_GETSTATE | ThyoneI_CMD_TYPE_REQ)
#define ThyoneI_CMD_GETSTATE_CNF (ThyoneI_CMD_GETSTATE | ThyoneI_CMD_TYPE_CNF)

#define ThyoneI_CMD_SLEEP (uint8_t)0x02
#define ThyoneI_CMD_SLEEP_REQ (ThyoneI_CMD_SLEEP | ThyoneI_CMD_TYPE_REQ)
#define ThyoneI_CMD_SLEEP_CNF (ThyoneI_CMD_SLEEP | ThyoneI_CMD_TYPE_CNF)
#define ThyoneI_CMD_SLEEP_IND (ThyoneI_CMD_SLEEP | ThyoneI_CMD_TYPE_IND)

#define ThyoneI_CMD_START_IND (uint8_t)0x73

#define ThyoneI_CMD_UNICAST_DATA (uint8_t)0x04
#define ThyoneI_CMD_UNICAST_DATA_REQ (ThyoneI_CMD_UNICAST_DATA | ThyoneI_CMD_TYPE_REQ)

/* Transmissions of any kind will be confirmed and indicated by the same message CMD_DATA_CNF od CMD_DATA_IND*/
#define ThyoneI_CMD_DATA_CNF (ThyoneI_CMD_UNICAST_DATA | ThyoneI_CMD_TYPE_CNF)
#define ThyoneI_CMD_DATA_IND (ThyoneI_CMD_UNICAST_DATA | ThyoneI_CMD_TYPE_IND)
#define ThyoneI_CMD_TXCOMPLETE_RSP (ThyoneI_CMD_UNICAST_DATA | ThyoneI_CMD_TYPE_RSP)

#define ThyoneI_CMD_MULTICAST_DATA (uint8_t)0x05
#define ThyoneI_CMD_MULTICAST_DATA_REQ (ThyoneI_CMD_MULTICAST_DATA | ThyoneI_CMD_TYPE_REQ)

#define ThyoneI_CMD_BROADCAST_DATA (uint8_t)0x06
#define ThyoneI_CMD_BROADCAST_DATA_REQ (ThyoneI_CMD_BROADCAST_DATA | ThyoneI_CMD_TYPE_REQ)

#define ThyoneI_CMD_UNICAST_DATA_EX (uint8_t)0x07
#define ThyoneI_CMD_UNICAST_DATA_EX_REQ (ThyoneI_CMD_UNICAST_DATA_EX | ThyoneI_CMD_TYPE_REQ)

#define ThyoneI_CMD_MULTICAST_DATA_EX (uint8_t)0x08
#define ThyoneI_CMD_MULTICAST_DATA_EX_REQ (ThyoneI_CMD_MULTICAST_DATA_EX | ThyoneI_CMD_TYPE_REQ)

#define ThyoneI_CMD_SETCHANNEL (uint8_t)0x09
#define ThyoneI_CMD_SETCHANNEL_REQ (ThyoneI_CMD_SETCHANNEL | ThyoneI_CMD_TYPE_REQ)
#define ThyoneI_CMD_SETCHANNEL_CNF (ThyoneI_CMD_SETCHANNEL | ThyoneI_CMD_TYPE_CNF)

#define ThyoneI_CMD_GET (uint8_t)0x10
#define ThyoneI_CMD_GET_REQ (ThyoneI_CMD_GET | ThyoneI_CMD_TYPE_REQ)
#define ThyoneI_CMD_GET_CNF (ThyoneI_CMD_GET | ThyoneI_CMD_TYPE_CNF)

#define ThyoneI_CMD_SET (uint8_t)0x11
#define ThyoneI_CMD_SET_REQ (ThyoneI_CMD_SET | ThyoneI_CMD_TYPE_REQ)
#define ThyoneI_CMD_SET_CNF (ThyoneI_CMD_SET | ThyoneI_CMD_TYPE_CNF)

#define ThyoneI_CMD_FACTORYRESET (uint8_t)0x1C
#define ThyoneI_CMD_FACTORYRESET_REQ (ThyoneI_CMD_FACTORYRESET | ThyoneI_CMD_TYPE_REQ)
#define ThyoneI_CMD_FACTORYRESET_CNF (ThyoneI_CMD_FACTORYRESET | ThyoneI_CMD_TYPE_CNF)

#define ThyoneI_CMD_GPIO_LOCAL_SETCONFIG (uint8_t)0x25
#define ThyoneI_CMD_GPIO_LOCAL_SETCONFIG_REQ (ThyoneI_CMD_GPIO_LOCAL_SETCONFIG | ThyoneI_CMD_TYPE_REQ)
#define ThyoneI_CMD_GPIO_LOCAL_SETCONFIG_CNF (ThyoneI_CMD_GPIO_LOCAL_SETCONFIG | ThyoneI_CMD_TYPE_CNF)

#define ThyoneI_CMD_GPIO_LOCAL_GETCONFIG (uint8_t)0x26
#define ThyoneI_CMD_GPIO_LOCAL_GETCONFIG_REQ (ThyoneI_CMD_GPIO_LOCAL_GETCONFIG | ThyoneI_CMD_TYPE_REQ)
#define ThyoneI_CMD_GPIO_LOCAL_GETCONFIG_CNF (ThyoneI_CMD_GPIO_LOCAL_GETCONFIG | ThyoneI_CMD_TYPE_CNF)

#define ThyoneI_CMD_GPIO_LOCAL_WRITE (uint8_t)0x27
#define ThyoneI_CMD_GPIO_LOCAL_WRITE_REQ (ThyoneI_CMD_GPIO_LOCAL_WRITE | ThyoneI_CMD_TYPE_REQ)
#define ThyoneI_CMD_GPIO_LOCAL_WRITE_CNF (ThyoneI_CMD_GPIO_LOCAL_WRITE | ThyoneI_CMD_TYPE_CNF)

#define ThyoneI_CMD_GPIO_LOCAL_READ (uint8_t)0x28
#define ThyoneI_CMD_GPIO_LOCAL_READ_REQ (ThyoneI_CMD_GPIO_LOCAL_READ | ThyoneI_CMD_TYPE_REQ)
#define ThyoneI_CMD_GPIO_LOCAL_READ_CNF (ThyoneI_CMD_GPIO_LOCAL_READ | ThyoneI_CMD_TYPE_CNF)

#define ThyoneI_CMD_GPIO_REMOTE_SETCONFIG (uint8_t)0x29
#define ThyoneI_CMD_GPIO_REMOTE_SETCONFIG_REQ (ThyoneI_CMD_GPIO_REMOTE_SETCONFIG | ThyoneI_CMD_TYPE_REQ)
#define ThyoneI_CMD_GPIO_REMOTE_SETCONFIG_CNF (ThyoneI_CMD_GPIO_REMOTE_SETCONFIG | ThyoneI_CMD_TYPE_CNF)

#define ThyoneI_CMD_GPIO_REMOTE_GETCONFIG (uint8_t)0x2A
#define ThyoneI_CMD_GPIO_REMOTE_GETCONFIG_REQ (ThyoneI_CMD_GPIO_REMOTE_GETCONFIG | ThyoneI_CMD_TYPE_REQ)
#define ThyoneI_CMD_GPIO_REMOTE_GETCONFIG_CNF (ThyoneI_CMD_GPIO_REMOTE_GETCONFIG | ThyoneI_CMD_TYPE_CNF)
#define ThyoneI_CMD_GPIO_REMOTE_GETCONFIG_RSP (ThyoneI_CMD_GPIO_REMOTE_GETCONFIG | ThyoneI_CMD_TYPE_RSP)

#define ThyoneI_CMD_GPIO_REMOTE_WRITE (uint8_t)0x2B
#define ThyoneI_CMD_GPIO_REMOTE_WRITE_REQ (ThyoneI_CMD_GPIO_REMOTE_WRITE | ThyoneI_CMD_TYPE_REQ)
#define ThyoneI_CMD_GPIO_REMOTE_WRITE_CNF (ThyoneI_CMD_GPIO_REMOTE_WRITE | ThyoneI_CMD_TYPE_CNF)

#define ThyoneI_CMD_GPIO_REMOTE_READ (uint8_t)0x2C
#define ThyoneI_CMD_GPIO_REMOTE_READ_REQ (ThyoneI_CMD_GPIO_REMOTE_READ | ThyoneI_CMD_TYPE_REQ)
#define ThyoneI_CMD_GPIO_REMOTE_READ_CNF (ThyoneI_CMD_GPIO_REMOTE_READ | ThyoneI_CMD_TYPE_CNF)
#define ThyoneI_CMD_GPIO_REMOTE_READ_RSP (ThyoneI_CMD_GPIO_REMOTE_READ | ThyoneI_CMD_TYPE_RSP)

#define CMD_ARRAY_SIZE() ((((uint16_t)CMD_Array[CMD_POSITION_LENGTH_LSB] << 0) | ((uint16_t)CMD_Array[CMD_POSITION_LENGTH_MSB] << 8)) + LENGTH_CMD_OVERHEAD)


/* type used to check the response, when a command was sent to the ThyoneI */
typedef enum CMD_Status_t
{
    CMD_Status_Success = (uint8_t)0x00,
    CMD_Status_Failed  = (uint8_t)0x01,
    CMD_Status_Invalid,
    CMD_Status_Reset,
    CMD_Status_NoStatus,
} CMD_Status_t;

typedef struct
{
    uint8_t cmd; /* variable to check if correct CMD has been confirmed */
    CMD_Status_t status; /* variable used to check the response (*_CNF), when a request (*_REQ) was sent to the ThyoneI */
} CMD_Confirmation_t;

/**************************************
 *          Static variables          *
 **************************************/
static uint8_t CMD_Array[MAX_CMD_LENGTH]; /* for UART TX to module*/
static uint8_t RxPacket[MAX_CMD_LENGTH];

#define CMDCONFIRMATIONARRAY_LENGTH 3
static CMD_Confirmation_t cmdConfirmation_array[CMDCONFIRMATIONARRAY_LENGTH];
static ThyoneI_Pins_t pins;
static uint8_t checksum = 0;
static uint16_t RxByteCounter = 0;
static uint16_t BytesToReceive = 0;
static uint8_t RxBuffer[MAX_CMD_LENGTH]; /* For UART RX from module */
void(*RXcallback)(uint8_t*,uint16_t,uint32_t,int8_t);       /* callback function */

/**************************************
 *         Static functions           *
 **************************************/

static void HandleRxPacket(uint8_t * pRxBuffer)
{
    CMD_Confirmation_t cmdConfirmation;
    cmdConfirmation.cmd = CNFINVALID;
    cmdConfirmation.status = CMD_Status_Invalid;

    uint16_t cmd_length = (uint16_t)(pRxBuffer[CMD_POSITION_LENGTH_LSB]+(pRxBuffer[CMD_POSITION_LENGTH_MSB]<<8));
    memcpy(&RxPacket[0], pRxBuffer, cmd_length + LENGTH_CMD_OVERHEAD);

    switch (RxPacket[CMD_POSITION_CMD])
    {
        case ThyoneI_CMD_RESET_CNF:
        case ThyoneI_CMD_GETSTATE_CNF:
        case ThyoneI_CMD_START_IND:
        case ThyoneI_CMD_GPIO_REMOTE_GETCONFIG_RSP:
        case ThyoneI_CMD_GPIO_REMOTE_READ_RSP:
        {
            cmdConfirmation.cmd = RxPacket[CMD_POSITION_CMD];
            cmdConfirmation.status = CMD_Status_NoStatus;
            break;
        }

        case ThyoneI_CMD_DATA_CNF:
        case ThyoneI_CMD_GET_CNF:
        case ThyoneI_CMD_SET_CNF:
        case ThyoneI_CMD_FACTORYRESET_CNF:
        case ThyoneI_CMD_SLEEP_CNF:
        case ThyoneI_CMD_GPIO_LOCAL_SETCONFIG_CNF:
        case ThyoneI_CMD_GPIO_LOCAL_GETCONFIG_CNF:
        case ThyoneI_CMD_GPIO_LOCAL_WRITE_CNF:
        case ThyoneI_CMD_GPIO_LOCAL_READ_CNF:
        case ThyoneI_CMD_GPIO_REMOTE_SETCONFIG_CNF:
        case ThyoneI_CMD_GPIO_REMOTE_GETCONFIG_CNF:
        case ThyoneI_CMD_GPIO_REMOTE_WRITE_CNF:
        case ThyoneI_CMD_TXCOMPLETE_RSP:
        {
            cmdConfirmation.cmd = RxPacket[CMD_POSITION_CMD];
            cmdConfirmation.status = RxPacket[CMD_POSITION_DATA];
            break;
        }

        case ThyoneI_CMD_GPIO_REMOTE_READ_CNF:
        {
            cmdConfirmation.cmd = RxPacket[CMD_POSITION_CMD];
            cmdConfirmation.status = CMD_Status_Invalid;

            break;
        }


        case ThyoneI_CMD_DATA_IND:
        {
			uint16_t payload_length = ((((uint16_t) RxPacket[CMD_POSITION_LENGTH_LSB] << 0) | ((uint16_t) RxPacket[CMD_POSITION_LENGTH_MSB] << 8))) - 5;

			uint32_t sourceAddress = (uint32_t)RxPacket[CMD_POSITION_DATA];
			sourceAddress = (sourceAddress << 8) + (uint32_t)RxPacket[CMD_POSITION_DATA+1];
			sourceAddress = (sourceAddress << 8) + (uint32_t)RxPacket[CMD_POSITION_DATA+2];
			sourceAddress = (sourceAddress << 8) + (uint32_t)RxPacket[CMD_POSITION_DATA+3];

			if(RXcallback != NULL)
			{
				RXcallback(&RxPacket[CMD_POSITION_DATA + 5], payload_length, sourceAddress, RxPacket[CMD_POSITION_DATA + 4]);
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

/* function that waits for the return value of ThyoneI (*_CNF), when a command (*_REQ) was sent before */
static bool Wait4CNF(int max_time_ms, uint8_t expectedCmdConfirmation, CMD_Status_t expectedStatus, bool reset_confirmstate)
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
        delay(time_step_ms);
    }
    return true;
}

/* function to add the checksum at the end of the data packet */
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

static bool InitPins(ThyoneI_Pins_t pins)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOA_CLK_ENABLE();

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(GPIOA, pins.boot.pin|pins.mode.pin|pins.sleep_wake_up.pin
					  |pins.reset.pin, GPIO_PIN_RESET);

	GPIO_InitStruct.Pin = pins.boot.pin|pins.mode.pin|pins.sleep_wake_up.pin
			  |pins.reset.pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    return true;
}

/**************************************
 *         Global functions           *
 **************************************/

void UART_HandleRxByte(uint8_t received_byte)
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


/*
 *Initialize the ThyoneI interface for serial interface
 *
 *input:
 * -baudrate:       baudrate of the interface
 * -flow_control:   enable/disable flowcontrol
 * -RXcb:           RX callback function
 *
 *Caution: the parameter baudrate must match the configured UserSettings of the ThyoneI
 *         -the baudrate parameter must match to perform a successful FTDI communication
 *          *updating this parameter during runtime may lead to communication errors
 *
 *return true if initialization succeeded
 *       false otherwise
 */
bool ThyoneI_Init(uint32_t baudrate, FlowControl_t flow_control, void(*RXcb)(uint8_t*,uint16_t,uint32_t,int8_t))
{
	/* set RX callback function */
	RXcallback = RXcb;

	/* initialize the pins */
	pins.reset.port = GPIOA;
	pins.reset.pin = GPIO_PIN_10;
	pins.sleep_wake_up.port = GPIOA;
	pins.sleep_wake_up.pin = GPIO_PIN_9;
	pins.boot.port = GPIOA;
	pins.boot.pin = GPIO_PIN_7;
	pins.mode.port = GPIOA;
	pins.mode.pin = GPIO_PIN_8;	
	if (false == InitPins(pins))
	{
		/* error */
		return false ;
	}
	SetPin(pins.boot,  SetPin_Out_High);
	SetPin(pins.sleep_wake_up, SetPin_Out_High);
	SetPin(pins.mode, SetPin_Out_Low);
	SetPin(pins.reset, SetPin_Out_High);
	
	UART_Init(baudrate, flow_control);
	delay(10);

	/* reset module*/
	if(ThyoneI_PinReset())
	{
		delay(ThyoneI_BOOT_DURATION);
	}
	else
	{
		fprintf(stdout, "Pin Reset failed\n");
		ThyoneI_Deinit();
		return false;
	}

	uint8_t driver_version[3];
	if(GetDriverVersion(driver_version))
	{
		fprintf (stdout, COLOR_CYAN "ThyoneI driver version %d.%d.%d\n" COLOR_RESET,driver_version[0],driver_version[1],driver_version[2]);
	}
	delay(100);

	return true;
}

/*
 *Deinitialize the ThyoneI interface
 *
 *return true if deinitialization succeeded
 *       false otherwise
 */
bool ThyoneI_Deinit()
{
    /* close the communication interface to the module */
	UART_DeInit();

    /* deinit pins */
    DeinitPin(pins.reset);
	DeinitPin(pins.sleep_wake_up);
	DeinitPin(pins.boot);
	DeinitPin(pins.mode);

	RXcallback = NULL;

    return true;
}

/*
 *Wakeup the ThyoneI from sleep by pin
 *
 *return true if wakeup succeeded
 *       false otherwise
 */
bool ThyoneI_PinWakeup()
{
    int i = 0;

    SetPin(pins.sleep_wake_up, SetPin_Out_Low);
    delay (5);
    for(i=0; i<CMDCONFIRMATIONARRAY_LENGTH; i++)
    {
        cmdConfirmation_array[i].status = CMD_Status_Invalid;
        cmdConfirmation_array[i].cmd = CNFINVALID;
    }
    SetPin(pins.sleep_wake_up, SetPin_Out_High);

    /* wait for cnf */
    return Wait4CNF(CMD_WAIT_TIME, ThyoneI_CMD_START_IND, CMD_Status_NoStatus, false);
}

/*
 *Reset the ThyoneI by pin
 *
 *return true if reset succeeded
 *       false otherwise
 */
bool ThyoneI_PinReset()
{
    SetPin(pins.reset, SetPin_Out_Low);
    delay(5);
    SetPin(pins.reset, SetPin_Out_High);

    /* wait for cnf */
    return Wait4CNF(CMD_WAIT_TIME, ThyoneI_CMD_START_IND, CMD_Status_NoStatus, true);
}

/*
 *Reset the ThyoneI by command
 *
 *return true if reset succeeded
 *       false otherwise
 */
bool ThyoneI_Reset()
{
    bool ret = false;

    /* fill CMD_ARRAY packet */
    CMD_Array[CMD_POSITION_STX] = CMD_STX;
    CMD_Array[CMD_POSITION_CMD] = ThyoneI_CMD_RESET_REQ;
    CMD_Array[CMD_POSITION_LENGTH_LSB] = (uint8_t)0;
    CMD_Array[CMD_POSITION_LENGTH_MSB] = (uint8_t)0;

    if (FillChecksum(CMD_Array, CMD_ARRAY_SIZE()))
    {
        /* now send CMD_ARRAY */
        UART_Transmit(CMD_Array, CMD_ARRAY_SIZE());

        /* wait for cnf */
        return Wait4CNF(CMD_WAIT_TIME, ThyoneI_CMD_START_IND, CMD_Status_NoStatus, true);
    }
    return ret;
}

/*
 *Put the ThyoneI into sleep mode
 *
 *return true if succeeded
 *       false otherwise
 */
bool
ThyoneI_Sleep()
{
    bool ret = false;
    /* fill CMD_ARRAY packet */
    CMD_Array[CMD_POSITION_STX] = CMD_STX;
    CMD_Array[CMD_POSITION_CMD] = ThyoneI_CMD_SLEEP_REQ;
    CMD_Array[CMD_POSITION_LENGTH_LSB] = (uint8_t)0;
    CMD_Array[CMD_POSITION_LENGTH_MSB] = (uint8_t)0;

    if (FillChecksum(CMD_Array, CMD_ARRAY_SIZE()))
    {
        /* now send CMD_ARRAY */
        UART_Transmit( CMD_Array, CMD_ARRAY_SIZE());

        /* wait for cnf */
        ret = Wait4CNF(CMD_WAIT_TIME, ThyoneI_CMD_SLEEP_CNF, CMD_Status_Success, true);
    }
    return ret;
}

/*
 *Transmit data as broadcast
 *
 *input:
 * -PayloadP: pointer to the data to transmit
 * -length:   length of the data to transmit
 *
 *return true if succeeded
 *       false otherwise
 */
bool
ThyoneI_TransmitBroadcast(uint8_t *payloadP, uint16_t length)
{
    bool ret = false;
    if (length <= MAX_PAYLOAD_LENGTH)
    {
        CMD_Array[CMD_POSITION_STX] = CMD_STX;
        CMD_Array[CMD_POSITION_CMD] = ThyoneI_CMD_BROADCAST_DATA_REQ;
        CMD_Array[CMD_POSITION_LENGTH_LSB] = (uint8_t)(length >> 0);
        CMD_Array[CMD_POSITION_LENGTH_MSB] = (uint8_t)(length >> 8);

        memcpy(&CMD_Array[CMD_POSITION_DATA], payloadP, length);

        if (FillChecksum(CMD_Array, CMD_ARRAY_SIZE()))
        {
            UART_Transmit( CMD_Array, CMD_ARRAY_SIZE());
            ret = Wait4CNF(CMD_WAIT_TIME, ThyoneI_CMD_TXCOMPLETE_RSP, CMD_Status_Success, true);
        }
    }

    return ret;
}

/*
 *Transmit data as multicast
 *
 *input:
 * -PayloadP: pointer to the data to transmit
 * -length:   length of the data to transmit
 *
 *return true if succeeded
 *       false otherwise
 */
bool
ThyoneI_TransmitMulticast(uint8_t *payloadP, uint16_t length)
{
    bool ret = false;
    if (length <= MAX_PAYLOAD_LENGTH)
    {
        CMD_Array[CMD_POSITION_STX] = CMD_STX;
        CMD_Array[CMD_POSITION_CMD] = ThyoneI_CMD_MULTICAST_DATA_REQ;
        CMD_Array[CMD_POSITION_LENGTH_LSB] = (uint8_t) (length >> 0);
        CMD_Array[CMD_POSITION_LENGTH_MSB] = (uint8_t) (length >> 8);

        memcpy(&CMD_Array[CMD_POSITION_DATA], payloadP, length);

        if (FillChecksum(CMD_Array, CMD_ARRAY_SIZE()))
        {
            UART_Transmit( CMD_Array, CMD_ARRAY_SIZE());
            ret = Wait4CNF(CMD_WAIT_TIME, ThyoneI_CMD_TXCOMPLETE_RSP, CMD_Status_Success, true);
        }
    }

    return ret;
}

/*
 *Transmit data as unicast
 *
 *input:
 * -PayloadP: pointer to the data to transmit
 * -length:   length of the data to transmit
 *
 *return true if succeeded
 *       false otherwise
 */
bool
ThyoneI_TransmitUnicast(uint8_t *payloadP, uint16_t length)
{
    bool ret = false;
    if (length <= MAX_PAYLOAD_LENGTH)
    {
        CMD_Array[CMD_POSITION_STX] = CMD_STX;
        CMD_Array[CMD_POSITION_CMD] = ThyoneI_CMD_UNICAST_DATA_REQ;
        CMD_Array[CMD_POSITION_LENGTH_LSB] = (uint8_t) (length >> 0);
        CMD_Array[CMD_POSITION_LENGTH_MSB] = (uint8_t) (length >> 8);

        memcpy(&CMD_Array[CMD_POSITION_DATA], payloadP, length);

        if (FillChecksum(CMD_Array, CMD_ARRAY_SIZE()))
        {
            UART_Transmit( CMD_Array, CMD_ARRAY_SIZE());
            ret = Wait4CNF(CMD_WAIT_TIME, ThyoneI_CMD_TXCOMPLETE_RSP, CMD_Status_Success, true);
        }
    }

    return ret;
}

/*
 *Transmit data as multicast
 *
 *input:
 * -groupID : groupID to multicast
 * -PayloadP: pointer to the data to transmit
 * -length:   length of the data to transmit
 *
 *return true if succeeded
 *       false otherwise
 */
bool
ThyoneI_TransmitMulticastExtended(uint8_t groupID, uint8_t *payloadP, uint16_t length)
{
    bool ret = false;
    if (length <= MAX_PAYLOAD_LENGTH_MULTICAST_EX)
    {
        uint16_t cmdLength = length + 1;
        CMD_Array[CMD_POSITION_STX] = CMD_STX;
        CMD_Array[CMD_POSITION_CMD] = ThyoneI_CMD_MULTICAST_DATA_EX_REQ;
        CMD_Array[CMD_POSITION_LENGTH_LSB] = (uint8_t) (cmdLength >> 0);
        CMD_Array[CMD_POSITION_LENGTH_MSB] = (uint8_t) (cmdLength >> 8);
        CMD_Array[CMD_POSITION_CMD] = groupID;

        memcpy(&CMD_Array[CMD_POSITION_DATA+1], payloadP, length);

        if (FillChecksum(CMD_Array, CMD_ARRAY_SIZE()))
        {
            UART_Transmit( CMD_Array, CMD_ARRAY_SIZE());
            ret = Wait4CNF(CMD_WAIT_TIME, ThyoneI_CMD_TXCOMPLETE_RSP, CMD_Status_Success, true);
        }
    }

    return ret;
}

/*
 *Transmit data as unicast
 *
 *input:
 * -address: address to sent to
 * -payloadP: pointer to the data to transmit
 * -length:   length of the data to transmit
 *
 *return true if succeeded
 *       false otherwise
 */
bool
ThyoneI_TransmitUnicastExtended(uint32_t address, uint8_t *payloadP, uint16_t length)
{
    bool ret = false;
    if (length <= MAX_PAYLOAD_LENGTH_UNICAST_EX)
    {
        uint16_t cmdLength = length + 4;
        CMD_Array[CMD_POSITION_STX] = CMD_STX;
        CMD_Array[CMD_POSITION_CMD] = ThyoneI_CMD_UNICAST_DATA_EX_REQ;
        CMD_Array[CMD_POSITION_LENGTH_LSB] = (uint8_t) (cmdLength >> 0);
        CMD_Array[CMD_POSITION_LENGTH_MSB] = (uint8_t) (cmdLength >> 8);

        memcpy(&CMD_Array[CMD_POSITION_DATA], &address, 4);
        memcpy(&CMD_Array[CMD_POSITION_DATA+4], payloadP, length);

        if (FillChecksum(CMD_Array, CMD_ARRAY_SIZE()))
        {
            UART_Transmit( CMD_Array, CMD_ARRAY_SIZE());
            ret = Wait4CNF(CMD_WAIT_TIME, ThyoneI_CMD_TXCOMPLETE_RSP, CMD_Status_Success, true);
        }
    }

    return ret;
}

/*
 *Factory reset the module
 *
 *return true if succeeded
 *       false otherwise
 */
bool
ThyoneI_FactoryReset()
{
    bool ret = false;
    /* fill CMD_ARRAY packet */
    CMD_Array[CMD_POSITION_STX] = CMD_STX;
    CMD_Array[CMD_POSITION_CMD] = ThyoneI_CMD_FACTORYRESET_REQ;
    CMD_Array[CMD_POSITION_LENGTH_LSB] = (uint8_t)0;
    CMD_Array[CMD_POSITION_LENGTH_MSB] = (uint8_t)0;

    if (FillChecksum(CMD_Array, CMD_ARRAY_SIZE()))
    {
        /* now send CMD_ARRAY */
        UART_Transmit(CMD_Array, CMD_ARRAY_SIZE());

        /* wait for reset after factory reset */
        return Wait4CNF(CMD_WAIT_TIME, ThyoneI_CMD_START_IND, CMD_Status_NoStatus, true);
    }
    return ret;

}

/*
 *Set a special user setting
 *
 *input:
 * -us:     user setting to be updated
 * -value:  pointer to the new settings value
 * -length: length of the value
 *
 *note: reset the module after the adaption of the setting such that it can take effect
 *note: use this function only in rare case, since flash can be updated only a limited number times
 *
 *return true if request succeeded
 *       false otherwise
 */
bool
ThyoneI_Set(ThyoneI_UserSettings_t userSetting, uint8_t *ValueP, uint8_t length)
{
    bool ret = false;

    /* fill CMD_ARRAY packet */
    CMD_Array[CMD_POSITION_STX] = CMD_STX;
    CMD_Array[CMD_POSITION_CMD] = ThyoneI_CMD_SET_REQ;
    CMD_Array[CMD_POSITION_LENGTH_LSB] = (uint8_t) (1 + length);
    CMD_Array[CMD_POSITION_LENGTH_MSB] = (uint8_t)0;
    CMD_Array[CMD_POSITION_DATA] = userSetting;
    memcpy(&CMD_Array[CMD_POSITION_DATA + 1], ValueP, length);

    if (FillChecksum(CMD_Array, CMD_ARRAY_SIZE()))
    {
        /* now send CMD_ARRAY */
        UART_Transmit(CMD_Array, CMD_ARRAY_SIZE());

        /* wait for cnf */
        return Wait4CNF(CMD_WAIT_TIME, ThyoneI_CMD_SET_CNF, CMD_Status_Success, true);
    }
    return ret;
}


/*
 *Set the tx power
 *
 *input:
 * -txPower: transmit power
 *
 *return true if request succeeded
 *       false otherwise
 */
bool
ThyoneI_SetTXPower(ThyoneI_TXPower_t txPower)
{
    return ThyoneI_Set(ThyoneI_USERSETTING_INDEX_RF_TX_POWER, (uint8_t*)&txPower, 1);
}

/*
 *Set the UART baudrate index
 *
 *input:
 * -baudrateindex: UART baudrate index
 *
 *note: reset the module after the adaption of the setting such that it can take effect
 *note: use this function only in rare case, since flash can be updated only a limited number times
 *
 *return true if request succeeded
 *       false otherwise
 */
bool ThyoneI_SetBaudrateIndex(ThyoneI_BaudRateIndex_t baudrate, ThyoneI_UartParity_t parity, bool flowcontrolEnable)
{
    uint8_t baudrateIndex = (uint8_t)baudrate;

    /* If flowcontrol is to be enabled UART index has to be increased by one in regard to base value */
    if(flowcontrolEnable)
    {
        baudrateIndex++;
    }

    /* If parity bit is even, UART index has to be increased by 64 in regard of base value*/
    if(ThyoneI_UartParity_Even == parity)
    {
        baudrateIndex += 64;
    }

    return ThyoneI_Set(ThyoneI_USERSETTING_INDEX_UART_CONFIG, (uint8_t*)&baudrateIndex, 1);
}

/*
 *Set the RF Channel
 *
 *input:
 * -channel: channel
 *
 *return true if request succeeded
 *       false otherwise
 */
bool ThyoneI_SetRFChannel(uint8_t channel)
{
    /* permissible value for channel: 0-38*/
    if(channel < 38)
    {
        return ThyoneI_Set(ThyoneI_USERSETTING_INDEX_RF_CHANNEL, (uint8_t*)&channel, 1);
    }
    else
    {
        return false;
    }
}

/*
 *Set the encryption mode
 *
 *input:
 * -encryptionMode: encryptionMode
 *
 *return true if request succeeded
 *       false otherwise
 */
bool
ThyoneI_SetEncryptionMode(ThyoneI_EncryptionMode_t encryptionMode)
{
    return ThyoneI_Set(ThyoneI_USERSETTING_INDEX_ENCRYPTION_MODE, (uint8_t*)&encryptionMode, 1);
}

/*
 *Set the rf profile
 *
 *input:
 * -profile: rf profile
 *
 *return true if request succeeded
 *       false otherwise
 */
bool ThyoneI_SetRfProfile(ThyoneI_Profile_t profile)
{
    return ThyoneI_Set(ThyoneI_USERSETTING_INDEX_RF_PROFILE, (uint8_t*)&profile, 1);
}

/*
 *Set the number of retries
 *
 *input:
 * -numRetries: number of retries
 *
 *return true if request succeeded
 *       false otherwise
 */
bool ThyoneI_SetNumRetries(uint8_t numRetries)
{
    return ThyoneI_Set(ThyoneI_USERSETTING_INDEX_RF_NUM_RETRIES, (uint8_t*)&numRetries, 1);
}

/*
 *Set the number of time slots
 *
 *input:
 * -numSlots: number of time slots
 *
 *return true if request succeeded
 *       false otherwise
 */
bool ThyoneI_SetRpNumSlots(uint8_t numSlots)
{
    return ThyoneI_Set(ThyoneI_USERSETTING_INDEX_RF_RP_NUM_SLOTS, (uint8_t*)&numSlots, 1);
}

/*
 *Set the source address
 *
 *input:
 * -sourceAddress: source address
 *
 *return true if request succeeded
 *       false otherwise
 */
bool ThyoneI_SetSourceAddress(uint32_t sourceAddress)
{
    return ThyoneI_Set(ThyoneI_USERSETTING_INDEX_MAC_SOURCE_ADDRESS, (uint8_t*)&sourceAddress, 4);
}

/*
 *Set the destination address
 *
 *input:
 * -destinationAddress: destination address
 *
 *return true if request succeeded
 *       false otherwise
 */
bool ThyoneI_SetDestinationAddress(uint32_t destinationAddress)
{
    return ThyoneI_Set(ThyoneI_USERSETTING_INDEX_MAC_DESTINATION_ADDRESS, (uint8_t*)&destinationAddress, 4);
}

/*
 *Set the group id
 *
 *input:
 * -groupid: groupID
 *
 *return true if request succeeded
 *       false otherwise
 */
bool ThyoneI_SetGroupID(uint8_t groupId)
{
    return ThyoneI_Set(ThyoneI_USERSETTING_INDEX_MAC_GROUP_ID, (uint8_t*)&groupId, 1);
}

/*
 *Set the encryption key
 *
 *input:
 * -keyP: pointer to 16-byte key
 *
 *return true if request succeeded
 *       false otherwise
 */
bool ThyoneI_SetEncryptionKey(uint8_t *keyP)
{
    return ThyoneI_Set(ThyoneI_USERSETTING_INDEX_MAC_ENCRYPTION_KEY, keyP, 16);
}

/*
 *Set the time-to-live defining the maximum of hops if repeating
 *
 *input:
 * -ttl: time-to-live value
 *
 *return true if request succeeded
 *       false otherwise
 */
bool ThyoneI_SetTimeToLive(uint8_t ttl)
{
    return ThyoneI_Set(ThyoneI_USERSETTING_INDEX_MAC_TLL, &ttl, 1);
}

/*
 *Set clear channel assessement mode
 *
 *input:
 * -ccaMode: clear channel assessment mode
 *
 *return true if request succeeded
 *       false otherwise
 */
bool ThyoneI_SetCCAMode(uint8_t ccaMode)
{
    return ThyoneI_Set(ThyoneI_USERSETTING_INDEX_CCA_MODE, &ccaMode, 1);
}

/*
 *Set threshold for clear channel assessement
 *
 *input:
 * -ccaThreshold: threshold for clear channel assessement
 *
 *return true if request succeeded
 *       false otherwise
 */
bool ThyoneI_SetCCAThreshold(uint8_t ccaThreshold)
{
    return ThyoneI_Set(ThyoneI_USERSETTING_INDEX_CCA_THRESHOLD, &ccaThreshold, 1);
}

/*
 *Set remote gpio config
 *
 *input:
 * -remoteConfig: remote gpio config
 *
 *return true if request succeeded
 *       false otherwise
 */
bool ThyoneI_SetGPIOBlockRemoteConfig(uint8_t remoteConfig)
{
    return ThyoneI_Set(ThyoneI_USERSETTING_INDEX_REMOTE_GPIO_CONFIG, &remoteConfig, 1);
}

/*
 *Set module mode
 *
 *input:
 * -moduleMode: operation mode of the module
 *
 *return true if request succeeded
 *       false otherwise
 */
bool ThyoneI_SetModuleMode(ThyoneI_OperatingMode_t moduleMode)
{
    return ThyoneI_Set(ThyoneI_USERSETTING_INDEX_MODULE_MODE, (uint8_t*)&moduleMode, 1);
}

/*
 *Request the current user settings
 *
 *input:
 * -userSetting: user setting to be requested
 *
 *output:
 * -responseP: pointer of the memory to put the requested content
 * -response_lengthP: length of the requested content
 *
 *return true if request succeeded
 *       false otherwise
 */
bool
ThyoneI_Get(ThyoneI_UserSettings_t userSetting, uint8_t *ResponseP, uint16_t *Response_LengthP)
{
    bool ret = false;

    /* fill CMD_ARRAY packet */
    CMD_Array[CMD_POSITION_STX] = CMD_STX;
    CMD_Array[CMD_POSITION_CMD] = ThyoneI_CMD_GET_REQ;
    CMD_Array[CMD_POSITION_LENGTH_LSB] = (uint8_t)1;
    CMD_Array[CMD_POSITION_LENGTH_MSB] = (uint8_t)0;
    CMD_Array[CMD_POSITION_DATA] = userSetting;

    if (FillChecksum(CMD_Array, CMD_ARRAY_SIZE()))
    {
        /* now send CMD_ARRAY */
        UART_Transmit(CMD_Array, CMD_ARRAY_SIZE());

        /* wait for cnf */
        if (Wait4CNF(CMD_WAIT_TIME, ThyoneI_CMD_GET_CNF, CMD_Status_Success, true))
        {
            uint16_t length = ((uint16_t) RxPacket[CMD_POSITION_LENGTH_LSB] << 0) + ((uint16_t) RxPacket[CMD_POSITION_LENGTH_MSB] << 8);
            memcpy(ResponseP, &RxPacket[CMD_POSITION_DATA + 1], length - 1); /* First Data byte is status, following bytes response*/
            *Response_LengthP = length - 1;
            ret = true;
        }
    }
    return ret;
}

/*
 *Request the 4 byte serial number
 *
 *output:
 * -versionP: pointer to the 4 byte serial number
 *
 *return true if request succeeded
 *       false otherwise
 */
bool
ThyoneI_GetSerialNumber(uint8_t *serialNumberP)
{
    uint16_t length;
    return ThyoneI_Get(ThyoneI_USERSETTING_INDEX_SERIAL_NUMBER, serialNumberP, &length);
}


/*
 *Request the 3 byte firmware version
 *
 *output:
 * -versionP: pointer to the 3 byte firmware version
 *
 *return true if request succeeded
 *       false otherwise
 */
bool
ThyoneI_GetFWVersion(uint8_t *versionP)
{
    uint16_t length;
    return ThyoneI_Get(ThyoneI_USERSETTING_INDEX_FW_VERSION, versionP, &length);
}

/*
 *Request the TX power
 *
 *output:
 * -txpowerP: pointer to the TX power
 *
 *return true if request succeeded
 *       false otherwise
 */
bool
ThyoneI_GetTXPower(ThyoneI_TXPower_t *txpowerP)
{
    uint16_t length;
    return ThyoneI_Get(ThyoneI_USERSETTING_INDEX_RF_TX_POWER, (uint8_t*)txpowerP, &length);
}

/*
 *Request the UART baudrate index
 *
 *output:
 * -baudrateIndexP: pointer to the UART baudrate index
 *
 *return true if request succeeded
*       false otherwise
*/
bool ThyoneI_GetBaudrateIndex(ThyoneI_BaudRateIndex_t *baudrateP, ThyoneI_UartParity_t *parityP, bool *flowcontrolEnableP)
{
    bool ret = false;
    uint16_t length;
    uint8_t uartIndex;

    if(ThyoneI_Get(ThyoneI_USERSETTING_INDEX_UART_CONFIG, (uint8_t*)&uartIndex, &length))
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
            *parityP = ThyoneI_UartParity_None;
        }
        else
        {
            *parityP = ThyoneI_UartParity_Even;
            uartIndex -= 64;
        }

        *baudrateP = (ThyoneI_BaudRateIndex_t)uartIndex;
        ret = true;
    }

    return ret;
}

/*
 *Get the encryption mode
 *
 *output:
 * -encryptionModeP: Pointer to encryptionMode
 *
 *return true if request succeeded
 *       false otherwise
 */
bool
ThyoneI_GetEncryptionMode(ThyoneI_EncryptionMode_t *encryptionModeP)
{
    uint16_t length;
    return ThyoneI_Get(ThyoneI_USERSETTING_INDEX_ENCRYPTION_MODE, (uint8_t*)encryptionModeP, &length);
}

/*
 *Get the rf profile
 *
 * *output:
 * -profileP: Pointer to rf profile
 *
 *return true if request succeeded
 *       false otherwise
 */
bool ThyoneI_GetRfProfile(ThyoneI_Profile_t *profileP)
{
    uint16_t length;
    return ThyoneI_Get(ThyoneI_USERSETTING_INDEX_RF_PROFILE, (uint8_t*)profileP, &length);
}


/*
 *Get the rf channel
 *
 * *output:
 * -channelP: Pointer to rf channel
 *
 *return true if request succeeded
 *       false otherwise
 */
bool ThyoneI_GetRFChannel(uint8_t *channelP)
{
    uint16_t length;
    return ThyoneI_Get(ThyoneI_USERSETTING_INDEX_RF_CHANNEL, (uint8_t*)channelP, &length);
}

/*
 *Get the number of retries
 *
 *output:
 * -numRetriesP: Pointer to number of retries
 *
 *return true if request succeeded
 *       false otherwise
 */
bool ThyoneI_GetNumRetries(uint8_t *numRetriesP)
{
    uint16_t length;
    return ThyoneI_Get(ThyoneI_USERSETTING_INDEX_RF_NUM_RETRIES, numRetriesP, &length);
}

/*
 *Get the number of time slots
 *
 *output:
 * -numSlotsP: pointer of number of time slots
 *
 *return true if request succeeded
 *       false otherwise
 */
bool ThyoneI_GetRpNumSlots(uint8_t *numSlotsP)
{
    uint16_t length;
    return ThyoneI_Get(ThyoneI_USERSETTING_INDEX_RF_RP_NUM_SLOTS, numSlotsP, &length);
}

/*
 *Get the source address
 *
 *output:
 * -sourceAddressP: pointer to source address
 *
 *return true if request succeeded
 *       false otherwise
 */
bool ThyoneI_GetSourceAddress(uint32_t *sourceAddressP)
{
    uint16_t length;
    return ThyoneI_Get(ThyoneI_USERSETTING_INDEX_MAC_SOURCE_ADDRESS, (uint8_t*)sourceAddressP, &length);
}

/*
 *Get the destination address
 *
 *output:
 * -destinationAddressP: pointer to destination address
 *
 *return true if request succeeded
 *       false otherwise
 */
bool ThyoneI_GetDestinationAddress(uint32_t *destinationAddressP)
{
    uint16_t length;
    return ThyoneI_Get(ThyoneI_USERSETTING_INDEX_MAC_DESTINATION_ADDRESS, (uint8_t*)destinationAddressP, &length);
}

/*
 *Get the group id
 *
 *output:
 * -groupIdP: pointer to groupID
 *
 *return true if request succeeded
 *       false otherwise
 */
bool ThyoneI_GetGroupID(uint8_t *groupIdP)
{
    uint16_t length;
    return ThyoneI_Get(ThyoneI_USERSETTING_INDEX_MAC_GROUP_ID, groupIdP, &length);
}

/*
 *Fet the time-to-live defining the maximum of hops if repeating
 *
 *output:
 * -ttlP: pointer to time-to-live
 *
 *return true if request succeeded
 *       false otherwise
 */
bool ThyoneI_GetTimeToLive(uint8_t *ttlP)
{
    uint16_t length;
    return ThyoneI_Get(ThyoneI_USERSETTING_INDEX_MAC_TLL, ttlP, &length);
}

/*
 *Get clear channel assessement mode
 *
 *output:
 * -ccaModeP: pointer to clear channel assessment mode
 *
 *return true if request succeeded
 *       false otherwise
 */
bool ThyoneI_GetCCAMode(uint8_t *ccaModeP)
{
    uint16_t length;
    return ThyoneI_Get(ThyoneI_USERSETTING_INDEX_CCA_MODE, ccaModeP, &length);
}

/*
 *Get threshold for clear channel assessement
 *
 *output:
 * -ccaThreshold: threshold for clear channel assessement
 *
 *return true if request succeeded
 *       false otherwise
 */
bool ThyoneI_GetCCAThreshold(uint8_t *ccaThresholdP)
{
    uint16_t length;
    return ThyoneI_Get(ThyoneI_USERSETTING_INDEX_CCA_THRESHOLD, ccaThresholdP, &length);
}

/*
 *Get remote gpio config
 *
 *output:
 * -remoteConfigP: pointer to remote gpio config
 *
 *return true if request succeeded
 *       false otherwise
 */
bool ThyoneI_GetGPIOBlockRemoteConfig(uint8_t *remoteConfigP)
{
    uint16_t length;
    return ThyoneI_Get(ThyoneI_USERSETTING_INDEX_REMOTE_GPIO_CONFIG, remoteConfigP, &length);
}

/*
 *Get module mode
 *
 *output:
 * -moduleMode: operation mode of the module
 *
 *return true if request succeeded
 *       false otherwise
 */
bool ThyoneI_GetModuleMode(ThyoneI_OperatingMode_t *moduleModeP)
{
    uint16_t length;
    return ThyoneI_Get(ThyoneI_USERSETTING_INDEX_MODULE_MODE, (uint8_t*)moduleModeP, &length);
}

/*
 *Request the module state
 *
 *output:
 * -state:   current state of the module
 *
 *return true if request succeeded
 *       false otherwise
 */
bool
ThyoneI_GetState(ThyoneI_States_t* state)
{
    bool ret = false;

    /* fill CMD_ARRAY packet */
    CMD_Array[CMD_POSITION_STX] = CMD_STX;
    CMD_Array[CMD_POSITION_CMD] = ThyoneI_CMD_GETSTATE_REQ;
    CMD_Array[CMD_POSITION_LENGTH_LSB] = (uint8_t)0;
    CMD_Array[CMD_POSITION_LENGTH_MSB] = (uint8_t)0;

    if (FillChecksum(CMD_Array, CMD_ARRAY_SIZE()))
    {
        /* now send CMD_ARRAY */
        UART_Transmit(CMD_Array, CMD_ARRAY_SIZE());
        /* wait for cnf */
        if (Wait4CNF(CMD_WAIT_TIME, ThyoneI_CMD_GETSTATE_CNF, CMD_Status_Success, true))
        {
            *state = RxPacket[CMD_POSITION_DATA+1];
            ret = true;
        }
    }
    return ret;
}

/*
 *Configure the local GPIO of the module
 *
 *input:
 * -configP: pointer to one or more pin configurations
 * -number_of_configs: number of entries in configP array
 *return true if request succeeded
 *       false otherwise
 */
bool ThyoneI_GPIOLocalSetConfig(ThyoneI_GPIOConfigBlock_t* configP, uint16_t number_of_configs)
{
    bool ret = false;
	uint16_t length = 0;

	for (uint16_t i=0; i < number_of_configs; i++)
	{
		switch(configP->function)
		{
			case ThyoneI_GPIO_IO_Disconnected:
			{
				CMD_Array[CMD_POSITION_DATA + length] = 3;
				CMD_Array[CMD_POSITION_DATA + length + 1] = configP->GPIO_ID;
				CMD_Array[CMD_POSITION_DATA + length + 2] = configP->function;
				CMD_Array[CMD_POSITION_DATA + length + 3] = 0x00;
				length += 4;
			}
			break;
			case ThyoneI_GPIO_IO_Input:
			{
				CMD_Array[CMD_POSITION_DATA + length] = 3;
				CMD_Array[CMD_POSITION_DATA + length + 1] = configP->GPIO_ID;
				CMD_Array[CMD_POSITION_DATA + length + 2] = configP->function;
				CMD_Array[CMD_POSITION_DATA + length + 3] = configP->value.input;
				length += 4;
			}
			break;
			case ThyoneI_GPIO_IO_Output:
			{
				CMD_Array[CMD_POSITION_DATA + length] = 3;
				CMD_Array[CMD_POSITION_DATA + length + 1] = configP->GPIO_ID;
				CMD_Array[CMD_POSITION_DATA + length + 2] = configP->function;
				CMD_Array[CMD_POSITION_DATA + length + 3] = configP->value.output;
				length += 4;
				}
			break;
			case ThyoneI_GPIO_IO_PWM:
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
    CMD_Array[CMD_POSITION_CMD] = ThyoneI_CMD_GPIO_LOCAL_SETCONFIG_REQ;
    CMD_Array[CMD_POSITION_LENGTH_LSB]= (length & 0x00FF);
    CMD_Array[CMD_POSITION_LENGTH_MSB]= (length & 0xFF00) >> 8;

    if (FillChecksum(CMD_Array, CMD_ARRAY_SIZE()))
    {
        /* now send CMD_ARRAY */
        UART_Transmit(CMD_Array, CMD_ARRAY_SIZE());

        /* wait for cnf */
        ret = Wait4CNF(CMD_WAIT_TIME, ThyoneI_CMD_GPIO_LOCAL_SETCONFIG_CNF, CMD_Status_Success, true);
    }

    return ret;
}

/*
 *Read the local GPIO configuration of the module
 *
 *output:
 * -configP: pointer to one or more pin configurations
 * -number_of_configsP: pointer to number of entries in configP array
 *return true if request succeeded
 *       false otherwise
 */
bool ThyoneI_GPIOLocalGetConfig(ThyoneI_GPIOConfigBlock_t* configP, uint16_t *number_of_configsP)
{
    bool ret = false;

    CMD_Array[CMD_POSITION_STX] = CMD_STX;
    CMD_Array[CMD_POSITION_CMD] = ThyoneI_CMD_GPIO_LOCAL_GETCONFIG_REQ;
    CMD_Array[CMD_POSITION_LENGTH_LSB]= 0;
    CMD_Array[CMD_POSITION_LENGTH_MSB]= 0;

    if (FillChecksum(CMD_Array, CMD_ARRAY_SIZE()))
    {
        /* now send CMD_ARRAY */
        UART_Transmit(CMD_Array, CMD_ARRAY_SIZE());

        /* wait for cnf */
        ret = Wait4CNF(CMD_WAIT_TIME, ThyoneI_CMD_GPIO_LOCAL_GETCONFIG_CNF, CMD_Status_Success, true);

		if(ret == true)
		{
			uint16_t length = ((uint16_t) RxPacket[CMD_POSITION_LENGTH_LSB] << 0) + ((uint16_t) RxPacket[CMD_POSITION_LENGTH_MSB] << 8);

			*number_of_configsP = 0;
			uint8_t* uartP = &RxPacket[CMD_POSITION_DATA+1];
			ThyoneI_GPIOConfigBlock_t* configP_running = configP;
			while(uartP < &RxPacket[CMD_POSITION_DATA+length])
			{
				switch(*(uartP + 2))
				{
					case ThyoneI_GPIO_IO_Disconnected:
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
					case ThyoneI_GPIO_IO_Input:
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
					case ThyoneI_GPIO_IO_Output:
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
					case ThyoneI_GPIO_IO_PWM:
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

/*
 *Set the output value of the local pin. Pin has to be configured first.
 *See ThyoneI_GPIOLocalWriteConfig
 *
 *input:
 * -controlP: pointer to one or more pin controls
 * -number_of_controls: number of entries in controlP array
 *return true if request succeeded
 *       false otherwise
 */
bool ThyoneI_GPIOLocalWrite(ThyoneI_GPIOControlBlock_t* controlP, uint16_t number_of_controls)
{
    bool ret = false;
	uint16_t length = 0;

	for (uint16_t i=0; i < number_of_controls; i++)
	{
		CMD_Array[CMD_POSITION_DATA + length] = 2;
		CMD_Array[CMD_POSITION_DATA + length + 1] = controlP->GPIO_ID;
		CMD_Array[CMD_POSITION_DATA + length + 2] = controlP->value.output;
		length += 3;
		controlP++;
	}

    CMD_Array[CMD_POSITION_STX] = CMD_STX;
    CMD_Array[CMD_POSITION_CMD] = ThyoneI_CMD_GPIO_LOCAL_WRITE_REQ;
    CMD_Array[CMD_POSITION_LENGTH_LSB]= (length & 0x00FF);
    CMD_Array[CMD_POSITION_LENGTH_MSB]= (length & 0xFF00) >> 8;

    if (FillChecksum(CMD_Array, CMD_ARRAY_SIZE()))
    {
        /* now send CMD_ARRAY */
        UART_Transmit(CMD_Array, CMD_ARRAY_SIZE());

        /* wait for cnf */
        ret = Wait4CNF(CMD_WAIT_TIME, ThyoneI_CMD_GPIO_LOCAL_WRITE_CNF, CMD_Status_Success, true);
    }

    return ret;
}

/*
 *Read the input of the pin. Pin has to be configured first.
 *See ThyoneI_GPIOLocalWriteConfig
 *
 *input:
 * -GPIOToReadP: One or more pins to read.
 * -amountGPIOToRead: amount of pins to read and therefore length of GPIOToRead
 *output:
 * -controlP: Pointer to controlBlock
 * -number_of_controlsP: pointer to number of entries in controlP array
 *return true if request succeeded
 *       false otherwise
 */
bool ThyoneI_GPIOLocalRead(uint8_t *GPIOToReadP, uint8_t amountGPIOToRead, ThyoneI_GPIOControlBlock_t* controlP, uint16_t* number_of_controlsP)
{
    bool ret = false;

    CMD_Array[CMD_POSITION_STX] = CMD_STX;
    CMD_Array[CMD_POSITION_CMD] = ThyoneI_CMD_GPIO_LOCAL_READ_REQ;
    CMD_Array[CMD_POSITION_LENGTH_LSB]= amountGPIOToRead + 1;
    CMD_Array[CMD_POSITION_LENGTH_MSB]= 0;
    CMD_Array[CMD_POSITION_DATA] = amountGPIOToRead;
    memcpy(&CMD_Array[CMD_POSITION_DATA + 1], GPIOToReadP, amountGPIOToRead);

    if (FillChecksum(CMD_Array, CMD_ARRAY_SIZE()))
    {
        /* now send CMD_ARRAY */
        UART_Transmit(CMD_Array, CMD_ARRAY_SIZE());

        /* wait for cnf */
        ret = Wait4CNF(CMD_WAIT_TIME, ThyoneI_CMD_GPIO_LOCAL_READ_CNF, CMD_Status_Success, true);

        if(ret)
        {
			uint16_t length = ((uint16_t) RxPacket[CMD_POSITION_LENGTH_LSB] << 0) + ((uint16_t) RxPacket[CMD_POSITION_LENGTH_MSB] << 8);

			*number_of_controlsP = 0;
			uint8_t* uartP = &RxPacket[CMD_POSITION_DATA+1];
			ThyoneI_GPIOControlBlock_t* controlP_running = controlP;
			while(uartP < &RxPacket[CMD_POSITION_DATA+length])
			{

				if(*uartP == 2)
				{
					controlP_running->GPIO_ID = *(uartP + 1);
					controlP_running->value.output   = *(uartP + 2);

					controlP_running++;
					*number_of_controlsP += 1;
				}
				uartP += *uartP + 1;
			}
        }
    }

    return ret;
}

/*
 *Configure the remote GPIO of the module
 *
 *input:
 * -destAddress: Destination address of the remote Thyone-I device
 * -configP: pointer to one or more pin configurations
 * -number_of_configs: number of entries in configP array
 *return true if request succeeded
 *       false otherwise
 */
bool ThyoneI_GPIORemoteSetConfig(uint32_t destAddress, ThyoneI_GPIOConfigBlock_t* configP, uint16_t number_of_configs)
{
    bool ret = false;
	uint16_t length = 4;

	for (uint16_t i=0; i < number_of_configs; i++)
	{
		switch(configP->function)
		{
			case ThyoneI_GPIO_IO_Disconnected:
			{
				CMD_Array[CMD_POSITION_DATA + length] = 3;
				CMD_Array[CMD_POSITION_DATA + length + 1] = configP->GPIO_ID;
				CMD_Array[CMD_POSITION_DATA + length + 2] = configP->function;
				CMD_Array[CMD_POSITION_DATA + length + 3] = 0x00;
				length += 4;
			}
			break;
			case ThyoneI_GPIO_IO_Input:
			{
				CMD_Array[CMD_POSITION_DATA + length] = 3;
				CMD_Array[CMD_POSITION_DATA + length + 1] = configP->GPIO_ID;
				CMD_Array[CMD_POSITION_DATA + length + 2] = configP->function;
				CMD_Array[CMD_POSITION_DATA + length + 3] = configP->value.input;
				length += 4;
			}
			break;
			case ThyoneI_GPIO_IO_Output:
			{
				CMD_Array[CMD_POSITION_DATA + length] = 3;
				CMD_Array[CMD_POSITION_DATA + length + 1] = configP->GPIO_ID;
				CMD_Array[CMD_POSITION_DATA + length + 2] = configP->function;
				CMD_Array[CMD_POSITION_DATA + length + 3] = configP->value.output;
				length += 4;
			}
			break;
			case ThyoneI_GPIO_IO_PWM:
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
    CMD_Array[CMD_POSITION_CMD] = ThyoneI_CMD_GPIO_REMOTE_SETCONFIG_REQ;
    CMD_Array[CMD_POSITION_LENGTH_LSB]= (length & 0x00FF);
    CMD_Array[CMD_POSITION_LENGTH_MSB]= (length & 0xFF00) >> 8;

    memcpy(&CMD_Array[CMD_POSITION_DATA], &destAddress, 4);

    if (FillChecksum(CMD_Array, CMD_ARRAY_SIZE()))
    {
        /* now send CMD_ARRAY */
        UART_Transmit(CMD_Array, CMD_ARRAY_SIZE());

        /* wait for cnf */
        ret = Wait4CNF(CMD_WAIT_TIME, ThyoneI_CMD_GPIO_REMOTE_SETCONFIG_CNF, CMD_Status_Success, true);
    }

    return ret;
}

/*
 *Read the remote GPIO configuration of the module
 *
 *output:
 * -destAddress: Destination address of the remote Thyone-I device
 * -configP: pointer to one or more pin configurations
 * -number_of_configsP: pointer to number of entries in configP array
 *return true if request succeeded
 *       false otherwise
 */
bool ThyoneI_GPIORemoteGetConfig(uint32_t destAddress, ThyoneI_GPIOConfigBlock_t* configP, uint16_t *number_of_configsP)
{
    bool ret = false;

    CMD_Array[CMD_POSITION_STX] = CMD_STX;
    CMD_Array[CMD_POSITION_CMD] = ThyoneI_CMD_GPIO_REMOTE_GETCONFIG_REQ;
    CMD_Array[CMD_POSITION_LENGTH_LSB]= 4;
    CMD_Array[CMD_POSITION_LENGTH_MSB]= 0;
    memcpy(&CMD_Array[CMD_POSITION_DATA], &destAddress, 4);

    if (FillChecksum(CMD_Array, CMD_ARRAY_SIZE()))
    {
        /* now send CMD_ARRAY */
        UART_Transmit(CMD_Array, CMD_ARRAY_SIZE());

        /* wait for cnf */
        ret = Wait4CNF(CMD_WAIT_TIME, ThyoneI_CMD_GPIO_REMOTE_GETCONFIG_RSP, CMD_Status_NoStatus, true);

        if(ret)
        {
			uint16_t length = ((uint16_t) RxPacket[CMD_POSITION_LENGTH_LSB] << 0) + ((uint16_t) RxPacket[CMD_POSITION_LENGTH_MSB] << 8);

			*number_of_configsP = 0;
			uint8_t* uartP = &RxPacket[CMD_POSITION_DATA+1+4];
			ThyoneI_GPIOConfigBlock_t* configP_running = configP;
			while(uartP < &RxPacket[CMD_POSITION_DATA+length])
			{
				switch(*(uartP + 2))
				{
					case ThyoneI_GPIO_IO_Disconnected:
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
					case ThyoneI_GPIO_IO_Input:
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
					case ThyoneI_GPIO_IO_Output:
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
					case ThyoneI_GPIO_IO_PWM:
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

/*
 *Set the output value of the remote pin. Pin has to be configured first.
 *See ThyoneI_GPIORemoteWriteConfig
 *
 *input:
 * -destAddress: Destination address of the remote Thyone-I device
 * -controlP: pointer to one or more pin controls
 * -number_of_controls: number of entries in controlP array
 *return true if request succeeded
 *       false otherwise
 */
bool ThyoneI_GPIORemoteWrite(uint32_t destAddress, ThyoneI_GPIOControlBlock_t* controlP, uint16_t number_of_controls)
{
    bool ret = false;
	uint16_t length = 4;

    for (uint16_t i=0; i < number_of_controls; i++)
	{
		CMD_Array[CMD_POSITION_DATA + length] = 2;
		CMD_Array[CMD_POSITION_DATA + length + 1] = controlP->GPIO_ID;
		CMD_Array[CMD_POSITION_DATA + length + 2] = controlP->value.output;
		length += 3;
		controlP += sizeof(ThyoneI_GPIOControlBlock_t);
	}

    CMD_Array[CMD_POSITION_STX] = CMD_STX;
    CMD_Array[CMD_POSITION_CMD] = ThyoneI_CMD_GPIO_REMOTE_WRITE_REQ;
    CMD_Array[CMD_POSITION_LENGTH_LSB]= (length & 0x00FF);
    CMD_Array[CMD_POSITION_LENGTH_MSB]= (length & 0xFF00) >> 8;

    memcpy(&CMD_Array[CMD_POSITION_DATA], &destAddress, 4);

    if (FillChecksum(CMD_Array, CMD_ARRAY_SIZE()))
    {
        /* now send CMD_ARRAY */
        UART_Transmit(CMD_Array, CMD_ARRAY_SIZE());

        /* wait for cnf */
        ret = Wait4CNF(CMD_WAIT_TIME, ThyoneI_CMD_GPIO_REMOTE_WRITE_CNF, CMD_Status_Success, true);
    }

    return ret;
}

/*
 *Read the input of the pins. Pin has to be configured first.
 *See ThyoneI_GPIORemoteWriteConfig
 *
 *input:
 * -destAddress: Destination address of the remote Thyone-I device
 * -GPIOToReadP: One or more pins to read.
 * -amountGPIOToRead: amount of pins to read and therefore length of GPIOToReadP
 *output:
 * -controlP: Pointer to controlBlock
 * -number_of_controlsP: pointer to number of entries in controlP array
 *return true if request succeeded
 *       false otherwise
 */
bool ThyoneI_GPIORemoteRead(uint32_t destAddress, uint8_t *GPIOToReadP, uint8_t amountGPIOToRead, ThyoneI_GPIOControlBlock_t* controlP, uint16_t* number_of_controlsP)
{
    bool ret = false;

    /* payload is 4-byte destination address + pin configuration*/
    uint16_t commandLength = 1 + amountGPIOToRead + 4;

    CMD_Array[CMD_POSITION_STX] = CMD_STX;
    CMD_Array[CMD_POSITION_CMD] = ThyoneI_CMD_GPIO_REMOTE_READ_REQ;
    CMD_Array[CMD_POSITION_LENGTH_LSB]= (commandLength & 0x00FF);
    CMD_Array[CMD_POSITION_LENGTH_MSB]= (commandLength & 0xFF00) >> 8;

    memcpy(&CMD_Array[CMD_POSITION_DATA], &destAddress, 4);
    CMD_Array[CMD_POSITION_DATA+4] = amountGPIOToRead;
    memcpy(&CMD_Array[CMD_POSITION_DATA + 5], GPIOToReadP, amountGPIOToRead);

    if (FillChecksum(CMD_Array, CMD_ARRAY_SIZE()))
    {
        /* now send CMD_ARRAY */
        UART_Transmit(CMD_Array, CMD_ARRAY_SIZE());

        /* wait for cnf */
        ret = Wait4CNF(1000, ThyoneI_CMD_GPIO_REMOTE_READ_RSP, CMD_Status_NoStatus, true);

        if(ret)
        {
			uint16_t length = ((uint16_t) RxPacket[CMD_POSITION_LENGTH_LSB] << 0) + ((uint16_t) RxPacket[CMD_POSITION_LENGTH_MSB] << 8);

			*number_of_controlsP = 0;
			uint8_t* uartP = &RxPacket[CMD_POSITION_DATA+1+4];
			ThyoneI_GPIOControlBlock_t* controlP_running = controlP;
			while(uartP < &RxPacket[CMD_POSITION_DATA+length])
			{

				if(*uartP == 2)
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

