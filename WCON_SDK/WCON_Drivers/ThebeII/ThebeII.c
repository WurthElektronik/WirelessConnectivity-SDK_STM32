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
 * @brief Thebe-II driver source file.
 */

#include "ThebeII.h"

#include <stdio.h>
#include <string.h>

#include "../global/global.h"

typedef enum ThebeII_Pin_t
{
    ThebeII_Pin_Reset,
    ThebeII_Pin_SleepWakeUp,
    ThebeII_Pin_Boot,
    ThebeII_Pin_Mode,
    ThebeII_Pin_Count
} ThebeII_Pin_t;

#define CMD_WAIT_TIME 500
#define CNFINVALID 255
#define MAX_PAYLOAD_LENGTH 224
#define MAX_DATA_BUFFER 255
#define TXPOWERINVALID -128
#define RSSIINVALID -128
#define CHANNELINVALID -1

#define CMD_STX 0x02
#define THEBEII_CMD_TYPE_REQ (0 << 6)
#define THEBEII_CMD_TYPE_CNF (1 << 6)
#define THEBEII_CMD_TYPE_IND (2 << 6)
#define THEBEII_CMD_TYPE_RSP (3 << 6)

#define THEBEII_CMD_DATA 0x00
#define THEBEII_CMD_DATA_REQ (THEBEII_CMD_DATA | THEBEII_CMD_TYPE_REQ)
#define THEBEII_CMD_DATA_CNF (THEBEII_CMD_DATA | THEBEII_CMD_TYPE_CNF)
#define THEBEII_CMD_REPEAT_IND (THEBEII_CMD_DATA | THEBEII_CMD_TYPE_IND)

#define THEBEII_CMD_DATAEX 0x01
#define THEBEII_CMD_DATAEX_REQ (THEBEII_CMD_DATAEX | THEBEII_CMD_TYPE_REQ)
#define THEBEII_CMD_DATAEX_IND (THEBEII_CMD_DATAEX | THEBEII_CMD_TYPE_IND)

#define THEBEII_CMD_RESET 0x05
#define THEBEII_CMD_RESET_REQ (THEBEII_CMD_RESET | THEBEII_CMD_TYPE_REQ)
#define THEBEII_CMD_RESET_CNF (THEBEII_CMD_RESET | THEBEII_CMD_TYPE_CNF)
#define THEBEII_CMD_RESET_IND (THEBEII_CMD_RESET | THEBEII_CMD_TYPE_IND)

#define THEBEII_CMD_SET_CHANNEL 0x06
#define THEBEII_CMD_SET_CHANNEL_REQ (THEBEII_CMD_SET_CHANNEL | THEBEII_CMD_TYPE_REQ)
#define THEBEII_CMD_SET_CHANNEL_CNF (THEBEII_CMD_SET_CHANNEL | THEBEII_CMD_TYPE_CNF)

#define THEBEII_CMD_SET_DESTNETID 0x07
#define THEBEII_CMD_SET_DESTNETID_REQ (THEBEII_CMD_SET_DESTNETID | THEBEII_CMD_TYPE_REQ)
#define THEBEII_CMD_SET_DESTNETID_CNF (THEBEII_CMD_SET_DESTNETID | THEBEII_CMD_TYPE_CNF)

#define THEBEII_CMD_SET_DESTADDR 0x08
#define THEBEII_CMD_SET_DESTADDR_REQ (THEBEII_CMD_SET_DESTADDR | THEBEII_CMD_TYPE_REQ)
#define THEBEII_CMD_SET_DESTADDR_CNF (THEBEII_CMD_SET_DESTADDR | THEBEII_CMD_TYPE_CNF)

#define THEBEII_CMD_SET 0x09
#define THEBEII_CMD_SET_REQ (THEBEII_CMD_SET | THEBEII_CMD_TYPE_REQ)
#define THEBEII_CMD_SET_CNF (THEBEII_CMD_SET | THEBEII_CMD_TYPE_CNF)

#define THEBEII_CMD_GET 0x0A
#define THEBEII_CMD_GET_REQ (THEBEII_CMD_GET | THEBEII_CMD_TYPE_REQ)
#define THEBEII_CMD_GET_CNF (THEBEII_CMD_GET | THEBEII_CMD_TYPE_CNF)

#define THEBEII_CMD_RSSI 0x0D
#define THEBEII_CMD_RSSI_REQ (THEBEII_CMD_RSSI | THEBEII_CMD_TYPE_REQ)
#define THEBEII_CMD_RSSI_CNF (THEBEII_CMD_RSSI | THEBEII_CMD_TYPE_CNF)

#define THEBEII_CMD_SHUTDOWN 0x0E
#define THEBEII_CMD_SHUTDOWN_REQ (THEBEII_CMD_SHUTDOWN | THEBEII_CMD_TYPE_REQ)
#define THEBEII_CMD_SHUTDOWN_CNF (THEBEII_CMD_SHUTDOWN | THEBEII_CMD_TYPE_CNF)

#define THEBEII_CMD_STANDBY 0x0F
#define THEBEII_CMD_STANDBY_REQ (THEBEII_CMD_STANDBY | THEBEII_CMD_TYPE_REQ)
#define THEBEII_CMD_STANDBY_CNF (THEBEII_CMD_STANDBY | THEBEII_CMD_TYPE_CNF)
#define THEBEII_CMD_STANDBY_IND (THEBEII_CMD_STANDBY | THEBEII_CMD_TYPE_IND)

#define THEBEII_CMD_SET_PAPOWER 0x11
#define THEBEII_CMD_SET_PAPOWER_REQ (THEBEII_CMD_SET_PAPOWER | THEBEII_CMD_TYPE_REQ)
#define THEBEII_CMD_SET_PAPOWER_CNF (THEBEII_CMD_SET_PAPOWER | THEBEII_CMD_TYPE_CNF)

