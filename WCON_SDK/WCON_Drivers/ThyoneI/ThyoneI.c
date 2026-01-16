/*
 ***************************************************************************************************
 * This file is part of WIRELESS CONNECTIVITY SDK:
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
 * @file ThyoneI.c
 * @brief Thyone-I driver source file.
 */

#include <ThyoneI/ThyoneI.h>
#include <global/global.h>
#include <stdio.h>
#include <string.h>

#define CMD_WAIT_TIME 1500
#define CMD_WAIT_TIME_STEP_MS 0
#define CNFINVALID 255

/* Normal overhead: Start signal + Command + Length + CS = 1+1+2+1=5 bytes */
#define LENGTH_CMD_OVERHEAD (uint16_t)5
#define LENGTH_CMD_OVERHEAD_WITHOUT_CRC (uint16_t)(LENGTH_CMD_OVERHEAD - 1)

#define MAX_RADIO_PAYLOAD_LENGTH (uint16_t)224

#define MAX_CMD_PAYLOAD_LENGTH (uint16_t)(MAX_RADIO_PAYLOAD_LENGTH + 6)

typedef struct
{
    const uint8_t Stx;
    uint8_t Cmd;
    uint16_t Length;
    uint8_t Data[MAX_CMD_PAYLOAD_LENGTH + 1]; /* +1 from CS */

} ThyoneI_CMD_Frame_t;

#define CMD_STX 0x02

#define THYONEI_CMD_TYPE_REQ (uint8_t)(0 << 6)
#define THYONEI_CMD_TYPE_CNF (uint8_t)(1 << 6)
#define THYONEI_CMD_TYPE_IND (uint8_t)(2 << 6)
#define THYONEI_CMD_TYPE_RSP (uint8_t)(3 << 6)

#define THYONEI_CMD_RESET (uint8_t)0x00
#define THYONEI_CMD_RESET_REQ (THYONEI_CMD_RESET | THYONEI_CMD_TYPE_REQ)
#define THYONEI_CMD_RESET_CNF (THYONEI_CMD_RESET | THYONEI_CMD_TYPE_CNF)

#define THYONEI_CMD_GETSTATE (uint8_t)0x01
#define THYONEI_CMD_GETSTATE_REQ (THYONEI_CMD_GETSTATE | THYONEI_CMD_TYPE_REQ)
#define THYONEI_CMD_GETSTATE_CNF (THYONEI_CMD_GETSTATE | THYONEI_CMD_TYPE_CNF)

#define THYONEI_CMD_SLEEP (uint8_t)0x02
#define THYONEI_CMD_SLEEP_REQ (THYONEI_CMD_SLEEP | THYONEI_CMD_TYPE_REQ)
#define THYONEI_CMD_SLEEP_CNF (THYONEI_CMD_SLEEP | THYONEI_CMD_TYPE_CNF)
#define THYONEI_CMD_SLEEP_IND (THYONEI_CMD_SLEEP | THYONEI_CMD_TYPE_IND)

#define THYONEI_CMD_START_IND (uint8_t)0x73

#define THYONEI_CMD_UNICAST_DATA (uint8_t)0x04
#define THYONEI_CMD_UNICAST_DATA_REQ (THYONEI_CMD_UNICAST_DATA | THYONEI_CMD_TYPE_REQ)

/* Transmissions of any kind will be confirmed and indicated by the same message CMD_DATA_CNF or CMD_DATA_IND */
#define THYONEI_CMD_DATA_CNF (THYONEI_CMD_UNICAST_DATA | THYONEI_CMD_TYPE_CNF)
#define THYONEI_CMD_DATA_IND (THYONEI_CMD_UNICAST_DATA | THYONEI_CMD_TYPE_IND)
#define THYONEI_CMD_TXCOMPLETE_RSP (THYONEI_CMD_UNICAST_DATA | THYONEI_CMD_TYPE_RSP)

#define THYONEI_CMD_MULTICAST_DATA (uint8_t)0x05
#define THYONEI_CMD_MULTICAST_DATA_REQ (THYONEI_CMD_MULTICAST_DATA | THYONEI_CMD_TYPE_REQ)

#define THYONEI_CMD_BROADCAST_DATA (uint8_t)0x06
#define THYONEI_CMD_BROADCAST_DATA_REQ (THYONEI_CMD_BROADCAST_DATA | THYONEI_CMD_TYPE_REQ)

#define THYONEI_CMD_UNICAST_DATA_EX (uint8_t)0x07
#define THYONEI_CMD_UNICAST_DATA_EX_REQ (THYONEI_CMD_UNICAST_DATA_EX | THYONEI_CMD_TYPE_REQ)

#define THYONEI_CMD_MULTICAST_DATA_EX (uint8_t)0x08
#define THYONEI_CMD_MULTICAST_DATA_EX_REQ (THYONEI_CMD_MULTICAST_DATA_EX | THYONEI_CMD_TYPE_REQ)

#define THYONEI_CMD_SNIFFER_IND (uint8_t)0x99

#define THYONEI_CMD_SETCHANNEL (uint8_t)0x09
#define THYONEI_CMD_SETCHANNEL_REQ (THYONEI_CMD_SETCHANNEL | THYONEI_CMD_TYPE_REQ)
#define THYONEI_CMD_SETCHANNEL_CNF (THYONEI_CMD_SETCHANNEL | THYONEI_CMD_TYPE_CNF)

#define THYONEI_CMD_GET (uint8_t)0x10
#define THYONEI_CMD_GET_REQ (THYONEI_CMD_GET | THYONEI_CMD_TYPE_REQ)
#define THYONEI_CMD_GET_CNF (THYONEI_CMD_GET | THYONEI_CMD_TYPE_CNF)

#define THYONEI_CMD_SET (uint8_t)0x11
#define THYONEI_CMD_SET_REQ (THYONEI_CMD_SET | THYONEI_CMD_TYPE_REQ)
#define THYONEI_CMD_SET_CNF (THYONEI_CMD_SET | THYONEI_CMD_TYPE_CNF)

#define THYONEI_CMD_FACTORYRESET (uint8_t)0x1C
#define THYONEI_CMD_FACTORYRESET_REQ (THYONEI_CMD_FACTORYRESET | THYONEI_CMD_TYPE_REQ)
#define THYONEI_CMD_FACTORYRESET_CNF (THYONEI_CMD_FACTORYRESET | THYONEI_CMD_TYPE_CNF)

#define THYONEI_CMD_GETRAM (uint8_t)0x20
#define THYONEI_CMD_GETRAM_REQ (THYONEI_CMD_GETRAM | THYONEI_CMD_TYPE_REQ)
#define THYONEI_CMD_GETRAM_CNF (THYONEI_CMD_GETRAM | THYONEI_CMD_TYPE_CNF)

#define THYONEI_CMD_SETRAM (uint8_t)0x21
#define THYONEI_CMD_SETRAM_REQ (THYONEI_CMD_SETRAM | THYONEI_CMD_TYPE_REQ)
#define THYONEI_CMD_SETRAM_CNF (THYONEI_CMD_SETRAM | THYONEI_CMD_TYPE_CNF)

#define THYONEI_CMD_GPIO_LOCAL_SETCONFIG (uint8_t)0x25
#define THYONEI_CMD_GPIO_LOCAL_SETCONFIG_REQ (THYONEI_CMD_GPIO_LOCAL_SETCONFIG | THYONEI_CMD_TYPE_REQ)
#define THYONEI_CMD_GPIO_LOCAL_SETCONFIG_CNF (THYONEI_CMD_GPIO_LOCAL_SETCONFIG | THYONEI_CMD_TYPE_CNF)

