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

#include "Metis.h"
#include "../global/global.h"

typedef struct Metis_Pins_t {
	Pin_t reset;
} Metis_Pins_t;

#define CMD_WAIT_TIME 500
#define CNFINVALID 255
#define MAX_PAYLOAD_LENGTH 255
#define TXPOWERINVALID -128
#define RSSIINVALID -128

#define CMD_STX 0xFF
#define Metis_CMD_TYPE_REQ (0 << 6)
#define Metis_CMD_TYPE_CNF (2 << 6)

#define Metis_CMD_DATA 0x00
#define Metis_CMD_DATA_REQ (Metis_CMD_DATA | Metis_CMD_TYPE_REQ)
#define Metis_CMD_DATA_CNF (Metis_CMD_DATA | Metis_CMD_TYPE_CNF)

#define Metis_CMD_DATA_IND 0x03

#define Metis_CMD_SET_MODE 0x04
#define Metis_CMD_SET_MODE_REQ (Metis_CMD_SET_MODE | Metis_CMD_TYPE_REQ)
#define Metis_CMD_SET_MODE_CNF (Metis_CMD_SET_MODE | Metis_CMD_TYPE_CNF)

#define Metis_CMD_RESET 0x05
#define Metis_CMD_RESET_REQ (Metis_CMD_RESET | Metis_CMD_TYPE_REQ)
#define Metis_CMD_RESET_CNF (Metis_CMD_RESET | Metis_CMD_TYPE_CNF)

#define Metis_CMD_SET 0x09
#define Metis_CMD_SET_REQ (Metis_CMD_SET | Metis_CMD_TYPE_REQ)
#define Metis_CMD_SET_CNF (Metis_CMD_SET | Metis_CMD_TYPE_CNF)

#define Metis_CMD_SETUARTSPEED 0X10
#define Metis_CMD_SETUARTSPEED_REQ (Metis_CMD_SETUARTSPEED | Metis_CMD_TYPE_REQ)
#define Metis_CMD_SETUARTSPEED_CNF (Metis_CMD_SETUARTSPEED | Metis_CMD_TYPE_CNF)

#define Metis_CMD_GET 0x0A
#define Metis_CMD_GET_REQ (Metis_CMD_GET | Metis_CMD_TYPE_REQ)
#define Metis_CMD_GET_CNF (Metis_CMD_GET | Metis_CMD_TYPE_CNF)

#define Metis_CMD_GET_SERIALNO 0x0B
#define Metis_CMD_GET_SERIALNO_REQ (Metis_CMD_GET_SERIALNO | Metis_CMD_TYPE_REQ)
#define Metis_CMD_GET_SERIALNO_CNF (Metis_CMD_GET_SERIALNO| Metis_CMD_TYPE_CNF)

#define Metis_CMD_GET_FWRELEASE 0x0C
#define Metis_CMD_GET_FWRELEASE_REQ (Metis_CMD_GET_FWRELEASE | Metis_CMD_TYPE_REQ)
#define Metis_CMD_GET_FWRELEASE_CNF (Metis_CMD_GET_FWRELEASE| Metis_CMD_TYPE_CNF)

#define Metis_CMD_RSSI 0x0D
#define Metis_CMD_RSSI_REQ (Metis_CMD_RSSI | Metis_CMD_TYPE_REQ)
#define Metis_CMD_RSSI_CNF (Metis_CMD_RSSI | Metis_CMD_TYPE_CNF)

#define Metis_CMD_FACTORYRESET 0X11
#define Metis_CMD_FACTORYRESET_REQ (Metis_CMD_FACTORYRESET | Metis_CMD_TYPE_REQ)
#define Metis_CMD_FACTORYRESET_CNF (Metis_CMD_FACTORYRESET | Metis_CMD_TYPE_CNF)

/* type used to check the response, when a command was sent to the module */
typedef enum CMD_Status_t
{
    CMD_Status_Success = 0x00,
    CMD_Status_Failed,
    CMD_Status_Invalid,
    CMD_Status_Reset,
} CMD_Status_t;

typedef struct
{
    uint8_t Stx;
    uint8_t Cmd;
    uint8_t Length;
    uint8_t Data[MAX_PAYLOAD_LENGTH + 1]; /* +1 for the CS */
} CMD_Frame_t;

typedef struct
{
    uint8_t cmd;                 /* variable to check if correct CMD has been confirmed */
    CMD_Status_t status;         /* variable used to check the response (*_CNF), when a request (*_REQ) was sent to the TarvosIII */
} CMD_Confirmation_t;

typedef struct
{
    uint8_t memoryPosition;                 /* memory position of requested usersetting */
    uint8_t lengthGetRequest;               /* length of one or more requested usersetting */
} US_Confirmation_t;