#define THEBEII_CMD_FACTORY_RESET 0x12
#define THEBEII_CMD_FACTORY_RESET_REQ (THEBEII_CMD_FACTORY_RESET | THEBEII_CMD_TYPE_REQ)
#define THEBEII_CMD_FACTORY_RESET_CNF (THEBEII_CMD_FACTORY_RESET | THEBEII_CMD_TYPE_CNF)


/* AMBER test commands */
#define THEBEII_CMD_PINGDUT 0x1F
#define THEBEII_CMD_PINGDUT_REQ (THEBEII_CMD_PINGDUT | THEBEII_CMD_TYPE_REQ)
#define THEBEII_CMD_PINGDUT_CNF (THEBEII_CMD_PINGDUT | THEBEII_CMD_TYPE_CNF)

/* Masks for FLAGS */
/* Sniffer mode is indicated by bit 1 of cfg-flags */
#define THEBEII_CFGFLAGS_SNIFFERMODEENABLE 0x0001
#define THEBEII_RPFLAGS_REPEATERENABLE 0X0001

/**
 * @brief Type used to check the response, when a command was sent to the ThebeII.
 */
typedef enum ThebeII_CMD_Status_t
{
    CMD_Status_Success = 0x00,
    CMD_Status_Failed,
    CMD_Status_Invalid,
    CMD_Status_Reset,
} ThebeII_CMD_Status_t;

typedef struct
{
    uint8_t Stx;
    uint8_t Cmd;
    uint8_t Length;
    uint8_t Data[MAX_DATA_BUFFER + 1]; /* +1 for the CS */
} ThebeII_CMD_Frame_t;

typedef struct
{
    uint8_t cmd;                    /* variable to check if correct CMD has been confirmed */
    ThebeII_CMD_Status_t status;    /* variable used to check the response (*_CNF), when a request (*_REQ) was sent to the ThebeII */
} ThebeII_CMD_Confirmation_t;


/**************************************
 *          Static variables          *
 **************************************/

static ThebeII_CMD_Frame_t RxPacket;                      /* data buffer for RX */

#define CMDCONFIRMATIONARRAY_LENGTH 2
static ThebeII_CMD_Confirmation_t cmdConfirmation_array[CMDCONFIRMATIONARRAY_LENGTH];
static uint8_t channelVolatile = CHANNELINVALID;           /* variable used to check if setting the channel was successful */
static uint8_t powerVolatile = TXPOWERINVALID;             /* variable used to check if setting the TXPower was successful */
static ThebeII_AddressMode_t addressmode = AddressMode_0;  /* initial address mode */
static WE_Pin_t ThebeII_pins[ThebeII_Pin_Count] = {0};
static uint8_t checksum = 0;
static uint8_t RxByteCounter = 0;
static uint8_t BytesToReceive = 0;
static uint8_t RxBuffer[sizeof(ThebeII_CMD_Frame_t)]; /* data buffer for RX */
void(*RxCallback)(uint8_t*,uint8_t,uint8_t,uint8_t,uint8_t,int8_t); /* callback function */

/**************************************
 *         Static functions           *
 **************************************/

/**
 * @brief Interpret the valid received UART data packet.
 */
