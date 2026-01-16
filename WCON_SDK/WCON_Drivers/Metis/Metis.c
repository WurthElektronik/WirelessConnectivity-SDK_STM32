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
 * @file
 * @brief Metis driver source file.
 */

#include <Metis/Metis.h>
#include <global/global.h>
#include <stdio.h>
#include <string.h>

#define CMD_WAIT_TIME 500
#define CNFINVALID 255
#define MAX_PAYLOAD_LENGTH 254
#define TXPOWERINVALID -128
#define RSSIINVALID -128

#define LENGTH_CMD_OVERHEAD (uint16_t)4
#define LENGTH_CMD_OVERHEAD_WITHOUT_CRC (uint16_t)(LENGTH_CMD_OVERHEAD - 1)
#define MAX_CMD_LENGTH (uint16_t)(Metis_MAX_PAYLOAD_LENGTH + LENGTH_CMD_OVERHEAD)

#define CMD_STX 0xFF
#define METIS_CMD_TYPE_REQ (0 << 6)
#define METIS_CMD_TYPE_CNF (2 << 6)

#define METIS_CMD_DATA 0x00
#define METIS_CMD_DATA_REQ (METIS_CMD_DATA | METIS_CMD_TYPE_REQ)
#define METIS_CMD_DATA_CNF (METIS_CMD_DATA | METIS_CMD_TYPE_CNF)

#define METIS_CMD_DATA_IND 0x03

#define METIS_CMD_SET_MODE 0x04
#define METIS_CMD_SET_MODE_REQ (METIS_CMD_SET_MODE | METIS_CMD_TYPE_REQ)
#define METIS_CMD_SET_MODE_CNF (METIS_CMD_SET_MODE | METIS_CMD_TYPE_CNF)

#define METIS_CMD_RESET 0x05
#define METIS_CMD_RESET_REQ (METIS_CMD_RESET | METIS_CMD_TYPE_REQ)
#define METIS_CMD_RESET_CNF (METIS_CMD_RESET | METIS_CMD_TYPE_CNF)

#define METIS_CMD_SET 0x09
#define METIS_CMD_SET_REQ (METIS_CMD_SET | METIS_CMD_TYPE_REQ)
#define METIS_CMD_SET_CNF (METIS_CMD_SET | METIS_CMD_TYPE_CNF)

#define METIS_CMD_SETUARTSPEED 0X10
#define METIS_CMD_SETUARTSPEED_REQ (METIS_CMD_SETUARTSPEED | METIS_CMD_TYPE_REQ)
#define METIS_CMD_SETUARTSPEED_CNF (METIS_CMD_SETUARTSPEED | METIS_CMD_TYPE_CNF)

#define METIS_CMD_GET 0x0A
#define METIS_CMD_GET_REQ (METIS_CMD_GET | METIS_CMD_TYPE_REQ)
#define METIS_CMD_GET_CNF (METIS_CMD_GET | METIS_CMD_TYPE_CNF)

#define METIS_CMD_GET_SERIALNO 0x0B
#define METIS_CMD_GET_SERIALNO_REQ (METIS_CMD_GET_SERIALNO | METIS_CMD_TYPE_REQ)
#define METIS_CMD_GET_SERIALNO_CNF (METIS_CMD_GET_SERIALNO | METIS_CMD_TYPE_CNF)

#define METIS_CMD_GET_FWRELEASE 0x0C
#define METIS_CMD_GET_FWRELEASE_REQ (METIS_CMD_GET_FWRELEASE | METIS_CMD_TYPE_REQ)
#define METIS_CMD_GET_FWRELEASE_CNF (METIS_CMD_GET_FWRELEASE | METIS_CMD_TYPE_CNF)

#define METIS_CMD_RSSI 0x0D
#define METIS_CMD_RSSI_REQ (METIS_CMD_RSSI | METIS_CMD_TYPE_REQ)
#define METIS_CMD_RSSI_CNF (METIS_CMD_RSSI | METIS_CMD_TYPE_CNF)

#define METIS_CMD_FACTORYRESET 0X11
#define METIS_CMD_FACTORYRESET_REQ (METIS_CMD_FACTORYRESET | METIS_CMD_TYPE_REQ)
#define METIS_CMD_FACTORYRESET_CNF (METIS_CMD_FACTORYRESET | METIS_CMD_TYPE_CNF)