/**************************************
 *          Static variables          *
 **************************************/

static Metis_Pins_t pins;
static CMD_Frame_t RxPacket;                        /* data buffer for RX */

#define CMDCONFIRMATIONARRAY_LENGTH 2
static CMD_Confirmation_t cmdConfirmation_array[CMDCONFIRMATIONARRAY_LENGTH];
static US_Confirmation_t usConfirmation;            /* variable used to check if GET function was successful */
static Metis_Frequency_t frequency;               /* frequency used by module */
static bool rssi_enable = false;
static uint8_t checksum = 0;
static uint8_t RxByteCounter = 0;
static uint8_t BytesToReceive = 0;                     /* read buffer for next available byte*/
static uint8_t RxBuffer[sizeof(CMD_Frame_t)];   /* data buffer for RX */
static void(*RXcallback)(uint8_t*,uint8_t,int8_t);  /* callback function */

/**************************************
 *          Static functions          *
 **************************************/

/* function to calculate the rssi value from the rx level */
static int8_t CalculateRSSIValue(uint8_t rxLevel)
{
    uint8_t offset = 74;
    int8_t rssi_value = RSSIINVALID;

    if(rxLevel < 128)
    {
        rssi_value = rxLevel/2 - offset;
    }
    else
    {
        rssi_value = (rxLevel - 256)/2 - offset;
    }
    return rssi_value;
}