static void HandleRxPacket(uint8_t*RxBuffer)
{
    ThebeII_CMD_Confirmation_t cmdConfirmation;
    cmdConfirmation.cmd = CNFINVALID;
    cmdConfirmation.status = CMD_Status_Invalid;

    uint8_t cmd_length = RxBuffer[2];
    memcpy((uint8_t*)&RxPacket,RxBuffer,cmd_length + 4);

    switch (RxPacket.Cmd)
    {
    case THEBEII_CMD_FACTORY_RESET_CNF:
    {
        /* check whether the module returns success */
        if ((RxPacket.Data[0] == 0x00))
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

    case THEBEII_CMD_RESET_CNF:
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

    case THEBEII_CMD_RESET_IND:
    {
        cmdConfirmation.status = CMD_Status_Success;
        cmdConfirmation.cmd = THEBEII_CMD_RESET_IND;
    }
    break;

    case THEBEII_CMD_STANDBY_IND:
    {
        cmdConfirmation.status = CMD_Status_Success;
        cmdConfirmation.cmd = THEBEII_CMD_STANDBY_IND;
    }
    break;

    case THEBEII_CMD_SHUTDOWN_CNF:
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

    case THEBEII_CMD_STANDBY_CNF:
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

    case THEBEII_CMD_DATA_CNF:
    {
        /* check whether the module returns success */
        if (RxPacket.Data[0] == 0x00)
        {
            /* transmission success, ACK received if enabled */
            cmdConfirmation.status = CMD_Status_Success;
        }
        else
        {
            /* transmission failed, no ACK received if enabled */
            cmdConfirmation.status = CMD_Status_Failed;
        }
        cmdConfirmation.cmd = RxPacket.Cmd;
    }
    break;

    case THEBEII_CMD_GET_CNF:
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

    case THEBEII_CMD_SET_CNF:
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

    case THEBEII_CMD_DATAEX_IND:
    {
        /* data received, give it to the RxCallback function */
        if (RxCallback != NULL)
        {
            switch (addressmode)
            {
            case AddressMode_0:
            {
                RxCallback(&RxPacket.Data[0], RxPacket.Length - 1, THEBEII_BROADCASTADDRESS, THEBEII_BROADCASTADDRESS, THEBEII_BROADCASTADDRESS, (int8_t)RxPacket.Data[RxPacket.Length-1]);
            }
            break;

            case AddressMode_1:
            {
                RxCallback(&RxPacket.Data[1], RxPacket.Length - 2, THEBEII_BROADCASTADDRESS, RxPacket.Data[0], THEBEII_BROADCASTADDRESS, (int8_t)RxPacket.Data[RxPacket.Length-1]);
            }
            break;

            case AddressMode_2:
            {
                RxCallback(&RxPacket.Data[2], RxPacket.Length - 3, RxPacket.Data[0], RxPacket.Data[1], THEBEII_BROADCASTADDRESS, (int8_t)RxPacket.Data[RxPacket.Length-1]);
            }
            break;

            case AddressMode_3:
            {
                RxCallback(&RxPacket.Data[3], RxPacket.Length - 4, RxPacket.Data[0], RxPacket.Data[1], RxPacket.Data[2], (int8_t)RxPacket.Data[RxPacket.Length-1]);
            }
            break;

            default:
                /* wrong address mode */
                break;
            }
        }
    }
    break;

    case THEBEII_CMD_SET_CHANNEL_CNF:
    {
        /* check whether the module set value of channel as requested */
        if(RxPacket.Data[0] == channelVolatile)
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

    case THEBEII_CMD_SET_DESTADDR_CNF:
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

    case THEBEII_CMD_SET_DESTNETID_CNF:
    {
        /* check whether the module returns success */
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

    case THEBEII_CMD_SET_PAPOWER_CNF:
    {
        /* check whether the module set value of power output as requested */
        if(RxPacket.Data[0] == powerVolatile)
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

    /* for internal use only */
    case THEBEII_CMD_PINGDUT_CNF:
    {
        /* check the received packets */
        if(RxPacket.Data[4] == 0x0A)
        {
            /* 10 packets received */
            cmdConfirmation.status = CMD_Status_Success;
        }
        else
        {
            /* no 10 packets received */
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

/**
 * @brief Function that waits for the return value of ThebeII (*_CNF), when a command (*_REQ) was sent before
 */
static bool Wait4CNF(int max_time_ms, uint8_t expectedCmdConfirmation, ThebeII_CMD_Status_t expectedStatus, bool reset_confirmstate)
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
 * @brief Function to add the checksum at the end of the data packet
 */
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


/**
 * @brief Initialize the ThebeII for serial interface
 *
 * Caution: The parameters baudrate and addrmode must match the configured UserSettings of the ThebeII.
 *          The baudrate parameter must match to perform a successful UART communication.
 *          Updating this parameter during runtime may lead to communication errors.
 *          The addrmode must match when RF packet transmission or reception is performed.
 *          This parameter can be updated to the correct value (used in ThebeII_Init function) as soon as no RF packet transmission or reception was performed.
 *
 * @param[in] baudrate:       baudrate of the interface
 * @param[in] flow_control:   enable/disable flowcontrol
 * @param[in] addrmode:       address mode of the ThebeII
 * @param[in] RXcb:           RX callback function
 *
 * @return true if initialization succeeded,
 *         false otherwise
 */
bool ThebeII_Init(uint32_t baudrate, WE_FlowControl_t flow_control, ThebeII_AddressMode_t addrmode, void(*RXcb)(uint8_t*,uint8_t,uint8_t,uint8_t,uint8_t,int8_t))
{
    /* set address mode */
    addressmode = addrmode;

    /* set RX callback function */
    RxCallback = RXcb;

    /* initialize the pins */
    ThebeII_pins[ThebeII_Pin_Reset].port = GPIOA;
    ThebeII_pins[ThebeII_Pin_Reset].pin = GPIO_PIN_10;
    ThebeII_pins[ThebeII_Pin_Reset].type = WE_Pin_Type_Output;
    ThebeII_pins[ThebeII_Pin_SleepWakeUp].port = GPIOA;
    ThebeII_pins[ThebeII_Pin_SleepWakeUp].pin = GPIO_PIN_9;
    ThebeII_pins[ThebeII_Pin_SleepWakeUp].type = WE_Pin_Type_Output;
    ThebeII_pins[ThebeII_Pin_Boot].port = GPIOA;
    ThebeII_pins[ThebeII_Pin_Boot].pin = GPIO_PIN_7;
    ThebeII_pins[ThebeII_Pin_Boot].type = WE_Pin_Type_Output;
    ThebeII_pins[ThebeII_Pin_Mode].port = GPIOA;
    ThebeII_pins[ThebeII_Pin_Mode].pin = GPIO_PIN_8;
    ThebeII_pins[ThebeII_Pin_Mode].type = WE_Pin_Type_Output;
    if (false == WE_InitPins(ThebeII_pins, ThebeII_Pin_Count))
    {
        /* error */
        return false;
    }
    WE_SetPin(ThebeII_pins[ThebeII_Pin_Boot], WE_Pin_Level_Low);
    WE_SetPin(ThebeII_pins[ThebeII_Pin_SleepWakeUp], WE_Pin_Level_Low);
    WE_SetPin(ThebeII_pins[ThebeII_Pin_Reset], WE_Pin_Level_High);
    WE_SetPin(ThebeII_pins[ThebeII_Pin_Mode], WE_Pin_Level_Low);

    WE_UART_Init(baudrate, flow_control, WE_Parity_None, false);
    WE_Delay(10);

    /* reset module */
    if(ThebeII_PinReset())
    {
        WE_Delay(300);
    }
    else
    {
        fprintf(stdout, "Pin Reset failed\n");
        ThebeII_Deinit();
        return false;
    }

    return true;
}
/**
 * @brief Deinitialize the ThebeII interface
 *
 * @return true if deinitialization succeeded,
 *         false otherwise
 */
bool ThebeII_Deinit()
{
    /* close the communication interface to the module */
    WE_UART_DeInit();

    /* deinit pins */
    WE_DeinitPin(ThebeII_pins[ThebeII_Pin_Reset]);
    WE_DeinitPin(ThebeII_pins[ThebeII_Pin_SleepWakeUp]);
    WE_DeinitPin(ThebeII_pins[ThebeII_Pin_Boot]);
    WE_DeinitPin(ThebeII_pins[ThebeII_Pin_Mode]);

    addressmode = AddressMode_0;
    RxCallback = NULL;

    return true;
}

/**
 * @brief Wakeup the ThebeII from standby or shutdown by pin
 *
 * @return true if wakeup succeeded,
 *         false otherwise
 */
bool ThebeII_PinWakeup()
{
    int i = 0;
    WE_SetPin(ThebeII_pins[ThebeII_Pin_SleepWakeUp], WE_Pin_Level_High);
    WE_Delay (5);
    for(i=0; i<CMDCONFIRMATIONARRAY_LENGTH; i++)
    {
        cmdConfirmation_array[i].status = CMD_Status_Invalid;
        cmdConfirmation_array[i].cmd = CNFINVALID;
    }
    WE_SetPin(ThebeII_pins[ThebeII_Pin_SleepWakeUp], WE_Pin_Level_Low);

    /* wait for cnf */
    return Wait4CNF(CMD_WAIT_TIME, THEBEII_CMD_RESET_IND, CMD_Status_Success, false);
}

/**
 * @brief Reset the ThebeII by pin
 *
 * @return true if reset succeeded,
 *         false otherwise
 */
bool ThebeII_PinReset()
{
    WE_SetPin(ThebeII_pins[ThebeII_Pin_Reset], WE_Pin_Level_Low);
    WE_Delay (5);
    WE_SetPin(ThebeII_pins[ThebeII_Pin_Reset], WE_Pin_Level_High);

    /* wait for cnf */
    return Wait4CNF(CMD_WAIT_TIME, THEBEII_CMD_RESET_IND, CMD_Status_Success, true);
}

/**
 * @brief Reset the ThebeII by command
 *
 * @return true if reset succeeded,
 *         false otherwise
 */
bool ThebeII_Reset()
{
    bool ret = false;

    /* fill CMD_ARRAY packet */
    uint8_t CMD_ARRAY[4];
    CMD_ARRAY[0] = CMD_STX;
    CMD_ARRAY[1] = THEBEII_CMD_RESET_REQ;
    CMD_ARRAY[2] = 0x00;
    if(FillChecksum(CMD_ARRAY,sizeof(CMD_ARRAY)))
    {
        /* now send CMD_ARRAY */
        WE_UART_Transmit(CMD_ARRAY,sizeof(CMD_ARRAY));

        /* wait for cnf */
        ret = Wait4CNF(CMD_WAIT_TIME, THEBEII_CMD_RESET_CNF, CMD_Status_Success, true);
    }
    return ret;
}

/**
 * @brief Factory reset the ThebeII
 *
 * Note: use only in rare cases, since flash can be updated only a limited number of times
 *
 * @return true if factory reset succeeded,
 *         false otherwise
 */
bool ThebeII_FactoryReset()
{
    bool ret = false;

    /* fill CMD_ARRAY packet */
    uint8_t CMD_ARRAY[4];
    CMD_ARRAY[0] = CMD_STX;
    CMD_ARRAY[1] = THEBEII_CMD_FACTORY_RESET_REQ;
    CMD_ARRAY[2] = 0x00;
    if(FillChecksum(CMD_ARRAY,sizeof(CMD_ARRAY)))
    {
        /* now send CMD_ARRAY */
        WE_UART_Transmit(CMD_ARRAY,sizeof(CMD_ARRAY));

        /* wait for cnf */
        ret = Wait4CNF(1500, THEBEII_CMD_FACTORY_RESET_CNF, CMD_Status_Success, true);
    }
    return ret;
}

/**
 * @brief Switch the module to standby mode
 *
 * @return true if switching succeeded,
 *         false otherwise
 */
bool ThebeII_Standby()
{
    bool ret = false;

    /* fill CMD_ARRAY packet */
    uint8_t CMD_ARRAY[4];
    CMD_ARRAY[0] = CMD_STX;
    CMD_ARRAY[1] = THEBEII_CMD_STANDBY_REQ;
    CMD_ARRAY[2] = 0x00;
    if(FillChecksum(CMD_ARRAY,sizeof(CMD_ARRAY)))
    {
        /* now send CMD_ARRAY */
        WE_UART_Transmit(CMD_ARRAY,sizeof(CMD_ARRAY));

        /* wait for cnf */
        ret = Wait4CNF(CMD_WAIT_TIME, THEBEII_CMD_STANDBY_CNF, CMD_Status_Success, true);
    }
    return ret;
}

/**
 * @brief Switch the module to shutdown mode
 *
 * @return true if switching succeeded,
 *         false otherwise
 */
bool ThebeII_Shutdown()
{
    bool ret = false;

    /* fill CMD_ARRAY packet */
    uint8_t CMD_ARRAY[4];
    CMD_ARRAY[0] = CMD_STX;
    CMD_ARRAY[1] = THEBEII_CMD_SHUTDOWN_REQ;
    CMD_ARRAY[2] = 0x00;
    if(FillChecksum(CMD_ARRAY,sizeof(CMD_ARRAY)))
    {
        /* now send CMD_ARRAY */
        WE_UART_Transmit(CMD_ARRAY,sizeof(CMD_ARRAY));

        /* wait for cnf */
        ret = Wait4CNF(CMD_WAIT_TIME, THEBEII_CMD_SHUTDOWN_CNF, CMD_Status_Success, true);
    }
    return ret;
}

/**
 * @brief Request the current ThebeII settings
 *
 * @param[in] us: user setting to be requested
 * @param[out] response: pointer of the memory to put the request content
 * @param[out] response_length: length of the request content
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ThebeII_Get(ThebeII_UserSettings_t us, uint8_t* response, uint8_t* response_length)
{
    bool ret = false;

    /* fill CMD_ARRAY packet */
    uint8_t CMD_ARRAY[5];
    CMD_ARRAY[0] = CMD_STX;
    CMD_ARRAY[1] = THEBEII_CMD_GET_REQ;
    CMD_ARRAY[2] = 0x01;
    CMD_ARRAY[3] = us;

    if(FillChecksum(CMD_ARRAY,sizeof(CMD_ARRAY)))
    {
        /* now send CMD_ARRAY */
        WE_UART_Transmit(CMD_ARRAY,sizeof(CMD_ARRAY));

        /* wait for cnf */
        if (Wait4CNF(CMD_WAIT_TIME, THEBEII_CMD_GET_CNF, CMD_Status_Success, true))
        {
            int length = RxPacket.Length - 1;
            memcpy(response,&RxPacket.Data[1],length);
            *response_length = length;
            ret = true;
        }
    }
    return ret;
}

/**
 * @brief Set a special ThebeII setting
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
bool ThebeII_Set(ThebeII_UserSettings_t us, uint8_t* value, uint8_t length)
{
    bool ret = false;

    /* fill CMD_ARRAY packet */
    uint8_t CMD_ARRAY[length + 5];
    CMD_ARRAY[0] = CMD_STX;
    CMD_ARRAY[1] = THEBEII_CMD_SET_REQ;
    CMD_ARRAY[2] = (1 + length);
    CMD_ARRAY[3] = us;
    memcpy(&CMD_ARRAY[4],value,length);
    if(FillChecksum(CMD_ARRAY,sizeof(CMD_ARRAY)))
    {
        /* now send CMD_ARRAY */
        WE_UART_Transmit(CMD_ARRAY,sizeof(CMD_ARRAY));

        /* wait for cnf */
        ret = Wait4CNF(CMD_WAIT_TIME, THEBEII_CMD_SET_CNF, CMD_Status_Success, true);
    }
    return ret;
}


/**
 * @brief Request the 3 byte firmware version
 *
 * @param[out] fw: pointer to the 3 byte firmware version
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ThebeII_GetFirmwareVersion(uint8_t* fw)
{
    uint8_t help[3];
    uint8_t help_length;

    if(ThebeII_Get(ThebeII_CMD_SETGET_OPTION_FWVERSION, help, &help_length))
    {
        fw[0] = help[2];
        fw[1] = help[1];
        fw[2] = help[0];
        return true;
    }
    else
    {
        return false;
    }
}

/**
 * @brief Request the 4 byte serial number
 *
 * @param[out] sn: pointer to the 4 byte serial number
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ThebeII_GetSerialNumber(uint8_t* sn)
{
    uint8_t help[8];
    uint8_t help_length;

    if(ThebeII_Get(ThebeII_CMD_SETGET_OPTION_FACTORYSETTINGS, help, &help_length))
    {
        sn[0] = help[3];
        sn[1] = help[2];
        sn[2] = help[1];
        sn[3] = help[0];
        return true;
    }
    else
    {
        return false;
    }
}

/**
 * @brief Request the default TX power
 *
 * @param[out] txpower: pointer to the TXpower
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ThebeII_GetDefaultTXPower(uint8_t* txpower)
{
    uint8_t length;

    if(ThebeII_Get(ThebeII_CMD_SETGET_OPTION_DEFAULTRFTXPOWER, txpower, &length))
    {
        return true;
    }
    else
    {
        *txpower = TXPOWERINVALID;
        return false;
    }
}

/**
 * @brief Get the default destination address
 *
 * @param[out] destaddr_lsb: LSB of the destination address
 * @param[out] destaddr_msb: MSB of the destination address
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ThebeII_GetDefaultDestAddr(uint8_t* destaddr_lsb, uint8_t* destaddr_msb)
{
    /* helper array */
    uint8_t help[2];
    uint8_t length;

    if(ThebeII_Get(ThebeII_CMD_SETGET_OPTION_DEFAULTDESTADDR, help, &length))
    {
        *destaddr_lsb = help[0];
        *destaddr_msb = help[1];
        return true;
    }
    else
    {
        return false;
    }
}

/**
 * @brief Get the default destination address
 *
 * @param[out] destnetid: destination net id
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ThebeII_GetDefaultDestNetID(uint8_t* destnetid)
{
    uint8_t length;

    if(ThebeII_Get(ThebeII_CMD_SETGET_OPTION_DEFAULTDESTNETID, destnetid, &length))
    {
        return true;
    }
    else
    {
        return false;
    }
}

/**
 * @brief Get the default source address
 *
 * @param[out] srcaddr_lsb: LSB of the source address
 * @param[out] srcaddr_msb: MSB of the source address
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ThebeII_GetSourceAddr(uint8_t* srcaddr_lsb, uint8_t *srcaddr_msb)
{
    /* helper array */
    uint8_t help[2];
    uint8_t length;

    if(ThebeII_Get(ThebeII_CMD_SETGET_OPTION_SOURCEADDR, help, &length))
    {
        *srcaddr_lsb = help[0];
        *srcaddr_msb = help[1];
        return true;
    }
    else
    {
        return false;
    }
}

/**
 * @brief Set the default source net id
 *
 * @param[out] srcnetid: source net id
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ThebeII_GetSourceNetID(uint8_t* srcnetid)
{
    uint8_t length;

    if(ThebeII_Get(ThebeII_CMD_SETGET_OPTION_SOURCENETID, srcnetid, &length))
    {
        return true;
    }
    else
    {
        return false;
    }
}

/**
 * @brief Get the default RF channel
 *
 * @param[out] channel: channel
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ThebeII_GetDefaultRFChannel(uint8_t* channel)
{
    uint8_t length;

    if(ThebeII_Get(ThebeII_CMD_SETGET_OPTION_DEFAULTRFCHANNEL, channel, &length))
    {
        return true;
    }
    else
    {
        return false;
    }
}

/**
 * @brief Get the default RF profile
 *
 * @param[out] profile: RF profile
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ThebeII_GetDefaultRFProfile(uint8_t* profile)
{
    uint8_t length;

    if(ThebeII_Get(ThebeII_CMD_SETGET_OPTION_DEFAULTRFPROFILE, profile, &length))
    {
        return true;
    }
    else
    {
        return false;
    }
}

/**
 * @brief Set the default TX power
 *
 * Note: Reset the module after the adaption of the setting so that it can take effect.
 * Note: Use this function only in rare case, since flash can be updated only a limited number of times.
 * Note: Use ThebeII_SetVolatile_TXPower for frequent adaption of the TX power.
 *
 * @param[in] txpower: TXpower
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ThebeII_SetDefaultTXPower(uint8_t txpower)
{
    /* check for invalid power */
    if((txpower == 12) || (txpower == 18) || (txpower == 21) || (txpower == 23) || (txpower == 24) || (txpower == 25) || (txpower == 26))
    {
        return ThebeII_Set(ThebeII_CMD_SETGET_OPTION_DEFAULTRFTXPOWER, &txpower, 1);
    }

    /*invalid power*/
    return false;
}

/**
 * @brief Set the default destination address
 *
 * Note: Reset the module after the adaption of the setting so that it can take effect.
 * Note: Use this function only in rare case, since flash can be updated only a limited number of times.
 * Note: Use ThebeII_SetVolatile_DestAddr for frequent adaption of the destination address.
 *
 * @param[in] destaddr_lsb: LSB of the destination address
 * @param[in] destaddr_msb: MSB of the destination address
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ThebeII_SetDefaultDestAddr(uint8_t destaddr_lsb, uint8_t destaddr_msb)
{
    if((destaddr_lsb < 0)||(destaddr_lsb > 255))
    {
        return false;
    }

    /* fill array */
    uint8_t help[2];
    help[0] = destaddr_lsb;
    help[1] = destaddr_msb;
    return ThebeII_Set(ThebeII_CMD_SETGET_OPTION_DEFAULTDESTADDR, help, 2);
}

/**
 * @brief Set the default destination address
 *
 * Note: Reset the module after the adaption of the setting so that it can take effect.
 * Note: Use this function only in rare case, since flash can be updated only a limited number of times.
 * Note: Use ThebeII_SetVolatile_DestNetID for frequent adaption of the destination net id.
 *
 * @param[in] destnetid: destination net id
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ThebeII_SetDefaultDestNetID(uint8_t destnetid)
{
    /* check for valid destnetid */
    if((destnetid < 0)||(destnetid > 255))
    {
        /* invalid destnetid */
        return false;
    }
    return ThebeII_Set(ThebeII_CMD_SETGET_OPTION_DEFAULTDESTNETID, &destnetid, 1);
}

/**
 * @brief Set the default source address
 *
 * Note: Reset the module after the adaption of the setting so that it can take effect.
 * Note: Use this function only in rare case, since flash can be updated only a limited number of times.
 *
 * @param[in] srcaddr_lsb: LSB of the source address
 * @param[in] srcaddr_msb: MSB of the source address
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ThebeII_SetSourceAddr(uint8_t srcaddr_lsb, uint8_t srcaddr_msb)
{
    if((srcaddr_lsb < 0)||(srcaddr_lsb > 255))
    {
        return false;
    }
    /* fill array */
    uint8_t help[2];
    help[0] = srcaddr_lsb;
    help[1] = srcaddr_msb;
    return ThebeII_Set(ThebeII_CMD_SETGET_OPTION_SOURCEADDR, help, 2);
}

