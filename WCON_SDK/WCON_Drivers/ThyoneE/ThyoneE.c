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
 * @file ThyoneE.c
 * @brief Thyone-e driver source file.
 */

#include <ThyoneE/ThyoneE.h>
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

#define THYONEE_CMD_GETRAM (uint8_t)0x20
#define THYONEE_CMD_GETRAM_REQ (THYONEE_CMD_GETRAM | THYONEE_CMD_TYPE_REQ)
#define THYONEE_CMD_GETRAM_CNF (THYONEE_CMD_GETRAM | THYONEE_CMD_TYPE_CNF)

#define THYONEE_CMD_SETRAM (uint8_t)0x21
#define THYONEE_CMD_SETRAM_REQ (THYONEE_CMD_SETRAM | THYONEE_CMD_TYPE_REQ)
#define THYONEE_CMD_SETRAM_CNF (THYONEE_CMD_SETRAM | THYONEE_CMD_TYPE_CNF)

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

void ThyoneE_HandleRxByte(uint8_t* dataP, size_t size);
static WE_UART_HandleRxByte_t byteRxCallback = ThyoneE_HandleRxByte;

/**
 * @brief Type used to check the response, when a command was sent to the ThyoneE
 */
typedef enum ThyoneE_CMD_Status_t
{
    CMD_Status_Success = (uint8_t)0x00,
    CMD_Status_Failed = (uint8_t)0x01,
    CMD_Status_Invalid,
    CMD_Status_Reset,
    CMD_Status_NoStatus,
} ThyoneE_CMD_Status_t;

typedef struct
{
    uint8_t cmd;                 /* variable to check if correct CMD has been confirmed */
    ThyoneE_CMD_Status_t status; /* variable used to check the response (*_CNF), when a request (*_REQ) was sent to the ThyoneE */
} ThyoneE_CMD_Confirmation_t;

/**************************************
 *          Static variables          *
 **************************************/
static ThyoneE_CMD_Frame_t txPacket = {.Stx = CMD_STX, .Length = 0}; /* request to be sent to the module */
static ThyoneE_CMD_Frame_t rxPacket = {.Stx = CMD_STX, .Length = 0}; /* received packet that has been sent by the module */
;

#define CMDCONFIRMATIONARRAY_LENGTH 3
static ThyoneE_CMD_Confirmation_t cmdConfirmation_array[CMDCONFIRMATIONARRAY_LENGTH];
/**
 * @brief Pin configuration struct pointer.
 */
static ThyoneE_Pins_t* ThyoneE_pinsP = NULL;
/**
 * @brief Uart configuration struct pointer.
 */
static WE_UART_t* ThyoneE_uartP = NULL;

static uint8_t checksum = 0;
static uint16_t rxByteCounter = 0;
static uint16_t bytesToReceive = 0;
static uint8_t rxBuffer[sizeof(ThyoneE_CMD_Frame_t)];            /* For UART RX from module */
static void (*RxCallback)(uint8_t*, uint16_t, uint32_t, int8_t); /* callback function */
static ThyoneE_OperationMode_t operationMode = ThyoneE_OperationMode_CommandMode;

/**************************************
 *         Static functions           *
 **************************************/