/* interpret the valid received data packet */
static void HandleRxPacket(uint8_t*RxBuffer)
{
    CMD_Confirmation_t cmdConfirmation;
    cmdConfirmation.cmd = CNFINVALID;
    cmdConfirmation.status = CMD_Status_Invalid;

    uint8_t cmd_length = RxBuffer[2];
    memcpy((uint8_t*)&RxPacket,RxBuffer,cmd_length + 4); /* payload + std + command + length byte + checksum */

    switch (RxPacket.Cmd)
    {
    case Metis_CMD_SET_MODE_CNF:
    {
        /* check whether the module returns success */
        if (RxPacket.Data[0] == 0x00)
        {
            cmdConfirmation.status = CMD_Status_Success;
        }
        else
        {
            cmdConfirmation.status = CMD_Status_Failed;
        }
        cmdConfirmation.cmd = RxPacket.Cmd;
    }
    break;

    case Metis_CMD_RESET_CNF:
    {
        /* check whether the module returns success */
        if (RxPacket.Data[0] == 0x00)
        {
            cmdConfirmation.status = CMD_Status_Success;
        }
        else
        {
            cmdConfirmation.status = CMD_Status_Failed;
        }
        cmdConfirmation.cmd = RxPacket.Cmd;
    }
    break;

    case Metis_CMD_DATA_CNF:
    {
        /* check whether the module returns success */
        if (RxPacket.Data[0] == 0x00)
        {
            cmdConfirmation.status = CMD_Status_Success;
        }
        else
        {
            cmdConfirmation.status = CMD_Status_Failed;
        }
        cmdConfirmation.cmd = RxPacket.Cmd;
    }
    break;

    case Metis_CMD_DATA_IND:
    {
        /* the call of the RXcallback strongly depends on the configuration of the module*/
    	if(RXcallback != NULL)
    	{
			if(rssi_enable == 0x01)
			{
				/* the following implementation expects that the RSSI_Enable usersetting is enabled */
				RxPacket.Length = RxPacket.Length - 1;
				RXcallback(&RxPacket.Length, RxPacket.Length + 1, CalculateRSSIValue(RxPacket.Data[RxPacket.Length]));
			}
			else
			{
				/* the following implementation expects that the RSSI_Enable usersetting is disabled */
				RXcallback(&RxPacket.Length, RxPacket.Length + 1, (int8_t)RSSIINVALID);
			}
    	}
    }
    break;

    case Metis_CMD_GET_CNF:
    {
        /* Data[0] contains memory postion of usersetting
         * Data[1] contains length of parameter, which is depending on usersetting
         * On success mode responds with usersetting, length of parameter and paramter
         */
        switch(RxPacket.Data[0])
        {
        /* usersettings with value length of 1 byte */
        case(Metis_USERSETTING_MEMPOSITION_UART_CMD_OUT_ENABLE):
        case(Metis_USERSETTING_MEMPOSITION_APP_AES_ENABLE):
        case(Metis_USERSETTING_MEMPOSITION_DEFAULTRFTXPOWER):
        case(Metis_USERSETTING_MEMPOSITION_RSSI_ENABLE):
        case(Metis_USERSETTING_MEMPOSITION_MODE_PRESELECT):
        {
            /* check if correct usersetting was changed and if length corresponds to usersetting */
            if((usConfirmation.memoryPosition == RxPacket.Data[0]) && (usConfirmation.lengthGetRequest == RxPacket.Data[1]))
            {
                cmdConfirmation.status = CMD_Status_Success;
            }
            else
            {
                cmdConfirmation.status = CMD_Status_Failed;
            }
            cmdConfirmation.cmd = RxPacket.Cmd;
        }
        break;
        /* usersettings with value length of 2 byte*/
        case(Metis_USERSETTING_MEMPOSITION_CFG_FLAGS):
        {
            /* check if correct usersetting was changed and if length corresponds to usersetting */
            if((usConfirmation.memoryPosition == RxPacket.Data[0]) && (usConfirmation.lengthGetRequest == RxPacket.Data[1]))
            {
                cmdConfirmation.status = CMD_Status_Success;
            }
            else
            {
                cmdConfirmation.status = CMD_Status_Failed;
            }
            cmdConfirmation.cmd = RxPacket.Cmd;
        }
        break;

        default:
            break;
        }
    }
    break;

    case Metis_CMD_SET_CNF:
    {
        /* check whether the module returns success */
        if (RxPacket.Data[0] == 0x00)
        {
            cmdConfirmation.status = CMD_Status_Success;
        }
        else
        {
            cmdConfirmation.status = CMD_Status_Failed;
        }
        cmdConfirmation.cmd = RxPacket.Cmd;
    }
    break;

    case Metis_CMD_GET_SERIALNO_CNF:
    {
        /* check whether the module returns serial number of 4 bytes */
        if (RxPacket.Length == 4)
        {
            cmdConfirmation.status = CMD_Status_Success;
        }
        else
        {
            cmdConfirmation.status = CMD_Status_Failed;
        }
        cmdConfirmation.cmd = RxPacket.Cmd;
    }
    break;

    case Metis_CMD_GET_FWRELEASE_CNF:
    {
        /* check whether the module returns firmware version of 3 bytes */
        if (RxPacket.Length == 3)
        {
            cmdConfirmation.status = CMD_Status_Success;
        }
        else
        {
            cmdConfirmation.status = CMD_Status_Failed;
        }
        cmdConfirmation.cmd = RxPacket.Cmd;
    }
    break;

    case Metis_CMD_SETUARTSPEED_CNF:
    {
        /* check whether the module returns success*/
        if(RxPacket.Data[0] == 0x00)
        {
            cmdConfirmation.status = CMD_Status_Success;
        }
        else
        {
            cmdConfirmation.status = CMD_Status_Failed;
        }
        cmdConfirmation.cmd = RxPacket.Cmd;
    }
    break;

    case Metis_CMD_FACTORYRESET_CNF:
    {
        /* check whether the module returns success*/
        if(RxPacket.Data[0] == 0x00)
        {
            cmdConfirmation.status = CMD_Status_Success;
        }
        else
        {
            cmdConfirmation.status = CMD_Status_Failed;
        }
        cmdConfirmation.cmd = RxPacket.Cmd;
    }
    break;

    default:
        break;
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

/* function that waits for the return value of Metis (*_CNF), when a command (*_REQ) was sent before */
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
static bool FillChecksum(uint8_t* array, uint8_t length)
{
    bool ret = false;

    if ((length >= 4) && (array[0] == CMD_STX))
    {
        uint8_t checksum = 0;
        uint8_t payload_length = array[2];
        int i = 0;
        for (i = 0;
                i < payload_length + 3;
                i++)
        {
            checksum ^= array[i];
        }
        array[payload_length + 3] = checksum;
        ret = true;
    }
    return ret;
}


static bool InitPins(Metis_Pins_t pins)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOA_CLK_ENABLE();

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(GPIOA, pins.reset.pin, GPIO_PIN_RESET);

	GPIO_InitStruct.Pin = pins.reset.pin;
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
		/* wait for SFD */
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
		/* length field */
		RxByteCounter++;
		BytesToReceive = (RxBuffer[RxByteCounter - 1] + 4); /* len + crc + sfd + cmd */
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
 *Initialize the AMB module for serial interface
 *
 *input:
 * -baudrate:       baudrate of the interface
 * -flow_control:   enable/disable flowcontrol
 * -frequency:      frequency used by the AMBER module(AMB8xxx-M uses 868Mhz, AMB36xx-M uses 169MHz)
 * -mode:           wMBus-mode preselect of the AMBER module
 * -enable_rssi:    enable rssi in data reception
 * -RXcb:           RX callback function
 *
 *Caution:  the baudrate parameter must match the module's baudrate to perform a successful communication
 *          updating this parameter during runtime may lead to communication errors
 *          the mode parameter must match the other participant of the RF communication
 *          Check Manual of the wMBus AMB modules for the suitable modes
 *
 *return true if initialization succeeded
 *       false otherwise
 */
bool Metis_Init(uint32_t baudrate, FlowControl_t flow_control, Metis_Frequency_t freq, Metis_Mode_Preselect_t mode, bool enable_rssi, void(*RXcb)(uint8_t*,uint8_t,int8_t))
{
	/* set frequency used by module */
	frequency = freq;

    /* set RX callback function */
    RXcallback = RXcb;

	/* set rssi_enable */
	rssi_enable = enable_rssi;

	pins.reset.port = GPIOA;
	pins.reset.pin = GPIO_PIN_10;
	if (false == InitPins(pins))
	{
		/* error */
		return false ;
	}
    SetPin(pins.reset, SetPin_Out_High);
	
    UART_Init(baudrate, flow_control);
    delay(1000);

	/* set recommended settings as described in the manual section 5.1 */

	/* enable uartOutEnable to print out received frames
	 * Setting is written to flash so write only if necessary
	 */
	uint8_t uartEnable;
	bool ret = Metis_GetUartOutEnable(&uartEnable);
	if(uartEnable != 1)
	{
		delay(50);
		if(Metis_SetUartOutEnable(1))
		{
			delay(50);
		}
		else
		{
			fprintf(stdout, "Set UART_CMD_OUT_MODE failed\n");
			Metis_Deinit();
			return false;
		}
	}

	/* enable rssi to be added to received frames
	 * Setting is written to flash so write only if necessary
	 */
	uint8_t rssi;
	ret = Metis_GetRSSIEnable(&rssi);
	if(rssi != rssi_enable)
	{
		delay(50);
		if(Metis_SetRSSIEnable(rssi_enable ? 1 : 0))
		{
			delay(50);
		}
		else
		{
			fprintf(stdout, "Set RSSI failed\n");
			Metis_Deinit();
			return false;
		}
	}

	/* disable AES encryption
	 * Setting is written to flash so write only if necessary
	 */
	uint8_t aesEnable;
	ret = Metis_GetAESEnable(&aesEnable);
	if(aesEnable != 0)
	{
		delay(50);
		if(Metis_SetAESEnable(0))
		{
			delay(50);
		}
		else
		{
			fprintf(stdout, "Set AESEnable failed\n");
			Metis_Deinit();
			return false;
		}
	}

	/* set mode preselect
	 * Setting is written to flash so write only if necessary
	 */
	uint8_t modePreselect;
	ret = Metis_GetModePreselect(&modePreselect);
	if(modePreselect != mode)
	{
		delay(50);
		if(Metis_SetModePreselect(mode))
		{
			delay(50);
		}
		else
		{
			fprintf(stdout, "Set mode preselect failed\n");
			Metis_Deinit();
			return false;
		}
	}

	/* Reset module to apply changes */
	if(Metis_Reset())
	{
		delay(300);
	}
	else
	{
		fprintf(stdout, "Reset failed\n");
		Metis_Deinit();
		return false;
	}
	return true;
}

/*
 * Deinitialize the Metis interface
 *
 * return true if deinitialization succeeded
 *        false otherwise
 */
bool Metis_Deinit()
{
	/* close the communication interface to the module */
	UART_DeInit();

    /* deinit pins */
    DeinitPin(pins.reset);

    /* deinit RX callback */
    RXcallback = NULL;

    return true;
}

/*
 *Reset the Metis by pin
 *
 *return true if reset succeeded
 *       false otherwise
 */
bool Metis_PinReset()
{
    SetPin(pins.reset, SetPin_Out_Low);
    delay(5);
    SetPin(pins.reset, SetPin_Out_High);

    return true;
}

/*
 *Reset the Metis by command
 *
 *return true if reset succeeded
 *       false otherwise
 */
bool Metis_Reset()
{
    bool ret = false;

    /* fill CMD_ARRAY packet */
    uint8_t CMD_ARRAY[4];
    CMD_ARRAY[0] = CMD_STX;
    CMD_ARRAY[1] = Metis_CMD_RESET_REQ;
    CMD_ARRAY[2] = 0x00;
    if(FillChecksum(CMD_ARRAY,sizeof(CMD_ARRAY)))
    {
        /* now send CMD_ARRAY */
        UART_Transmit(CMD_ARRAY,sizeof(CMD_ARRAY));

        /* wait for cnf */
        return Wait4CNF(CMD_WAIT_TIME, Metis_CMD_RESET_CNF, CMD_Status_Success, true);
    }
    return ret;
}

/*
 *Factory reset the Metis
 *
 *note: use them only in rare cases, since flash can be updated only a limited number times
 *      the factory reset must be followed by a reset such that the changes become effective
 *
 *return true if factory reset succeeded
 *       false otherwise
 */
bool Metis_FactoryReset()
{
    bool ret = false;

    /* fill CMD_ARRAY packet */
    uint8_t CMD_ARRAY[4];
    CMD_ARRAY[0] = CMD_STX;
    CMD_ARRAY[1] = Metis_CMD_FACTORYRESET_REQ;
    CMD_ARRAY[2] = 0x00;
    if(FillChecksum(CMD_ARRAY,sizeof(CMD_ARRAY)))
    {
        /* now send CMD_ARRAY */
        UART_Transmit(CMD_ARRAY,sizeof(CMD_ARRAY));

        /* wait for cnf */
        ret = Wait4CNF(CMD_WAIT_TIME, Metis_CMD_FACTORYRESET_CNF, CMD_Status_Success, true);
    }
    return ret;
}

/*
 *sets the baudrate of the module
 *input:
 *  -baudrate: baudrate
 *note: use them only in rare cases, since flash can be updated only a limited number times
 *
 *return true if setting baudrate succeeded
 *       false otherwise
 */
bool Metis_SetUartSpeed(Metis_UartBaudrate_t baudrate)
{
    bool ret = false;

    /* fill REQ */
    uint8_t CMD_ARRAY[5];
    CMD_ARRAY[0] = CMD_STX;
    CMD_ARRAY[1] = Metis_CMD_SETUARTSPEED;
    CMD_ARRAY[2] = 0x01;
    CMD_ARRAY[3] = baudrate;

    if(FillChecksum(CMD_ARRAY,sizeof(CMD_ARRAY)))
    {
        /* now send CMD_ARRAY */
        UART_Transmit(CMD_ARRAY,sizeof(CMD_ARRAY));

        /* wait for cnf */
        ret = Wait4CNF(CMD_WAIT_TIME, Metis_CMD_SETUARTSPEED_CNF, CMD_Status_Success, true);
    }
    return ret;
}

/*
 *Request the current Metis settings
 *
 *input:
 * -us: user setting to be requested
 *
 *output:
 * -response: pointer of the memory to put the request content
 * -response_length: length of the request content
 *
 *return true if request succeeded
 *       false otherwise
 */
bool Metis_Get(Metis_UserSettings_t us, uint8_t* response, uint8_t* response_length)
{
    bool ret = false;

    /* fill CMD_ARRAY packet */
    uint8_t CMD_ARRAY[6];
    CMD_ARRAY[0] = CMD_STX;
    CMD_ARRAY[1] = Metis_CMD_GET_REQ;
    CMD_ARRAY[2] = 0x02;
    CMD_ARRAY[3] = us;

    switch(us)
    {
    /* usersettings with value length 1 */
    case(Metis_USERSETTING_MEMPOSITION_UART_CMD_OUT_ENABLE):
    case(Metis_USERSETTING_MEMPOSITION_APP_AES_ENABLE):
    case(Metis_USERSETTING_MEMPOSITION_DEFAULTRFTXPOWER):
    case(Metis_USERSETTING_MEMPOSITION_RSSI_ENABLE):
    case(Metis_USERSETTING_MEMPOSITION_MODE_PRESELECT):
    {
        CMD_ARRAY[4] = 0x01;
    }
    break;
    /* usersettings with value length 2 */
    case(Metis_USERSETTING_MEMPOSITION_CFG_FLAGS):
    {
        CMD_ARRAY[4] = 0x02;
    }
    break;
    default:
        break;
    }

    if(FillChecksum(CMD_ARRAY,sizeof(CMD_ARRAY)))
    {
        usConfirmation.memoryPosition = us;
        usConfirmation.lengthGetRequest = CMD_ARRAY[4];

        /* now send CMD_ARRAY */
        UART_Transmit(CMD_ARRAY,sizeof(CMD_ARRAY));

        /* wait for cnf */
        if (Wait4CNF(CMD_WAIT_TIME, Metis_CMD_GET_CNF, CMD_Status_Success, true))
        {
            int length = RxPacket.Length - 2;
            memcpy(response,&RxPacket.Data[2],length);
            *response_length = length;
            ret = true;
        }
        usConfirmation.memoryPosition = -1;
        usConfirmation.lengthGetRequest = -1;
    }
    return ret;
}

/*
 *Request multiple of the current Metis settings
 *
 *input:
 * -startAddress: first usersetting to be read
 * -lengthToRead: Length of memory to be read
 *
 *output:
 * -response: pointer of the memory to put the request content
 * -response_length: length of the request content
 *
 *return true if request succeeded
 *       false otherwise
 */
bool Metis_GetMultiple(uint8_t startAddress, uint8_t lengthToRead, uint8_t *response, uint8_t *response_length)
{
    bool ret = false;

    uint8_t CMD_ARRAY[6];
    CMD_ARRAY[0] = CMD_STX;
    CMD_ARRAY[1] = Metis_CMD_GET_REQ;
    CMD_ARRAY[2] = 0x02;
    CMD_ARRAY[3] = startAddress;
    CMD_ARRAY[4] = lengthToRead;

    if(FillChecksum(CMD_ARRAY,sizeof(CMD_ARRAY)))
    {
        usConfirmation.memoryPosition = startAddress;
        usConfirmation.lengthGetRequest = lengthToRead;

        /* now send CMD_ARRAY */
        UART_Transmit(CMD_ARRAY,sizeof(CMD_ARRAY));

        /* wait for cnf */
        if (Wait4CNF(CMD_WAIT_TIME, Metis_CMD_GET_CNF, CMD_Status_Success, true))
        {
            int length = RxPacket.Length - 2;
            memcpy(response,&RxPacket.Data[2],length);
            *response_length = length;
            ret = true;
        }
        usConfirmation.memoryPosition = -1;
        usConfirmation.lengthGetRequest = -1;
    }
    return ret;
}

/*
 *Set a special Metis setting
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
bool Metis_Set(Metis_UserSettings_t us, uint8_t* value, uint8_t length)
{
    bool ret = false;

    /* fill CMD_ARRAY packet */
    uint8_t CMD_ARRAY[length + 6];
    CMD_ARRAY[0] = CMD_STX;
    CMD_ARRAY[1] = Metis_CMD_SET_REQ;
    CMD_ARRAY[2] = (2 + length);
    CMD_ARRAY[3] = us;
    CMD_ARRAY[4] = length;
    memcpy(&CMD_ARRAY[5],value,length);
    if(FillChecksum(CMD_ARRAY,sizeof(CMD_ARRAY)))
    {
        /* now send CMD_ARRAY */
        UART_Transmit(CMD_ARRAY,sizeof(CMD_ARRAY));

        /* wait for cnf */
        ret = Wait4CNF(CMD_WAIT_TIME, Metis_CMD_SET_CNF, CMD_Status_Success, true);
    }
    return ret;
}

/*
 *Request the 3 byte firmware version
 *
 *output:
 * -fw: pointer to the 3 byte firmware version
 *
 *return true if request succeeded
 *       false otherwise
 */
bool Metis_GetFirmwareVersion(uint8_t* fw)
{
    uint8_t CMD_ARRAY[4];
    CMD_ARRAY[0] = CMD_STX;
    CMD_ARRAY[1] = Metis_CMD_GET_FWRELEASE;
    CMD_ARRAY[2] = 0;

    if(FillChecksum(CMD_ARRAY,sizeof(CMD_ARRAY)))
    {
        /* now send CMD_ARRAY */
        UART_Transmit(CMD_ARRAY,sizeof(CMD_ARRAY));

        /* wait for cnf */
        if (Wait4CNF(CMD_WAIT_TIME, Metis_CMD_GET_FWRELEASE_CNF, CMD_Status_Success, true))
        {
            memcpy(fw,&RxPacket.Data[0],RxPacket.Length);
            return true;
        }
    }
    return false;
}

/*
 *Request the 4 byte serial number
 *
 *output:
 * -sn: pointer to the 4 byte serial number
 *
 *return true if request succeeded
 *       false otherwise
 */
bool Metis_GetSerialNumber(uint8_t* sn)
{
    uint8_t CMD_ARRAY[4];
    CMD_ARRAY[0] = CMD_STX;
    CMD_ARRAY[1] = Metis_CMD_GET_SERIALNO;
    CMD_ARRAY[2] = 0;

    if(FillChecksum(CMD_ARRAY,sizeof(CMD_ARRAY)))
    {
        /* now send CMD_ARRAY */
        UART_Transmit(CMD_ARRAY,sizeof(CMD_ARRAY));

        /* wait for cnf */
        if (Wait4CNF(CMD_WAIT_TIME, Metis_CMD_GET_SERIALNO_CNF, CMD_Status_Success, true))
        {
            memcpy(sn,&RxPacket.Data[0], RxPacket.Length);
            return true;
        }
    }
    return false;
}

/*
 *Request the default TX power
 *
 *output:
 * -txpower: pointer to the TXpower
 *
 *return true if request succeeded
 *       false otherwise
 */
bool Metis_GetDefaultTXPower(int8_t* txpower)
{
    uint8_t length;

    if(Metis_Get(Metis_USERSETTING_MEMPOSITION_DEFAULTRFTXPOWER, (uint8_t*)txpower, &length))
    {
        return true;
    }
    else
    {
        *txpower = TXPOWERINVALID;
        return false;
    }
}

/*
 *Get the Uart Out Enable byte
 *output:
 * -uartEnable: pointer to uartEnable
 *
 *return true if request succeeded
 *       false otherwise
 */
bool Metis_GetUartOutEnable(uint8_t* uartEnable)
{
    uint8_t length;
    return Metis_Get(Metis_USERSETTING_MEMPOSITION_UART_CMD_OUT_ENABLE, uartEnable, &length);
}

/*
 *Get the RSSI Enable byte
 *output:
 * -rssiEnable: pointer to rssiEnable
 *
 *return true if request succeeded
 *       false otherwise
 */
bool Metis_GetRSSIEnable(uint8_t* rssiEnable)
{
    uint8_t length;
    return Metis_Get(Metis_USERSETTING_MEMPOSITION_RSSI_ENABLE, rssiEnable, &length);
}

/*
 *Get the M-Bus mode preselect byte
 *output:
 * -modePreselect: Pointer to mode Preselect
 *
 *return true if request succeeded
 *       false otherwise
 */

bool Metis_GetModePreselect(uint8_t* modePreselect)
{
    uint8_t length;
    return Metis_Get(Metis_USERSETTING_MEMPOSITION_MODE_PRESELECT, modePreselect, &length);
}

/*
 *Get the AES Enable byte
 *output:
 * -aesEnable: pointer to aes enable
 *
 *return true if request succeeded
 *       false otherwise
 */
bool Metis_GetAESEnable(uint8_t* aesEnable)
{
    uint8_t length;
    return Metis_Get(Metis_USERSETTING_MEMPOSITION_APP_AES_ENABLE, aesEnable, &length);
}

/*
 *Set the default TX power
 *
 *input:
 * -txpower: TXpower
 *
 *note: reset the module after the adaption of the setting such that it can take effect
 *note: use this function only in rare case, since flash can be updated only a limited number times
 *note: use Metis_SetVolatile_TXPower for frequent adaption of the TX power
 *
 *return true if request succeeded
 *       false otherwise
 */
bool Metis_SetDefaultTXPower(int8_t txpower)
{
    /* check for invalid power */
    if((txpower < -11) || (txpower > 15))
    {
        /* invalid power */
        return false;
    }
    return Metis_Set(Metis_USERSETTING_MEMPOSITION_DEFAULTRFTXPOWER, (uint8_t*)&txpower, 1);
}

/*
 *Set the Uart Out Enable
 *
 *input:
 * -uartEnable: uart enable
 *
 *note: reset the module after the adaption of the setting such that it can take effect
 *note: use this function only in rare case, since flash can be updated only a limited number times
 *
 *return true if request succeeded
 *       false otherwise
 */
bool Metis_SetUartOutEnable(uint8_t uartEnable)
{
    if((uartEnable != 0) && (uartEnable != 1))
    {
        return false;
    }
    return Metis_Set(Metis_USERSETTING_MEMPOSITION_UART_CMD_OUT_ENABLE, &uartEnable, 1);
}
/*
 *Set the RSSI Enable byte
 *
 *input:
 * -rssiEnable: rssi Enable
 *
 *note: reset the module after the adaption of the setting such that it can take effect
 *note: use this function only in rare case, since flash can be updated only a limited number times
 *
 *return true if request succeeded
 *       false otherwise
 */
bool Metis_SetRSSIEnable(uint8_t rssiEnable)
{
    if((rssiEnable !=0) && (rssiEnable != 1))
    {
        return false;
    }
    return Metis_Set(Metis_USERSETTING_MEMPOSITION_RSSI_ENABLE, &rssiEnable,1);
}

/*
 *Set the AES Enable byte
 *
 *input:
 * -aesEnable: AES enable
 *
 *note: functions for AES are not implemented. Can only be used for disabling AES.
 *note: reset the module after the adaption of the setting such that it can take effect
 *note: use this function only in rare case, since flash can be updated only a limited number times
 *
 *return true if request succeeded
 *       false otherwise
 */
bool Metis_SetAESEnable(uint8_t aesEnable)
{
    if(aesEnable !=0)
    {
        return false;
    }
    return Metis_Set(Metis_USERSETTING_MEMPOSITION_APP_AES_ENABLE, &aesEnable,1);
}

/*
 *Set the default M-Bus mode preselect
 *
 *input:
 * -modePreselect: M-Bus mode preselect
 *
 *note: reset the module after the adaption of the setting such that it can take effect
 *note: use this function only in rare case, since flash can be updated only a limited number times
 *note: use Metis_SetVolatile_ModePreselect for frequent adaption of the channel

 *return true if request succeeded
 *       false otherwise
 */
bool Metis_SetModePreselect(Metis_Mode_Preselect_t modePreselect)
{
    return Metis_Set(Metis_USERSETTING_MEMPOSITION_MODE_PRESELECT, (uint8_t*)&modePreselect, 1);
}

/*
 *Set the volatile mode preselect
 *
 *input:
 * -modePreselect: new mode preselect value
 *
 *return true if request succeeded
 *       false otherwise
 */
bool Metis_SetVolatile_ModePreselect(Metis_Mode_Preselect_t modePreselect)
{
    bool ret = false;

    /* fill CMD_ARRAY packet */
    uint8_t CMD_ARRAY[5];
    CMD_ARRAY[0] = CMD_STX;
    CMD_ARRAY[1] = Metis_CMD_SET_MODE;
    CMD_ARRAY[2] = 0x01;
    CMD_ARRAY[3] = modePreselect;
    if(FillChecksum(CMD_ARRAY,sizeof(CMD_ARRAY)))
    {
        /* now send CMD_ARRAY */
        UART_Transmit(CMD_ARRAY,sizeof(CMD_ARRAY));

        /* wait for cnf*/
        ret = Wait4CNF(CMD_WAIT_TIME, Metis_CMD_SET_CNF, CMD_Status_Success, true);
    }
    return ret;
}

/*
 *Transmit data using the configured settings
 *
 *input:
 * -payload: pointer to the data
 *
 *return true if request succeeded
 *       false otherwise
 */
bool Metis_Transmit(uint8_t* payload)
{
    bool ret = false;
    uint8_t length = *payload; /* first byte of wM-BUS frame is length field */

    if(length > MAX_PAYLOAD_LENGTH)
    {
        fprintf(stdout, "Data exceeds maximal payload length");
        return false;
    }

    /* mode preselect C2/T2 for frequency 868 is not suitable for sending frames */
    if((frequency == MBus_Frequency_868))
    {
        uint8_t modePreselect;
        Metis_GetModePreselect(&modePreselect);
        if(modePreselect == MBus_Mode_868_C2_T2_other)
        {
            /* module can not send in this mode. */
            fprintf(stdout, "Mode Preselect %x is not suitable for transmiting\n", modePreselect);
            return false;
        }
    }


    /* fill CMD_ARRAY packet */
    uint8_t CMD_ARRAY[length + 4];
    CMD_ARRAY[0] = CMD_STX;
    CMD_ARRAY[1] = Metis_CMD_DATA_REQ;
    CMD_ARRAY[2] = length;
    memcpy(&CMD_ARRAY[3],&payload[1],length);
    if(FillChecksum(CMD_ARRAY,sizeof(CMD_ARRAY)))
    {
        /* now send CMD_ARRAY */
        UART_Transmit(CMD_ARRAY,sizeof(CMD_ARRAY));

        /* wait for cnf */
        ret = Wait4CNF(CMD_WAIT_TIME, Metis_CMD_DATA_CNF, CMD_Status_Success, true);
    }
    return ret;
}

/*
 *Configure the Metis
 *
 *input:
 * -config: pointer to the configuration struct
 * -config_length: length of the configuration struct
 * -factory_reset: apply a factory reset before or not
 *
 *return true if request succeeded
 *       false otherwise
*/
bool Metis_Configure(Metis_Configuration_t* config, uint8_t config_length, bool factory_reset)
{
    int i = 0;
    uint8_t help_length;
    uint8_t help[MAX_USERSETTING_LENGTH];

    if(factory_reset)
    {
        /* perform a factory reset */
        if(false == Metis_FactoryReset())
        {
            /* error */
            return false;
        }
    }
    delay(500);

    /* now check all settings and update them if necessary */
    for(i=0; i<config_length; i++)
    {
        /* read current value */
        if(false == Metis_Get(config[i].usersetting, help, &help_length))
        {
            /* error */
            return false;
        }
        delay(200);

        /* check the value read out */
        if(help_length != config[i].value_length)
        {
            /* error, length does not match */
            return false;
        }
        if(memcmp(help,config[i].value,config[i].value_length) != 0)
        {
            /* read value is not up to date, thus write the new value */
            if(false == Metis_Set(config[i].usersetting, config[i].value, config[i].value_length))
            {
                /* error */
                return false;
            }
        }
        delay(200);
    }

    /* reset to take effect of the updated parameters */
    if(false == Metis_PinReset())
    {
        return false;
    }
    return true;
}