/**
 * @brief Set the default source net id
 *
 * Note: Reset the module after the adaption of the setting so that it can take effect.
 * Note: Use this function only in rare case, since flash can be updated only a limited number of times.
 *
 * @param[in] srcnetid: source net id
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ThebeII_SetSourceNetID(uint8_t srcnetid)
{
    /* check for invalid srcnetid */
    if((srcnetid < 0)||(srcnetid > 254))
    {
        /* invalid destnetid */
        return false;
    }
    return ThebeII_Set(ThebeII_CMD_SETGET_OPTION_SOURCENETID, &srcnetid, 1);
}

/**
 * @brief Set the default RF channel
 *
 * Note: Reset the module after the adaption of the setting so that it can take effect.
 * Note: Use this function only in rare case, since flash can be updated only a limited number of times.
 * Note: Use ThebeII_SetVolatile_Channel for frequent adaption of the channel.
 *
 * @param[in] channel: channel
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ThebeII_SetDefaultRFChannel(uint8_t channel)
{
    /* check for valid channel */
    if((channel < THEBEII_MIN_RFCHANNEL)||(channel > THEBEII_MAX_RFCHANNEL))
    {
        /* invalid channel */
        return false;
    }
    return ThebeII_Set(ThebeII_CMD_SETGET_OPTION_DEFAULTRFCHANNEL, &channel, 1);
}