void Metis_HandleRxByte(uint8_t* dataP, size_t size);
static WE_UART_HandleRxByte_t byteRxCallback = Metis_HandleRxByte;

/**
 * @brief Type used to check the response, when a command was sent to the module
 */
typedef enum Metis_CMD_Status_t
{
    CMD_Status_Success = 0x00,
    CMD_Status_Failed,
    CMD_Status_Invalid,
    CMD_Status_Reset,
} Metis_CMD_Status_t;

typedef struct
{
    const uint8_t Stx;
    uint8_t Cmd;
    uint8_t Length;
    uint8_t Data[MAX_PAYLOAD_LENGTH + 1]; /* +1 for the CS */
} Metis_CMD_Frame_t;

typedef struct
{
    uint8_t cmd;               /* variable to check if correct CMD has been confirmed */
    Metis_CMD_Status_t status; /* variable used to check the response (*_CNF), when a request (*_REQ) was sent to the module */
} Metis_CMD_Confirmation_t;

typedef struct
{
    uint8_t memoryPosition;   /* memory position of requested usersetting */
    uint8_t lengthGetRequest; /* length of one or more requested usersetting */
} Metis_US_Confirmation_t;

/**************************************
 *          Static variables          *
 **************************************/

/**
 * @brief Pin configuration struct pointer.
 */
static Metis_Pins_t* Metis_pinsP = NULL;

/**
 * @brief Uart configuration struct pointer.
 */
static WE_UART_t* Metis_uartP = NULL;
static Metis_CMD_Frame_t rxPacket;                                 /* data buffer for RX */
static Metis_CMD_Frame_t txPacket = {.Stx = CMD_STX, .Length = 0}; /* request to be sent to the module */

#define CMDCONFIRMATIONARRAY_LENGTH 2
static Metis_CMD_Confirmation_t cmdConfirmation_array[CMDCONFIRMATIONARRAY_LENGTH];
static Metis_US_Confirmation_t usConfirmation; /* variable used to check if GET function was successful */
static Metis_Frequency_t frequency;            /* frequency used by module */
static bool rssi_enable = false;
static uint8_t checksum = 0;
static uint16_t rxByteCounter = 0;
static uint8_t bytesToReceive = 0;                  /* read buffer for next available byte */
static uint8_t rxBuffer[sizeof(Metis_CMD_Frame_t)]; /* data buffer for RX */
static Metis_RxCallback_t RxCallback;
/* callback function */

/**************************************
 *          Static functions          *
 **************************************/

/**
 * @brief Function to add the checksum at the end of the data packet. 
 */
static void FillChecksum(Metis_CMD_Frame_t* cmd)
{
    uint8_t checksum = (uint8_t)cmd->Stx;
    uint8_t* pArray = (uint8_t*)cmd;
    for (uint8_t i = 1; i < (cmd->Length + LENGTH_CMD_OVERHEAD_WITHOUT_CRC); i++)
    {
        checksum ^= pArray[i];
    }
    cmd->Data[cmd->Length] = checksum;
}

static int8_t CalculateRSSIValue(uint8_t rxLevel)
{
    const uint8_t offset = 74;
    return (rxLevel < 128) ? (rxLevel / 2 - offset) : ((rxLevel - 256) / 2 - offset);
}