#define THYONEI_CMD_GPIO_LOCAL_GETCONFIG (uint8_t)0x26
#define THYONEI_CMD_GPIO_LOCAL_GETCONFIG_REQ (THYONEI_CMD_GPIO_LOCAL_GETCONFIG | THYONEI_CMD_TYPE_REQ)
#define THYONEI_CMD_GPIO_LOCAL_GETCONFIG_CNF (THYONEI_CMD_GPIO_LOCAL_GETCONFIG | THYONEI_CMD_TYPE_CNF)

#define THYONEI_CMD_GPIO_LOCAL_WRITE (uint8_t)0x27
#define THYONEI_CMD_GPIO_LOCAL_WRITE_REQ (THYONEI_CMD_GPIO_LOCAL_WRITE | THYONEI_CMD_TYPE_REQ)
#define THYONEI_CMD_GPIO_LOCAL_WRITE_CNF (THYONEI_CMD_GPIO_LOCAL_WRITE | THYONEI_CMD_TYPE_CNF)

#define THYONEI_CMD_GPIO_LOCAL_READ (uint8_t)0x28
#define THYONEI_CMD_GPIO_LOCAL_READ_REQ (THYONEI_CMD_GPIO_LOCAL_READ | THYONEI_CMD_TYPE_REQ)
#define THYONEI_CMD_GPIO_LOCAL_READ_CNF (THYONEI_CMD_GPIO_LOCAL_READ | THYONEI_CMD_TYPE_CNF)

#define THYONEI_CMD_GPIO_REMOTE_SETCONFIG (uint8_t)0x29
#define THYONEI_CMD_GPIO_REMOTE_SETCONFIG_REQ (THYONEI_CMD_GPIO_REMOTE_SETCONFIG | THYONEI_CMD_TYPE_REQ)
#define THYONEI_CMD_GPIO_REMOTE_SETCONFIG_CNF (THYONEI_CMD_GPIO_REMOTE_SETCONFIG | THYONEI_CMD_TYPE_CNF)

#define THYONEI_CMD_GPIO_REMOTE_GETCONFIG (uint8_t)0x2A
#define THYONEI_CMD_GPIO_REMOTE_GETCONFIG_REQ (THYONEI_CMD_GPIO_REMOTE_GETCONFIG | THYONEI_CMD_TYPE_REQ)
#define THYONEI_CMD_GPIO_REMOTE_GETCONFIG_CNF (THYONEI_CMD_GPIO_REMOTE_GETCONFIG | THYONEI_CMD_TYPE_CNF)
#define THYONEI_CMD_GPIO_REMOTE_GETCONFIG_RSP (THYONEI_CMD_GPIO_REMOTE_GETCONFIG | THYONEI_CMD_TYPE_RSP)

#define THYONEI_CMD_GPIO_REMOTE_WRITE (uint8_t)0x2B
#define THYONEI_CMD_GPIO_REMOTE_WRITE_REQ (THYONEI_CMD_GPIO_REMOTE_WRITE | THYONEI_CMD_TYPE_REQ)
#define THYONEI_CMD_GPIO_REMOTE_WRITE_CNF (THYONEI_CMD_GPIO_REMOTE_WRITE | THYONEI_CMD_TYPE_CNF)

#define THYONEI_CMD_GPIO_REMOTE_READ (uint8_t)0x2C
#define THYONEI_CMD_GPIO_REMOTE_READ_REQ (THYONEI_CMD_GPIO_REMOTE_READ | THYONEI_CMD_TYPE_REQ)
#define THYONEI_CMD_GPIO_REMOTE_READ_CNF (THYONEI_CMD_GPIO_REMOTE_READ | THYONEI_CMD_TYPE_CNF)
#define THYONEI_CMD_GPIO_REMOTE_READ_RSP (THYONEI_CMD_GPIO_REMOTE_READ | THYONEI_CMD_TYPE_RSP)

void ThyoneI_HandleRxByte(uint8_t* dataP, size_t size);
static WE_UART_HandleRxByte_t byteRxCallback = ThyoneI_HandleRxByte;

/**
 * @brief Type used to check the response, when a command was sent to the ThyoneI
 */
typedef enum ThyoneI_CMD_Status_t
{
    CMD_Status_Success = (uint8_t)0x00,
    CMD_Status_Failed = (uint8_t)0x01,
    CMD_Status_Invalid,
    CMD_Status_Reset,
    CMD_Status_NoStatus,
} ThyoneI_CMD_Status_t;

typedef struct
{
    uint8_t cmd;                 /* variable to check if correct CMD has been confirmed */
    ThyoneI_CMD_Status_t status; /* variable used to check the response (*_CNF), when a request (*_REQ) was sent to the ThyoneI */
} ThyoneI_CMD_Confirmation_t;

/**************************************
 *          Static variables          *
 **************************************/
static ThyoneI_CMD_Frame_t txPacket = {.Stx = CMD_STX, .Length = 0}; /* request to be sent to the module */
static ThyoneI_CMD_Frame_t rxPacket = {.Stx = CMD_STX, .Length = 0}; /* received packet that has been sent by the module */
;

#define CMDCONFIRMATIONARRAY_LENGTH 3
static ThyoneI_CMD_Confirmation_t cmdConfirmation_array[CMDCONFIRMATIONARRAY_LENGTH];

/**
 * @brief Pin configuration struct pointer.
 */
static ThyoneI_Pins_t* ThyoneI_pinsP = NULL;

/**
 * @brief Uart configuration struct pointer.
 */
static WE_UART_t* ThyoneI_uartP = NULL;

static uint8_t checksum = 0;
static uint16_t rxByteCounter = 0;
static uint16_t bytesToReceive = 0;
static uint8_t rxBuffer[sizeof(ThyoneI_CMD_Frame_t)];            /* For UART RX from module */
static void (*RxCallback)(uint8_t*, uint16_t, uint32_t, int8_t); /* callback function */
static ThyoneI_OperationMode_t operationMode = ThyoneI_OperationMode_CommandMode;

/**************************************
 *         Static functions           *
 **************************************/