/**
 * @brief Set the default RF profile
 *
 * Note: Reset the module after the adaption of the setting so that it can take effect.
 * Note: Use this function only in rare case, since flash can be updated only a limited number of times.
 *
 * @param[in] profile: RF profile
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ThebeII_SetDefaultRFProfile(uint8_t profile)
{
    return ThebeII_Set(ThebeII_CMD_SETGET_OPTION_DEFAULTRFPROFILE, &profile, 1);
}

/**
 * @brief Enables the Sniffer mode
 *
 * Note: Reset the module after the adaption of the setting so that it can take effect.
 * Note: Use this function only in rare case, since flash can be updated only a limited number of times.
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ThebeII_EnableSnifferMode()
{
    bool ret = false;

    uint16_t rpFlags;
    uint16_t cfgFlags;
    uint8_t length;

    ret = ThebeII_Get(ThebeII_CMD_SETGET_OPTION_CFG_FLAGS, (uint8_t*)&cfgFlags, &length);
    if(ret == true)
    {
        /* set sniffer mode if not set already */
        if(THEBEII_CFGFLAGS_SNIFFERMODEENABLE != (cfgFlags & THEBEII_CFGFLAGS_SNIFFERMODEENABLE))
        {
            cfgFlags |= THEBEII_CFGFLAGS_SNIFFERMODEENABLE;
            ret = ThebeII_Set(ThebeII_CMD_SETGET_OPTION_CFG_FLAGS, (uint8_t*)&cfgFlags, 2);
        }
        else
        {
            ret = true;
        }

        if(ret == true)
        {
            /* Make sure repeater mode is disabled once sniffer mode is active. Sniffer mode and repeater mode can not be used simultaneously */
            ret = ThebeII_Get(ThebeII_CMD_SETGET_OPTION_RP_FLAGS, (uint8_t*)&rpFlags, &length);
            if(ret == true)
            {
                if(THEBEII_RPFLAGS_REPEATERENABLE == (rpFlags & THEBEII_RPFLAGS_REPEATERENABLE))
                {
                    rpFlags &= ~THEBEII_RPFLAGS_REPEATERENABLE;
                    ret &= ThebeII_Set(ThebeII_CMD_SETGET_OPTION_RP_FLAGS, (uint8_t*)&rpFlags, 2);
                }
                else
                {
                    ret = true;
                }
            }
        }
    }
    return ret;
}