static void HandleRxPacket(uint8_t* prxBuffer)
{
    ThyoneE_CMD_Confirmation_t cmdConfirmation;
    cmdConfirmation.cmd = CNFINVALID;
    cmdConfirmation.status = CMD_Status_Invalid;

    uint16_t cmdLength = ((ThyoneE_CMD_Frame_t*)prxBuffer)->Length;
    memcpy(&rxPacket, prxBuffer, cmdLength + LENGTH_CMD_OVERHEAD);

    switch (rxPacket.Cmd)
    {
        case THYONEE_CMD_RESET_CNF:
        case THYONEE_CMD_START_IND:
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
        case THYONEE_CMD_GETRAM_CNF:
        case THYONEE_CMD_SETRAM_CNF:
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

        case THYONEE_CMD_SNIFFER_IND:
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
 * @brief Function that waits for the return value of ThyoneE (*_CNF), when a command (*_REQ) was sent before
 */
static bool Wait4CNF(uint32_t max_time_ms, uint8_t expectedCmdConfirmation, ThyoneE_CMD_Status_t expectedStatus, bool reset_confirmstate)
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
static void FillChecksum(ThyoneE_CMD_Frame_t* cmd)
{
    uint8_t checksum = (uint8_t)cmd->Stx;
    uint8_t* pArray = (uint8_t*)cmd;
    for (uint16_t i = 1; i < (cmd->Length + LENGTH_CMD_OVERHEAD_WITHOUT_CRC); i++)
    {
        checksum ^= pArray[i];
    }
    cmd->Data[cmd->Length] = checksum;
}

void ThyoneE_HandleRxByte(uint8_t* dataP, size_t size)
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
bool ThyoneE_Transparent_Transmit(const uint8_t* data, uint16_t dataLength)
{
    if ((data == NULL) || (dataLength == 0))
    {
        return false;
    }

    return ThyoneE_uartP->uartTransmit((uint8_t*)data, dataLength);
}

bool ThyoneE_Init(WE_UART_t* uartP, ThyoneE_Pins_t* pinoutP, ThyoneE_OperationMode_t opMode, void (*RXcb)(uint8_t*, uint16_t, uint32_t, int8_t))
{
    /* set RX callback function */
    RxCallback = RXcb;

    operationMode = opMode;

    if ((pinoutP == NULL) || (uartP == NULL) || (uartP->uartInit == NULL) || (uartP->uartDeinit == NULL) || (uartP->uartTransmit == NULL))
    {
        return false;
    }

    ThyoneE_pinsP = pinoutP;
    ThyoneE_pinsP->ThyoneE_Pin_Reset.type = WE_Pin_Type_Output;
    ThyoneE_pinsP->ThyoneE_Pin_Reset.initial_value.output = WE_Pin_Level_High;
    ThyoneE_pinsP->ThyoneE_Pin_Busy.type = WE_Pin_Type_Input;
    ThyoneE_pinsP->ThyoneE_Pin_Busy.initial_value.input_pull = WE_Pin_PullType_No;
    ThyoneE_pinsP->ThyoneE_Pin_Mode.type = WE_Pin_Type_Output;
    ThyoneE_pinsP->ThyoneE_Pin_Mode.initial_value.output = ((operationMode == ThyoneE_OperationMode_TransparentMode) ? WE_Pin_Level_High : WE_Pin_Level_Low);

    WE_Pin_t pins[sizeof(ThyoneE_Pins_t) / sizeof(WE_Pin_t)];
    uint8_t pin_count = 0;
    memcpy(&pins[pin_count++], &ThyoneE_pinsP->ThyoneE_Pin_Reset, sizeof(WE_Pin_t));
    memcpy(&pins[pin_count++], &ThyoneE_pinsP->ThyoneE_Pin_Mode, sizeof(WE_Pin_t));
    memcpy(&pins[pin_count++], &ThyoneE_pinsP->ThyoneE_Pin_Busy, sizeof(WE_Pin_t));

    if (!WE_InitPins(pins, pin_count))
    {
        /* error */
        return false;
    }

    ThyoneE_uartP = uartP;
    if (false == ThyoneE_uartP->uartInit(ThyoneE_uartP->baudrate, ThyoneE_uartP->flowControl, ThyoneE_uartP->parity, &byteRxCallback))
    {
        return false;
    }
    WE_Delay(10);

    /* reset module */
    if (!ThyoneE_PinReset())
    {
        WE_DEBUG_PRINT_INFO("Pin reset failed\r\n");
        ThyoneE_Deinit();
        return false;
    }

    return true;
}

bool ThyoneE_Deinit()
{
    WE_Pin_t pins[sizeof(ThyoneE_Pins_t) / sizeof(WE_Pin_t)];
    uint8_t pin_count = 0;
    memcpy(&pins[pin_count++], &ThyoneE_pinsP->ThyoneE_Pin_Reset, sizeof(WE_Pin_t));
    memcpy(&pins[pin_count++], &ThyoneE_pinsP->ThyoneE_Pin_Mode, sizeof(WE_Pin_t));
    memcpy(&pins[pin_count++], &ThyoneE_pinsP->ThyoneE_Pin_Busy, sizeof(WE_Pin_t));

    /* deinit pins */
    if (!WE_DeinitPins(pins, pin_count))
    {
        return false;
    }

    RxCallback = NULL;

    return ThyoneE_uartP->uartDeinit();
}

bool ThyoneE_PinReset()
{
    if (!WE_SetPin(ThyoneE_pinsP->ThyoneE_Pin_Reset, WE_Pin_Level_Low))
    {
        return false;
    }

    WE_Delay(5);

    if (!WE_SetPin(ThyoneE_pinsP->ThyoneE_Pin_Reset, WE_Pin_Level_High))
    {
        return false;
    }

    if (operationMode == ThyoneE_OperationMode_TransparentMode)
    {
        WE_Delay(THYONEE_BOOT_DURATION);
        /* transparent mode is ready (the module doesn't send a "ready for operation" message in transparent mode) */
        return true;
    }

    /* wait for cnf */
    return Wait4CNF(CMD_WAIT_TIME, THYONEE_CMD_START_IND, CMD_Status_NoStatus, true);
}

bool ThyoneE_Reset()
{
    txPacket.Cmd = THYONEE_CMD_RESET_REQ;
    txPacket.Length = 0;

    FillChecksum(&txPacket);

    if (!ThyoneE_Transparent_Transmit((uint8_t*)&txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD))
    {
        return false;
    }

    /* wait for cnf */
    return Wait4CNF(CMD_WAIT_TIME, THYONEE_CMD_START_IND, CMD_Status_NoStatus, true);
}

bool ThyoneE_Sleep()
{
    txPacket.Cmd = THYONEE_CMD_SLEEP_REQ;
    txPacket.Length = 0;

    FillChecksum(&txPacket);

    if (!ThyoneE_Transparent_Transmit((uint8_t*)&txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD))
    {
        return false;
    }

    /* wait for cnf */
    return Wait4CNF(CMD_WAIT_TIME, THYONEE_CMD_SLEEP_CNF, CMD_Status_Success, true);
}

bool ThyoneE_TransmitBroadcast(uint8_t* payloadP, uint16_t length)
{
    if ((payloadP == NULL) || (length > MAX_RADIO_PAYLOAD_LENGTH))
    {
        return false;
    }

    txPacket.Cmd = THYONEE_CMD_BROADCAST_DATA_REQ;
    txPacket.Length = length;

    memcpy(&txPacket.Data[0], payloadP, length);

    FillChecksum(&txPacket);

    if (!ThyoneE_Transparent_Transmit((uint8_t*)&txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD))
    {
        return false;
    }

    if (!Wait4CNF(CMD_WAIT_TIME, THYONEE_CMD_DATA_CNF, CMD_Status_Success, true))
    {
        return false;
    }

    return Wait4CNF(CMD_WAIT_TIME, THYONEE_CMD_TXCOMPLETE_RSP, CMD_Status_Success, false);
}

bool ThyoneE_TransmitMulticast(uint8_t* payloadP, uint16_t length)
{
    if ((payloadP == NULL) || (length > MAX_RADIO_PAYLOAD_LENGTH))
    {
        return false;
    }

    txPacket.Cmd = THYONEE_CMD_MULTICAST_DATA_REQ;
    txPacket.Length = length;

    memcpy(&txPacket.Data[0], payloadP, length);

    FillChecksum(&txPacket);

    if (!ThyoneE_Transparent_Transmit((uint8_t*)&txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD))
    {
        return false;
    }

    if (!Wait4CNF(CMD_WAIT_TIME, THYONEE_CMD_DATA_CNF, CMD_Status_Success, true))
    {
        return false;
    }

    return Wait4CNF(CMD_WAIT_TIME, THYONEE_CMD_TXCOMPLETE_RSP, CMD_Status_Success, false);
}

bool ThyoneE_TransmitUnicast(uint8_t* payloadP, uint16_t length)
{
    if ((payloadP == NULL) || (length > MAX_RADIO_PAYLOAD_LENGTH))
    {
        return false;
    }

    txPacket.Cmd = THYONEE_CMD_UNICAST_DATA_REQ;
    txPacket.Length = length;

    memcpy(&txPacket.Data[0], payloadP, length);

    FillChecksum(&txPacket);

    if (!ThyoneE_Transparent_Transmit((uint8_t*)&txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD))
    {
        return false;
    }

    if (!Wait4CNF(CMD_WAIT_TIME, THYONEE_CMD_DATA_CNF, CMD_Status_Success, true))
    {
        return false;
    }

    return Wait4CNF(CMD_WAIT_TIME, THYONEE_CMD_TXCOMPLETE_RSP, CMD_Status_Success, false);
}

bool ThyoneE_TransmitMulticastExtended(uint8_t groupID, uint8_t* payloadP, uint16_t length)
{
    if ((payloadP == NULL) || (length > MAX_RADIO_PAYLOAD_LENGTH))
    {
        return false;
    }

    txPacket.Cmd = THYONEE_CMD_MULTICAST_DATA_EX_REQ;
    txPacket.Length = length + 1;
    txPacket.Data[0] = groupID;

    memcpy(&txPacket.Data[1], payloadP, length);

    FillChecksum(&txPacket);

    if (!ThyoneE_Transparent_Transmit((uint8_t*)&txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD))
    {
        return false;
    }

    if (!Wait4CNF(CMD_WAIT_TIME, THYONEE_CMD_DATA_CNF, CMD_Status_Success, true))
    {
        return false;
    }

    return Wait4CNF(CMD_WAIT_TIME, THYONEE_CMD_TXCOMPLETE_RSP, CMD_Status_Success, false);
}

bool ThyoneE_TransmitUnicastExtended(uint32_t address, uint8_t* payloadP, uint16_t length)
{
    if ((payloadP == NULL) || (length > MAX_RADIO_PAYLOAD_LENGTH))
    {
        return false;
    }

    txPacket.Cmd = THYONEE_CMD_UNICAST_DATA_EX_REQ;
    txPacket.Length = length + 4;

    memcpy(&txPacket.Data[0], &address, 4);
    memcpy(&txPacket.Data[4], payloadP, length);

    FillChecksum(&txPacket);

    if (!ThyoneE_Transparent_Transmit((uint8_t*)&txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD))
    {
        return false;
    }

    if (!Wait4CNF(CMD_WAIT_TIME, THYONEE_CMD_DATA_CNF, CMD_Status_Success, true))
    {
        return false;
    }

    return Wait4CNF(CMD_WAIT_TIME, THYONEE_CMD_TXCOMPLETE_RSP, CMD_Status_Success, false);
}

bool ThyoneE_SetRam(ThyoneE_RuntimeSettings_t runtimeSetting, uint8_t* valueP, uint8_t length)
{
    if (valueP == NULL)
    {
        return false;
    }

    txPacket.Cmd = THYONEE_CMD_SETRAM_REQ;
    txPacket.Length = 1 + length;
    txPacket.Data[0] = runtimeSetting;
    memcpy(&txPacket.Data[1], valueP, length);

    FillChecksum(&txPacket);

    if (!ThyoneE_Transparent_Transmit((uint8_t*)&txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD))
    {
        return false;
    }

    return Wait4CNF(CMD_WAIT_TIME, THYONEE_CMD_SETRAM_CNF, CMD_Status_Success, true);
}

bool ThyoneE_FactoryReset()
{
    txPacket.Cmd = THYONEE_CMD_FACTORYRESET_REQ;
    txPacket.Length = 0;

    FillChecksum(&txPacket);

    if (!ThyoneE_Transparent_Transmit((uint8_t*)&txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD))
    {
        return false;
    }

    /* wait for reset after factory reset */
    return Wait4CNF(CMD_WAIT_TIME, THYONEE_CMD_START_IND, CMD_Status_NoStatus, true);
}

bool ThyoneE_CheckNSet(ThyoneE_UserSettings_t userSetting, uint8_t* valueP, uint8_t length)
{
    if (valueP == NULL)
    {
        return false;
    }

    uint8_t current_value[length];
    uint16_t current_length = length;

    if (!ThyoneE_Get(userSetting, current_value, &current_length))
    {
        return false;
    }

    if ((length == current_length) && (0 == memcmp(valueP, current_value, length)))
    {
        /* value is already set, no need to set it again */
        return true;
    }

    /* value differs, and thus must be set */
    return ThyoneE_Set(userSetting, valueP, length);
}

bool ThyoneE_Set(ThyoneE_UserSettings_t userSetting, uint8_t* valueP, uint8_t length)
{
    if (valueP == NULL)
    {
        return false;
    }

    txPacket.Cmd = THYONEE_CMD_SET_REQ;
    txPacket.Length = 1 + length;
    txPacket.Data[0] = userSetting;
    memcpy(&txPacket.Data[1], valueP, length);

    FillChecksum(&txPacket);

    if (!ThyoneE_Transparent_Transmit((uint8_t*)&txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD))
    {
        return false;
    }

    /* wait for cnf */
    if (!Wait4CNF(CMD_WAIT_TIME, THYONEE_CMD_SET_CNF, CMD_Status_Success, true))
    {
        return false;
    }

    return Wait4CNF(CMD_WAIT_TIME, THYONEE_CMD_START_IND, CMD_Status_NoStatus, false);
}

bool ThyoneE_SetTXPower(ThyoneE_TXPower_t txPower) { return ThyoneE_Set(ThyoneE_USERSETTING_INDEX_RF_TX_POWER, (uint8_t*)&txPower, 1); }

bool ThyoneE_SetBaudrateIndex(ThyoneE_BaudRateIndex_t baudrate, ThyoneE_UartParity_t parity, bool flowcontrolEnable)
{
    uint8_t baudrateIndex = (uint8_t)baudrate;

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

    return ThyoneE_Set(ThyoneE_USERSETTING_INDEX_UART_CONFIG, (uint8_t*)&baudrateIndex, 1);
}

bool ThyoneE_SetRFChannel(uint8_t channel)
{
    /* permissible value for channel: 0-39 */
    if (channel > 39)
    {
        return false;
    }

    return ThyoneE_Set(ThyoneE_USERSETTING_INDEX_RF_CHANNEL, (uint8_t*)&channel, 1);
}

bool ThyoneE_SetRFChannelRuntime(uint8_t channel)
{
    /* permissible value for channel: 0-39 */
    if (channel > 39)
    {
        return false;
    }

    txPacket.Cmd = THYONEE_CMD_SETCHANNEL_REQ;
    txPacket.Length = 1;
    txPacket.Data[0] = channel;

    FillChecksum(&txPacket);

    if (!ThyoneE_Transparent_Transmit((uint8_t*)&txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD))
    {
        return false;
    }

    return Wait4CNF(CMD_WAIT_TIME, THYONEE_CMD_SETCHANNEL_CNF, CMD_Status_Success, true);
}

bool ThyoneE_SetEncryptionMode(ThyoneE_EncryptionMode_t encryptionMode) { return ThyoneE_Set(ThyoneE_USERSETTING_INDEX_ENCRYPTION_MODE, (uint8_t*)&encryptionMode, 1); }

bool ThyoneE_SetRfProfile(ThyoneE_Profile_t profile) { return ThyoneE_Set(ThyoneE_USERSETTING_INDEX_RF_PROFILE, (uint8_t*)&profile, 1); }

bool ThyoneE_SetNumRetries(uint8_t numRetries) { return ThyoneE_Set(ThyoneE_USERSETTING_INDEX_RF_NUM_RETRIES, (uint8_t*)&numRetries, 1); }

bool ThyoneE_SetRpNumSlots(uint8_t numSlots) { return ThyoneE_Set(ThyoneE_USERSETTING_INDEX_RF_RP_NUM_SLOTS, (uint8_t*)&numSlots, 1); }

bool ThyoneE_SetSourceAddress(uint32_t sourceAddress) { return ThyoneE_Set(ThyoneE_USERSETTING_INDEX_MAC_SOURCE_ADDRESS, (uint8_t*)&sourceAddress, 4); }

bool ThyoneE_SetDestinationAddress(uint32_t destinationAddress) { return ThyoneE_Set(ThyoneE_USERSETTING_INDEX_MAC_DESTINATION_ADDRESS, (uint8_t*)&destinationAddress, 4); }

bool ThyoneE_SetGroupID(uint8_t groupId) { return ThyoneE_Set(ThyoneE_USERSETTING_INDEX_MAC_GROUP_ID, (uint8_t*)&groupId, 1); }

bool ThyoneE_SetEncryptionKey(uint8_t* keyP) { return ThyoneE_Set(ThyoneE_USERSETTING_INDEX_MAC_ENCRYPTION_KEY, keyP, 16); }

bool ThyoneE_SetTimeToLive(uint8_t ttl) { return ThyoneE_Set(ThyoneE_USERSETTING_INDEX_MAC_TLL, &ttl, 1); }

bool ThyoneE_SetGPIOBlockRemoteConfig(uint8_t remoteConfig) { return ThyoneE_Set(ThyoneE_USERSETTING_INDEX_REMOTE_GPIO_CONFIG, &remoteConfig, 1); }

bool ThyoneE_SetModuleMode(ThyoneE_ModuleMode_t moduleMode) { return ThyoneE_Set(ThyoneE_USERSETTING_INDEX_MODULE_MODE, (uint8_t*)&moduleMode, 1); }

bool ThyoneE_GetRam(ThyoneE_RuntimeSettings_t runtimeSetting, uint8_t* responseP, uint16_t* lengthP)
{
    if (responseP == NULL || lengthP == NULL)
    {
        return false;
    }

    txPacket.Cmd = THYONEE_CMD_GETRAM_REQ;
    txPacket.Length = 1;
    txPacket.Data[0] = runtimeSetting;

    FillChecksum(&txPacket);

    if (!ThyoneE_Transparent_Transmit((uint8_t*)&txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD))
    {
        return false;
    }

    /* wait for cnf */
    if (!Wait4CNF(CMD_WAIT_TIME, THYONEE_CMD_GETRAM_CNF, CMD_Status_Success, true))
    {
        return false;
    }

    uint16_t length = rxPacket.Length;
    memcpy(responseP, &rxPacket.Data[1], length - 1); /* First Data byte is status, following bytes response*/
    *lengthP = length - 1;

    return true;
}

bool ThyoneE_Get(ThyoneE_UserSettings_t userSetting, uint8_t* responseP, uint16_t* lengthP)
{
    if (responseP == NULL || lengthP == NULL)
    {
        return false;
    }

    txPacket.Cmd = THYONEE_CMD_GET_REQ;
    txPacket.Length = 1;
    txPacket.Data[0] = userSetting;

    FillChecksum(&txPacket);

    if (!ThyoneE_Transparent_Transmit((uint8_t*)&txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD))
    {
        return false;
    }

    /* wait for cnf */
    if (!Wait4CNF(CMD_WAIT_TIME, THYONEE_CMD_GET_CNF, CMD_Status_Success, true))
    {
        return false;
    }

    uint16_t length = rxPacket.Length;
    memcpy(responseP, &rxPacket.Data[1], length - 1); /* First Data byte is status, following bytes response*/
    *lengthP = length - 1;

    return true;
}

bool ThyoneE_GetSerialNumber(uint8_t* serialNumberP)
{
    uint16_t length;
    return ThyoneE_Get(ThyoneE_USERSETTING_INDEX_SERIAL_NUMBER, serialNumberP, &length);
}

bool ThyoneE_GetFWVersion(uint8_t* versionP)
{
    uint16_t length;
    return ThyoneE_Get(ThyoneE_USERSETTING_INDEX_FW_VERSION, versionP, &length);
}

bool ThyoneE_GetTXPower(ThyoneE_TXPower_t* txpowerP)
{
    uint16_t length;
    return ThyoneE_Get(ThyoneE_USERSETTING_INDEX_RF_TX_POWER, (uint8_t*)txpowerP, &length);
}

bool ThyoneE_GetBaudrateIndex(ThyoneE_BaudRateIndex_t* baudrateP, ThyoneE_UartParity_t* parityP, bool* flowcontrolEnableP)
{
    if ((baudrateP == NULL) || (parityP == NULL) || (flowcontrolEnableP == NULL))
    {
        return false;
    }

    uint16_t length;
    uint8_t uartIndex;

    if (!ThyoneE_Get(ThyoneE_USERSETTING_INDEX_UART_CONFIG, (uint8_t*)&uartIndex, &length))
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
        *parityP = ThyoneE_UartParity_None;
    }
    else
    {
        *parityP = ThyoneE_UartParity_Even;
        uartIndex -= 64;
    }

    *baudrateP = (ThyoneE_BaudRateIndex_t)uartIndex;

    return true;
}

bool ThyoneE_GetEncryptionMode(ThyoneE_EncryptionMode_t* encryptionModeP)
{
    uint16_t length;
    return ThyoneE_Get(ThyoneE_USERSETTING_INDEX_ENCRYPTION_MODE, (uint8_t*)encryptionModeP, &length);
}

bool ThyoneE_GetRfProfile(ThyoneE_Profile_t* profileP)
{
    uint16_t length;
    return ThyoneE_Get(ThyoneE_USERSETTING_INDEX_RF_PROFILE, (uint8_t*)profileP, &length);
}

bool ThyoneE_GetRFChannel(uint8_t* channelP)
{
    uint16_t length;
    return ThyoneE_Get(ThyoneE_USERSETTING_INDEX_RF_CHANNEL, (uint8_t*)channelP, &length);
}

bool ThyoneE_GetNumRetries(uint8_t* numRetriesP)
{
    uint16_t length;
    return ThyoneE_Get(ThyoneE_USERSETTING_INDEX_RF_NUM_RETRIES, numRetriesP, &length);
}

bool ThyoneE_GetRpNumSlots(uint8_t* numSlotsP)
{
    uint16_t length;
    return ThyoneE_Get(ThyoneE_USERSETTING_INDEX_RF_RP_NUM_SLOTS, numSlotsP, &length);
}

bool ThyoneE_GetSourceAddress(uint32_t* sourceAddressP)
{
    uint16_t length;
    return ThyoneE_Get(ThyoneE_USERSETTING_INDEX_MAC_SOURCE_ADDRESS, (uint8_t*)sourceAddressP, &length);
}

bool ThyoneE_GetDestinationAddress(uint32_t* destinationAddressP)
{
    uint16_t length;
    return ThyoneE_Get(ThyoneE_USERSETTING_INDEX_MAC_DESTINATION_ADDRESS, (uint8_t*)destinationAddressP, &length);
}

bool ThyoneE_GetGroupID(uint8_t* groupIdP)
{
    uint16_t length;
    return ThyoneE_Get(ThyoneE_USERSETTING_INDEX_MAC_GROUP_ID, groupIdP, &length);
}

bool ThyoneE_GetTimeToLive(uint8_t* ttlP)
{
    uint16_t length;
    return ThyoneE_Get(ThyoneE_USERSETTING_INDEX_MAC_TLL, ttlP, &length);
}

bool ThyoneE_GetGPIOBlockRemoteConfig(uint8_t* remoteConfigP)
{
    uint16_t length;
    return ThyoneE_Get(ThyoneE_USERSETTING_INDEX_REMOTE_GPIO_CONFIG, remoteConfigP, &length);
}

bool ThyoneE_GetModuleMode(ThyoneE_ModuleMode_t* moduleModeP)
{
    uint16_t length;
    return ThyoneE_Get(ThyoneE_USERSETTING_INDEX_MODULE_MODE, (uint8_t*)moduleModeP, &length);
}

bool ThyoneE_GetState(ThyoneE_States_t* state)
{
    if (state == NULL)
    {
        return false;
    }

    txPacket.Cmd = THYONEE_CMD_GETSTATE_REQ;
    txPacket.Length = 0;

    FillChecksum(&txPacket);

    if (!ThyoneE_Transparent_Transmit((uint8_t*)&txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD))
    {
        return false;
    }

    /* wait for cnf */
    if (!Wait4CNF(CMD_WAIT_TIME, THYONEE_CMD_GETSTATE_CNF, CMD_Status_Success, true))
    {
        return false;
    }

    *state = rxPacket.Data[1];
    return true;
}

bool ThyoneE_GPIOLocalSetConfig(ThyoneE_GPIOConfigBlock_t* configP, uint16_t number_of_configs)
{
    if ((configP == NULL) || (number_of_configs == 0))
    {
        return false;
    }

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

    FillChecksum(&txPacket);

    if (!ThyoneE_Transparent_Transmit((uint8_t*)&txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD))
    {
        return false;
    }

    /* wait for cnf */
    return Wait4CNF(CMD_WAIT_TIME, THYONEE_CMD_GPIO_LOCAL_SETCONFIG_CNF, CMD_Status_Success, true);
}

bool ThyoneE_GPIOLocalGetConfig(ThyoneE_GPIOConfigBlock_t* configP, uint16_t* number_of_configsP)
{
    if ((configP == NULL) || (number_of_configsP == NULL))
    {
        return false;
    }

    txPacket.Cmd = THYONEE_CMD_GPIO_LOCAL_GETCONFIG_REQ;
    txPacket.Length = 0;

    FillChecksum(&txPacket);

    if (!ThyoneE_Transparent_Transmit((uint8_t*)&txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD))
    {
        return false;
    }

    /* wait for cnf */
    if (!Wait4CNF(CMD_WAIT_TIME, THYONEE_CMD_GPIO_LOCAL_GETCONFIG_CNF, CMD_Status_Success, true))
    {
        return false;
    }

    uint16_t length = rxPacket.Length;

    *number_of_configsP = 0;
    uint8_t* uartP = &rxPacket.Data[1];
    ThyoneE_GPIOConfigBlock_t* configP_running = configP;
    while (uartP < &rxPacket.Data[length])
    {
        switch (*(uartP + 2))
        {
            case ThyoneE_GPIO_IO_Disconnected:
            {
                if (*uartP == 2)
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
    return true;
}

bool ThyoneE_GPIOLocalWrite(ThyoneE_GPIOControlBlock_t* controlP, uint16_t number_of_controls)
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

    txPacket.Cmd = THYONEE_CMD_GPIO_LOCAL_WRITE_REQ;
    txPacket.Length = length;

    FillChecksum(&txPacket);

    if (!ThyoneE_Transparent_Transmit((uint8_t*)&txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD))
    {
        return false;
    }

    /* wait for cnf */
    return Wait4CNF(CMD_WAIT_TIME, THYONEE_CMD_GPIO_LOCAL_WRITE_CNF, CMD_Status_Success, true);
}

bool ThyoneE_GPIOLocalRead(uint8_t* GPIOToReadP, uint8_t amountGPIOToRead, ThyoneE_GPIOControlBlock_t* controlP, uint16_t* number_of_controlsP)
{
    if ((GPIOToReadP == NULL) || (amountGPIOToRead == 0) || (controlP == NULL) || (number_of_controlsP == NULL))
    {
        return false;
    }

    txPacket.Cmd = THYONEE_CMD_GPIO_LOCAL_READ_REQ;
    txPacket.Length = amountGPIOToRead + 1;
    txPacket.Data[0] = amountGPIOToRead;
    memcpy(&txPacket.Data[1], GPIOToReadP, amountGPIOToRead);

    FillChecksum(&txPacket);

    if (!ThyoneE_Transparent_Transmit((uint8_t*)&txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD))
    {
        return false;
    }

    /* wait for cnf */
    if (!Wait4CNF(CMD_WAIT_TIME, THYONEE_CMD_GPIO_LOCAL_READ_CNF, CMD_Status_Success, true))
    {
        return false;
    }

    uint16_t length = rxPacket.Length;

    *number_of_controlsP = 0;
    uint8_t* uartP = &rxPacket.Data[1];
    ThyoneE_GPIOControlBlock_t* controlP_running = controlP;
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
    return true;
}

bool ThyoneE_GPIORemoteSetConfig(uint32_t destAddress, ThyoneE_GPIOConfigBlock_t* configP, uint16_t number_of_configs)
{
    if ((configP == NULL) || (number_of_configs == 0))
    {
        return false;
    }

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

    FillChecksum(&txPacket);

    if (!ThyoneE_Transparent_Transmit((uint8_t*)&txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD))
    {
        return false;
    }

    /* wait for cnf */
    return Wait4CNF(CMD_WAIT_TIME, THYONEE_CMD_GPIO_REMOTE_SETCONFIG_CNF, CMD_Status_Success, true);
}

bool ThyoneE_GPIORemoteGetConfig(uint32_t destAddress, ThyoneE_GPIOConfigBlock_t* configP, uint16_t* number_of_configsP)
{
    if ((configP == NULL) || (number_of_configsP == NULL))
    {
        return false;
    }

    txPacket.Cmd = THYONEE_CMD_GPIO_REMOTE_GETCONFIG_REQ;
    txPacket.Length = 4;
    memcpy(&txPacket.Data[0], &destAddress, 4);

    FillChecksum(&txPacket);

    if (!ThyoneE_Transparent_Transmit((uint8_t*)&txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD))
    {
        return false;
    }

    /* wait for cnf */
    if (!Wait4CNF(CMD_WAIT_TIME, THYONEE_CMD_GPIO_REMOTE_GETCONFIG_RSP, CMD_Status_NoStatus, true))
    {
        return false;
    }

    uint16_t length = rxPacket.Length;

    *number_of_configsP = 0;
    uint8_t* uartP = &rxPacket.Data[1 + 4];
    ThyoneE_GPIOConfigBlock_t* configP_running = configP;
    while (uartP < &rxPacket.Data[length])
    {
        switch (*(uartP + 2))
        {
            case ThyoneE_GPIO_IO_Disconnected:
            {
                if (*uartP == 2)
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
    return true;
}

bool ThyoneE_GPIORemoteWrite(uint32_t destAddress, ThyoneE_GPIOControlBlock_t* controlP, uint16_t number_of_controls)
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
        controlP += sizeof(ThyoneE_GPIOControlBlock_t);
    }

    txPacket.Cmd = THYONEE_CMD_GPIO_REMOTE_WRITE_REQ;
    txPacket.Length = length;

    memcpy(&txPacket.Data[0], &destAddress, 4);

    FillChecksum(&txPacket);

    if (!ThyoneE_Transparent_Transmit((uint8_t*)&txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD))
    {
        return false;
    }

    /* wait for cnf */
    return Wait4CNF(CMD_WAIT_TIME, THYONEE_CMD_GPIO_REMOTE_WRITE_CNF, CMD_Status_Success, true);
}

bool ThyoneE_GPIORemoteRead(uint32_t destAddress, uint8_t* GPIOToReadP, uint8_t amountGPIOToRead, ThyoneE_GPIOControlBlock_t* controlP, uint16_t* number_of_controlsP)
{
    if ((GPIOToReadP == NULL) || (amountGPIOToRead == 0) || (controlP == NULL) || (number_of_controlsP == NULL))
    {
        return false;
    }

    /* payload is 4-byte destination address + pin configuration*/
    uint16_t commandLength = 1 + amountGPIOToRead + 4;

    txPacket.Cmd = THYONEE_CMD_GPIO_REMOTE_READ_REQ;
    txPacket.Length = commandLength;

    memcpy(&txPacket.Data[0], &destAddress, 4);
    txPacket.Data[4] = amountGPIOToRead;
    memcpy(&txPacket.Data[5], GPIOToReadP, amountGPIOToRead);

    FillChecksum(&txPacket);

    if (!ThyoneE_Transparent_Transmit((uint8_t*)&txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD))
    {
        return false;
    }

    /* wait for cnf */
    if (!Wait4CNF(1000, THYONEE_CMD_GPIO_REMOTE_READ_RSP, CMD_Status_NoStatus, true))
    {
        return false;
    }

    uint16_t length = rxPacket.Length;

    *number_of_controlsP = 0;
    uint8_t* uartP = &rxPacket.Data[1 + 4];
    ThyoneE_GPIOControlBlock_t* controlP_running = controlP;
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
    return true;
}

bool ThyoneE_IsTransparentModeBusy(bool* busyStateP)
{
    if (busyStateP == NULL)
    {
        return false;
    }

    WE_Pin_Level_t pin_level;

    if (!WE_GetPinLevel(ThyoneE_pinsP->ThyoneE_Pin_Busy, &pin_level))
    {
        return false;
    }

    *busyStateP = (pin_level == WE_Pin_Level_High);

    return true;
}