static void HandleRxPacket(uint8_t* prxBuffer)
{
    ThyoneI_CMD_Confirmation_t cmdConfirmation;
    cmdConfirmation.cmd = CNFINVALID;
    cmdConfirmation.status = CMD_Status_Invalid;

    uint16_t cmdLength = ((ThyoneI_CMD_Frame_t*)prxBuffer)->Length;
    memcpy(&rxPacket, prxBuffer, cmdLength + LENGTH_CMD_OVERHEAD);

    switch (rxPacket.Cmd)
    {
        case THYONEI_CMD_RESET_CNF:
        case THYONEI_CMD_START_IND:
        case THYONEI_CMD_GPIO_REMOTE_GETCONFIG_RSP:
        case THYONEI_CMD_GPIO_REMOTE_READ_RSP:
        {
            cmdConfirmation.cmd = rxPacket.Cmd;
            cmdConfirmation.status = CMD_Status_NoStatus;
            break;
        }

        case THYONEI_CMD_DATA_CNF:
        case THYONEI_CMD_GET_CNF:
        case THYONEI_CMD_SET_CNF:
        case THYONEI_CMD_GETRAM_CNF:
        case THYONEI_CMD_SETRAM_CNF:
        case THYONEI_CMD_SETCHANNEL_CNF:
        case THYONEI_CMD_FACTORYRESET_CNF:
        case THYONEI_CMD_SLEEP_CNF:
        case THYONEI_CMD_GPIO_LOCAL_SETCONFIG_CNF:
        case THYONEI_CMD_GPIO_LOCAL_GETCONFIG_CNF:
        case THYONEI_CMD_GPIO_LOCAL_WRITE_CNF:
        case THYONEI_CMD_GPIO_LOCAL_READ_CNF:
        case THYONEI_CMD_GPIO_REMOTE_SETCONFIG_CNF:
        case THYONEI_CMD_GPIO_REMOTE_GETCONFIG_CNF:
        case THYONEI_CMD_GPIO_REMOTE_WRITE_CNF:
        case THYONEI_CMD_TXCOMPLETE_RSP:
        case THYONEI_CMD_GETSTATE_CNF:
        {
            cmdConfirmation.cmd = rxPacket.Cmd;
            cmdConfirmation.status = rxPacket.Data[0];
            break;
        }

        case THYONEI_CMD_GPIO_REMOTE_READ_CNF:
        {
            cmdConfirmation.cmd = rxPacket.Cmd;
            cmdConfirmation.status = CMD_Status_Invalid;

            break;
        }

        case THYONEI_CMD_DATA_IND:
        {
            if (RxCallback != NULL)
            {
                uint32_t src_address;
                memcpy(&src_address, &rxPacket.Data[0], 4);
                RxCallback(&rxPacket.Data[5], rxPacket.Length - 5, src_address, rxPacket.Data[4]);
            }
            break;
        }

        case THYONEI_CMD_SNIFFER_IND:
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

    for (uint8_t i = 0; i < CMDCONFIRMATIONARRAY_LENGTH; i++)
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
 * @brief Function that waits for the return value of ThyoneI (*_CNF), when a command (*_REQ) was sent before
 */
static bool Wait4CNF(uint32_t max_time_ms, uint8_t expectedCmdConfirmation, ThyoneI_CMD_Status_t expectedStatus, bool reset_confirmstate)
{
    uint32_t t0 = WE_GetTick();

    if (reset_confirmstate)
    {
        for (uint8_t i = 0; i < CMDCONFIRMATIONARRAY_LENGTH; i++)
        {
            cmdConfirmation_array[i].cmd = CNFINVALID;
        }
    }
    while (1)
    {
        for (uint8_t i = 0; i < CMDCONFIRMATIONARRAY_LENGTH; i++)
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
static void FillChecksum(ThyoneI_CMD_Frame_t* cmd)
{
    uint8_t checksum = (uint8_t)cmd->Stx;
    uint8_t* pArray = (uint8_t*)cmd;
    for (uint16_t i = 1; i < (cmd->Length + LENGTH_CMD_OVERHEAD_WITHOUT_CRC); i++)
    {
        checksum ^= pArray[i];
    }
    cmd->Data[cmd->Length] = checksum;
}

void ThyoneI_HandleRxByte(uint8_t* dataP, size_t size)
{
    for (; size > 0; size--, dataP++)
    {
        if (rxByteCounter < sizeof(rxBuffer))
        {
            rxBuffer[rxByteCounter] = *dataP;
        }

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
                bytesToReceive = (uint16_t)(rxBuffer[rxByteCounter - 1]);
                break;

            case 3:
                /* length field MSB */
                rxByteCounter++;
                bytesToReceive += (((uint16_t)rxBuffer[rxByteCounter - 1] << 8) + LENGTH_CMD_OVERHEAD); /* len_msb + len_lsb + crc + sfd + cmd */
                break;

            default:
                /* data field */
                rxByteCounter++;
                if (rxByteCounter >= bytesToReceive)
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
}

/**************************************
 *         Global functions           *
 **************************************/

bool ThyoneI_Transparent_Transmit(const uint8_t* data, uint16_t dataLength)
{
    if ((data == NULL) || (dataLength == 0))
    {
        return false;
    }

    return ThyoneI_uartP->uartTransmit((uint8_t*)data, dataLength);
}

bool ThyoneI_Init(WE_UART_t* uartP, ThyoneI_Pins_t* pinoutP, ThyoneI_OperationMode_t opMode, void (*RXcb)(uint8_t*, uint16_t, uint32_t, int8_t))
{
    /* set RX callback function */
    RxCallback = RXcb;

    operationMode = opMode;

    if ((pinoutP == NULL) || (uartP == NULL) || (uartP->uartInit == NULL) || (uartP->uartDeinit == NULL) || (uartP->uartTransmit == NULL))
    {
        return false;
    }

    ThyoneI_pinsP = pinoutP;
    ThyoneI_pinsP->ThyoneI_Pin_Reset.type = WE_Pin_Type_Output;
    ThyoneI_pinsP->ThyoneI_Pin_Reset.initial_value.output = WE_Pin_Level_High;
    ThyoneI_pinsP->ThyoneI_Pin_SleepWakeUp.type = WE_Pin_Type_Output;
    ThyoneI_pinsP->ThyoneI_Pin_SleepWakeUp.initial_value.output = WE_Pin_Level_High;
    ThyoneI_pinsP->ThyoneI_Pin_Busy.type = WE_Pin_Type_Input;
    ThyoneI_pinsP->ThyoneI_Pin_Busy.initial_value.input_pull = WE_Pin_PullType_No;
    ThyoneI_pinsP->ThyoneI_Pin_Boot.type = WE_Pin_Type_Output;
    ThyoneI_pinsP->ThyoneI_Pin_Boot.initial_value.output = WE_Pin_Level_High;
    ThyoneI_pinsP->ThyoneI_Pin_Mode.type = WE_Pin_Type_Output;
    ThyoneI_pinsP->ThyoneI_Pin_Mode.initial_value.output = ((operationMode == ThyoneI_OperationMode_TransparentMode) ? WE_Pin_Level_High : WE_Pin_Level_Low);

    WE_Pin_t pins[sizeof(ThyoneI_Pins_t) / sizeof(WE_Pin_t)];
    uint8_t pin_count = 0;
    memcpy(&pins[pin_count++], &ThyoneI_pinsP->ThyoneI_Pin_Reset, sizeof(WE_Pin_t));
    memcpy(&pins[pin_count++], &ThyoneI_pinsP->ThyoneI_Pin_SleepWakeUp, sizeof(WE_Pin_t));
    memcpy(&pins[pin_count++], &ThyoneI_pinsP->ThyoneI_Pin_Boot, sizeof(WE_Pin_t));
    memcpy(&pins[pin_count++], &ThyoneI_pinsP->ThyoneI_Pin_Mode, sizeof(WE_Pin_t));
    memcpy(&pins[pin_count++], &ThyoneI_pinsP->ThyoneI_Pin_Busy, sizeof(WE_Pin_t));

    if (!WE_InitPins(pins, pin_count))
    {
        /* error */
        return false;
    }

    ThyoneI_uartP = uartP;
    if (false == ThyoneI_uartP->uartInit(ThyoneI_uartP->baudrate, ThyoneI_uartP->flowControl, ThyoneI_uartP->parity, &byteRxCallback))
    {
        return false;
    }
    WE_Delay(10);

    /* reset module */
    if (!(IS_WE_PIN_UNDEFINED(ThyoneI_pinsP->ThyoneI_Pin_Reset) ? ThyoneI_Reset() : ThyoneI_PinReset()))
    {
        WE_DEBUG_PRINT_INFO("Reset failed\r\n");
        ThyoneI_Deinit();
        return false;
    }

    return true;
}

bool ThyoneI_Deinit()
{
    WE_Pin_t pins[sizeof(ThyoneI_Pins_t) / sizeof(WE_Pin_t)];
    uint8_t pin_count = 0;
    memcpy(&pins[pin_count++], &ThyoneI_pinsP->ThyoneI_Pin_Reset, sizeof(WE_Pin_t));
    memcpy(&pins[pin_count++], &ThyoneI_pinsP->ThyoneI_Pin_SleepWakeUp, sizeof(WE_Pin_t));
    memcpy(&pins[pin_count++], &ThyoneI_pinsP->ThyoneI_Pin_Boot, sizeof(WE_Pin_t));
    memcpy(&pins[pin_count++], &ThyoneI_pinsP->ThyoneI_Pin_Mode, sizeof(WE_Pin_t));
    memcpy(&pins[pin_count++], &ThyoneI_pinsP->ThyoneI_Pin_Busy, sizeof(WE_Pin_t));

    /* deinit pins */
    if (!WE_DeinitPins(pins, pin_count))
    {
        return false;
    }

    RxCallback = NULL;

    return ThyoneI_uartP->uartDeinit();
}

bool ThyoneI_PinWakeup()
{
    if (!WE_SetPin(ThyoneI_pinsP->ThyoneI_Pin_SleepWakeUp, WE_Pin_Level_Low))
    {
        return false;
    }

    WE_Delay(5);

    for (uint8_t i = 0; i < CMDCONFIRMATIONARRAY_LENGTH; i++)
    {
        cmdConfirmation_array[i].status = CMD_Status_Invalid;
        cmdConfirmation_array[i].cmd = CNFINVALID;
    }

    if (!WE_SetPin(ThyoneI_pinsP->ThyoneI_Pin_SleepWakeUp, WE_Pin_Level_High))
    {
        return false;
    }

    if (operationMode == ThyoneI_OperationMode_TransparentMode)
    {
        /* transparent mode is ready (the module doesn't send a "ready for operation" message in transparent mode) */
        return true;
    }

    /* wait for cnf */
    return Wait4CNF(CMD_WAIT_TIME, THYONEI_CMD_START_IND, CMD_Status_NoStatus, false);
}

bool ThyoneI_PinReset()
{
    if (!WE_SetPin(ThyoneI_pinsP->ThyoneI_Pin_Reset, WE_Pin_Level_Low))
    {
        return false;
    }

    WE_Delay(5);

    if (!WE_SetPin(ThyoneI_pinsP->ThyoneI_Pin_Reset, WE_Pin_Level_High))
    {
        return false;
    }

    if (operationMode == ThyoneI_OperationMode_TransparentMode)
    {
        WE_Delay(THYONEI_BOOT_DURATION);
        /* transparent mode is ready (the module doesn't send a "ready for operation" message in transparent mode) */
        return true;
    }

    /* wait for cnf */
    return Wait4CNF(CMD_WAIT_TIME, THYONEI_CMD_START_IND, CMD_Status_NoStatus, true);
}

bool ThyoneI_Reset()
{
    txPacket.Cmd = THYONEI_CMD_RESET_REQ;
    txPacket.Length = 0;

    FillChecksum(&txPacket);

    if (!ThyoneI_Transparent_Transmit((uint8_t*)&txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD))
    {
        return false;
    }

    /* wait for cnf */
    return Wait4CNF(CMD_WAIT_TIME, THYONEI_CMD_START_IND, CMD_Status_NoStatus, true);
}

bool ThyoneI_Sleep()
{
    txPacket.Cmd = THYONEI_CMD_SLEEP_REQ;
    txPacket.Length = 0;

    FillChecksum(&txPacket);

    if (!ThyoneI_Transparent_Transmit((uint8_t*)&txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD))
    {
        return false;
    }

    /* wait for cnf */
    return Wait4CNF(CMD_WAIT_TIME, THYONEI_CMD_SLEEP_CNF, CMD_Status_Success, true);
}

bool ThyoneI_TransmitBroadcast(uint8_t* payloadP, uint16_t length)
{
    if ((payloadP == NULL) || (length > MAX_RADIO_PAYLOAD_LENGTH))
    {
        return false;
    }

    txPacket.Cmd = THYONEI_CMD_BROADCAST_DATA_REQ;
    txPacket.Length = length;

    memcpy(&txPacket.Data[0], payloadP, length);

    FillChecksum(&txPacket);

    if (!ThyoneI_Transparent_Transmit((uint8_t*)&txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD))
    {
        return false;
    }

    if (!Wait4CNF(CMD_WAIT_TIME, THYONEI_CMD_DATA_CNF, CMD_Status_Success, true))
    {
        return false;
    }

    return Wait4CNF(CMD_WAIT_TIME, THYONEI_CMD_TXCOMPLETE_RSP, CMD_Status_Success, false);
}

bool ThyoneI_TransmitMulticast(uint8_t* payloadP, uint16_t length)
{
    if ((payloadP == NULL) || (length > MAX_RADIO_PAYLOAD_LENGTH))
    {
        return false;
    }

    txPacket.Cmd = THYONEI_CMD_MULTICAST_DATA_REQ;
    txPacket.Length = length;

    memcpy(&txPacket.Data[0], payloadP, length);

    FillChecksum(&txPacket);

    if (!ThyoneI_Transparent_Transmit((uint8_t*)&txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD))
    {
        return false;
    }

    if (!Wait4CNF(CMD_WAIT_TIME, THYONEI_CMD_DATA_CNF, CMD_Status_Success, true))
    {
        return false;
    }

    return Wait4CNF(CMD_WAIT_TIME, THYONEI_CMD_TXCOMPLETE_RSP, CMD_Status_Success, false);
}

bool ThyoneI_TransmitUnicast(uint8_t* payloadP, uint16_t length)
{
    if ((payloadP == NULL) || (length > MAX_RADIO_PAYLOAD_LENGTH))
    {
        return false;
    }

    txPacket.Cmd = THYONEI_CMD_UNICAST_DATA_REQ;
    txPacket.Length = length;

    memcpy(&txPacket.Data[0], payloadP, length);

    FillChecksum(&txPacket);

    if (!ThyoneI_Transparent_Transmit((uint8_t*)&txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD))
    {
        return false;
    }

    if (!Wait4CNF(CMD_WAIT_TIME, THYONEI_CMD_DATA_CNF, CMD_Status_Success, true))
    {
        return false;
    }

    return Wait4CNF(CMD_WAIT_TIME, THYONEI_CMD_TXCOMPLETE_RSP, CMD_Status_Success, false);
}

bool ThyoneI_TransmitMulticastExtended(uint8_t groupID, uint8_t* payloadP, uint16_t length)
{
    if ((payloadP == NULL) || (length > MAX_RADIO_PAYLOAD_LENGTH))
    {
        return false;
    }

    txPacket.Cmd = THYONEI_CMD_MULTICAST_DATA_EX_REQ;
    txPacket.Length = length + 1;
    txPacket.Data[0] = groupID;

    memcpy(&txPacket.Data[1], payloadP, length);

    FillChecksum(&txPacket);

    if (!ThyoneI_Transparent_Transmit((uint8_t*)&txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD))
    {
        return false;
    }

    if (!Wait4CNF(CMD_WAIT_TIME, THYONEI_CMD_DATA_CNF, CMD_Status_Success, true))
    {
        return false;
    }

    return Wait4CNF(CMD_WAIT_TIME, THYONEI_CMD_TXCOMPLETE_RSP, CMD_Status_Success, false);
}

bool ThyoneI_TransmitUnicastExtended(uint32_t address, uint8_t* payloadP, uint16_t length)
{
    if ((payloadP == NULL) || (length > MAX_RADIO_PAYLOAD_LENGTH))
    {
        return false;
    }

    txPacket.Cmd = THYONEI_CMD_UNICAST_DATA_EX_REQ;
    txPacket.Length = length + 4;

    memcpy(&txPacket.Data[0], &address, 4);
    memcpy(&txPacket.Data[4], payloadP, length);

    FillChecksum(&txPacket);

    if (!ThyoneI_Transparent_Transmit((uint8_t*)&txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD))
    {
        return false;
    }

    if (!Wait4CNF(CMD_WAIT_TIME, THYONEI_CMD_DATA_CNF, CMD_Status_Success, true))
    {
        return false;
    }

    return Wait4CNF(CMD_WAIT_TIME, THYONEI_CMD_TXCOMPLETE_RSP, CMD_Status_Success, false);
}

bool ThyoneI_SetRam(ThyoneI_RuntimeSettings_t runtimeSetting, uint8_t* valueP, uint8_t length)
{
    if (valueP == NULL)
    {
        return false;
    }

    txPacket.Cmd = THYONEI_CMD_SETRAM_REQ;
    txPacket.Length = 1 + length;
    txPacket.Data[0] = runtimeSetting;
    memcpy(&txPacket.Data[1], valueP, length);

    FillChecksum(&txPacket);

    if (!ThyoneI_Transparent_Transmit((uint8_t*)&txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD))
    {
        return false;
    }

    return Wait4CNF(CMD_WAIT_TIME, THYONEI_CMD_SETRAM_CNF, CMD_Status_Success, true);
}

bool ThyoneI_FactoryReset()
{
    txPacket.Cmd = THYONEI_CMD_FACTORYRESET_REQ;
    txPacket.Length = 0;

    FillChecksum(&txPacket);

    if (!ThyoneI_Transparent_Transmit((uint8_t*)&txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD))
    {
        return false;
    }

    /* wait for reset after factory reset */
    return Wait4CNF(CMD_WAIT_TIME, THYONEI_CMD_START_IND, CMD_Status_NoStatus, true);
}

bool ThyoneI_CheckNSet(ThyoneI_UserSettings_t userSetting, uint8_t* valueP, uint8_t length)
{
    if (valueP == NULL)
    {
        return false;
    }

    uint8_t current_value[length];
    uint16_t current_length = length;

    if (!ThyoneI_Get(userSetting, current_value, &current_length))
    {
        return false;
    }

    if ((length == current_length) && (0 == memcmp(valueP, current_value, length)))
    {
        /* value is already set, no need to set it again */
        return true;
    }

    /* value differs, and thus must be set */
    return ThyoneI_Set(userSetting, valueP, length);
}

bool ThyoneI_Set(ThyoneI_UserSettings_t userSetting, uint8_t* valueP, uint8_t length)
{
    if (valueP == NULL)
    {
        return false;
    }

    txPacket.Cmd = THYONEI_CMD_SET_REQ;
    txPacket.Length = 1 + length;
    txPacket.Data[0] = userSetting;
    memcpy(&txPacket.Data[1], valueP, length);

    FillChecksum(&txPacket);

    if (!ThyoneI_Transparent_Transmit((uint8_t*)&txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD))
    {
        return false;
    }

    /* wait for cnf */
    if (!Wait4CNF(CMD_WAIT_TIME, THYONEI_CMD_SET_CNF, CMD_Status_Success, true))
    {
        return false;
    }

    return Wait4CNF(CMD_WAIT_TIME, THYONEI_CMD_START_IND, CMD_Status_NoStatus, false);
}

bool ThyoneI_SetTXPower(ThyoneI_TXPower_t txPower) { return ThyoneI_Set(ThyoneI_USERSETTING_INDEX_RF_TX_POWER, (uint8_t*)&txPower, 1); }

bool ThyoneI_SetBaudrateIndex(ThyoneI_BaudRateIndex_t baudrate, ThyoneI_UartParity_t parity, bool flowcontrolEnable)
{
    uint8_t baudrateIndex = (uint8_t)baudrate;

    /* If flowcontrol is to be enabled UART index has to be increased by one in regard to base value */
    if (flowcontrolEnable)
    {
        baudrateIndex++;
    }

    /* If parity bit is even, UART index has to be increased by 64 in regard of base value*/
    if (ThyoneI_UartParity_Even == parity)
    {
        baudrateIndex += 64;
    }

    return ThyoneI_Set(ThyoneI_USERSETTING_INDEX_UART_CONFIG, (uint8_t*)&baudrateIndex, 1);
}

bool ThyoneI_SetRFChannel(uint8_t channel)
{
    /* permissible value for channel: 0-38 */
    if (channel > 38)
    {
        return false;
    }

    return ThyoneI_Set(ThyoneI_USERSETTING_INDEX_RF_CHANNEL, (uint8_t*)&channel, 1);
}

bool ThyoneI_SetRFChannelRuntime(uint8_t channel)
{
    /* permissible value for channel: 0-38 */
    if (channel > 38)
    {
        return false;
    }

    txPacket.Cmd = THYONEI_CMD_SETCHANNEL_REQ;
    txPacket.Length = 1;
    txPacket.Data[0] = channel;

    FillChecksum(&txPacket);

    if (!ThyoneI_Transparent_Transmit((uint8_t*)&txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD))
    {
        return false;
    }

    return Wait4CNF(CMD_WAIT_TIME, THYONEI_CMD_SETCHANNEL_CNF, CMD_Status_Success, true);
}

bool ThyoneI_SetEncryptionMode(ThyoneI_EncryptionMode_t encryptionMode) { return ThyoneI_Set(ThyoneI_USERSETTING_INDEX_ENCRYPTION_MODE, (uint8_t*)&encryptionMode, 1); }

bool ThyoneI_SetRfProfile(ThyoneI_Profile_t profile) { return ThyoneI_Set(ThyoneI_USERSETTING_INDEX_RF_PROFILE, (uint8_t*)&profile, 1); }

bool ThyoneI_SetNumRetries(uint8_t numRetries) { return ThyoneI_Set(ThyoneI_USERSETTING_INDEX_RF_NUM_RETRIES, (uint8_t*)&numRetries, 1); }

bool ThyoneI_SetRpNumSlots(uint8_t numSlots) { return ThyoneI_Set(ThyoneI_USERSETTING_INDEX_RF_RP_NUM_SLOTS, (uint8_t*)&numSlots, 1); }

bool ThyoneI_SetSourceAddress(uint32_t sourceAddress) { return ThyoneI_Set(ThyoneI_USERSETTING_INDEX_MAC_SOURCE_ADDRESS, (uint8_t*)&sourceAddress, 4); }

bool ThyoneI_SetDestinationAddress(uint32_t destinationAddress) { return ThyoneI_Set(ThyoneI_USERSETTING_INDEX_MAC_DESTINATION_ADDRESS, (uint8_t*)&destinationAddress, 4); }

bool ThyoneI_SetGroupID(uint8_t groupId) { return ThyoneI_Set(ThyoneI_USERSETTING_INDEX_MAC_GROUP_ID, (uint8_t*)&groupId, 1); }

bool ThyoneI_SetEncryptionKey(uint8_t* keyP) { return ThyoneI_Set(ThyoneI_USERSETTING_INDEX_MAC_ENCRYPTION_KEY, keyP, 16); }

bool ThyoneI_SetTimeToLive(uint8_t ttl) { return ThyoneI_Set(ThyoneI_USERSETTING_INDEX_MAC_TLL, &ttl, 1); }

bool ThyoneI_SetCCAMode(uint8_t ccaMode) { return ThyoneI_Set(ThyoneI_USERSETTING_INDEX_CCA_MODE, &ccaMode, 1); }

bool ThyoneI_SetCCAThreshold(uint8_t ccaThreshold) { return ThyoneI_Set(ThyoneI_USERSETTING_INDEX_CCA_THRESHOLD, &ccaThreshold, 1); }

bool ThyoneI_SetGPIOBlockRemoteConfig(uint8_t remoteConfig) { return ThyoneI_Set(ThyoneI_USERSETTING_INDEX_REMOTE_GPIO_CONFIG, &remoteConfig, 1); }

bool ThyoneI_SetModuleMode(ThyoneI_ModuleMode_t moduleMode) { return ThyoneI_Set(ThyoneI_USERSETTING_INDEX_MODULE_MODE, (uint8_t*)&moduleMode, 1); }

bool ThyoneI_GetRam(ThyoneI_RuntimeSettings_t runtimeSetting, uint8_t* ResponseP, uint16_t* Response_LengthP)
{
    if (ResponseP == NULL || Response_LengthP == NULL)
    {
        return false;
    }

    txPacket.Cmd = THYONEI_CMD_GETRAM_REQ;
    txPacket.Length = 1;
    txPacket.Data[0] = runtimeSetting;

    FillChecksum(&txPacket);

    if (!ThyoneI_Transparent_Transmit((uint8_t*)&txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD))
    {
        return false;
    }

    /* wait for cnf */
    if (!Wait4CNF(CMD_WAIT_TIME, THYONEI_CMD_GETRAM_CNF, CMD_Status_Success, true))
    {
        return false;
    }

    uint16_t length = rxPacket.Length;
    memcpy(ResponseP, &rxPacket.Data[1], length - 1); /* First Data byte is status, following bytes response*/
    *Response_LengthP = length - 1;

    return true;
}

bool ThyoneI_Get(ThyoneI_UserSettings_t userSetting, uint8_t* ResponseP, uint16_t* Response_LengthP)
{
    if (ResponseP == NULL || Response_LengthP == NULL)
    {
        return false;
    }

    txPacket.Cmd = THYONEI_CMD_GET_REQ;
    txPacket.Length = 1;
    txPacket.Data[0] = userSetting;

    FillChecksum(&txPacket);

    if (!ThyoneI_Transparent_Transmit((uint8_t*)&txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD))
    {
        return false;
    }

    /* wait for cnf */
    if (!Wait4CNF(CMD_WAIT_TIME, THYONEI_CMD_GET_CNF, CMD_Status_Success, true))
    {
        return false;
    }

    uint16_t length = rxPacket.Length;
    memcpy(ResponseP, &rxPacket.Data[1], length - 1); /* First Data byte is status, following bytes response*/
    *Response_LengthP = length - 1;

    return true;
}

bool ThyoneI_GetSerialNumber(uint8_t* serialNumberP)
{
    uint16_t length;
    return ThyoneI_Get(ThyoneI_USERSETTING_INDEX_SERIAL_NUMBER, serialNumberP, &length);
}

bool ThyoneI_GetFWVersion(uint8_t* versionP)
{
    uint16_t length;
    return ThyoneI_Get(ThyoneI_USERSETTING_INDEX_FW_VERSION, versionP, &length);
}

bool ThyoneI_GetTXPower(ThyoneI_TXPower_t* txpowerP)
{
    uint16_t length;
    return ThyoneI_Get(ThyoneI_USERSETTING_INDEX_RF_TX_POWER, (uint8_t*)txpowerP, &length);
}

bool ThyoneI_GetBaudrateIndex(ThyoneI_BaudRateIndex_t* baudrateP, ThyoneI_UartParity_t* parityP, bool* flowcontrolEnableP)
{
    if ((baudrateP == NULL) || (parityP == NULL) || (flowcontrolEnableP == NULL))
    {
        return false;
    }

    uint16_t length;
    uint8_t uartIndex;

    if (!ThyoneI_Get(ThyoneI_USERSETTING_INDEX_UART_CONFIG, (uint8_t*)&uartIndex, &length))
    {
        return false;
    }

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
        *parityP = ThyoneI_UartParity_None;
    }
    else
    {
        *parityP = ThyoneI_UartParity_Even;
        uartIndex -= 64;
    }

    *baudrateP = (ThyoneI_BaudRateIndex_t)uartIndex;

    return true;
}

bool ThyoneI_GetEncryptionMode(ThyoneI_EncryptionMode_t* encryptionModeP)
{
    uint16_t length;
    return ThyoneI_Get(ThyoneI_USERSETTING_INDEX_ENCRYPTION_MODE, (uint8_t*)encryptionModeP, &length);
}

bool ThyoneI_GetRfProfile(ThyoneI_Profile_t* profileP)
{
    uint16_t length;
    return ThyoneI_Get(ThyoneI_USERSETTING_INDEX_RF_PROFILE, (uint8_t*)profileP, &length);
}

bool ThyoneI_GetRFChannel(uint8_t* channelP)
{
    uint16_t length;
    return ThyoneI_Get(ThyoneI_USERSETTING_INDEX_RF_CHANNEL, (uint8_t*)channelP, &length);
}

bool ThyoneI_GetNumRetries(uint8_t* numRetriesP)
{
    uint16_t length;
    return ThyoneI_Get(ThyoneI_USERSETTING_INDEX_RF_NUM_RETRIES, numRetriesP, &length);
}

bool ThyoneI_GetRpNumSlots(uint8_t* numSlotsP)
{
    uint16_t length;
    return ThyoneI_Get(ThyoneI_USERSETTING_INDEX_RF_RP_NUM_SLOTS, numSlotsP, &length);
}

bool ThyoneI_GetSourceAddress(uint32_t* sourceAddressP)
{
    uint16_t length;
    return ThyoneI_Get(ThyoneI_USERSETTING_INDEX_MAC_SOURCE_ADDRESS, (uint8_t*)sourceAddressP, &length);
}

bool ThyoneI_GetDestinationAddress(uint32_t* destinationAddressP)
{
    uint16_t length;
    return ThyoneI_Get(ThyoneI_USERSETTING_INDEX_MAC_DESTINATION_ADDRESS, (uint8_t*)destinationAddressP, &length);
}

bool ThyoneI_GetGroupID(uint8_t* groupIdP)
{
    uint16_t length;
    return ThyoneI_Get(ThyoneI_USERSETTING_INDEX_MAC_GROUP_ID, groupIdP, &length);
}

bool ThyoneI_GetTimeToLive(uint8_t* ttlP)
{
    uint16_t length;
    return ThyoneI_Get(ThyoneI_USERSETTING_INDEX_MAC_TLL, ttlP, &length);
}

bool ThyoneI_GetCCAMode(uint8_t* ccaModeP)
{
    uint16_t length;
    return ThyoneI_Get(ThyoneI_USERSETTING_INDEX_CCA_MODE, ccaModeP, &length);
}

bool ThyoneI_GetCCAThreshold(uint8_t* ccaThresholdP)
{
    uint16_t length;
    return ThyoneI_Get(ThyoneI_USERSETTING_INDEX_CCA_THRESHOLD, ccaThresholdP, &length);
}

bool ThyoneI_GetGPIOBlockRemoteConfig(uint8_t* remoteConfigP)
{
    uint16_t length;
    return ThyoneI_Get(ThyoneI_USERSETTING_INDEX_REMOTE_GPIO_CONFIG, remoteConfigP, &length);
}

bool ThyoneI_GetModuleMode(ThyoneI_ModuleMode_t* moduleModeP)
{
    uint16_t length;
    return ThyoneI_Get(ThyoneI_USERSETTING_INDEX_MODULE_MODE, (uint8_t*)moduleModeP, &length);
}

bool ThyoneI_GetState(ThyoneI_States_t* state)
{
    if (state == NULL)
    {
        return false;
    }

    txPacket.Cmd = THYONEI_CMD_GETSTATE_REQ;
    txPacket.Length = 0;

    FillChecksum(&txPacket);

    if (!ThyoneI_Transparent_Transmit((uint8_t*)&txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD))
    {
        return false;
    }

    /* wait for cnf */
    if (!Wait4CNF(CMD_WAIT_TIME, THYONEI_CMD_GETSTATE_CNF, CMD_Status_Success, true))
    {
        return false;
    }

    *state = rxPacket.Data[1];
    return true;
}

bool ThyoneI_GPIOLocalSetConfig(ThyoneI_GPIOConfigBlock_t* configP, uint16_t numberOfControls)
{
    if ((configP == NULL) || (numberOfControls == 0))
    {
        return false;
    }

    uint16_t length = 0;

    for (uint16_t i = 0; i < numberOfControls; i++)
    {
        switch (configP->function)
        {
            case ThyoneI_GPIO_IO_Disconnected:
            {
                txPacket.Data[length] = 3;
                txPacket.Data[length + 1] = configP->GPIO_ID;
                txPacket.Data[length + 2] = configP->function;
                txPacket.Data[length + 3] = 0x00;
                length += 4;
            }
            break;
            case ThyoneI_GPIO_IO_Input:
            {
                txPacket.Data[length] = 3;
                txPacket.Data[length + 1] = configP->GPIO_ID;
                txPacket.Data[length + 2] = configP->function;
                txPacket.Data[length + 3] = configP->value.input;
                length += 4;
            }
            break;
            case ThyoneI_GPIO_IO_Output:
            {
                txPacket.Data[length] = 3;
                txPacket.Data[length + 1] = configP->GPIO_ID;
                txPacket.Data[length + 2] = configP->function;
                txPacket.Data[length + 3] = configP->value.output;
                length += 4;
            }
            break;
            case ThyoneI_GPIO_IO_PWM:
            {
                txPacket.Data[length] = 5;
                txPacket.Data[length + 1] = configP->GPIO_ID;
                txPacket.Data[length + 2] = configP->function;
                memcpy(&txPacket.Data[length + 3], &configP->value.pwm.period, 2);
                txPacket.Data[length + 5] = configP->value.pwm.ratio;
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

    txPacket.Cmd = THYONEI_CMD_GPIO_LOCAL_SETCONFIG_REQ;
    txPacket.Length = length;

    FillChecksum(&txPacket);

    if (!ThyoneI_Transparent_Transmit((uint8_t*)&txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD))
    {
        return false;
    }

    /* wait for cnf */
    return Wait4CNF(CMD_WAIT_TIME, THYONEI_CMD_GPIO_LOCAL_SETCONFIG_CNF, CMD_Status_Success, true);
}

bool ThyoneI_GPIOLocalGetConfig(ThyoneI_GPIOConfigBlock_t* configP, uint16_t* numberOfControlsP)
{
    if ((configP == NULL) || (numberOfControlsP == NULL))
    {
        return false;
    }

    txPacket.Cmd = THYONEI_CMD_GPIO_LOCAL_GETCONFIG_REQ;
    txPacket.Length = 0;

    FillChecksum(&txPacket);

    if (!ThyoneI_Transparent_Transmit((uint8_t*)&txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD))
    {
        return false;
    }

    /* wait for cnf */
    if (!Wait4CNF(CMD_WAIT_TIME, THYONEI_CMD_GPIO_LOCAL_GETCONFIG_CNF, CMD_Status_Success, true))
    {
        return false;
    }

    uint16_t length = rxPacket.Length;

    *numberOfControlsP = 0;
    uint8_t* uartP = &rxPacket.Data[1];
    ThyoneI_GPIOConfigBlock_t* configP_running = configP;
    while (uartP < &rxPacket.Data[length])
    {
        switch (*(uartP + 2))
        {
            case ThyoneI_GPIO_IO_Disconnected:
            {
                if (*uartP == 2)
                {
                    configP_running->GPIO_ID = *(uartP + 1);
                    configP_running->function = *(uartP + 2);

                    configP_running++;
                    *numberOfControlsP += 1;
                }
            }
            break;
            case ThyoneI_GPIO_IO_Input:
            {
                if (*uartP == 3)
                {
                    configP_running->GPIO_ID = *(uartP + 1);
                    configP_running->function = *(uartP + 2);
                    configP_running->value.input = *(uartP + 3);

                    configP_running++;
                    *numberOfControlsP += 1;
                }
            }
            break;
            case ThyoneI_GPIO_IO_Output:
            {
                if (*uartP == 3)
                {
                    configP_running->GPIO_ID = *(uartP + 1);
                    configP_running->function = *(uartP + 2);
                    configP_running->value.output = *(uartP + 3);

                    configP_running++;
                    *numberOfControlsP += 1;
                }
            }
            break;
            case ThyoneI_GPIO_IO_PWM:
            {
                if (*uartP == 5)
                {
                    configP_running->GPIO_ID = *(uartP + 1);
                    configP_running->function = *(uartP + 2);
                    memcpy(&configP_running->value.pwm.period, (uartP + 3), 2);
                    configP_running->value.pwm.ratio = *(uartP + 5);

                    configP_running++;
                    *numberOfControlsP += 1;
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
    return true;
}

bool ThyoneI_GPIOLocalWrite(ThyoneI_GPIOControlBlock_t* controlP, uint16_t number_of_controls)
{
    if ((controlP == NULL) || (number_of_controls == 0))
    {
        return false;
    }

    uint16_t length = 0;

    for (uint16_t i = 0; i < number_of_controls; i++)
    {
        txPacket.Data[length] = 2;
        txPacket.Data[length + 1] = controlP->GPIO_ID;
        txPacket.Data[length + 2] = controlP->value.output;
        length += 3;
        controlP++;
    }

    txPacket.Cmd = THYONEI_CMD_GPIO_LOCAL_WRITE_REQ;
    txPacket.Length = length;

    FillChecksum(&txPacket);

    if (!ThyoneI_Transparent_Transmit((uint8_t*)&txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD))
    {
        return false;
    }

    /* wait for cnf */
    return Wait4CNF(CMD_WAIT_TIME, THYONEI_CMD_GPIO_LOCAL_WRITE_CNF, CMD_Status_Success, true);
}

bool ThyoneI_GPIOLocalRead(uint8_t* GPIOToReadP, uint8_t amountGPIOToRead, ThyoneI_GPIOControlBlock_t* controlP, uint16_t* numberOfControls)
{
    if ((GPIOToReadP == NULL) || (amountGPIOToRead == 0) || (controlP == NULL) || (numberOfControls == NULL))
    {
        return false;
    }

    txPacket.Cmd = THYONEI_CMD_GPIO_LOCAL_READ_REQ;
    txPacket.Length = amountGPIOToRead + 1;
    txPacket.Data[0] = amountGPIOToRead;
    memcpy(&txPacket.Data[1], GPIOToReadP, amountGPIOToRead);

    FillChecksum(&txPacket);

    if (!ThyoneI_Transparent_Transmit((uint8_t*)&txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD))
    {
        return false;
    }

    /* wait for cnf */
    if (!Wait4CNF(CMD_WAIT_TIME, THYONEI_CMD_GPIO_LOCAL_READ_CNF, CMD_Status_Success, true))
    {
        return false;
    }

    uint16_t length = rxPacket.Length;

    *numberOfControls = 0;
    uint8_t* uartP = &rxPacket.Data[1];
    ThyoneI_GPIOControlBlock_t* controlP_running = controlP;
    while (uartP < &rxPacket.Data[length])
    {
        if (*uartP == 2)
        {
            controlP_running->GPIO_ID = *(uartP + 1);
            controlP_running->value.output = *(uartP + 2);

            controlP_running++;
            *numberOfControls += 1;
        }
        uartP += *uartP + 1;
    }
    return true;
}

bool ThyoneI_GPIORemoteSetConfig(uint32_t destAddress, ThyoneI_GPIOConfigBlock_t* configP, uint16_t numberOfControls)
{
    if ((configP == NULL) || (numberOfControls == 0))
    {
        return false;
    }

    uint16_t length = 4;

    for (uint16_t i = 0; i < numberOfControls; i++)
    {
        switch (configP->function)
        {
            case ThyoneI_GPIO_IO_Disconnected:
            {
                txPacket.Data[length] = 3;
                txPacket.Data[length + 1] = configP->GPIO_ID;
                txPacket.Data[length + 2] = configP->function;
                txPacket.Data[length + 3] = 0x00;
                length += 4;
            }
            break;
            case ThyoneI_GPIO_IO_Input:
            {
                txPacket.Data[length] = 3;
                txPacket.Data[length + 1] = configP->GPIO_ID;
                txPacket.Data[length + 2] = configP->function;
                txPacket.Data[length + 3] = configP->value.input;
                length += 4;
            }
            break;
            case ThyoneI_GPIO_IO_Output:
            {
                txPacket.Data[length] = 3;
                txPacket.Data[length + 1] = configP->GPIO_ID;
                txPacket.Data[length + 2] = configP->function;
                txPacket.Data[length + 3] = configP->value.output;
                length += 4;
            }
            break;
            case ThyoneI_GPIO_IO_PWM:
            {
                txPacket.Data[length] = 5;
                txPacket.Data[length + 1] = configP->GPIO_ID;
                txPacket.Data[length + 2] = configP->function;
                memcpy(&txPacket.Data[length + 3], &configP->value.pwm.period, 2);
                txPacket.Data[length + 5] = configP->value.pwm.ratio;
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

    txPacket.Cmd = THYONEI_CMD_GPIO_REMOTE_SETCONFIG_REQ;
    txPacket.Length = length;

    memcpy(&txPacket.Data[0], &destAddress, 4);

    FillChecksum(&txPacket);

    if (!ThyoneI_Transparent_Transmit((uint8_t*)&txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD))
    {
        return false;
    }

    /* wait for cnf */
    return Wait4CNF(CMD_WAIT_TIME, THYONEI_CMD_GPIO_REMOTE_SETCONFIG_CNF, CMD_Status_Success, true);
}

bool ThyoneI_GPIORemoteGetConfig(uint32_t destAddress, ThyoneI_GPIOConfigBlock_t* configP, uint16_t* numberOfControlsP)
{
    if ((configP == NULL) || (numberOfControlsP == NULL))
    {
        return false;
    }

    txPacket.Cmd = THYONEI_CMD_GPIO_REMOTE_GETCONFIG_REQ;
    txPacket.Length = 4;
    memcpy(&txPacket.Data[0], &destAddress, 4);

    FillChecksum(&txPacket);

    if (!ThyoneI_Transparent_Transmit((uint8_t*)&txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD))
    {
        return false;
    }

    /* wait for cnf */
    if (!Wait4CNF(CMD_WAIT_TIME, THYONEI_CMD_GPIO_REMOTE_GETCONFIG_RSP, CMD_Status_NoStatus, true))
    {
        return false;
    }

    uint16_t length = rxPacket.Length;

    *numberOfControlsP = 0;
    uint8_t* uartP = &rxPacket.Data[1 + 4];
    ThyoneI_GPIOConfigBlock_t* configP_running = configP;
    while (uartP < &rxPacket.Data[length])
    {
        switch (*(uartP + 2))
        {
            case ThyoneI_GPIO_IO_Disconnected:
            {
                if (*uartP == 2)
                {
                    configP_running->GPIO_ID = *(uartP + 1);
                    configP_running->function = *(uartP + 2);

                    configP_running++;
                    *numberOfControlsP += 1;
                }
            }
            break;
            case ThyoneI_GPIO_IO_Input:
            {
                if (*uartP == 3)
                {
                    configP_running->GPIO_ID = *(uartP + 1);
                    configP_running->function = *(uartP + 2);
                    configP_running->value.input = *(uartP + 3);

                    configP_running++;
                    *numberOfControlsP += 1;
                }
            }
            break;
            case ThyoneI_GPIO_IO_Output:
            {
                if (*uartP == 3)
                {
                    configP_running->GPIO_ID = *(uartP + 1);
                    configP_running->function = *(uartP + 2);
                    configP_running->value.output = *(uartP + 3);

                    configP_running++;
                    *numberOfControlsP += 1;
                }
            }
            break;
            case ThyoneI_GPIO_IO_PWM:
            {
                if (*uartP == 5)
                {
                    configP_running->GPIO_ID = *(uartP + 1);
                    configP_running->function = *(uartP + 2);
                    memcpy(&configP_running->value.pwm.period, (uartP + 3), 2);
                    configP_running->value.pwm.ratio = *(uartP + 5);

                    configP_running++;
                    *numberOfControlsP += 1;
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
    return true;
}

bool ThyoneI_GPIORemoteWrite(uint32_t destAddress, ThyoneI_GPIOControlBlock_t* controlP, uint16_t number_of_controls)
{
    if ((controlP == NULL) || (number_of_controls == 0))
    {
        return false;
    }

    uint16_t length = 4;

    for (uint16_t i = 0; i < number_of_controls; i++)
    {
        txPacket.Data[length] = 2;
        txPacket.Data[length + 1] = controlP->GPIO_ID;
        txPacket.Data[length + 2] = controlP->value.output;
        length += 3;
        controlP += sizeof(ThyoneI_GPIOControlBlock_t);
    }

    txPacket.Cmd = THYONEI_CMD_GPIO_REMOTE_WRITE_REQ;
    txPacket.Length = length;

    memcpy(&txPacket.Data[0], &destAddress, 4);

    FillChecksum(&txPacket);

    if (!ThyoneI_Transparent_Transmit((uint8_t*)&txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD))
    {
        return false;
    }

    /* wait for cnf */
    return Wait4CNF(CMD_WAIT_TIME, THYONEI_CMD_GPIO_REMOTE_WRITE_CNF, CMD_Status_Success, true);
}

bool ThyoneI_GPIORemoteRead(uint32_t destAddress, uint8_t* GPIOToReadP, uint8_t amountGPIOToRead, ThyoneI_GPIOControlBlock_t* controlP, uint16_t* numberOfControls)
{
    if ((GPIOToReadP == NULL) || (amountGPIOToRead == 0) || (controlP == NULL) || (numberOfControls == NULL))
    {
        return false;
    }

    /* payload is 4-byte destination address + pin configuration*/
    uint16_t commandLength = 1 + amountGPIOToRead + 4;

    txPacket.Cmd = THYONEI_CMD_GPIO_REMOTE_READ_REQ;
    txPacket.Length = commandLength;

    memcpy(&txPacket.Data[0], &destAddress, 4);
    txPacket.Data[4] = amountGPIOToRead;
    memcpy(&txPacket.Data[5], GPIOToReadP, amountGPIOToRead);

    FillChecksum(&txPacket);

    if (!ThyoneI_Transparent_Transmit((uint8_t*)&txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD))
    {
        return false;
    }

    /* wait for cnf */
    if (!Wait4CNF(1000, THYONEI_CMD_GPIO_REMOTE_READ_RSP, CMD_Status_NoStatus, true))
    {
        return false;
    }

    uint16_t length = rxPacket.Length;

    *numberOfControls = 0;
    uint8_t* uartP = &rxPacket.Data[1 + 4];
    ThyoneI_GPIOControlBlock_t* controlP_running = controlP;
    while (uartP < &rxPacket.Data[length])
    {
        if (*uartP == 2)
        {
            controlP_running->GPIO_ID = *(uartP + 1);
            controlP_running->value.output = *(uartP + 2);

            controlP_running++;
            *numberOfControls += 1;
        }
        uartP += *uartP + 1;
    }
    return true;
}

bool ThyoneI_IsTransparentModeBusy(bool* busyStateP)
{
    if (busyStateP == NULL)
    {
        return false;
    }

    WE_Pin_Level_t pin_level;

    if (!WE_GetPinLevel(ThyoneI_pinsP->ThyoneI_Pin_Busy, &pin_level))
    {
        return false;
    }

    *busyStateP = (pin_level == WE_Pin_Level_High);

    return true;
}