/**
 * @brief Set the volatile TX power
 *
 * @param[in] power: new TX power value
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ThebeII_SetVolatile_TXPower(uint8_t power)
{
    bool ret = false;

    /* check for invalid power */
       if((power == 12) || (power == 18) || (power == 21) || (power == 23) || (power == 24) || (power == 25) || (power == 26))
    {
        /* fill CMD_ARRAY packet */
        uint8_t CMD_ARRAY[5];
        CMD_ARRAY[0] = CMD_STX;
        CMD_ARRAY[1] = THEBEII_CMD_SET_PAPOWER_REQ;
        CMD_ARRAY[2] = 0x01;
        CMD_ARRAY[3] = power;
        if(FillChecksum(CMD_ARRAY,sizeof(CMD_ARRAY)))
        {
            powerVolatile = power;
            /* now send CMD_ARRAY */
            WE_UART_Transmit(CMD_ARRAY,sizeof(CMD_ARRAY));

            /* wait for cnf */
            ret = Wait4CNF(CMD_WAIT_TIME, THEBEII_CMD_SET_PAPOWER_CNF, CMD_Status_Success, true);
            powerVolatile = TXPOWERINVALID;
        }
    }
    return ret;
}

/**
 * @brief Set the volatile channel
 *
 * @param[in] channel: new channel value
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ThebeII_SetVolatile_Channel(uint8_t channel)
{
    bool ret = false;

    /* check for valid channel */
    if((channel < THEBEII_MIN_RFCHANNEL)||(channel > THEBEII_MAX_RFCHANNEL))
    {
        /* invalid channel */
        return false;
    }

    /* fill CMD_ARRAY packet */
    uint8_t CMD_ARRAY[5];
    CMD_ARRAY[0] = CMD_STX;
    CMD_ARRAY[1] = THEBEII_CMD_SET_CHANNEL_REQ;
    CMD_ARRAY[2] = 0x01;
    CMD_ARRAY[3] = channel;
    if(FillChecksum(CMD_ARRAY,sizeof(CMD_ARRAY)))
    {
        channelVolatile = channel;
        /* now send CMD_ARRAY */
        WE_UART_Transmit(CMD_ARRAY,sizeof(CMD_ARRAY));

        /* wait for cnf */
        ret = Wait4CNF(CMD_WAIT_TIME, THEBEII_CMD_SET_CHANNEL_CNF, CMD_Status_Success, true);
        channelVolatile = CHANNELINVALID;
    }
    return ret;
}