static void HandleRxPacket(uint8_t* packetData)
{
    Metis_CMD_Confirmation_t cmdConfirmation;
    cmdConfirmation.cmd = CNFINVALID;
    cmdConfirmation.status = CMD_Status_Invalid;

    uint8_t cmd_length = packetData[2];
    memcpy((uint8_t*)&rxPacket, packetData, cmd_length + 4); /* payload + std + command + length byte + checksum */

    switch (rxPacket.Cmd)
    {
        case METIS_CMD_DATA_IND:
        {
            /* the call of the RxCallback strongly depends on the configuration of the module */
            if (RxCallback != NULL)
            {
                if (rssi_enable == 0x01)
                {
                    /* the following implementation expects that the RSSI_Enable usersetting is enabled */
                    rxPacket.Length = rxPacket.Length - 1;
                    RxCallback(&rxPacket.Length, rxPacket.Length + 1, CalculateRSSIValue(rxPacket.Data[rxPacket.Length]));
                }
                else
                {
                    /* the following implementation expects that the RSSI_Enable usersetting is disabled */
                    RxCallback(&rxPacket.Length, rxPacket.Length + 1, (int8_t)RSSIINVALID);
                }
            }
        }
        break;

        case METIS_CMD_GET_CNF:
        {
            /* Data[0] contains memory position of usersetting
         * Data[1] contains length of parameter, which is depending on usersetting
         * On success mode responds with usersetting, length of parameter and parameter
         */
            switch (rxPacket.Data[0])
            {
                /* usersettings with value length of 1 byte */
                case (Metis_USERSETTING_MEMPOSITION_UART_CMD_OUT_ENABLE):
                case (Metis_USERSETTING_MEMPOSITION_APP_AES_ENABLE):
                case (Metis_USERSETTING_MEMPOSITION_DEFAULTRFTXPOWER):
                case (Metis_USERSETTING_MEMPOSITION_RSSI_ENABLE):
                case (Metis_USERSETTING_MEMPOSITION_MODE_PRESELECT):
                {
                    /* check if correct usersetting was changed and if length corresponds to usersetting */
                    if ((usConfirmation.memoryPosition == rxPacket.Data[0]) && (usConfirmation.lengthGetRequest == rxPacket.Data[1]))
                    {
                        cmdConfirmation.status = CMD_Status_Success;
                    }
                    else
                    {
                        cmdConfirmation.status = CMD_Status_Failed;
                    }
                    cmdConfirmation.cmd = rxPacket.Cmd;
                }
                break;
                    /* usersettings with value length of 2 byte*/
                case (Metis_USERSETTING_MEMPOSITION_CFG_FLAGS):
                {
                    /* check if correct usersetting was changed and if length corresponds to usersetting */
                    if ((usConfirmation.memoryPosition == rxPacket.Data[0]) && (usConfirmation.lengthGetRequest == rxPacket.Data[1]))
                    {
                        cmdConfirmation.status = CMD_Status_Success;
                    }
                    else
                    {
                        cmdConfirmation.status = CMD_Status_Failed;
                    }
                    cmdConfirmation.cmd = rxPacket.Cmd;
                }
                break;

                default:
                    break;
            }
        }
        break;

        case METIS_CMD_FACTORYRESET_CNF:
        case METIS_CMD_SET_CNF:
        case METIS_CMD_SET_MODE_CNF:
        case METIS_CMD_RESET_CNF:
        case METIS_CMD_DATA_CNF:
        case METIS_CMD_SETUARTSPEED_CNF:
        {
            cmdConfirmation.status = (rxPacket.Data[0] == 0x00) ? CMD_Status_Success : CMD_Status_Failed;
            cmdConfirmation.cmd = rxPacket.Cmd;
        }
        break;

        case METIS_CMD_GET_SERIALNO_CNF:
        {
            cmdConfirmation.status = (rxPacket.Length == 4) ? CMD_Status_Success : CMD_Status_Failed;
            cmdConfirmation.cmd = rxPacket.Cmd;
        }
        break;

        case METIS_CMD_GET_FWRELEASE_CNF:
        {
            cmdConfirmation.status = (rxPacket.Length == 3) ? CMD_Status_Success : CMD_Status_Failed;
            cmdConfirmation.cmd = rxPacket.Cmd;
        }
        break;
        default:
            break;
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

static bool Wait4CNF(uint32_t max_time_ms, uint8_t expectedCmdConfirmation, Metis_CMD_Status_t expectedStatus, bool reset_confirmstate)
{
    int count = 0;
    int time_step_ms = 5; /* 5ms */
    int max_count = max_time_ms / time_step_ms;

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

void Metis_HandleRxByte(uint8_t* dataP, size_t size)
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
                /* wait for SFD */
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
                /* length field */
                rxByteCounter++;
                bytesToReceive = (rxBuffer[rxByteCounter - 1] + 4); /* len + crc + sfd + cmd */
                break;

            default:
                /* data field */
                rxByteCounter++;
                if (rxByteCounter >= bytesToReceive)
                {
                    /* check CRC */
                    checksum = 0;
                    for (uint8_t i = 0; i < (bytesToReceive - 1); i++)
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

bool Metis_Transparent_Transmit(const uint8_t* dataP, uint16_t dataLength)
{
    if ((dataP == NULL) || (dataLength == 0))
    {
        return false;
    }

    return Metis_uartP->uartTransmit((uint8_t*)dataP, dataLength);
}

bool Metis_Init(WE_UART_t* uartP, Metis_Pins_t* pinoutP, Metis_Frequency_t freq, Metis_Mode_Preselect_t mode, bool enable_rssi, Metis_RxCallback_t RXcb)
{
    /* set frequency used by module */
    frequency = freq;

    /* set RX callback function */
    RxCallback = RXcb;

    /* set rssi_enable */
    rssi_enable = enable_rssi;

    if ((pinoutP == NULL) || (uartP == NULL) || (uartP->uartInit == NULL) || (uartP->uartDeinit == NULL) || (uartP->uartTransmit == NULL))
    {
        return false;
    }

    Metis_pinsP = pinoutP;
    Metis_pinsP->Metis_Pin_Reset.type = WE_Pin_Type_Output;
    Metis_pinsP->Metis_Pin_Reset.initial_value.output = WE_Pin_Level_High;

    if (!WE_InitPins(&Metis_pinsP->Metis_Pin_Reset, 1))
    {
        /* error */
        return false;
    }

    Metis_uartP = uartP;
    if (!Metis_uartP->uartInit(Metis_uartP->baudrate, Metis_uartP->flowControl, Metis_uartP->parity, &byteRxCallback))
    {
        return false;
    }
    WE_Delay(1000);

    /* set recommended settings as described in the manual section 5.1 */

    /* enable uartOutEnable to print out received frames
     * Setting is written to flash so write only if necessary
     */
    uint8_t uartEnable;
    if (!Metis_GetUartOutEnable(&uartEnable))
    {
        return false;
    }

    if (uartEnable != 1)
    {
        WE_Delay(50);
        if (Metis_SetUartOutEnable(1))
        {
            WE_Delay(50);
        }
        else
        {
            WE_DEBUG_PRINT_INFO("Set UART_CMD_OUT_MODE failed\r\n");
            Metis_Deinit();
            return false;
        }
    }

    /* enable rssi to be added to received frames
     * Setting is written to flash so write only if necessary
     */
    uint8_t rssi;
    if (!Metis_GetRSSIEnable(&rssi))
    {
        return false;
    }

    if (rssi != rssi_enable)
    {
        WE_Delay(50);
        if (Metis_SetRSSIEnable(rssi_enable ? 1 : 0))
        {
            WE_Delay(50);
        }
        else
        {
            WE_DEBUG_PRINT_INFO("Set RSSI failed\r\n");
            Metis_Deinit();
            return false;
        }
    }

    /* disable AES encryption
     * Setting is written to flash so write only if necessary
     */
    uint8_t aesEnable;
    if (!Metis_GetAESEnable(&aesEnable))
    {
        return false;
    }

    if (aesEnable != 0)
    {
        WE_Delay(50);
        if (Metis_SetAESEnable(0))
        {
            WE_Delay(50);
        }
        else
        {
            WE_DEBUG_PRINT_INFO("Set AESEnable failed\r\n");
            Metis_Deinit();
            return false;
        }
    }

    /* set mode preselect
     * Setting is written to flash so write only if necessary
     */
    uint8_t modePreselect;
    if (!Metis_GetModePreselect(&modePreselect))
    {
        return false;
    }

    if (modePreselect != mode)
    {
        WE_Delay(50);
        if (Metis_SetModePreselect(mode))
        {
            WE_Delay(50);
        }
        else
        {
            WE_DEBUG_PRINT_INFO("Set mode preselect failed\r\n");
            Metis_Deinit();
            return false;
        }
    }

    /* Reset module to apply changes */
    if (Metis_Reset())
    {
        WE_Delay(300);
    }
    else
    {
        WE_DEBUG_PRINT_INFO("Reset failed\r\n");
        Metis_Deinit();
        return false;
    }
    return true;
}

bool Metis_Deinit()
{
    /* deinit pins */
    if (!WE_DeinitPins(&Metis_pinsP->Metis_Pin_Reset, 1))
    {
        return false;
    }

    /* deinit RX callback */
    RxCallback = NULL;

    return Metis_uartP->uartDeinit();
}

bool Metis_PinReset()
{
    if (!WE_SetPin(Metis_pinsP->Metis_Pin_Reset, WE_Pin_Level_Low))
    {
        return false;
    }

    WE_Delay(5);

    return WE_SetPin(Metis_pinsP->Metis_Pin_Reset, WE_Pin_Level_High);
    ;
}

bool Metis_Reset()
{
    txPacket.Cmd = METIS_CMD_RESET_REQ;
    txPacket.Length = 0x00;

    FillChecksum(&txPacket);

    if (!Metis_Transparent_Transmit((uint8_t*)&txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD))
    {
        return false;
    }

    /* wait for cnf */
    return Wait4CNF(CMD_WAIT_TIME, METIS_CMD_RESET_CNF, CMD_Status_Success, true);
}

bool Metis_FactoryReset()
{
    txPacket.Cmd = METIS_CMD_FACTORYRESET_REQ;
    txPacket.Length = 0x00;

    FillChecksum(&txPacket);

    if (!Metis_Transparent_Transmit((uint8_t*)&txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD))
    {
        return false;
    }

    /* wait for cnf */
    return Wait4CNF(CMD_WAIT_TIME, METIS_CMD_FACTORYRESET_CNF, CMD_Status_Success, true);
}

bool Metis_SetUartSpeed(Metis_UartBaudrate_t baudrate)
{
    txPacket.Cmd = METIS_CMD_SETUARTSPEED;
    txPacket.Length = 0x01;
    txPacket.Data[0] = baudrate;

    FillChecksum(&txPacket);

    if (!Metis_Transparent_Transmit((uint8_t*)&txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD))
    {
        return false;
    }

    /* wait for cnf */
    return Wait4CNF(CMD_WAIT_TIME, METIS_CMD_SETUARTSPEED_CNF, CMD_Status_Success, true);
}

bool Metis_Get(Metis_UserSettings_t us, uint8_t* responseP, uint8_t* response_lengthP)
{
    if (responseP == NULL || response_lengthP == NULL)
    {
        return false;
    }

    txPacket.Cmd = METIS_CMD_GET_REQ;
    txPacket.Length = 0x02;
    txPacket.Data[0] = us;

    switch (us)
    {
        /* usersettings with value length 1 */
        case (Metis_USERSETTING_MEMPOSITION_UART_CMD_OUT_ENABLE):
        case (Metis_USERSETTING_MEMPOSITION_APP_AES_ENABLE):
        case (Metis_USERSETTING_MEMPOSITION_DEFAULTRFTXPOWER):
        case (Metis_USERSETTING_MEMPOSITION_RSSI_ENABLE):
        case (Metis_USERSETTING_MEMPOSITION_MODE_PRESELECT):
        {
            txPacket.Data[1] = 0x01;
        }
        break;
            /* usersettings with value length 2 */
        case (Metis_USERSETTING_MEMPOSITION_CFG_FLAGS):
        {
            txPacket.Data[1] = 0x02;
        }
        break;
        default:
            break;
    }

    FillChecksum(&txPacket);

    usConfirmation.memoryPosition = us;
    usConfirmation.lengthGetRequest = txPacket.Data[1];

    if (!Metis_Transparent_Transmit((uint8_t*)&txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD))
    {
        return false;
    }

    /* wait for cnf */
    bool ret = Wait4CNF(CMD_WAIT_TIME, METIS_CMD_GET_CNF, CMD_Status_Success, true);

    if (ret)
    {
        int length = rxPacket.Length - 2;
        memcpy(responseP, &rxPacket.Data[2], length);
        *response_lengthP = length;
    }

    usConfirmation.memoryPosition = -1;
    usConfirmation.lengthGetRequest = -1;

    return ret;
}

bool Metis_GetMultiple(uint8_t startAddress, uint8_t lengthToRead, uint8_t* responseP, uint8_t* response_lengthP)
{
    if ((responseP == NULL) || (response_lengthP == NULL) || (lengthToRead == 0))
    {
        return false;
    }

    txPacket.Cmd = METIS_CMD_GET_REQ;
    txPacket.Length = 0x02;
    txPacket.Data[0] = startAddress;
    txPacket.Data[1] = lengthToRead;

    FillChecksum(&txPacket);

    usConfirmation.memoryPosition = startAddress;
    usConfirmation.lengthGetRequest = lengthToRead;

    if (!Metis_Transparent_Transmit((uint8_t*)&txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD))
    {
        return false;
    }

    /* wait for cnf */
    bool ret = Wait4CNF(CMD_WAIT_TIME, METIS_CMD_GET_CNF, CMD_Status_Success, true);

    if (ret)
    {
        int length = rxPacket.Length - 2;
        memcpy(responseP, &rxPacket.Data[2], length);
        *response_lengthP = length;
    }

    usConfirmation.memoryPosition = -1;
    usConfirmation.lengthGetRequest = -1;

    return ret;
}

bool Metis_CheckNSet(Metis_UserSettings_t userSetting, uint8_t* valueP, uint8_t length)
{
    if (valueP == NULL)
    {
        return false;
    }

    uint8_t current_value[length];
    uint8_t current_length = length;

    if (!Metis_Get(userSetting, current_value, &current_length))
    {
        return false;
    }

    if ((length == current_length) && (0 == memcmp(valueP, current_value, length)))
    {
        /* value is already set, no need to set it again */
        return true;
    }

    /* value differs, and thus must be set */
    return Metis_Set(userSetting, valueP, length);
}

bool Metis_Set(Metis_UserSettings_t us, uint8_t* valueP, uint8_t length)
{
    if (valueP == NULL)
    {
        return false;
    }

    txPacket.Cmd = METIS_CMD_SET_REQ;
    txPacket.Length = (2 + length);
    txPacket.Data[0] = us;
    txPacket.Data[1] = length;
    memcpy(&txPacket.Data[2], valueP, length);

    FillChecksum(&txPacket);

    if (!Metis_Transparent_Transmit((uint8_t*)&txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD))
    {
        return false;
    }

    /* wait for cnf */
    return Wait4CNF(CMD_WAIT_TIME, METIS_CMD_SET_CNF, CMD_Status_Success, true);
}

bool Metis_GetFirmwareVersion(uint8_t* fwP)
{
    if (fwP == NULL)
    {
        return false;
    }

    txPacket.Cmd = METIS_CMD_GET_FWRELEASE;
    txPacket.Length = 0;

    FillChecksum(&txPacket);

    if (!Metis_Transparent_Transmit((uint8_t*)&txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD))
    {
        return false;
    }

    /* wait for cnf */
    if (!Wait4CNF(CMD_WAIT_TIME, METIS_CMD_GET_FWRELEASE_CNF, CMD_Status_Success, true))
    {
        return false;
    }

    memcpy(fwP, &rxPacket.Data[0], rxPacket.Length);
    return true;
}

bool Metis_GetSerialNumber(uint8_t* snP)
{
    if (snP == NULL)
    {
        return false;
    }

    txPacket.Cmd = METIS_CMD_GET_SERIALNO;
    txPacket.Length = 0;

    FillChecksum(&txPacket);

    if (!Metis_Transparent_Transmit((uint8_t*)&txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD))
    {
        return false;
    }

    /* wait for cnf */
    if (!Wait4CNF(CMD_WAIT_TIME, METIS_CMD_GET_SERIALNO_CNF, CMD_Status_Success, true))
    {
        return false;
    }

    memcpy(snP, &rxPacket.Data[0], rxPacket.Length);
    return true;
}

bool Metis_GetDefaultTXPower(int8_t* txpowerP)
{
    if (txpowerP == NULL)
    {
        return false;
    }

    *txpowerP = TXPOWERINVALID;
    uint8_t length;

    return Metis_Get(Metis_USERSETTING_MEMPOSITION_DEFAULTRFTXPOWER, (uint8_t*)txpowerP, &length);
}

bool Metis_GetUartOutEnable(uint8_t* uartEnableP)
{
    uint8_t length;
    return Metis_Get(Metis_USERSETTING_MEMPOSITION_UART_CMD_OUT_ENABLE, uartEnableP, &length);
}

bool Metis_GetRSSIEnable(uint8_t* rssiEnableP)
{
    uint8_t length;
    return Metis_Get(Metis_USERSETTING_MEMPOSITION_RSSI_ENABLE, rssiEnableP, &length);
}

bool Metis_GetModePreselect(uint8_t* modePreselectP)
{
    uint8_t length;
    return Metis_Get(Metis_USERSETTING_MEMPOSITION_MODE_PRESELECT, modePreselectP, &length);
}

bool Metis_GetAESEnable(uint8_t* aesEnableP)
{
    uint8_t length;
    return Metis_Get(Metis_USERSETTING_MEMPOSITION_APP_AES_ENABLE, aesEnableP, &length);
}

bool Metis_SetDefaultTXPower(int8_t txpower)
{
    /* check for invalid power */
    if ((txpower < -11) || (txpower > 15))
    {
        /* invalid power */
        return false;
    }
    return Metis_Set(Metis_USERSETTING_MEMPOSITION_DEFAULTRFTXPOWER, (uint8_t*)&txpower, 1);
}

bool Metis_SetUartOutEnable(uint8_t uartEnable)
{
    if ((uartEnable != 0) && (uartEnable != 1))
    {
        return false;
    }
    return Metis_Set(Metis_USERSETTING_MEMPOSITION_UART_CMD_OUT_ENABLE, &uartEnable, 1);
}
bool Metis_SetRSSIEnable(uint8_t rssiEnable)
{
    if ((rssiEnable != 0) && (rssiEnable != 1))
    {
        return false;
    }
    return Metis_Set(Metis_USERSETTING_MEMPOSITION_RSSI_ENABLE, &rssiEnable, 1);
}

bool Metis_SetAESEnable(uint8_t aesEnable)
{
    if (aesEnable != 0)
    {
        return false;
    }
    return Metis_Set(Metis_USERSETTING_MEMPOSITION_APP_AES_ENABLE, &aesEnable, 1);
}

bool Metis_SetModePreselect(Metis_Mode_Preselect_t modePreselect) { return Metis_Set(Metis_USERSETTING_MEMPOSITION_MODE_PRESELECT, (uint8_t*)&modePreselect, 1); }

bool Metis_SetVolatile_ModePreselect(Metis_Mode_Preselect_t modePreselect)
{
    txPacket.Cmd = METIS_CMD_SET_MODE;
    txPacket.Length = 0x01;
    txPacket.Data[0] = modePreselect;

    FillChecksum(&txPacket);

    if (!Metis_Transparent_Transmit((uint8_t*)&txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD))
    {
        return false;
    }

    /* wait for cnf*/
    return Wait4CNF(CMD_WAIT_TIME, METIS_CMD_SET_CNF, CMD_Status_Success, true);
}

bool Metis_Transmit(uint8_t* payloadP)
{
    if (payloadP == NULL)
    {
        return false;
    }

    uint8_t length = *payloadP; /* first byte of wM-BUS frame is length field */

    if (length > MAX_PAYLOAD_LENGTH)
    {
        WE_DEBUG_PRINT_INFO("Data exceeds maximal payload length");
        return false;
    }

    /* mode preselect C2/T2 for frequency 868 is not suitable for sending frames */
    if ((frequency == Metis_Frequency_868))
    {
        uint8_t modePreselect;
        if (!Metis_GetModePreselect(&modePreselect))
        {
            return false;
        }
        if (modePreselect == Metis_Mode_Preselect_868_C2_T2_other)
        {
            /* module can not send in this mode. */
            WE_DEBUG_PRINT_INFO("Mode Preselect %x is not suitable for transmitting\n", modePreselect);
            return false;
        }
    }

    txPacket.Cmd = METIS_CMD_DATA_REQ;
    txPacket.Length = length;
    memcpy(&txPacket.Data[0], &payloadP[1], length);

    FillChecksum(&txPacket);

    if (!Metis_Transparent_Transmit((uint8_t*)&txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD))
    {
        return false;
    }

    /* wait for cnf */
    return Wait4CNF(CMD_WAIT_TIME, METIS_CMD_DATA_CNF, CMD_Status_Success, true);
}

bool Metis_Configure(Metis_Configuration_t* configP, uint8_t config_length, bool factory_reset)
{
    if ((configP == NULL) || (config_length == 0))
    {
        return false;
    }

    uint8_t help_length;
    uint8_t help[METIS_MAX_USERSETTING_LENGTH];

    if (factory_reset)
    {
        /* perform a factory reset */
        if (!Metis_FactoryReset())
        {
            /* error */
            return false;
        }
    }
    WE_Delay(500);

    /* now check all settings and update them if necessary */
    for (uint8_t i = 0; i < config_length; i++)
    {
        /* read current value */
        if (!Metis_Get(configP[i].usersetting, help, &help_length))
        {
            /* error */
            return false;
        }
        WE_Delay(200);

        /* check the value read out */
        if (help_length != configP[i].value_length)
        {
            /* error, length does not match */
            return false;
        }
        if (memcmp(help, configP[i].value, configP[i].value_length) != 0)
        {
            /* read value is not up to date, thus write the new value */
            if (!Metis_Set(configP[i].usersetting, configP[i].value, configP[i].value_length))
            {
                /* error */
                return false;
            }
        }
        WE_Delay(200);
    }

    /* reset to take effect of the updated parameters */
    return Metis_PinReset();
}
