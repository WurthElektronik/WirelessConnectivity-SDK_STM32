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
 * @brief Metis driver source file.
 */

#include <stdio.h>

#include "string.h"

#include "Metis.h"
#include "../global/global.h"

typedef enum Metis_Pin_t
{
    Metis_Pin_Reset,
    Metis_Pin_Count
} Metis_Pin_t;

#define CMD_WAIT_TIME 500
#define CNFINVALID 255
#define MAX_PAYLOAD_LENGTH 255
#define TXPOWERINVALID -128
#define RSSIINVALID -128

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
#define METIS_CMD_GET_SERIALNO_CNF (METIS_CMD_GET_SERIALNO| METIS_CMD_TYPE_CNF)

#define METIS_CMD_GET_FWRELEASE 0x0C
#define METIS_CMD_GET_FWRELEASE_REQ (METIS_CMD_GET_FWRELEASE | METIS_CMD_TYPE_REQ)
#define METIS_CMD_GET_FWRELEASE_CNF (METIS_CMD_GET_FWRELEASE| METIS_CMD_TYPE_CNF)

#define METIS_CMD_RSSI 0x0D
#define METIS_CMD_RSSI_REQ (METIS_CMD_RSSI | METIS_CMD_TYPE_REQ)
#define METIS_CMD_RSSI_CNF (METIS_CMD_RSSI | METIS_CMD_TYPE_CNF)

#define METIS_CMD_FACTORYRESET 0X11
#define METIS_CMD_FACTORYRESET_REQ (METIS_CMD_FACTORYRESET | METIS_CMD_TYPE_REQ)
#define METIS_CMD_FACTORYRESET_CNF (METIS_CMD_FACTORYRESET | METIS_CMD_TYPE_CNF)

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
    uint8_t Stx;
    uint8_t Cmd;
    uint8_t Length;
    uint8_t Data[MAX_PAYLOAD_LENGTH + 1]; /* +1 for the CS */
} Metis_CMD_Frame_t;

typedef struct
{
    uint8_t cmd;                        /* variable to check if correct CMD has been confirmed */
    Metis_CMD_Status_t status;          /* variable used to check the response (*_CNF), when a request (*_REQ) was sent to the module */
} Metis_CMD_Confirmation_t;

typedef struct
{
    uint8_t memoryPosition;             /* memory position of requested usersetting */
    uint8_t lengthGetRequest;           /* length of one or more requested usersetting */
} Metis_US_Confirmation_t;

/**************************************
 *          Static variables          *
 **************************************/

static WE_Pin_t Metis_pins[Metis_Pin_Count] = {0};

static Metis_CMD_Frame_t RxPacket;                      /* data buffer for RX */

#define CMDCONFIRMATIONARRAY_LENGTH 2
static Metis_CMD_Confirmation_t cmdConfirmation_array[CMDCONFIRMATIONARRAY_LENGTH];
static Metis_US_Confirmation_t usConfirmation;          /* variable used to check if GET function was successful */
static Metis_Frequency_t frequency;                     /* frequency used by module */
static bool rssi_enable = false;
static uint8_t checksum = 0;
static uint8_t RxByteCounter = 0;
static uint8_t BytesToReceive = 0;                      /* read buffer for next available byte */
static uint8_t RxBuffer[sizeof(Metis_CMD_Frame_t)];     /* data buffer for RX */
static Metis_RxCallback RxCallback;                     /* callback function */

/**************************************
 *          Static functions          *
 **************************************/

/**
 * @brief Function to calculate the rssi value from the rx level
 */
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

/**
 * @brief Interpret the valid received data packet
 */