/**
 * @brief Set the volatile destination net ID
 *
 * @param[in] destnetid: new destination net ID
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ThebeII_SetVolatile_DestNetID(uint8_t destnetid)
{
    bool ret = false;

    /* check for valid destnetid */
    if((destnetid < 0)||(destnetid > 255))
    {
        /* invalid destnetid */
        return false;
    }

    /* fill CMD_ARRAY packet */
    uint8_t CMD_ARRAY[5];
    CMD_ARRAY[0] = CMD_STX;
    CMD_ARRAY[1] = THEBEII_CMD_SET_DESTNETID_REQ;
    CMD_ARRAY[2] = 0x01;
    CMD_ARRAY[3] = destnetid;
    if(FillChecksum(CMD_ARRAY,sizeof(CMD_ARRAY)))
    {
        /* now send CMD_ARRAY */
        WE_UART_Transmit(CMD_ARRAY,sizeof(CMD_ARRAY));

        /* wait for cnf */
        ret = Wait4CNF(CMD_WAIT_TIME, THEBEII_CMD_SET_DESTNETID_CNF, CMD_Status_Success, true);
    }
    return ret;
}

/**
 * @brief Set the volatile destination address
 *
 * @param[in] destaddr_lsb: lsb of the new destination address value
 * @param[in] destaddr_msb: msb of the new destination address value
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ThebeII_SetVolatile_DestAddr(uint8_t destaddr_lsb, uint8_t destaddr_msb)
{
    bool ret = false;

    if(destaddr_lsb < 0 || destaddr_lsb > 255)
    {
        return false;
    }

    if(destaddr_msb < 0 || destaddr_msb > 255)
    {
        return false;
    }

    uint8_t CMD_ARRAY[6];

    /* fill CMD_ARRAY packet */
    switch (addressmode)
    {
    case AddressMode_0:
    case AddressMode_1:
    case AddressMode_2:
    {
        CMD_ARRAY[0] = CMD_STX;
        CMD_ARRAY[1] = THEBEII_CMD_SET_DESTADDR_REQ;
        CMD_ARRAY[2] = 0x01;
        CMD_ARRAY[3] = destaddr_lsb;
        ret = FillChecksum(CMD_ARRAY,5);
    }
    break;
    case AddressMode_3:
    {
        CMD_ARRAY[0] = CMD_STX;
        CMD_ARRAY[1] = THEBEII_CMD_SET_DESTADDR_REQ;
        CMD_ARRAY[2] = 0x02;
        CMD_ARRAY[3] = destaddr_lsb;
        CMD_ARRAY[4] = destaddr_msb;
        ret = FillChecksum(CMD_ARRAY,6);
    }
    break;
    default:
        return false;
    }

    if(ret == true)
    {
        /* now send CMD_ARRAY */
        WE_UART_Transmit(CMD_ARRAY,sizeof(CMD_ARRAY));

        /* wait for cnf */
        ret = Wait4CNF(CMD_WAIT_TIME, THEBEII_CMD_SET_DESTADDR_CNF, CMD_Status_Success, true);
    }
    return ret;
}