static void HandleRxPacket(uint8_t *packetData)
{
    Metis_CMD_Confirmation_t cmdConfirmation;
    cmdConfirmation.cmd = CNFINVALID;
    cmdConfirmation.status = CMD_Status_Invalid;

    uint8_t cmd_length = packetData[2];
    memcpy((uint8_t*)&RxPacket, packetData, cmd_length + 4); /* payload + std + command + length byte + checksum */

    switch (RxPacket.Cmd)
    {
    case METIS_CMD_SET_MODE_CNF:
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

    case METIS_CMD_RESET_CNF:
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

    case METIS_CMD_DATA_CNF:
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

    case METIS_CMD_DATA_IND:
    {
        /* the call of the RxCallback strongly depends on the configuration of the module */
        if(RxCallback != NULL)
        {
            if(rssi_enable == 0x01)
            {
                /* the following implementation expects that the RSSI_Enable usersetting is enabled */
                RxPacket.Length = RxPacket.Length - 1;
                RxCallback(&RxPacket.Length, RxPacket.Length + 1, CalculateRSSIValue(RxPacket.Data[RxPacket.Length]));
            }
            else
            {
                /* the following implementation expects that the RSSI_Enable usersetting is disabled */
                RxCallback(&RxPacket.Length, RxPacket.Length + 1, (int8_t)RSSIINVALID);
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

    case METIS_CMD_SET_CNF:
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

    case METIS_CMD_GET_SERIALNO_CNF:
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

    case METIS_CMD_GET_FWRELEASE_CNF:
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

    case METIS_CMD_SETUARTSPEED_CNF:
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

    case METIS_CMD_FACTORYRESET_CNF:
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

/**
 * @brief Function that waits for the return value of Metis (*_CNF), when a command (*_REQ) was sent before.
 */
static bool Wait4CNF(int max_time_ms, uint8_t expectedCmdConfirmation, Metis_CMD_Status_t expectedStatus, bool reset_confirmstate)
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

/**
 * @brief Initialize the AMB module for serial interface.
 *
 * Caution: The baudrate parameter must match the module's baudrate to perform a successful communication.
 *          Updating this parameter during runtime may lead to communication errors.
 *          The mode parameter must match the other participant of the RF communication.
 *          Check manual of the wM-Bus AMB modules for the suitable modes.
 *
 * @param[in] baudrate       baud rate of the interface
 * @param[in] flow_control   enable/disable flow control
 * @param[in] frequency      frequency used by the AMBER module(AMB8xxx-M uses 868Mhz, AMB36xx-M uses 169MHz)
 * @param[in] mode           wM-Bus mode preselect of the AMBER module
 * @param[in] enable_rssi    enable rssi in data reception
 * @param[in] RXcb           RX callback function
 *
 * @return true if initialization succeeded,
 *         false otherwise
 */
bool Metis_Init(uint32_t baudrate,
                WE_FlowControl_t flow_control,
                Metis_Frequency_t freq,
                Metis_Mode_Preselect_t mode,
                bool enable_rssi,
                Metis_RxCallback RXcb)
{
    /* set frequency used by module */
    frequency = freq;

    /* set RX callback function */
    RxCallback = RXcb;

    /* set rssi_enable */
    rssi_enable = enable_rssi;

    Metis_pins[Metis_Pin_Reset].port = GPIOA;
    Metis_pins[Metis_Pin_Reset].pin = GPIO_PIN_10;
    Metis_pins[Metis_Pin_Reset].type = WE_Pin_Type_Output;
    if (false == WE_InitPins(Metis_pins, Metis_Pin_Count))
    {
        /* error */
        return false;
    }
    WE_SetPin(Metis_pins[Metis_Pin_Reset], WE_Pin_Level_High);
    
    WE_UART_Init(baudrate, flow_control, WE_Parity_None, false);
    WE_Delay(1000);

    /* set recommended settings as described in the manual section 5.1 */

    /* enable uartOutEnable to print out received frames
     * Setting is written to flash so write only if necessary
     */
    uint8_t uartEnable;
    bool ret = Metis_GetUartOutEnable(&uartEnable);
    if(uartEnable != 1)
    {
        WE_Delay(50);
        if(Metis_SetUartOutEnable(1))
        {
            WE_Delay(50);
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
        WE_Delay(50);
        if(Metis_SetRSSIEnable(rssi_enable ? 1 : 0))
        {
            WE_Delay(50);
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
        WE_Delay(50);
        if(Metis_SetAESEnable(0))
        {
            WE_Delay(50);
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
        WE_Delay(50);
        if(Metis_SetModePreselect(mode))
        {
            WE_Delay(50);
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
        WE_Delay(300);
    }
    else
    {
        fprintf(stdout, "Reset failed\n");
        Metis_Deinit();
        return false;
    }
    return true;
}

/**
 * @brief Deinitialize the Metis interface.
 *
 * @return true if deinitialization succeeded,
 *         false otherwise
 */
bool Metis_Deinit()
{
    /* close the communication interface to the module */
    WE_UART_DeInit();

    /* deinit pins */
    WE_DeinitPin(Metis_pins[Metis_Pin_Reset]);

    /* deinit RX callback */
    RxCallback = NULL;

    return true;
}

/**
 * @brief Reset the Metis by pin
 *
 * @return true if reset succeeded
 *         false otherwise
 */
bool Metis_PinReset()
{
    WE_SetPin(Metis_pins[Metis_Pin_Reset], WE_Pin_Level_Low);
    WE_Delay(5);
    WE_SetPin(Metis_pins[Metis_Pin_Reset], WE_Pin_Level_High);

    return true;
}

/**
 * @brief Reset the Metis by command
 *
 * @return true if reset succeeded,
 *         false otherwise
 */
bool Metis_Reset()
{
    bool ret = false;

    /* fill CMD_ARRAY packet */
    uint8_t CMD_ARRAY[4];
    CMD_ARRAY[0] = CMD_STX;
    CMD_ARRAY[1] = METIS_CMD_RESET_REQ;
    CMD_ARRAY[2] = 0x00;
    if(FillChecksum(CMD_ARRAY,sizeof(CMD_ARRAY)))
    {
        /* now send CMD_ARRAY */
        WE_UART_Transmit(CMD_ARRAY,sizeof(CMD_ARRAY));

        /* wait for cnf */
        return Wait4CNF(CMD_WAIT_TIME, METIS_CMD_RESET_CNF, CMD_Status_Success, true);
    }
    return ret;
}

/**
 * @brief Factory reset the Metis
 *
 * Note: Use only in rare cases, since flash can be updated only a limited number of times.
 *       The factory reset must be followed by a reset such that the changes become effective.
 *
 * @return true if factory reset succeeded,
 *         false otherwise
 */
bool Metis_FactoryReset()
{
    bool ret = false;

    /* fill CMD_ARRAY packet */
    uint8_t CMD_ARRAY[4];
    CMD_ARRAY[0] = CMD_STX;
    CMD_ARRAY[1] = METIS_CMD_FACTORYRESET_REQ;
    CMD_ARRAY[2] = 0x00;
    if(FillChecksum(CMD_ARRAY,sizeof(CMD_ARRAY)))
    {
        /* now send CMD_ARRAY */
        WE_UART_Transmit(CMD_ARRAY,sizeof(CMD_ARRAY));

        /* wait for cnf */
        ret = Wait4CNF(CMD_WAIT_TIME, METIS_CMD_FACTORYRESET_CNF, CMD_Status_Success, true);
    }
    return ret;
}

/**
 * @brief Sets the baud rate of the module
 *
 * Note: Use only in rare cases, since flash can be updated only a limited number of times.
 *
 * @param[in] baudrate baud rate
 *
 * @return true if setting baud rate succeeded,
 *         false otherwise
 */
bool Metis_SetUartSpeed(Metis_UartBaudrate_t baudrate)
{
    bool ret = false;

    /* fill REQ */
    uint8_t CMD_ARRAY[5];
    CMD_ARRAY[0] = CMD_STX;
    CMD_ARRAY[1] = METIS_CMD_SETUARTSPEED;
    CMD_ARRAY[2] = 0x01;
    CMD_ARRAY[3] = baudrate;

    if(FillChecksum(CMD_ARRAY,sizeof(CMD_ARRAY)))
    {
        /* now send CMD_ARRAY */
        WE_UART_Transmit(CMD_ARRAY,sizeof(CMD_ARRAY));

        /* wait for cnf */
        ret = Wait4CNF(CMD_WAIT_TIME, METIS_CMD_SETUARTSPEED_CNF, CMD_Status_Success, true);
    }
    return ret;
}

/**
 * @brief Request the current Metis settings.
 *
 * @param[in] us: user setting to be requested
 * @param[out] response: pointer of the memory to put the request content
 * @param[out] response_length: length of the request content
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool Metis_Get(Metis_UserSettings_t us, uint8_t* response, uint8_t* response_length)
{
    bool ret = false;

    /* fill CMD_ARRAY packet */
    uint8_t CMD_ARRAY[6];
    CMD_ARRAY[0] = CMD_STX;
    CMD_ARRAY[1] = METIS_CMD_GET_REQ;
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
        WE_UART_Transmit(CMD_ARRAY,sizeof(CMD_ARRAY));

        /* wait for cnf */
        if (Wait4CNF(CMD_WAIT_TIME, METIS_CMD_GET_CNF, CMD_Status_Success, true))
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

/**
 * @brief Request multiple of the current Metis settings
 *
 * @param[in] startAddress: first usersetting to be read
 * @param[in] lengthToRead: Length of memory to be read
 * @param[out] response: pointer of the memory to put the request content
 * @param[out] response_length: length of the request content
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool Metis_GetMultiple(uint8_t startAddress, uint8_t lengthToRead, uint8_t *response, uint8_t *response_length)
{
    bool ret = false;

    uint8_t CMD_ARRAY[6];
    CMD_ARRAY[0] = CMD_STX;
    CMD_ARRAY[1] = METIS_CMD_GET_REQ;
    CMD_ARRAY[2] = 0x02;
    CMD_ARRAY[3] = startAddress;
    CMD_ARRAY[4] = lengthToRead;

    if(FillChecksum(CMD_ARRAY,sizeof(CMD_ARRAY)))
    {
        usConfirmation.memoryPosition = startAddress;
        usConfirmation.lengthGetRequest = lengthToRead;

        /* now send CMD_ARRAY */
        WE_UART_Transmit(CMD_ARRAY,sizeof(CMD_ARRAY));

        /* wait for cnf */
        if (Wait4CNF(CMD_WAIT_TIME, METIS_CMD_GET_CNF, CMD_Status_Success, true))
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

/**
 * @brief Set a special Metis setting
 *
 * Note: Reset the module after the adaption of the setting such that it can take effect.
 * Note: Use this function only in rare case, since flash can be updated only a limited number of times.
 *
 * @param[in] us:     user setting to be updated
 * @param[in] value:  pointer to the new settings value
 * @param[in] length: length of the value
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool Metis_Set(Metis_UserSettings_t us, uint8_t* value, uint8_t length)
{
    bool ret = false;

    /* fill CMD_ARRAY packet */
    uint8_t CMD_ARRAY[length + 6];
    CMD_ARRAY[0] = CMD_STX;
    CMD_ARRAY[1] = METIS_CMD_SET_REQ;
    CMD_ARRAY[2] = (2 + length);
    CMD_ARRAY[3] = us;
    CMD_ARRAY[4] = length;
    memcpy(&CMD_ARRAY[5],value,length);
    if(FillChecksum(CMD_ARRAY,sizeof(CMD_ARRAY)))
    {
        /* now send CMD_ARRAY */
        WE_UART_Transmit(CMD_ARRAY,sizeof(CMD_ARRAY));

        /* wait for cnf */
        ret = Wait4CNF(CMD_WAIT_TIME, METIS_CMD_SET_CNF, CMD_Status_Success, true);
    }
    return ret;
}

/**
 * @brief Request the 3 byte firmware version.
 *
 * @param[out] fw: pointer to the 3 byte firmware version
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool Metis_GetFirmwareVersion(uint8_t* fw)
{
    uint8_t CMD_ARRAY[4];
    CMD_ARRAY[0] = CMD_STX;
    CMD_ARRAY[1] = METIS_CMD_GET_FWRELEASE;
    CMD_ARRAY[2] = 0;

    if(FillChecksum(CMD_ARRAY,sizeof(CMD_ARRAY)))
    {
        /* now send CMD_ARRAY */
        WE_UART_Transmit(CMD_ARRAY,sizeof(CMD_ARRAY));

        /* wait for cnf */
        if (Wait4CNF(CMD_WAIT_TIME, METIS_CMD_GET_FWRELEASE_CNF, CMD_Status_Success, true))
        {
            memcpy(fw,&RxPacket.Data[0],RxPacket.Length);
            return true;
        }
    }
    return false;
}

/**
 * @brief Request the 4 byte serial number
 *
 * @param[out] sn: pointer to the 4 byte serial number
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool Metis_GetSerialNumber(uint8_t* sn)
{
    uint8_t CMD_ARRAY[4];
    CMD_ARRAY[0] = CMD_STX;
    CMD_ARRAY[1] = METIS_CMD_GET_SERIALNO;
    CMD_ARRAY[2] = 0;

    if(FillChecksum(CMD_ARRAY,sizeof(CMD_ARRAY)))
    {
        /* now send CMD_ARRAY */
        WE_UART_Transmit(CMD_ARRAY,sizeof(CMD_ARRAY));

        /* wait for cnf */
        if (Wait4CNF(CMD_WAIT_TIME, METIS_CMD_GET_SERIALNO_CNF, CMD_Status_Success, true))
        {
            memcpy(sn,&RxPacket.Data[0], RxPacket.Length);
            return true;
        }
    }
    return false;
}

/**
 * @brief Request the default TX power.
 *
 * @param[out] txpower: pointer to the TXpower
 *
 * @return true if request succeeded,
 *         false otherwise
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

/**
 * @brief Get the Uart Out Enable byte
 *
 * @param[out] uartEnable: pointer to uartEnable
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool Metis_GetUartOutEnable(uint8_t* uartEnable)
{
    uint8_t length;
    return Metis_Get(Metis_USERSETTING_MEMPOSITION_UART_CMD_OUT_ENABLE, uartEnable, &length);
}

/**
 * @brief Get the RSSI Enable byte
 *
 * @param[out] rssiEnable: pointer to rssiEnable
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool Metis_GetRSSIEnable(uint8_t* rssiEnable)
{
    uint8_t length;
    return Metis_Get(Metis_USERSETTING_MEMPOSITION_RSSI_ENABLE, rssiEnable, &length);
}

/**
 * @brief Get the M-Bus mode preselect byte
 *
 * @param[out] modePreselect: Pointer to mode Preselect
 *
 * @return true if request succeeded,
 *         false otherwise
 */

bool Metis_GetModePreselect(uint8_t* modePreselect)
{
    uint8_t length;
    return Metis_Get(Metis_USERSETTING_MEMPOSITION_MODE_PRESELECT, modePreselect, &length);
}

/**
 * @brief Get the AES Enable byte
 *
 * @param[out] aesEnable: pointer to aes enable
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool Metis_GetAESEnable(uint8_t* aesEnable)
{
    uint8_t length;
    return Metis_Get(Metis_USERSETTING_MEMPOSITION_APP_AES_ENABLE, aesEnable, &length);
}

/**
 * @brief Set the default TX power
 *
 * Note: Reset the module after the adaption of the setting so that it can take effect.
 * Note: Use this function only in rare case, since flash can be updated only a limited number of times.
 * Note: Use Metis_SetVolatile_TXPower for frequent adaption of the TX power.
 *
 * @param[in] txpower: TXpower
 *
 * @return true if request succeeded,
 *         false otherwise
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

/**
 * @brief Set the Uart Out Enable
 *
 * Note: Reset the module after the adaption of the setting so that it can take effect.
 * Note: Use this function only in rare case, since flash can be updated only a limited number of times
 *
 * @param[in] uartEnable: uart enable
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool Metis_SetUartOutEnable(uint8_t uartEnable)
{
    if((uartEnable != 0) && (uartEnable != 1))
    {
        return false;
    }
    return Metis_Set(Metis_USERSETTING_MEMPOSITION_UART_CMD_OUT_ENABLE, &uartEnable, 1);
}
/**
 * @brief Set the RSSI Enable byte
 *
 * Note: Reset the module after the adaption of the setting so that it can take effect.
 * Note: Use this function only in rare case, since flash can be updated only a limited number of times.
 *
 * @param[in] rssiEnable: rssi Enable
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool Metis_SetRSSIEnable(uint8_t rssiEnable)
{
    if((rssiEnable !=0) && (rssiEnable != 1))
    {
        return false;
    }
    return Metis_Set(Metis_USERSETTING_MEMPOSITION_RSSI_ENABLE, &rssiEnable,1);
}

/**
 * @brief Set the AES Enable byte
 *
 * Note: Functions for AES are not implemented. Can only be used for disabling AES.
 * Note: Reset the module after the adaption of the setting so that it can take effect.
 * Note: Use this function only in rare case, since flash can be updated only a limited number of times.
 *
 * @param[in] aesEnable: AES enable
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool Metis_SetAESEnable(uint8_t aesEnable)
{
    if(aesEnable !=0)
    {
        return false;
    }
    return Metis_Set(Metis_USERSETTING_MEMPOSITION_APP_AES_ENABLE, &aesEnable,1);
}

/**
 * @brief Set the default M-Bus mode preselect
 *
 * Note: Reset the module after the adaption of the setting so that it can take effect.
 * Note: Use this function only in rare case, since flash can be updated only a limited number of times.
 * Note: Use Metis_SetVolatile_ModePreselect for frequent adaption of the channel.
 *
 * @param[in] modePreselect: M-Bus mode preselect

 * @return true if request succeeded,
 *         false otherwise
 */
bool Metis_SetModePreselect(Metis_Mode_Preselect_t modePreselect)
{
    return Metis_Set(Metis_USERSETTING_MEMPOSITION_MODE_PRESELECT, (uint8_t*)&modePreselect, 1);
}

/**
 * @brief Set the volatile mode preselect
 *
 * @param[in] modePreselect: new mode preselect value
 *
 * @return true if request succeeded,
 *         false otherwise
 */
bool Metis_SetVolatile_ModePreselect(Metis_Mode_Preselect_t modePreselect)
{
    bool ret = false;

    /* fill CMD_ARRAY packet */
    uint8_t CMD_ARRAY[5];
    CMD_ARRAY[0] = CMD_STX;
    CMD_ARRAY[1] = METIS_CMD_SET_MODE;
    CMD_ARRAY[2] = 0x01;
    CMD_ARRAY[3] = modePreselect;
    if(FillChecksum(CMD_ARRAY,sizeof(CMD_ARRAY)))
    {
        /* now send CMD_ARRAY */
        WE_UART_Transmit(CMD_ARRAY,sizeof(CMD_ARRAY));

        /* wait for cnf*/
        ret = Wait4CNF(CMD_WAIT_TIME, METIS_CMD_SET_CNF, CMD_Status_Success, true);
    }
    return ret;
}

/**
 * @brief Transmit data using the configured settings
 *
 * @param[in] payload: pointer to the data
 *
 * @return true if request succeeded,
 *         false otherwise
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
            fprintf(stdout, "Mode Preselect %x is not suitable for transmitting\n", modePreselect);
            return false;
        }
    }


    /* fill CMD_ARRAY packet */
    uint8_t CMD_ARRAY[length + 4];
    CMD_ARRAY[0] = CMD_STX;
    CMD_ARRAY[1] = METIS_CMD_DATA_REQ;
    CMD_ARRAY[2] = length;
    memcpy(&CMD_ARRAY[3],&payload[1],length);
    if(FillChecksum(CMD_ARRAY,sizeof(CMD_ARRAY)))
    {
        /* now send CMD_ARRAY */
        WE_UART_Transmit(CMD_ARRAY,sizeof(CMD_ARRAY));

        /* wait for cnf */
        ret = Wait4CNF(CMD_WAIT_TIME, METIS_CMD_DATA_CNF, CMD_Status_Success, true);
    }
    return ret;
}

/**
 * @brief Configure the Metis
 *
 * @param[in] config: pointer to the configuration struct
 * @param[in] config_length: length of the configuration struct
 * @param[in] factory_reset: apply a factory reset before or not
 *
 * @return true if request succeeded,
 *         false otherwise
*/
bool Metis_Configure(Metis_Configuration_t* config, uint8_t config_length, bool factory_reset)
{
    int i = 0;
    uint8_t help_length;
    uint8_t help[METIS_MAX_USERSETTING_LENGTH];

    if(factory_reset)
    {
        /* perform a factory reset */
        if(false == Metis_FactoryReset())
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
        if(false == Metis_Get(config[i].usersetting, help, &help_length))
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
            if(false == Metis_Set(config[i].usersetting, config[i].value, config[i].value_length))
            {
                /* error */
                return false;
            }
        }
        WE_Delay(200);
    }

    /* reset to take effect of the updated parameters */
    if(false == Metis_PinReset())
    {
        return false;
    }
    return true;
}