/**
 * @brief Transmit data using the configured settings
 *
 * @param[in] payload: pointer to the data
 * @param[in] length: length of the data
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ThebeII_Transmit(uint8_t* payload, uint8_t length)
{
    bool ret = false;

    if(length > MAX_PAYLOAD_LENGTH)
    {
        fprintf(stdout, "Data exceeds maximal payload length\n");
        return false;
    }

    /* fill CMD_ARRAY packet */
    uint8_t CMD_ARRAY[length + 4];
    CMD_ARRAY[0] = CMD_STX;
    CMD_ARRAY[1] = THEBEII_CMD_DATA_REQ;
    CMD_ARRAY[2] = length;
    memcpy(&CMD_ARRAY[3],payload,length);
    if(FillChecksum(CMD_ARRAY,sizeof(CMD_ARRAY)))
    {

        /* now send CMD_ARRAY */
        WE_UART_Transmit(CMD_ARRAY,sizeof(CMD_ARRAY));

        /* wait for cnf */
        ret = Wait4CNF(CMD_WAIT_TIME, THEBEII_CMD_DATA_CNF, CMD_Status_Success, true);
    }
    return ret;
}

/**
 * @brief Transmit data
 *
 * @param[in] payload: pointer to the data
 * @param[in] length: length of the data
 * @param[in] channel: channel to be used
 * @param[in] dest_network_id: destination network ID
 * @param[in] dest_address_lsb: destination address lsb
 * @param[in] dest_address_msb: destination address msb
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool ThebeII_Transmit_Extended(uint8_t* payload, uint8_t length, uint8_t channel, uint8_t dest_network_id, uint8_t dest_address_lsb, uint8_t dest_address_msb)
{
    bool ret = false;

    if(length > MAX_PAYLOAD_LENGTH)
    {
        fprintf(stdout, "Data exceeds maximal payload length\n");
        return false;
    }

    /* fill CMD_ARRAY packet */
    uint8_t CMD_ARRAY[length + addressmode + 4 + 1];
    CMD_ARRAY[0] = CMD_STX;
    CMD_ARRAY[1] = THEBEII_CMD_DATAEX_REQ;

    switch (addressmode)
    {
    case AddressMode_0:
    {
        CMD_ARRAY[2] = (length + 1);
        CMD_ARRAY[3] = channel;
        memcpy(&CMD_ARRAY[4],payload,length);
    }
    break;

    case AddressMode_1:
    {
        CMD_ARRAY[2] = (length + 2);
        CMD_ARRAY[3] = channel;
        CMD_ARRAY[4] = dest_address_lsb;
        memcpy(&CMD_ARRAY[5],payload,length);
    }
    break;

    case AddressMode_2:
    {
        CMD_ARRAY[2] = (length + 3);
        CMD_ARRAY[3] = channel;
        CMD_ARRAY[4] = dest_network_id;
        CMD_ARRAY[5] = dest_address_lsb;
        memcpy(&CMD_ARRAY[6],payload,length);
    }
    break;

    case AddressMode_3:
    {
        CMD_ARRAY[2] = (length + 4);
        CMD_ARRAY[3] = channel;
        CMD_ARRAY[4] = dest_network_id;
        CMD_ARRAY[5] = dest_address_lsb;
        CMD_ARRAY[6] = dest_address_msb;
        memcpy(&CMD_ARRAY[7],payload,length);
    }
    break;

    default:
        /* wrong address mode */
        return false;
    }

    if(FillChecksum(CMD_ARRAY,sizeof(CMD_ARRAY)))
    {
        /* now send CMD_ARRAY */
        WE_UART_Transmit(CMD_ARRAY,sizeof(CMD_ARRAY));

        /* wait for cnf */
        ret = Wait4CNF(CMD_WAIT_TIME, THEBEII_CMD_DATA_CNF, CMD_Status_Success, true);
    }
    return ret;
}


/**
 * @brief Use the ping test command
 *
 * Note: Do not use this command. Just used for internal purposes!
 *
 * @return true if request succeeded,
 *         false otherwise
*/
bool ThebeII_Ping()
{
    /* rf-profil 5, ch134, +14dbm, 10 packets */
    uint8_t ping_command[] = {0x02,0x1F,0x08,0x20,0x05,0x86,0x0E,0x0A,0xFF,0xFF,0xFF,0x4D};

    /* now send the data */
    WE_UART_Transmit(ping_command,sizeof(ping_command));

    /* wait for cnf */
    return Wait4CNF(10000 /*10s*/, THEBEII_CMD_PINGDUT_CNF, CMD_Status_Success, true);
}

/**
 * @brief Configure the ThebeII
 *
 * @param[in] config: pointer to the configuration struct
 * @param[in] config_length: length of the configuration struct
 * @param[in] factory_reset: apply a factory reset before or not
 *
 * @return true if request succeeded,
 *         false otherwise
*/
bool ThebeII_Configure(ThebeII_Configuration_t* config, uint8_t config_length, bool factory_reset)
{
    int i = 0;
    uint8_t help_length;
    uint8_t help[THEBEII_MAX_USERSETTING_LENGTH];

    if(factory_reset)
    {
        /* perform a factory reset */
        if(false == ThebeII_FactoryReset())
        {
            /* error */
            return false;
        }
    }
    WE_Delay(500);

    /* now check all settings and update them if necessary */
    for(i=0; i<config_length; i++)
    {
        /* read current value */
        if(false == ThebeII_Get(config[i].usersetting, help, &help_length))
        {
            /* error */
            return false;
        }
        WE_Delay(200);

        /* check the value read out */
        if(help_length != config[i].value_length)
        {
            /* error, length does not match */
            return false;
        }
        if(memcmp(help,config[i].value,config[i].value_length) != 0)
        {
            /* read value is not up to date, thus write the new value */
            if(false == ThebeII_Set(config[i].usersetting, config[i].value, config[i].value_length))
            {
                /* error */
                return false;
            }
        }
        WE_Delay(200);
    }

    /* reset to take effect of the updated parameters */
    if(false == ThebeII_PinReset())
    {
        return false;
    }
    return true;
}
