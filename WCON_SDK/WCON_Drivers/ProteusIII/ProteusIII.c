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
 * @brief Proteus-III driver source file.
 */

#include <ProteusIII/ProteusIII.h>
#include <global/global.h>
#include <stdio.h>
#include <string.h>

#define CMD_WAIT_TIME 500
#define CNFINVALID 255
#define BTMAC_LENGTH 6

#define LENGTH_CMD_OVERHEAD (uint16_t)5
#define LENGTH_CMD_OVERHEAD_WITHOUT_CRC (uint16_t)(LENGTH_CMD_OVERHEAD - 1)
#define MAX_CMD_LENGTH (uint16_t)(PROTEUSIII_MAX_PAYLOAD_LENGTH + LENGTH_CMD_OVERHEAD)

typedef struct
{
    const uint8_t Stx;
    uint8_t Cmd;
    uint16_t Length;
    uint8_t Data[PROTEUSIII_MAX_CMD_PAYLOAD_LENGTH + 1]; /* +1 from CS */

} ProteusIII_CMD_Frame_t;

#define CMD_STX 0x02

#define PROTEUSIII_CMD_TYPE_REQ (uint8_t)(0 << 6)
#define PROTEUSIII_CMD_TYPE_CNF (uint8_t)(1 << 6)
#define PROTEUSIII_CMD_TYPE_IND (uint8_t)(2 << 6)
#define PROTEUSIII_CMD_TYPE_RSP (uint8_t)(3 << 6)

#define PROTEUSIII_CMD_RESET (uint8_t)0x00
#define PROTEUSIII_CMD_RESET_REQ (PROTEUSIII_CMD_RESET | PROTEUSIII_CMD_TYPE_REQ)
#define PROTEUSIII_CMD_RESET_CNF (PROTEUSIII_CMD_RESET | PROTEUSIII_CMD_TYPE_CNF)

#define PROTEUSIII_CMD_GETSTATE (uint8_t)0x01
#define PROTEUSIII_CMD_GETSTATE_REQ (PROTEUSIII_CMD_GETSTATE | PROTEUSIII_CMD_TYPE_REQ)
#define PROTEUSIII_CMD_GETSTATE_CNF (PROTEUSIII_CMD_GETSTATE | PROTEUSIII_CMD_TYPE_CNF)
#define PROTEUSIII_CMD_GETSTATE_IND (PROTEUSIII_CMD_GETSTATE | PROTEUSIII_CMD_TYPE_IND)

#define PROTEUSIII_CMD_SLEEP (uint8_t)0x02
#define PROTEUSIII_CMD_SLEEP_REQ (PROTEUSIII_CMD_SLEEP | PROTEUSIII_CMD_TYPE_REQ)
#define PROTEUSIII_CMD_SLEEP_CNF (PROTEUSIII_CMD_SLEEP | PROTEUSIII_CMD_TYPE_CNF)
#define PROTEUSIII_CMD_SLEEP_IND (PROTEUSIII_CMD_SLEEP | PROTEUSIII_CMD_TYPE_IND)

#define PROTEUSIII_CMD_UART_DISABLE (uint8_t)0x1B
#define PROTEUSIII_CMD_UART_DISABLE_REQ (PROTEUSIII_CMD_UART_DISABLE | PROTEUSIII_CMD_TYPE_REQ)
#define PROTEUSIII_CMD_UART_DISABLE_CNF (PROTEUSIII_CMD_UART_DISABLE | PROTEUSIII_CMD_TYPE_CNF)

#define PROTEUSIII_CMD_UART_ENABLE_IND (uint8_t)0x9B

#define PROTEUSIII_CMD_DATA (uint8_t)0x04
#define PROTEUSIII_CMD_DATA_REQ (PROTEUSIII_CMD_DATA | PROTEUSIII_CMD_TYPE_REQ)
#define PROTEUSIII_CMD_DATA_CNF (PROTEUSIII_CMD_DATA | PROTEUSIII_CMD_TYPE_CNF)
#define PROTEUSIII_CMD_DATA_IND (PROTEUSIII_CMD_DATA | PROTEUSIII_CMD_TYPE_IND)
#define PROTEUSIII_CMD_TXCOMPLETE_RSP (PROTEUSIII_CMD_DATA | PROTEUSIII_CMD_TYPE_RSP)

#define PROTEUSIII_CMD_CONNECT (uint8_t)0x06
#define PROTEUSIII_CMD_CONNECT_REQ (PROTEUSIII_CMD_CONNECT | PROTEUSIII_CMD_TYPE_REQ)
#define PROTEUSIII_CMD_CONNECT_CNF (PROTEUSIII_CMD_CONNECT | PROTEUSIII_CMD_TYPE_CNF)
#define PROTEUSIII_CMD_CONNECT_IND (PROTEUSIII_CMD_CONNECT | PROTEUSIII_CMD_TYPE_IND)
#define PROTEUSIII_CMD_CHANNELOPEN_RSP (PROTEUSIII_CMD_CONNECT | PROTEUSIII_CMD_TYPE_RSP)

#define PROTEUSIII_CMD_DISCONNECT (uint8_t)0x07
#define PROTEUSIII_CMD_DISCONNECT_REQ (PROTEUSIII_CMD_DISCONNECT | PROTEUSIII_CMD_TYPE_REQ)
#define PROTEUSIII_CMD_DISCONNECT_CNF (PROTEUSIII_CMD_DISCONNECT | PROTEUSIII_CMD_TYPE_CNF)
#define PROTEUSIII_CMD_DISCONNECT_IND (PROTEUSIII_CMD_DISCONNECT | PROTEUSIII_CMD_TYPE_IND)

#define PROTEUSIII_CMD_SECURITY_IND (uint8_t)0x88

#define PROTEUSIII_CMD_SCANSTART (uint8_t)0x09
#define PROTEUSIII_CMD_SCANSTART_REQ (PROTEUSIII_CMD_SCANSTART | PROTEUSIII_CMD_TYPE_REQ)
#define PROTEUSIII_CMD_SCANSTART_CNF (PROTEUSIII_CMD_SCANSTART | PROTEUSIII_CMD_TYPE_CNF)
#define PROTEUSIII_CMD_SCANSTART_IND (PROTEUSIII_CMD_SCANSTART | PROTEUSIII_CMD_TYPE_IND)

#define PROTEUSIII_CMD_SCANSTOP (uint8_t)0x0A
#define PROTEUSIII_CMD_SCANSTOP_REQ (PROTEUSIII_CMD_SCANSTOP | PROTEUSIII_CMD_TYPE_REQ)
#define PROTEUSIII_CMD_SCANSTOP_CNF (PROTEUSIII_CMD_SCANSTOP | PROTEUSIII_CMD_TYPE_CNF)
#define PROTEUSIII_CMD_SCANSTOP_IND (PROTEUSIII_CMD_SCANSTOP | PROTEUSIII_CMD_TYPE_IND)

#define PROTEUSIII_CMD_GETDEVICES (uint8_t)0x0B
#define PROTEUSIII_CMD_GETDEVICES_REQ (PROTEUSIII_CMD_GETDEVICES | PROTEUSIII_CMD_TYPE_REQ)
#define PROTEUSIII_CMD_GETDEVICES_CNF (PROTEUSIII_CMD_GETDEVICES | PROTEUSIII_CMD_TYPE_CNF)

#define PROTEUSIII_CMD_SETBEACON (uint8_t)0x0C
#define PROTEUSIII_CMD_SETBEACON_REQ (PROTEUSIII_CMD_SETBEACON | PROTEUSIII_CMD_TYPE_REQ)
#define PROTEUSIII_CMD_SETBEACON_CNF (PROTEUSIII_CMD_SETBEACON | PROTEUSIII_CMD_TYPE_CNF)
#define PROTEUSIII_CMD_BEACON_IND (PROTEUSIII_CMD_SETBEACON | PROTEUSIII_CMD_TYPE_IND)
#define PROTEUSIII_CMD_BEACON_RSP (PROTEUSIII_CMD_SETBEACON | PROTEUSIII_CMD_TYPE_RSP)
#define PROTEUSIII_CMD_RSSI_IND (uint8_t)0x8B

#define PROTEUSIII_CMD_PASSKEY (uint8_t)0x0D
#define PROTEUSIII_CMD_PASSKEY_REQ (PROTEUSIII_CMD_PASSKEY | PROTEUSIII_CMD_TYPE_REQ)
#define PROTEUSIII_CMD_PASSKEY_CNF (PROTEUSIII_CMD_PASSKEY | PROTEUSIII_CMD_TYPE_CNF)
#define PROTEUSIII_CMD_PASSKEY_IND (PROTEUSIII_CMD_PASSKEY | PROTEUSIII_CMD_TYPE_IND)

#define PROTEUSIII_CMD_GET (uint8_t)0x10
#define PROTEUSIII_CMD_GET_REQ (PROTEUSIII_CMD_GET | PROTEUSIII_CMD_TYPE_REQ)
#define PROTEUSIII_CMD_GET_CNF (PROTEUSIII_CMD_GET | PROTEUSIII_CMD_TYPE_CNF)

#define PROTEUSIII_CMD_SET (uint8_t)0x11
#define PROTEUSIII_CMD_SET_REQ (PROTEUSIII_CMD_SET | PROTEUSIII_CMD_TYPE_REQ)
#define PROTEUSIII_CMD_SET_CNF (PROTEUSIII_CMD_SET | PROTEUSIII_CMD_TYPE_CNF)

#define PROTEUSIII_CMD_PHYUPDATE (uint8_t)0x1A
#define PROTEUSIII_CMD_PHYUPDATE_REQ (PROTEUSIII_CMD_PHYUPDATE | PROTEUSIII_CMD_TYPE_REQ)
#define PROTEUSIII_CMD_PHYUPDATE_CNF (PROTEUSIII_CMD_PHYUPDATE | PROTEUSIII_CMD_TYPE_CNF)
#define PROTEUSIII_CMD_PHYUPDATE_IND (PROTEUSIII_CMD_PHYUPDATE | PROTEUSIII_CMD_TYPE_IND)

#define PROTEUSIII_CMD_FACTORYRESET (uint8_t)0x1C
#define PROTEUSIII_CMD_FACTORYRESET_REQ (PROTEUSIII_CMD_FACTORYRESET | PROTEUSIII_CMD_TYPE_REQ)
#define PROTEUSIII_CMD_FACTORYRESET_CNF (PROTEUSIII_CMD_FACTORYRESET | PROTEUSIII_CMD_TYPE_CNF)

#define PROTEUSIII_CMD_DTMSTART (uint8_t)0x1D
#define PROTEUSIII_CMD_DTMSTART_REQ (PROTEUSIII_CMD_DTMSTART | PROTEUSIII_CMD_TYPE_REQ)
#define PROTEUSIII_CMD_DTMSTART_CNF (PROTEUSIII_CMD_DTMSTART | PROTEUSIII_CMD_TYPE_CNF)

#define PROTEUSIII_CMD_DTM (uint8_t)0x1E
#define PROTEUSIII_CMD_DTM_REQ (PROTEUSIII_CMD_DTM | PROTEUSIII_CMD_TYPE_REQ)
#define PROTEUSIII_CMD_DTM_CNF (PROTEUSIII_CMD_DTM | PROTEUSIII_CMD_TYPE_CNF)

#define PROTEUSIII_CMD_NUMERIC_COMP (uint8_t)0x24
#define PROTEUSIII_CMD_NUMERIC_COMP_REQ (PROTEUSIII_CMD_NUMERIC_COMP | PROTEUSIII_CMD_TYPE_REQ)
#define PROTEUSIII_CMD_NUMERIC_COMP_CNF (PROTEUSIII_CMD_NUMERIC_COMP | PROTEUSIII_CMD_TYPE_CNF)
#define PROTEUSIII_CMD_DISPLAY_PASSKEY_IND (PROTEUSIII_CMD_NUMERIC_COMP | PROTEUSIII_CMD_TYPE_IND)

#define PROTEUSIII_CMD_GPIO_LOCAL_WRITECONFIG (uint8_t)0x25
#define PROTEUSIII_CMD_GPIO_LOCAL_WRITECONFIG_REQ (PROTEUSIII_CMD_GPIO_LOCAL_WRITECONFIG | PROTEUSIII_CMD_TYPE_REQ)
#define PROTEUSIII_CMD_GPIO_LOCAL_WRITECONFIG_CNF (PROTEUSIII_CMD_GPIO_LOCAL_WRITECONFIG | PROTEUSIII_CMD_TYPE_CNF)

#define PROTEUSIII_CMD_GPIO_LOCAL_READCONFIG (uint8_t)0x2B
#define PROTEUSIII_CMD_GPIO_LOCAL_READCONFIG_REQ (PROTEUSIII_CMD_GPIO_LOCAL_READCONFIG | PROTEUSIII_CMD_TYPE_REQ)
#define PROTEUSIII_CMD_GPIO_LOCAL_READCONFIG_CNF (PROTEUSIII_CMD_GPIO_LOCAL_READCONFIG | PROTEUSIII_CMD_TYPE_CNF)

#define PROTEUSIII_CMD_GPIO_LOCAL_WRITE (uint8_t)0x26
#define PROTEUSIII_CMD_GPIO_LOCAL_WRITE_REQ (PROTEUSIII_CMD_GPIO_LOCAL_WRITE | PROTEUSIII_CMD_TYPE_REQ)
#define PROTEUSIII_CMD_GPIO_LOCAL_WRITE_CNF (PROTEUSIII_CMD_GPIO_LOCAL_WRITE | PROTEUSIII_CMD_TYPE_CNF)
#define PROTEUSIII_CMD_GPIO_LOCAL_WRITE_IND (PROTEUSIII_CMD_GPIO_LOCAL_WRITE | PROTEUSIII_CMD_TYPE_IND)

#define PROTEUSIII_CMD_GPIO_LOCAL_READ (uint8_t)0x27
#define PROTEUSIII_CMD_GPIO_LOCAL_READ_REQ (PROTEUSIII_CMD_GPIO_LOCAL_READ | PROTEUSIII_CMD_TYPE_REQ)
#define PROTEUSIII_CMD_GPIO_LOCAL_READ_CNF (PROTEUSIII_CMD_GPIO_LOCAL_READ | PROTEUSIII_CMD_TYPE_CNF)

#define PROTEUSIII_CMD_GPIO_REMOTE_WRITECONFIG (uint8_t)0x28
#define PROTEUSIII_CMD_GPIO_REMOTE_WRITECONFIG_REQ (PROTEUSIII_CMD_GPIO_REMOTE_WRITECONFIG | PROTEUSIII_CMD_TYPE_REQ)
#define PROTEUSIII_CMD_GPIO_REMOTE_WRITECONFIG_CNF (PROTEUSIII_CMD_GPIO_REMOTE_WRITECONFIG | PROTEUSIII_CMD_TYPE_CNF)
#define PROTEUSIII_CMD_GPIO_REMOTE_WRITECONFIG_IND (PROTEUSIII_CMD_GPIO_REMOTE_WRITECONFIG | PROTEUSIII_CMD_TYPE_IND)

#define PROTEUSIII_CMD_GPIO_REMOTE_READCONFIG (uint8_t)0x2C
#define PROTEUSIII_CMD_GPIO_REMOTE_READCONFIG_REQ (PROTEUSIII_CMD_GPIO_REMOTE_READCONFIG | PROTEUSIII_CMD_TYPE_REQ)
#define PROTEUSIII_CMD_GPIO_REMOTE_READCONFIG_CNF (PROTEUSIII_CMD_GPIO_REMOTE_READCONFIG | PROTEUSIII_CMD_TYPE_CNF)

#define PROTEUSIII_CMD_GPIO_REMOTE_WRITE (uint8_t)0x29
#define PROTEUSIII_CMD_GPIO_REMOTE_WRITE_REQ (PROTEUSIII_CMD_GPIO_REMOTE_WRITE | PROTEUSIII_CMD_TYPE_REQ)
#define PROTEUSIII_CMD_GPIO_REMOTE_WRITE_CNF (PROTEUSIII_CMD_GPIO_REMOTE_WRITE | PROTEUSIII_CMD_TYPE_CNF)
#define PROTEUSIII_CMD_GPIO_REMOTE_WRITE_IND (PROTEUSIII_CMD_GPIO_REMOTE_WRITE | PROTEUSIII_CMD_TYPE_IND)

#define PROTEUSIII_CMD_GPIO_REMOTE_READ (uint8_t)0x2A
#define PROTEUSIII_CMD_GPIO_REMOTE_READ_REQ (PROTEUSIII_CMD_GPIO_REMOTE_READ | PROTEUSIII_CMD_TYPE_REQ)
#define PROTEUSIII_CMD_GPIO_REMOTE_READ_CNF (PROTEUSIII_CMD_GPIO_REMOTE_READ | PROTEUSIII_CMD_TYPE_CNF)

#define PROTEUSIII_CMD_GET_BONDS (uint8_t)0x0F
#define PROTEUSIII_CMD_GET_BONDS_REQ (PROTEUSIII_CMD_GET_BONDS | PROTEUSIII_CMD_TYPE_REQ)
#define PROTEUSIII_CMD_GET_BONDS_CNF (PROTEUSIII_CMD_GET_BONDS | PROTEUSIII_CMD_TYPE_CNF)

#define PROTEUSIII_CMD_DELETE_BONDS (uint8_t)0x0E
#define PROTEUSIII_CMD_DELETE_BONDS_REQ (PROTEUSIII_CMD_DELETE_BONDS | PROTEUSIII_CMD_TYPE_REQ)
#define PROTEUSIII_CMD_DELETE_BONDS_CNF (PROTEUSIII_CMD_DELETE_BONDS | PROTEUSIII_CMD_TYPE_CNF)

#define PROTEUSIII_CMD_ALLOWUNBONDEDCONNECTIONS (uint8_t)0x2D
#define PROTEUSIII_CMD_ALLOWUNBONDEDCONNECTIONS_REQ (PROTEUSIII_CMD_ALLOWUNBONDEDCONNECTIONS | PROTEUSIII_CMD_TYPE_REQ)
#define PROTEUSIII_CMD_ALLOWUNBONDEDCONNECTIONS_CNF (PROTEUSIII_CMD_ALLOWUNBONDEDCONNECTIONS | PROTEUSIII_CMD_TYPE_CNF)

#define PROTEUSIII_CMD_ERROR_IND (uint8_t)0xA2

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
    uint8_t cmd;                    /**< Variable to check if correct CMD has been confirmed */
    ProteusIII_CMD_Status_t status; /**< Variable used to check the response (*_CNF), when a request (*_REQ) was sent to the ProteusIII */
} ProteusIII_CMD_Confirmation_t;

/**************************************
 *          Static variables          *
 **************************************/
static ProteusIII_CMD_Frame_t txPacket = {.Stx = CMD_STX, .Length = 0}; /* request to be sent to the module */
static ProteusIII_CMD_Frame_t rxPacket = {.Stx = CMD_STX, .Length = 0}; /* received packet that has been sent by the module */

#define CMDCONFIRMATIONARRAY_LENGTH 2
static ProteusIII_CMD_Confirmation_t cmdConfirmationArray[CMDCONFIRMATIONARRAY_LENGTH];
static ProteusIII_OperationMode_t operationMode = ProteusIII_OperationMode_CommandMode;
static ProteusIII_GetDevices_t* ProteusIII_getDevicesP = NULL;
static ProteusIII_DriverState_t bleState;
/**
 * @brief Pin configuration struct pointer.
 */
static ProteusIII_Pins_t* ProteusIII_pinsP = NULL;

/**
 * @brief Uart configuration struct pointer.
 */
static WE_UART_t* ProteusIII_uartP = NULL;

static ProteusIII_CallbackConfig_t callbacks;
static WE_UART_HandleRxByte_t byteRxCallback = NULL;
static uint8_t checksum = 0;
static uint16_t rxByteCounter = 0;
static uint16_t bytesToReceive = 0;
static uint8_t rxBuffer[sizeof(ProteusIII_CMD_Frame_t)]; /* For UART RX from module */

/**************************************
 *         Static functions           *
 **************************************/

static void ClearReceiveBuffers()
{
    bytesToReceive = 0;
    rxByteCounter = 0;
    checksum = 0;
    for (uint8_t i = 0; i < CMDCONFIRMATIONARRAY_LENGTH; i++)
    {
        cmdConfirmationArray[i].cmd = CNFINVALID;
    }
}

static void HandleRxPacket(uint8_t* prxBuffer)
{
    ProteusIII_CMD_Confirmation_t cmdConfirmation;
    cmdConfirmation.cmd = CNFINVALID;
    cmdConfirmation.status = CMD_Status_Invalid;

    uint16_t cmdLength = ((ProteusIII_CMD_Frame_t*)prxBuffer)->Length;
    memcpy(&rxPacket, prxBuffer, cmdLength + LENGTH_CMD_OVERHEAD);

    switch (rxPacket.Cmd)
    {
        case PROTEUSIII_CMD_GETDEVICES_CNF:
        {
            cmdConfirmation.cmd = rxPacket.Cmd;
            cmdConfirmation.status = rxPacket.Data[0];
            if ((cmdConfirmation.status == CMD_Status_Success) && (ProteusIII_getDevicesP != NULL))
            {
                uint8_t size = rxPacket.Data[1];
                if (size >= PROTEUSIII_MAX_NUMBER_OF_DEVICES)
                {
                    size = PROTEUSIII_MAX_NUMBER_OF_DEVICES;
                }
                ProteusIII_getDevicesP->numberOfDevices = size;

                uint16_t len = 2;
                for (uint8_t i = 0; i < ProteusIII_getDevicesP->numberOfDevices; i++)
                {
                    memcpy(&ProteusIII_getDevicesP->devices[i].btmac[0], &rxPacket.Data[len], 6);
                    ProteusIII_getDevicesP->devices[i].rssi = rxPacket.Data[len + 6];
                    ProteusIII_getDevicesP->devices[i].txPower = rxPacket.Data[len + 7];
                    ProteusIII_getDevicesP->devices[i].deviceNameLength = rxPacket.Data[len + 8];
                    memcpy(&ProteusIII_getDevicesP->devices[i].deviceName[0], &rxPacket.Data[len + 9], ProteusIII_getDevicesP->devices[i].deviceNameLength);
                    len += (9 + ProteusIII_getDevicesP->devices[i].deviceNameLength);
                }
            }
            break;
        }
        case PROTEUSIII_CMD_RESET_CNF:
        case PROTEUSIII_CMD_SCANSTART_CNF:
        case PROTEUSIII_CMD_SCANSTOP_CNF:
        case PROTEUSIII_CMD_GET_CNF:
        case PROTEUSIII_CMD_SET_CNF:
        case PROTEUSIII_CMD_SETBEACON_CNF:
        case PROTEUSIII_CMD_PASSKEY_CNF:
        case PROTEUSIII_CMD_PHYUPDATE_CNF:
        case PROTEUSIII_CMD_CONNECT_CNF:
        case PROTEUSIII_CMD_DATA_CNF:
        case PROTEUSIII_CMD_DISCONNECT_CNF:
        case PROTEUSIII_CMD_FACTORYRESET_CNF:
        case PROTEUSIII_CMD_SLEEP_CNF:
        case PROTEUSIII_CMD_UART_DISABLE_CNF:
        case PROTEUSIII_CMD_UART_ENABLE_IND:
        case PROTEUSIII_CMD_GPIO_LOCAL_WRITECONFIG_CNF:
        case PROTEUSIII_CMD_GPIO_LOCAL_READCONFIG_CNF:
        case PROTEUSIII_CMD_GPIO_LOCAL_WRITE_CNF:
        case PROTEUSIII_CMD_GPIO_LOCAL_READ_CNF:
        case PROTEUSIII_CMD_GPIO_REMOTE_WRITECONFIG_CNF:
        case PROTEUSIII_CMD_GPIO_REMOTE_READCONFIG_CNF:
        case PROTEUSIII_CMD_GPIO_REMOTE_WRITE_CNF:
        case PROTEUSIII_CMD_GPIO_REMOTE_READ_CNF:
        case PROTEUSIII_CMD_GET_BONDS_CNF:
        case PROTEUSIII_CMD_DELETE_BONDS_CNF:
        case PROTEUSIII_CMD_ALLOWUNBONDEDCONNECTIONS_CNF:
        case PROTEUSIII_CMD_TXCOMPLETE_RSP:
        case PROTEUSIII_CMD_NUMERIC_COMP_CNF:
        case PROTEUSIII_CMD_DTMSTART_CNF:
        case PROTEUSIII_CMD_DTM_CNF:
        {
            cmdConfirmation.cmd = rxPacket.Cmd;
            cmdConfirmation.status = rxPacket.Data[0];
            break;
        }

        case PROTEUSIII_CMD_GETSTATE_CNF:
        {
            cmdConfirmation.cmd = rxPacket.Cmd;
            /* GETSTATE_CNF has no status field*/
            cmdConfirmation.status = CMD_Status_NoStatus;

            switch (rxPacket.Data[1])
            {
                case ProteusIII_BLE_Action_Idle:
                {
                    bleState = ProteusIII_DriverState_BLE_Idle;
                }
                break;
                case ProteusIII_BLE_Action_Connected:
                case ProteusIII_BLE_Action_None:
                case ProteusIII_BLE_Action_Scanning:
                case ProteusIII_BLE_Action_Sleep:
                case ProteusIII_BLE_Action_DTM:
                default:
                {
                    /* do not use this information */
                }
                break;
            }

            break;
        }

        case PROTEUSIII_CMD_CHANNELOPEN_RSP:
        {
            /* Payload of CHANNELOPEN_RSP: Status (1 byte), BTMAC (6 byte), Max Payload (1byte)*/
            bleState = ProteusIII_DriverState_BLE_ChannelOpen;
            if (callbacks.channelOpenCb != NULL)
            {
                callbacks.channelOpenCb(&rxPacket.Data[1], (uint16_t)rxPacket.Data[7]);
            }
            break;
        }

        case PROTEUSIII_CMD_CONNECT_IND:
        {
            bool success = (rxPacket.Data[0] == CMD_Status_Success);
            if (success)
            {
                bleState = ProteusIII_DriverState_BLE_Connected;
            }
            if (callbacks.connectCb != NULL)
            {
                uint8_t btMac[BTMAC_LENGTH];
                if (rxPacket.Length >= 7)
                {
                    memcpy(btMac, &rxPacket.Data[1], sizeof(btMac));
                }
                else
                {
                    /* Packet doesn't contain BTMAC (e.g. connection failed) */
                    memset(btMac, 0, sizeof(btMac));
                }
                callbacks.connectCb(success, btMac);
            }
            break;
        }

        case PROTEUSIII_CMD_DISCONNECT_IND:
        {
            bleState = ProteusIII_DriverState_BLE_Idle;
            if (callbacks.disconnectCb != NULL)
            {
                ProteusIII_DisconnectReason_t reason = ProteusIII_DisconnectReason_Unknown;
                switch (rxPacket.Data[0])
                {
                    case 0x08:
                        reason = ProteusIII_DisconnectReason_ConnectionTimeout;
                        break;

                    case 0x13:
                        reason = ProteusIII_DisconnectReason_UserTerminatedConnection;
                        break;

                    case 0x16:
                        reason = ProteusIII_DisconnectReason_HostTerminatedConnection;
                        break;

                    case 0x3B:
                        reason = ProteusIII_DisconnectReason_ConnectionIntervalUnacceptable;
                        break;

                    case 0x3D:
                        reason = ProteusIII_DisconnectReason_MicFailure;
                        break;

                    case 0x3E:
                        reason = ProteusIII_DisconnectReason_ConnectionSetupFailed;
                        break;
                }
                callbacks.disconnectCb(reason);
            }
            break;
        }

        case PROTEUSIII_CMD_DATA_IND:
        {
            if (callbacks.rxCb != NULL)
            {
                callbacks.rxCb(&rxPacket.Data[7], rxPacket.Length - 7, &rxPacket.Data[0], rxPacket.Data[6]);
            }
            break;
        }

        case PROTEUSIII_CMD_BEACON_IND:
        case PROTEUSIII_CMD_BEACON_RSP:
        {
            if (callbacks.beaconRxCb != NULL)
            {
                callbacks.beaconRxCb(&rxPacket.Data[7], rxPacket.Length - 7, &rxPacket.Data[0], rxPacket.Data[6]);
            }
            break;
        }

        case PROTEUSIII_CMD_RSSI_IND:
        {
            if (callbacks.rssiCb != NULL)
            {
                if (rxPacket.Length >= 8)
                {
                    callbacks.rssiCb(&rxPacket.Data[0], rxPacket.Data[6], rxPacket.Data[7]);
                }
            }
            break;
        }

        case PROTEUSIII_CMD_SECURITY_IND:
        {
            if (callbacks.securityCb != NULL)
            {
                callbacks.securityCb(&rxPacket.Data[1], rxPacket.Data[0]);
            }
            break;
        }

        case PROTEUSIII_CMD_PASSKEY_IND:
        {
            if (callbacks.passkeyCb != NULL)
            {
                callbacks.passkeyCb(&rxPacket.Data[1]);
            }
            break;
        }

        case PROTEUSIII_CMD_DISPLAY_PASSKEY_IND:
        {
            if (callbacks.displayPasskeyCb != NULL)
            {
                callbacks.displayPasskeyCb((ProteusIII_DisplayPasskeyAction_t)rxPacket.Data[0], &rxPacket.Data[1], &rxPacket.Data[7]);
            }
            break;
        }

        case PROTEUSIII_CMD_PHYUPDATE_IND:
        {
            if (callbacks.phyUpdateCb != NULL)
            {
                uint8_t btMac[BTMAC_LENGTH];
                if (rxPacket.Length >= 9)
                {
                    memcpy(btMac, &rxPacket.Data[3], sizeof(btMac));
                }
                else
                {
                    /* Packet doesn't contain BTMAC (e.g. Phy update failed) */
                    memset(btMac, 0, sizeof(btMac));
                }
                callbacks.phyUpdateCb((rxPacket.Data[0] == CMD_Status_Success), btMac, (ProteusIII_Phy_t)rxPacket.Data[1], (ProteusIII_Phy_t)rxPacket.Data[2]);
            }
            break;
        }

        case PROTEUSIII_CMD_SLEEP_IND:
        {
            if (callbacks.sleepCb != NULL)
            {
                callbacks.sleepCb();
            }
            break;
        }

        case PROTEUSIII_CMD_GPIO_LOCAL_WRITE_IND:
        case PROTEUSIII_CMD_GPIO_REMOTE_WRITE_IND:
        {
            if (callbacks.gpioWriteCb != NULL)
            {
                uint8_t pos = 0;
                while (pos < rxPacket.Length)
                {
                    uint8_t blockLength = rxPacket.Data[pos] + 1;

                    /* Note that the gpioId parameter is of type uint8_t instead of ProteusIII_GPIO_t, as the
                 * remote device may support other GPIOs than this device. */
                    uint8_t gpioId = rxPacket.Data[1 + pos];
                    uint8_t value = rxPacket.Data[2 + pos];
                    callbacks.gpioWriteCb(PROTEUSIII_CMD_GPIO_REMOTE_WRITE_IND == rxPacket.Cmd, gpioId, value);

                    pos += blockLength;
                }
            }
            break;
        }

        case PROTEUSIII_CMD_GPIO_REMOTE_WRITECONFIG_IND:
        {
            if (callbacks.gpioRemoteConfigCb != NULL)
            {
                uint8_t pos = 0;
                while (pos < rxPacket.Length)
                {
                    uint8_t blockLength = rxPacket.Data[pos] + 1;
                    uint8_t gpioId = rxPacket.Data[1 + pos];
                    uint8_t function = rxPacket.Data[2 + pos];
                    uint8_t* value = &rxPacket.Data[3 + pos];

                    ProteusIII_GPIOConfigBlock_t gpioConfig = {0};
                    gpioConfig.gpioId = (ProteusIII_GPIO_t)gpioId;
                    gpioConfig.function = (ProteusIII_GPIO_IO_t)function;
                    switch (gpioConfig.function)
                    {
                        case ProteusIII_GPIO_IO_Disconnected:
                            break;

                        case ProteusIII_GPIO_IO_Input:
                            gpioConfig.value.input = (ProteusIII_GPIO_Input_t)*value;
                            break;

                        case ProteusIII_GPIO_IO_Output:
                            gpioConfig.value.output = (ProteusIII_GPIO_Output_t)*value;
                            break;

                        case ProteusIII_GPIO_IO_PWM:
                            gpioConfig.value.pwm.period = (((uint16_t)value[1]) << 8) | ((uint16_t)value[0]);
                            gpioConfig.value.pwm.ratio = value[2];
                            break;
                    }
                    callbacks.gpioRemoteConfigCb(&gpioConfig);
                    pos += blockLength;
                }
            }
            break;
        }

        case PROTEUSIII_CMD_ERROR_IND:
        {
            if (callbacks.errorCb != NULL)
            {
                callbacks.errorCb(rxPacket.Data[0]);
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
        if (cmdConfirmationArray[i].cmd == CNFINVALID)
        {
            cmdConfirmationArray[i].cmd = cmdConfirmation.cmd;
            cmdConfirmationArray[i].status = cmdConfirmation.status;
            break;
        }
    }
}

static void ProteusIII_HandleRxByte(uint8_t* dataP, size_t size)
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
                /* length field lsb */
                rxByteCounter++;
                bytesToReceive = (uint16_t)(rxBuffer[rxByteCounter - 1]);
                break;

            case 3:
                /* length field msb */
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

/**
 * @brief Function that waits for the return value of ProteusIII (*_CNF),
 * when a command (*_REQ) was sent before.
 */
static bool Wait4CNF(uint32_t maxTimeMs, uint8_t expectedCmdConfirmation, ProteusIII_CMD_Status_t expectedStatus, bool resetConfirmState)
{
    int count = 0;
    int timeStepMs = 5; /* 5ms */
    int maxCount = maxTimeMs / timeStepMs;

    if (resetConfirmState)
    {
        for (uint8_t i = 0; i < CMDCONFIRMATIONARRAY_LENGTH; i++)
        {
            cmdConfirmationArray[i].cmd = CNFINVALID;
        }
    }
    while (1)
    {
        for (uint8_t i = 0; i < CMDCONFIRMATIONARRAY_LENGTH; i++)
        {
            if (expectedCmdConfirmation == cmdConfirmationArray[i].cmd)
            {
                return (cmdConfirmationArray[i].status == expectedStatus);
            }
        }

        if (count >= maxCount)
        {
            /* received no correct response within timeout */
            return false;
        }

        /* wait */
        count++;
        WE_Delay(timeStepMs);
    }
    return true;
}

/**
 * @brief Function to add the checksum at the end of the data packet.
 */
static bool FillChecksum(ProteusIII_CMD_Frame_t* cmd)
{
    uint8_t checksum = (uint8_t)cmd->Stx;
    uint8_t* pArray = (uint8_t*)cmd;
    for (uint16_t i = 1; i < (cmd->Length + LENGTH_CMD_OVERHEAD_WITHOUT_CRC); i++)
    {
        checksum ^= pArray[i];
    }
    cmd->Data[cmd->Length] = checksum;
    return true;
}

/**************************************
 *         Global functions           *
 **************************************/
/**
 * @brief Transmitting the data via UART.
 *
 * @param[in] data: Pointer to the data.
 * @param[in] dataLength: Length of the data.
 *
 * @return true if transmission succeeded,
 *         false otherwise
 */
bool ProteusIII_Transparent_Transmit(const uint8_t* data, uint16_t dataLength)
{
    if ((data == NULL) || (dataLength == 0))
    {
        return false;
    }
    return ProteusIII_uartP->uartTransmit((uint8_t*)data, dataLength);
}

bool ProteusIII_Init(WE_UART_t* uartP, ProteusIII_Pins_t* pinoutP, ProteusIII_OperationMode_t opMode, ProteusIII_CallbackConfig_t callbackConfig)
{
    operationMode = opMode;

    if ((pinoutP == NULL) || (uartP == NULL) || (uartP->uartInit == NULL) || (uartP->uartDeinit == NULL) || (uartP->uartTransmit == NULL))
    {
        return false;
    }

    ProteusIII_pinsP = pinoutP;
    ProteusIII_pinsP->ProteusIII_Pin_Reset.type = WE_Pin_Type_Output;
    ProteusIII_pinsP->ProteusIII_Pin_Reset.initial_value.output = WE_Pin_Level_High;
    ProteusIII_pinsP->ProteusIII_Pin_SleepWakeUp.type = WE_Pin_Type_Output;
    ProteusIII_pinsP->ProteusIII_Pin_SleepWakeUp.initial_value.output = WE_Pin_Level_High;
    ProteusIII_pinsP->ProteusIII_Pin_Boot.type = WE_Pin_Type_Output;
    ProteusIII_pinsP->ProteusIII_Pin_Boot.initial_value.output = WE_Pin_Level_High;
    ProteusIII_pinsP->ProteusIII_Pin_Mode.type = WE_Pin_Type_Output;
    ProteusIII_pinsP->ProteusIII_Pin_Mode.initial_value.output = ((operationMode == ProteusIII_OperationMode_PeripheralOnlyMode) ? WE_Pin_Level_High : WE_Pin_Level_Low);
    ProteusIII_pinsP->ProteusIII_Pin_Busy.type = WE_Pin_Type_Input;
    ProteusIII_pinsP->ProteusIII_Pin_Busy.initial_value.input_pull = WE_Pin_PullType_No;
    ProteusIII_pinsP->ProteusIII_Pin_StatusLed2.type = WE_Pin_Type_Input;
    ProteusIII_pinsP->ProteusIII_Pin_StatusLed2.initial_value.input_pull = WE_Pin_PullType_No;

    WE_Pin_t pins[sizeof(ProteusIII_Pins_t) / sizeof(WE_Pin_t)];
    uint8_t pin_count = 0;
    memcpy(&pins[pin_count++], &ProteusIII_pinsP->ProteusIII_Pin_Reset, sizeof(WE_Pin_t));
    memcpy(&pins[pin_count++], &ProteusIII_pinsP->ProteusIII_Pin_SleepWakeUp, sizeof(WE_Pin_t));
    memcpy(&pins[pin_count++], &ProteusIII_pinsP->ProteusIII_Pin_Boot, sizeof(WE_Pin_t));
    memcpy(&pins[pin_count++], &ProteusIII_pinsP->ProteusIII_Pin_Mode, sizeof(WE_Pin_t));
    memcpy(&pins[pin_count++], &ProteusIII_pinsP->ProteusIII_Pin_Busy, sizeof(WE_Pin_t));
    memcpy(&pins[pin_count++], &ProteusIII_pinsP->ProteusIII_Pin_StatusLed2, sizeof(WE_Pin_t));

    if (!WE_InitPins(pins, pin_count))
    {
        /* error */
        return false;
    }

    /* set callback functions */
    callbacks = callbackConfig;
    byteRxCallback = ProteusIII_HandleRxByte;

    ProteusIII_uartP = uartP;
    if (false == ProteusIII_uartP->uartInit(ProteusIII_uartP->baudrate, ProteusIII_uartP->flowControl, ProteusIII_uartP->parity, &byteRxCallback))
    {
        return false;
    }
    WE_Delay(10);

    /* reset module */
    if (!(IS_WE_PIN_UNDEFINED(ProteusIII_pinsP->ProteusIII_Pin_Reset) ? ProteusIII_Reset() : ProteusIII_PinReset()))
    {
        WE_DEBUG_PRINT_INFO("Reset failed\r\n");
        ProteusIII_Deinit();
        return false;
    }

    bleState = ProteusIII_DriverState_BLE_Idle;
    ProteusIII_getDevicesP = NULL;

    return true;
}

bool ProteusIII_Deinit()
{
    WE_Pin_t pins[sizeof(ProteusIII_Pins_t) / sizeof(WE_Pin_t)];
    uint8_t pin_count = 0;
    memcpy(&pins[pin_count++], &ProteusIII_pinsP->ProteusIII_Pin_Reset, sizeof(WE_Pin_t));
    memcpy(&pins[pin_count++], &ProteusIII_pinsP->ProteusIII_Pin_SleepWakeUp, sizeof(WE_Pin_t));
    memcpy(&pins[pin_count++], &ProteusIII_pinsP->ProteusIII_Pin_Boot, sizeof(WE_Pin_t));
    memcpy(&pins[pin_count++], &ProteusIII_pinsP->ProteusIII_Pin_Mode, sizeof(WE_Pin_t));
    memcpy(&pins[pin_count++], &ProteusIII_pinsP->ProteusIII_Pin_Busy, sizeof(WE_Pin_t));
    memcpy(&pins[pin_count++], &ProteusIII_pinsP->ProteusIII_Pin_StatusLed2, sizeof(WE_Pin_t));

    /* deinit pins */
    if (!WE_DeinitPins(pins, pin_count))
    {
        return false;
    }

    /* reset callbacks */
    memset(&callbacks, 0, sizeof(callbacks));

    /* make sure any bytes remaining in receive buffer are discarded */
    ClearReceiveBuffers();

    return ProteusIII_uartP->uartDeinit();
}

bool ProteusIII_PinWakeup()
{
    if (!WE_SetPin(ProteusIII_pinsP->ProteusIII_Pin_SleepWakeUp, WE_Pin_Level_Low))
    {
        return false;
    }
    WE_Delay(5);
    for (uint8_t i = 0; i < CMDCONFIRMATIONARRAY_LENGTH; i++)
    {
        cmdConfirmationArray[i].status = CMD_Status_Invalid;
        cmdConfirmationArray[i].cmd = CNFINVALID;
    }
    if (!WE_SetPin(ProteusIII_pinsP->ProteusIII_Pin_SleepWakeUp, WE_Pin_Level_High))
    {
        return false;
    }

    /* wait for cnf */
    return Wait4CNF(CMD_WAIT_TIME, PROTEUSIII_CMD_GETSTATE_CNF, CMD_Status_NoStatus, false);
}

bool ProteusIII_PinUartEnable()
{
    if (!WE_SetPin(ProteusIII_pinsP->ProteusIII_Pin_SleepWakeUp, WE_Pin_Level_Low))
    {
        return false;
    }
    WE_Delay(15);
    for (uint8_t i = 0; i < CMDCONFIRMATIONARRAY_LENGTH; i++)
    {
        cmdConfirmationArray[i].status = CMD_Status_Invalid;
        cmdConfirmationArray[i].cmd = CNFINVALID;
    }
    if (!WE_SetPin(ProteusIII_pinsP->ProteusIII_Pin_SleepWakeUp, WE_Pin_Level_High))
    {
        return false;
    }

    /* wait for UART enable indication */
    return Wait4CNF(CMD_WAIT_TIME, PROTEUSIII_CMD_UART_ENABLE_IND, CMD_Status_Success, false);
}

bool ProteusIII_PinReset()
{
    /* set to output mode */
    if (!WE_SetPin(ProteusIII_pinsP->ProteusIII_Pin_Reset, WE_Pin_Level_Low))
    {
        return false;
    }
    WE_Delay(5);
    /* make sure any bytes remaining in receive buffer are discarded */
    ClearReceiveBuffers();
    if (!WE_SetPin(ProteusIII_pinsP->ProteusIII_Pin_Reset, WE_Pin_Level_High))
    {
        return false;
    }

    if (operationMode == ProteusIII_OperationMode_PeripheralOnlyMode)
    {
        WE_Delay(PROTEUSIII_BOOT_DURATION);
        /* peripheral only mode is ready (the module doesn't send a "ready for operation" message in peripheral only mode) */
        return true;
    }

    /* wait for cnf */
    return Wait4CNF(CMD_WAIT_TIME, PROTEUSIII_CMD_GETSTATE_CNF, CMD_Status_NoStatus, true);
}

bool ProteusIII_Reset()
{
    txPacket.Cmd = PROTEUSIII_CMD_RESET_REQ;
    txPacket.Length = 0;

    FillChecksum(&txPacket);
    if (!ProteusIII_Transparent_Transmit((uint8_t*)&txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD))
    {
        return false;
    }

    /* wait for cnf */
    return Wait4CNF(CMD_WAIT_TIME, PROTEUSIII_CMD_GETSTATE_CNF, CMD_Status_NoStatus, true);
}

bool ProteusIII_Disconnect()
{
    txPacket.Cmd = PROTEUSIII_CMD_DISCONNECT_REQ;
    txPacket.Length = 0;

    FillChecksum(&txPacket);
    if (!ProteusIII_Transparent_Transmit((uint8_t*)&txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD))
    {
        return false;
    }

    /* Confirmation is sent before performing the disconnect. After disconnect, the module sends a disconnect indication */
    return Wait4CNF(CMD_WAIT_TIME, PROTEUSIII_CMD_DISCONNECT_CNF, CMD_Status_Success, true);
}

bool ProteusIII_Sleep()
{
    txPacket.Cmd = PROTEUSIII_CMD_SLEEP_REQ;
    txPacket.Length = 0;

    FillChecksum(&txPacket);
    if (!ProteusIII_Transparent_Transmit((uint8_t*)&txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD))
    {
        return false;
    }

    /* wait for cnf */
    return Wait4CNF(CMD_WAIT_TIME, PROTEUSIII_CMD_SLEEP_CNF, CMD_Status_Success, true);
}

bool ProteusIII_UartDisable()
{
    txPacket.Cmd = PROTEUSIII_CMD_UART_DISABLE_REQ;
    txPacket.Length = 0;

    FillChecksum(&txPacket);
    if (!ProteusIII_Transparent_Transmit((uint8_t*)&txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD))
    {
        return false;
    }

    /* wait for cnf */
    return Wait4CNF(CMD_WAIT_TIME, PROTEUSIII_CMD_UART_DISABLE_CNF, CMD_Status_Success, true);
}

bool ProteusIII_Transmit(uint8_t* payloadP, uint16_t length)
{
    if ((payloadP == NULL) || (length == 0) || (length > PROTEUSIII_MAX_RADIO_PAYLOAD_LENGTH) || (ProteusIII_DriverState_BLE_ChannelOpen != ProteusIII_GetDriverState()))
    {
        return false;
    }

    txPacket.Cmd = PROTEUSIII_CMD_DATA_REQ;
    txPacket.Length = length;

    memcpy(&txPacket.Data[0], payloadP, length);

    FillChecksum(&txPacket);
    if (!ProteusIII_Transparent_Transmit((uint8_t*)&txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD))
    {
        return false;
    }

    if (!Wait4CNF(CMD_WAIT_TIME, PROTEUSIII_CMD_DATA_CNF, CMD_Status_Success, true))
    {
        return false;
    }

    return Wait4CNF(CMD_WAIT_TIME, PROTEUSIII_CMD_TXCOMPLETE_RSP, CMD_Status_Success, false);
}

bool ProteusIII_SetBeacon(uint8_t* beaconDataP, uint16_t length)
{
    if ((beaconDataP == NULL) || (length == 0) || (length > PROTEUSIII_MAX_BEACON_LENGTH) || (ProteusIII_DriverState_BLE_Idle != ProteusIII_GetDriverState()))
    {
        return false;
    }

    txPacket.Cmd = PROTEUSIII_CMD_SETBEACON_REQ;
    txPacket.Length = length;

    memcpy(&txPacket.Data[0], beaconDataP, length);

    FillChecksum(&txPacket);
    if (!ProteusIII_Transparent_Transmit((uint8_t*)&txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD))
    {
        return false;
    }
    return Wait4CNF(CMD_WAIT_TIME, PROTEUSIII_CMD_SETBEACON_CNF, CMD_Status_Success, true);
}

/*
 * @brief Factory reset of the module.
 *
 * @return true if succeeded,
 *         false otherwise
 */
bool ProteusIII_FactoryReset()
{
    txPacket.Cmd = PROTEUSIII_CMD_FACTORYRESET_REQ;
    txPacket.Length = 0;

    FillChecksum(&txPacket);
    if (!ProteusIII_Transparent_Transmit((uint8_t*)&txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD))
    {
        return false;
    }

    /* wait for reset after factory reset */
    return Wait4CNF(CMD_WAIT_TIME, PROTEUSIII_CMD_GETSTATE_CNF, CMD_Status_NoStatus, true);
}

bool ProteusIII_CheckNSet(ProteusIII_UserSettings_t userSetting, uint8_t* valueP, uint8_t length)
{
    if ((valueP == NULL) || (length == 0))
    {
        return false;
    }

    uint8_t current_value[length];
    uint16_t current_length = length;

    if (true == ProteusIII_Get(userSetting, current_value, &current_length))
    {
        if ((length == current_length) && (0 == memcmp(valueP, current_value, length)))
        {
            /* value is already set, no need to set it again */
            return true;
        }
        else
        {
            /* value differs, and thus must be set */
            return ProteusIII_Set(userSetting, valueP, length);
        }
    }
    else
    {
        /* failed reading current value */
        return false;
    }
}

bool ProteusIII_Set(ProteusIII_UserSettings_t userSetting, uint8_t* valueP, uint8_t length)
{
    if ((valueP == NULL) || (length == 0))
    {
        return false;
    }

    txPacket.Cmd = PROTEUSIII_CMD_SET_REQ;
    txPacket.Length = 1 + length;
    txPacket.Data[0] = userSetting;
    memcpy(&txPacket.Data[1], valueP, length);

    FillChecksum(&txPacket);
    if (!ProteusIII_Transparent_Transmit((uint8_t*)&txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD))
    {
        return false;
    }

    /* wait for cnf */
    if (!Wait4CNF(CMD_WAIT_TIME, PROTEUSIII_CMD_SET_CNF, CMD_Status_Success, true))
    {
        return false;
    }

    return Wait4CNF(CMD_WAIT_TIME, PROTEUSIII_CMD_GETSTATE_CNF, CMD_Status_NoStatus, false);
}

bool ProteusIII_SetDeviceName(uint8_t* deviceNameP, uint8_t nameLength) { return ProteusIII_Set(ProteusIII_USERSETTING_RF_DEVICE_NAME, deviceNameP, nameLength); }

bool ProteusIII_SetAdvertisingTimeout(uint16_t advTimeout)
{
    uint8_t help[2];
    memcpy(help, (uint8_t*)&advTimeout, 2);
    return ProteusIII_Set(ProteusIII_USERSETTING_RF_ADVERTISING_TIMEOUT, help, 2);
}

bool ProteusIII_SetAdvertisingFlags(ProteusIII_AdvertisingFlags_t advFlags)
{
    uint8_t flags = (uint8_t)advFlags;
    return ProteusIII_Set(ProteusIII_USERSETTING_RF_ADVERTISING_FLAGS, &flags, 1);
}

bool ProteusIII_SetScanFlags(uint8_t scanFlags) { return ProteusIII_Set(ProteusIII_USERSETTING_RF_SCAN_FLAGS, &scanFlags, 1); }

bool ProteusIII_SetBeaconFlags(ProteusIII_BeaconFlags_t beaconFlags)
{
    uint8_t flags = (uint8_t)beaconFlags;
    return ProteusIII_Set(ProteusIII_USERSETTING_RF_BEACON_FLAGS, &flags, 1);
}

bool ProteusIII_SetCFGFlags(uint16_t cfgFlags)
{
    uint8_t help[2];
    memcpy(help, (uint8_t*)&cfgFlags, 2);
    return ProteusIII_Set(ProteusIII_USERSETTING_RF_CFGFLAGS, help, 2);
}

bool ProteusIII_SetConnectionTiming(ProteusIII_ConnectionTiming_t connectionTiming) { return ProteusIII_Set(ProteusIII_USERSETTING_RF_CONNECTION_TIMING, (uint8_t*)&connectionTiming, 1); }

bool ProteusIII_SetScanTiming(ProteusIII_ScanTiming_t scanTiming) { return ProteusIII_Set(ProteusIII_USERSETTING_RF_SCAN_TIMING, (uint8_t*)&scanTiming, 1); }

bool ProteusIII_SetScanFactor(uint8_t scanFactor)
{
    if ((scanFactor > 10) || (scanFactor < 1))
    {
        return false;
    }
    return ProteusIII_Set(ProteusIII_USERSETTING_RF_SCAN_FACTOR, &scanFactor, 1);
}

bool ProteusIII_SetTXPower(ProteusIII_TXPower_t txPower) { return ProteusIII_Set(ProteusIII_USERSETTING_RF_TX_POWER, (uint8_t*)&txPower, 1); }

bool ProteusIII_SetSecFlags(ProteusIII_SecFlags_t secFlags) { return ProteusIII_Set(ProteusIII_USERSETTING_RF_SEC_FLAGS, (uint8_t*)&secFlags, 1); }

bool ProteusIII_SetSecFlagsPeripheralOnly(ProteusIII_SecFlags_t secFlags) { return ProteusIII_Set(ProteusIII_USERSETTING_RF_SECFLAGSPERONLY, (uint8_t*)&secFlags, 1); }

bool ProteusIII_SetBaudrateIndex(ProteusIII_BaudRate_t baudrate, ProteusIII_UartParity_t parity, bool flowControlEnable)
{
    uint8_t baudrateIndex = (uint8_t)baudrate;

    /* If flow control is to be enabled UART index has to be increased by one in regard to base value */
    if (flowControlEnable)
    {
        baudrateIndex++;
    }

    /* If parity bit is even, UART index has to be increased by 64 in regard of base value*/
    if (ProteusIII_UartParity_Even == parity)
    {
        baudrateIndex += 64;
    }

    return ProteusIII_Set(ProteusIII_USERSETTING_UART_CONFIG_INDEX, (uint8_t*)&baudrateIndex, 1);
}

bool ProteusIII_SetBTMAC(uint8_t* btMacP) { return ProteusIII_Set(ProteusIII_USERSETTING_FS_BTMAC, btMacP, 6); }

bool ProteusIII_SetStaticPasskey(uint8_t* staticPasskeyP) { return ProteusIII_Set(ProteusIII_USERSETTING_RF_STATIC_PASSKEY, staticPasskeyP, 6); }

bool ProteusIII_SetAppearance(uint16_t appearance)
{
    uint8_t help[2];
    memcpy(help, (uint8_t*)&appearance, 2);
    return ProteusIII_Set(ProteusIII_USERSETTING_RF_APPEARANCE, help, 2);
}

bool ProteusIII_SetSppBaseUuid(uint8_t* uuidP) { return ProteusIII_Set(ProteusIII_USERSETTING_RF_SPPBASEUUID, uuidP, 16); }

bool ProteusIII_SetSppServiceUuid(uint8_t* uuidP) { return ProteusIII_Set(ProteusIII_USERSETTING_RF_SPPServiceUUID, uuidP, 2); }

bool ProteusIII_SetSppRxUuid(uint8_t* uuidP) { return ProteusIII_Set(ProteusIII_USERSETTING_RF_SPPRXUUID, uuidP, 2); }

bool ProteusIII_SetSppTxUuid(uint8_t* uuidP) { return ProteusIII_Set(ProteusIII_USERSETTING_RF_SPPTXUUID, uuidP, 2); }

bool ProteusIII_Get(ProteusIII_UserSettings_t userSetting, uint8_t* responseP, uint16_t* responseLengthP)
{
    if ((responseP == NULL) || (responseLengthP == NULL))
    {
        return false;
    }

    txPacket.Cmd = PROTEUSIII_CMD_GET_REQ;
    txPacket.Length = 1;
    txPacket.Data[0] = userSetting;

    FillChecksum(&txPacket);
    if (!ProteusIII_Transparent_Transmit((uint8_t*)&txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD))
    {
        return false;
    }

    /* wait for cnf */
    if (Wait4CNF(CMD_WAIT_TIME, PROTEUSIII_CMD_GET_CNF, CMD_Status_Success, true))
    {
        uint16_t length = rxPacket.Length;
        memcpy(responseP, &rxPacket.Data[1], length - 1); /* First Data byte is status, following bytes response */
        *responseLengthP = length - 1;
        return true;
    }

    return false;
}

bool ProteusIII_GetFWVersion(uint8_t* versionP)
{
    uint16_t length;
    return ProteusIII_Get(ProteusIII_USERSETTING_FS_FWVersion, versionP, &length);
}

bool ProteusIII_GetDeviceInfo(ProteusIII_DeviceInfo_t* deviceInfoP)
{
    if (deviceInfoP == NULL)
    {
        return false;
    }

    uint8_t help[12];
    uint16_t length;
    if (!ProteusIII_Get(ProteusIII_USERSETTING_FS_DEVICE_INFO, help, &length))
    {
        return false;
    }
    memcpy(&deviceInfoP->osVersion, help, 2);
    memcpy(&deviceInfoP->buildCode, help + 2, 4);
    memcpy(&deviceInfoP->packageVariant, help + 6, 2);
    memcpy(&deviceInfoP->chipId, help + 8, 4);
    return true;
}

bool ProteusIII_GetSerialNumber(uint8_t* serialNumberP)
{
    uint16_t length;
    if (!ProteusIII_Get(ProteusIII_USERSETTING_FS_SERIAL_NUMBER, serialNumberP, &length))
    {
        return false;
    }
    return true;
}

bool ProteusIII_GetDeviceName(uint8_t* deviceNameP, uint16_t* nameLengthP) { return ProteusIII_Get(ProteusIII_USERSETTING_RF_DEVICE_NAME, deviceNameP, nameLengthP); }

bool ProteusIII_GetMAC(uint8_t* macP)
{
    uint16_t length;
    return ProteusIII_Get(ProteusIII_USERSETTING_FS_MAC, macP, &length);
}

bool ProteusIII_GetBTMAC(uint8_t* btMacP)
{
    uint16_t length;
    return ProteusIII_Get(ProteusIII_USERSETTING_FS_BTMAC, btMacP, &length);
}

bool ProteusIII_GetAdvertisingTimeout(uint16_t* advTimeoutP)
{
    if (advTimeoutP == NULL)
    {
        return false;
    }

    uint16_t length;
    uint8_t help[2];

    if (ProteusIII_Get(ProteusIII_USERSETTING_RF_ADVERTISING_TIMEOUT, help, &length))
    {
        memcpy((uint8_t*)advTimeoutP, help, 2);
        return true;
    }

    return false;
}

bool ProteusIII_GetAdvertisingFlags(ProteusIII_AdvertisingFlags_t* advFlagsP)
{
    if (advFlagsP == NULL)
    {
        return false;
    }

    uint16_t length;
    uint8_t flags;
    if (ProteusIII_Get(ProteusIII_USERSETTING_RF_ADVERTISING_FLAGS, &flags, &length))
    {
        *advFlagsP = flags;
        return true;
    }
    return false;
}

bool ProteusIII_GetScanFlags(uint8_t* scanFlagsP)
{
    uint16_t length;
    return ProteusIII_Get(ProteusIII_USERSETTING_RF_SCAN_FLAGS, scanFlagsP, &length);
}

bool ProteusIII_GetBeaconFlags(ProteusIII_BeaconFlags_t* beaconFlagsP)
{
    if (beaconFlagsP == NULL)
    {
        return false;
    }

    uint16_t length;
    uint8_t flags;
    if (ProteusIII_Get(ProteusIII_USERSETTING_RF_BEACON_FLAGS, &flags, &length))
    {
        *beaconFlagsP = flags;
        return true;
    }
    return false;
}

bool ProteusIII_GetConnectionTiming(ProteusIII_ConnectionTiming_t* connectionTimingP)
{
    uint16_t length;
    return ProteusIII_Get(ProteusIII_USERSETTING_RF_CONNECTION_TIMING, (uint8_t*)connectionTimingP, &length);
}

bool ProteusIII_GetScanTiming(ProteusIII_ScanTiming_t* scanTimingP)
{
    uint16_t length;
    return ProteusIII_Get(ProteusIII_USERSETTING_RF_SCAN_TIMING, (uint8_t*)scanTimingP, &length);
}

bool ProteusIII_GetScanFactor(uint8_t* scanFactorP)
{
    uint16_t length;
    return ProteusIII_Get(ProteusIII_USERSETTING_RF_SCAN_FACTOR, scanFactorP, &length);
}

bool ProteusIII_GetTXPower(ProteusIII_TXPower_t* txPowerP)
{
    uint16_t length;
    return ProteusIII_Get(ProteusIII_USERSETTING_RF_TX_POWER, (uint8_t*)txPowerP, &length);
}

bool ProteusIII_GetSecFlags(ProteusIII_SecFlags_t* secFlagsP)
{
    uint16_t length;
    return ProteusIII_Get(ProteusIII_USERSETTING_RF_SEC_FLAGS, (uint8_t*)secFlagsP, &length);
}

bool ProteusIII_GetSecFlagsPeripheralOnly(ProteusIII_SecFlags_t* secFlagsP)
{
    uint16_t length;
    return ProteusIII_Get(ProteusIII_USERSETTING_RF_SECFLAGSPERONLY, (uint8_t*)secFlagsP, &length);
}

bool ProteusIII_GetBaudrateIndex(ProteusIII_BaudRate_t* baudrateP, ProteusIII_UartParity_t* parityP, bool* flowControlEnableP)
{
    if ((baudrateP == NULL) || (parityP == NULL) || (flowControlEnableP == NULL))
    {
        return false;
    }

    uint16_t length;
    uint8_t uartIndex;

    if (ProteusIII_Get(ProteusIII_USERSETTING_UART_CONFIG_INDEX, (uint8_t*)&uartIndex, &length))
    {
        /* if index is even, flow control is off.
         * If flow control is on, decrease index by one to later determine the base baudrate */
        if (0x01 == (uartIndex & 0x01))
        {
            /* odd */
            *flowControlEnableP = true;
            uartIndex--;
        }
        else
        {
            /* even */
            *flowControlEnableP = false;
        }

        /* If baudrate index is greater than or equal to 64, parity bit is even*/
        if (uartIndex < 64)
        {
            *parityP = ProteusIII_UartParity_None;
        }
        else
        {
            *parityP = ProteusIII_UartParity_Even;
            uartIndex -= 64;
        }

        *baudrateP = (ProteusIII_BaudRate_t)uartIndex;
        return true;
    }

    return false;
}

bool ProteusIII_GetStaticPasskey(uint8_t* staticPasskeyP)
{
    uint16_t length;
    return ProteusIII_Get(ProteusIII_USERSETTING_RF_STATIC_PASSKEY, staticPasskeyP, &length);
}

bool ProteusIII_GetAppearance(uint16_t* appearanceP)
{
    if (appearanceP == NULL)
    {
        return false;
    }

    uint16_t length;
    uint8_t help[2];

    if (ProteusIII_Get(ProteusIII_USERSETTING_RF_APPEARANCE, help, &length))
    {
        memcpy((uint8_t*)appearanceP, help, 2);
        return true;
    }

    return false;
}

bool ProteusIII_GetSppBaseUuid(uint8_t* uuidP)
{
    uint16_t length;
    return ProteusIII_Get(ProteusIII_USERSETTING_RF_SPPBASEUUID, uuidP, &length);
}

bool ProteusIII_GetSppServiceUuid(uint8_t* uuidP)
{
    uint16_t length;
    return ProteusIII_Get(ProteusIII_USERSETTING_RF_SPPServiceUUID, uuidP, &length);
}

bool ProteusIII_GetSppRxUuid(uint8_t* uuidP)
{
    uint16_t length;
    return ProteusIII_Get(ProteusIII_USERSETTING_RF_SPPRXUUID, uuidP, &length);
}

bool ProteusIII_GetSppTxUuid(uint8_t* uuidP)
{
    uint16_t length;
    return ProteusIII_Get(ProteusIII_USERSETTING_RF_SPPTXUUID, uuidP, &length);
}

bool ProteusIII_GetCFGFlags(uint16_t* cfgFlagsP)
{
    if (cfgFlagsP == NULL)
    {
        return false;
    }

    uint16_t length;
    uint8_t help[2];

    if (ProteusIII_Get(ProteusIII_USERSETTING_RF_CFGFLAGS, help, &length))
    {
        memcpy((uint8_t*)cfgFlagsP, help, 2);
        return true;
    }

    return false;
}

bool ProteusIII_GetState(ProteusIII_ModuleState_t* moduleStateP)
{
    if (moduleStateP == NULL)
    {
        return false;
    }

    txPacket.Cmd = PROTEUSIII_CMD_GETSTATE_REQ;
    txPacket.Length = 0;

    FillChecksum(&txPacket);
    if (!ProteusIII_Transparent_Transmit((uint8_t*)&txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD))
    {
        return false;
    }

    /* wait for cnf */
    if (Wait4CNF(CMD_WAIT_TIME, PROTEUSIII_CMD_GETSTATE_CNF, CMD_Status_NoStatus, true))
    {
        uint16_t length = rxPacket.Length;
        moduleStateP->role = rxPacket.Data[0];
        moduleStateP->action = rxPacket.Data[1];

        if (moduleStateP->action == ProteusIII_BLE_Action_Connected && length >= 8)
        {
            memcpy(moduleStateP->connectedDeviceBtMac, &rxPacket.Data[2], 6);
        }
        else
        {
            memset(moduleStateP->connectedDeviceBtMac, 0, 6);
        }

        return true;
    }

    return false;
}

ProteusIII_DriverState_t ProteusIII_GetDriverState() { return bleState; }

bool ProteusIII_ScanStart()
{
    txPacket.Cmd = PROTEUSIII_CMD_SCANSTART_REQ;
    txPacket.Length = 0;

    FillChecksum(&txPacket);
    if (!ProteusIII_Transparent_Transmit((uint8_t*)&txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD))
    {
        return false;
    }

    /* wait for cnf */
    return Wait4CNF(CMD_WAIT_TIME, PROTEUSIII_CMD_SCANSTART_CNF, CMD_Status_Success, true);
}

bool ProteusIII_ScanStop()
{
    txPacket.Cmd = PROTEUSIII_CMD_SCANSTOP_REQ;
    txPacket.Length = 0;

    FillChecksum(&txPacket);
    if (!ProteusIII_Transparent_Transmit((uint8_t*)&txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD))
    {
        return false;
    }

    /* wait for cnf */
    return Wait4CNF(CMD_WAIT_TIME, PROTEUSIII_CMD_SCANSTOP_CNF, CMD_Status_Success, true);
}

bool ProteusIII_GetDevices(ProteusIII_GetDevices_t* devicesP)
{
    ProteusIII_getDevicesP = devicesP;
    if (ProteusIII_getDevicesP != NULL)
    {
        ProteusIII_getDevicesP->numberOfDevices = 0;
    }

    txPacket.Cmd = PROTEUSIII_CMD_GETDEVICES_REQ;
    txPacket.Length = 0;

    FillChecksum(&txPacket);
    if (!ProteusIII_Transparent_Transmit((uint8_t*)&txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD))
    {
        ProteusIII_getDevicesP = NULL;
        return false;
    }

    /* wait for cnf */
    bool ret = Wait4CNF(CMD_WAIT_TIME, PROTEUSIII_CMD_GETDEVICES_CNF, CMD_Status_Success, true);
    ProteusIII_getDevicesP = NULL;
    return ret;
}

bool ProteusIII_Connect(uint8_t* btMacP)
{
    if (btMacP == NULL)
    {
        return false;
    }

    txPacket.Cmd = PROTEUSIII_CMD_CONNECT_REQ;
    txPacket.Length = 6;
    memcpy(&txPacket.Data[0], btMacP, 6);

    FillChecksum(&txPacket);
    if (!ProteusIII_Transparent_Transmit((uint8_t*)&txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD))
    {
        return false;
    }

    /* wait for cnf */
    return Wait4CNF(3000, PROTEUSIII_CMD_CONNECT_CNF, CMD_Status_Success, true);
}

bool ProteusIII_Passkey(uint8_t* passkeyP)
{
    if (passkeyP == NULL)
    {
        return false;
    }

    txPacket.Cmd = PROTEUSIII_CMD_PASSKEY_REQ;
    txPacket.Length = 6;
    memcpy(&txPacket.Data[0], passkeyP, 6);

    FillChecksum(&txPacket);
    if (!ProteusIII_Transparent_Transmit((uint8_t*)&txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD))
    {
        return false;
    }

    /* wait for cnf */
    return Wait4CNF(CMD_WAIT_TIME, PROTEUSIII_CMD_PASSKEY_CNF, CMD_Status_Success, true);
}

bool ProteusIII_NumericCompareConfirm(bool keyIsOk)
{
    txPacket.Cmd = PROTEUSIII_CMD_NUMERIC_COMP_REQ;
    txPacket.Length = 1;
    txPacket.Data[0] = keyIsOk ? 0x00 : 0x01;

    FillChecksum(&txPacket);
    if (!ProteusIII_Transparent_Transmit((uint8_t*)&txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD))
    {
        return false;
    }

    /* wait for cnf */
    return Wait4CNF(CMD_WAIT_TIME, PROTEUSIII_CMD_NUMERIC_COMP_CNF, CMD_Status_Success, true);
}

bool ProteusIII_PhyUpdate(ProteusIII_Phy_t phy)
{
    if (ProteusIII_DriverState_BLE_ChannelOpen == ProteusIII_GetDriverState())
    {
        txPacket.Cmd = PROTEUSIII_CMD_PHYUPDATE_REQ;
        txPacket.Length = 1;
        txPacket.Data[0] = (uint8_t)phy;

        FillChecksum(&txPacket);
        if (!ProteusIII_Transparent_Transmit((uint8_t*)&txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD))
        {
            return false;
        }

        /* wait for cnf */
        return Wait4CNF(CMD_WAIT_TIME, PROTEUSIII_CMD_PHYUPDATE_CNF, CMD_Status_Success, true);
    }
    return false;
}

bool ProteusIII_GetStatusLed2PinLevel(WE_Pin_Level_t* statusLed2LevelP) { return WE_GetPinLevel(ProteusIII_pinsP->ProteusIII_Pin_StatusLed2, statusLed2LevelP); }

bool ProteusIII_IsPeripheralOnlyModeBusy(bool* busyStateP)
{
    if (busyStateP == NULL)
    {
        return false;
    }

    WE_Pin_Level_t pin_level;

    if (!WE_GetPinLevel(ProteusIII_pinsP->ProteusIII_Pin_Busy, &pin_level))
    {
        return false;
    }

    *busyStateP = (pin_level == WE_Pin_Level_High);

    return true;
}

void ProteusIII_SetByteRxCallback(WE_UART_HandleRxByte_t callback) { byteRxCallback = (callback == NULL) ? ProteusIII_HandleRxByte : callback; }

bool ProteusIII_GPIOLocalWriteConfig(ProteusIII_GPIOConfigBlock_t* configP, uint16_t numberOfConfigs)
{
    if (configP == NULL)
    {
        return false;
    }

    uint16_t length = 0;

    for (uint16_t i = 0; i < numberOfConfigs; i++)
    {
        switch (configP->function)
        {
            case ProteusIII_GPIO_IO_Disconnected:
            {
                txPacket.Data[length] = 3;
                txPacket.Data[length + 1] = configP->gpioId;
                txPacket.Data[length + 2] = configP->function;
                txPacket.Data[length + 3] = 0x00;
                length += 4;
                break;
            }
            case ProteusIII_GPIO_IO_Input:
            {
                txPacket.Data[length] = 3;
                txPacket.Data[length + 1] = configP->gpioId;
                txPacket.Data[length + 2] = configP->function;
                txPacket.Data[length + 3] = configP->value.input;
                length += 4;
                break;
            }
            case ProteusIII_GPIO_IO_Output:
            {
                txPacket.Data[length] = 3;
                txPacket.Data[length + 1] = configP->gpioId;
                txPacket.Data[length + 2] = configP->function;
                txPacket.Data[length + 3] = configP->value.output;
                length += 4;
                break;
            }
            case ProteusIII_GPIO_IO_PWM:
            {
                txPacket.Data[length] = 5;
                txPacket.Data[length + 1] = configP->gpioId;
                txPacket.Data[length + 2] = configP->function;
                memcpy(&txPacket.Data[length + 3], &configP->value.pwm.period, 2);
                txPacket.Data[length + 5] = configP->value.pwm.ratio;
                length += 6;
                break;
            }
            default:
                break;
        }

        /* Move pointer to next element. configP is increased by sizeof(ProteusIII_GPIOConfigBlock_t)*/
        configP++;
    }

    txPacket.Cmd = PROTEUSIII_CMD_GPIO_LOCAL_WRITECONFIG_REQ;
    txPacket.Length = length;

    FillChecksum(&txPacket);
    if (!ProteusIII_Transparent_Transmit((uint8_t*)&txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD))
    {
        return false;
    }

    /* wait for cnf */
    return Wait4CNF(CMD_WAIT_TIME, PROTEUSIII_CMD_GPIO_LOCAL_WRITECONFIG_CNF, CMD_Status_Success, true);
}

bool ProteusIII_GPIOLocalReadConfig(ProteusIII_GPIOConfigBlock_t* configP, uint16_t* numberOfConfigsP)
{
    if ((configP == NULL) || (numberOfConfigsP == NULL))
    {
        return false;
    }

    txPacket.Cmd = PROTEUSIII_CMD_GPIO_LOCAL_READCONFIG_REQ;
    txPacket.Length = 0;

    FillChecksum(&txPacket);
    if (!ProteusIII_Transparent_Transmit((uint8_t*)&txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD))
    {
        return false;
    }

    /* wait for cnf */
    if (Wait4CNF(CMD_WAIT_TIME, PROTEUSIII_CMD_GPIO_LOCAL_READCONFIG_CNF, CMD_Status_Success, true))
    {
        uint16_t length = rxPacket.Length;

        *numberOfConfigsP = 0;
        uint8_t* uartP = &rxPacket.Data[1];
        ProteusIII_GPIOConfigBlock_t* configP_running = configP;
        while (uartP < &rxPacket.Data[length])
        {
            switch (*(uartP + 2))
            {
                case ProteusIII_GPIO_IO_Disconnected:
                {
                    if (*uartP == 2)
                    {
                        configP_running->gpioId = *(uartP + 1);
                        configP_running->function = *(uartP + 2);

                        configP_running++;
                        *numberOfConfigsP += 1;
                    }
                    break;
                }
                case ProteusIII_GPIO_IO_Input:
                {
                    if (*uartP == 3)
                    {
                        configP_running->gpioId = *(uartP + 1);
                        configP_running->function = *(uartP + 2);
                        configP_running->value.input = *(uartP + 3);

                        configP_running++;
                        *numberOfConfigsP += 1;
                    }
                    break;
                }
                case ProteusIII_GPIO_IO_Output:
                {
                    if (*uartP == 3)
                    {
                        configP_running->gpioId = *(uartP + 1);
                        configP_running->function = *(uartP + 2);
                        configP_running->value.output = *(uartP + 3);

                        configP_running++;
                        *numberOfConfigsP += 1;
                    }
                    break;
                }
                case ProteusIII_GPIO_IO_PWM:
                {
                    if (*uartP == 5)
                    {
                        configP_running->gpioId = *(uartP + 1);
                        configP_running->function = *(uartP + 2);
                        memcpy(&configP_running->value.pwm.period, (uartP + 3), 2);
                        configP_running->value.pwm.ratio = *(uartP + 5);

                        configP_running++;
                        *numberOfConfigsP += 1;
                    }
                    break;
                }
                default:
                    break;
            }

            uartP += *uartP + 1;
        }
        return true;
    }

    return false;
}

bool ProteusIII_GPIOLocalWrite(ProteusIII_GPIOControlBlock_t* controlP, uint16_t numberOfControls)
{
    if ((controlP == NULL) || (numberOfControls == 0))
    {
        return false;
    }

    uint16_t length = 0;

    for (uint16_t i = 0; i < numberOfControls; i++)
    {
        txPacket.Data[length] = 2;
        txPacket.Data[length + 1] = controlP->gpioId;
        txPacket.Data[length + 2] = controlP->value.output;
        length += 3;

        /* Move pointer to next element. configP is increased by sizeof(ProteusIII_GPIOControlBlock_t)*/
        controlP++;
    }

    txPacket.Cmd = PROTEUSIII_CMD_GPIO_LOCAL_WRITE_REQ;
    txPacket.Length = length;

    FillChecksum(&txPacket);
    if (!ProteusIII_Transparent_Transmit((uint8_t*)&txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD))
    {
        return false;
    }

    /* wait for cnf */
    return Wait4CNF(CMD_WAIT_TIME, PROTEUSIII_CMD_GPIO_LOCAL_WRITE_CNF, CMD_Status_Success, true);
}

bool ProteusIII_GPIOLocalRead(uint8_t* gpioToReadP, uint8_t amountGPIOToRead, ProteusIII_GPIOControlBlock_t* controlP, uint16_t* numberOfControlsP)
{
    if ((gpioToReadP == NULL) || (controlP == NULL) || (numberOfControlsP == NULL) || (amountGPIOToRead == 0))
    {
        return false;
    }

    txPacket.Cmd = PROTEUSIII_CMD_GPIO_LOCAL_READ_REQ;
    txPacket.Length = amountGPIOToRead + 1;
    txPacket.Data[0] = amountGPIOToRead;
    memcpy(&txPacket.Data[1], gpioToReadP, amountGPIOToRead);

    FillChecksum(&txPacket);
    if (!ProteusIII_Transparent_Transmit((uint8_t*)&txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD))
    {
        return false;
    }

    /* wait for cnf */
    if (Wait4CNF(CMD_WAIT_TIME, PROTEUSIII_CMD_GPIO_LOCAL_READ_CNF, CMD_Status_Success, true))
    {
        uint16_t length = rxPacket.Length;

        *numberOfControlsP = 0;
        uint8_t* uartP = &rxPacket.Data[1];
        ProteusIII_GPIOControlBlock_t* controlP_running = controlP;
        while (uartP < &rxPacket.Data[length])
        {
            /* each ControlBlock starts with length field which is currently fixed to "2" */
            if (*uartP == 2)
            {
                controlP_running->gpioId = *(uartP + 1);
                controlP_running->value.output = *(uartP + 2);

                /* Move pointer to next element. configP is increased by sizeof(ProteusIII_GPIOControlBlock_t)*/
                controlP_running++;
                *numberOfControlsP += 1;
            }

            /* uartP points to length field of control block. So increase address by value of length + 1 */
            uartP += *uartP + 1;
        }
        return true;
    }

    return false;
}

bool ProteusIII_GPIORemoteWriteConfig(ProteusIII_GPIOConfigBlock_t* configP, uint16_t numberOfConfigs)
{
    if ((configP == NULL) || (numberOfConfigs == 0))
    {
        return false;
    }

    uint16_t length = 0;

    for (uint16_t i = 0; i < numberOfConfigs; i++)
    {
        switch (configP->function)
        {
            case ProteusIII_GPIO_IO_Disconnected:
            {
                txPacket.Data[length] = 3;
                txPacket.Data[length + 1] = configP->gpioId;
                txPacket.Data[length + 2] = configP->function;
                txPacket.Data[length + 3] = 0x00;
                length += 4;
                break;
            }
            case ProteusIII_GPIO_IO_Input:
            {
                txPacket.Data[length] = 3;
                txPacket.Data[length + 1] = configP->gpioId;
                txPacket.Data[length + 2] = configP->function;
                txPacket.Data[length + 3] = configP->value.input;
                length += 4;
                break;
            }
            case ProteusIII_GPIO_IO_Output:
            {
                txPacket.Data[length] = 3;
                txPacket.Data[length + 1] = configP->gpioId;
                txPacket.Data[length + 2] = configP->function;
                txPacket.Data[length + 3] = configP->value.output;
                length += 4;
                break;
            }
            case ProteusIII_GPIO_IO_PWM:
            {
                txPacket.Data[length] = 5;
                txPacket.Data[length + 1] = configP->gpioId;
                txPacket.Data[length + 2] = configP->function;
                memcpy(&txPacket.Data[length + 3], &configP->value.pwm.period, 2);
                txPacket.Data[length + 5] = configP->value.pwm.ratio;
                length += 6;
                break;
            }
            default:
                break;
        }
        configP++;
    }

    txPacket.Cmd = PROTEUSIII_CMD_GPIO_REMOTE_WRITECONFIG_REQ;
    txPacket.Length = length;

    FillChecksum(&txPacket);
    if (!ProteusIII_Transparent_Transmit((uint8_t*)&txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD))
    {
        return false;
    }

    /* wait for cnf */
    return Wait4CNF(CMD_WAIT_TIME, PROTEUSIII_CMD_GPIO_REMOTE_WRITECONFIG_CNF, CMD_Status_Success, true);
}

bool ProteusIII_GPIORemoteReadConfig(ProteusIII_GPIOConfigBlock_t* configP, uint16_t* numberOfConfigsP)
{
    if ((configP == NULL) || (numberOfConfigsP == NULL))
    {
        return false;
    }

    txPacket.Cmd = PROTEUSIII_CMD_GPIO_REMOTE_READCONFIG_REQ;
    txPacket.Length = 0;

    FillChecksum(&txPacket);
    if (!ProteusIII_Transparent_Transmit((uint8_t*)&txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD))
    {
        return false;
    }

    /* wait for cnf */
    if (Wait4CNF(CMD_WAIT_TIME, PROTEUSIII_CMD_GPIO_REMOTE_READCONFIG_CNF, CMD_Status_Success, true))
    {
        uint16_t length = rxPacket.Length;

        *numberOfConfigsP = 0;
        uint8_t* uartP = &rxPacket.Data[1];
        ProteusIII_GPIOConfigBlock_t* configP_running = configP;
        while (uartP < &rxPacket.Data[length])
        {
            switch (*(uartP + 2))
            {
                case ProteusIII_GPIO_IO_Disconnected:
                {
                    if (*uartP == 2)
                    {
                        configP_running->gpioId = *(uartP + 1);
                        configP_running->function = *(uartP + 2);

                        configP_running++;
                        *numberOfConfigsP += 1;
                    }
                    break;
                }
                case ProteusIII_GPIO_IO_Input:
                {
                    if (*uartP == 3)
                    {
                        configP_running->gpioId = *(uartP + 1);
                        configP_running->function = *(uartP + 2);
                        configP_running->value.input = *(uartP + 3);

                        configP_running++;
                        *numberOfConfigsP += 1;
                    }
                    break;
                }
                case ProteusIII_GPIO_IO_Output:
                {
                    if (*uartP == 3)
                    {
                        configP_running->gpioId = *(uartP + 1);
                        configP_running->function = *(uartP + 2);
                        configP_running->value.output = *(uartP + 3);

                        configP_running++;
                        *numberOfConfigsP += 1;
                    }
                    break;
                }
                case ProteusIII_GPIO_IO_PWM:
                {
                    if (*uartP == 5)
                    {
                        configP_running->gpioId = *(uartP + 1);
                        configP_running->function = *(uartP + 2);
                        memcpy(&configP_running->value.pwm.period, (uartP + 3), 2);
                        configP_running->value.pwm.ratio = *(uartP + 5);

                        configP_running++;
                        *numberOfConfigsP += 1;
                    }
                    break;
                }
                default:
                    break;
            }
            uartP += *uartP + 1;
        }
        return true;
    }

    return false;
}

bool ProteusIII_GPIORemoteWrite(ProteusIII_GPIOControlBlock_t* controlP, uint16_t numberOfControls)
{
    if ((controlP == NULL) || (numberOfControls == 0))
    {
        return false;
    }

    uint16_t length = 0;
    for (uint16_t i = 0; i < numberOfControls; i++)
    {
        txPacket.Data[length] = 2;
        txPacket.Data[length + 1] = controlP->gpioId;
        txPacket.Data[length + 2] = controlP->value.ratio;
        length += 3;
        controlP++;
    }

    txPacket.Cmd = PROTEUSIII_CMD_GPIO_REMOTE_WRITE_REQ;
    txPacket.Length = length;

    FillChecksum(&txPacket);
    if (!ProteusIII_Transparent_Transmit((uint8_t*)&txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD))
    {
        return false;
    }

    /* wait for cnf */
    return Wait4CNF(CMD_WAIT_TIME, PROTEUSIII_CMD_GPIO_REMOTE_WRITE_CNF, CMD_Status_Success, true);
}

bool ProteusIII_GPIORemoteRead(uint8_t* gpioToReadP, uint8_t amountGPIOToRead, ProteusIII_GPIOControlBlock_t* controlP, uint16_t* numberOfControlsP)
{
    if ((gpioToReadP == NULL) || (controlP == NULL) || (numberOfControlsP == NULL) || (amountGPIOToRead == 0))
    {
        return false;
    }

    txPacket.Cmd = PROTEUSIII_CMD_GPIO_REMOTE_READ_REQ;
    txPacket.Length = amountGPIOToRead + 1;
    txPacket.Data[0] = amountGPIOToRead;
    memcpy(&txPacket.Data[1], gpioToReadP, amountGPIOToRead);

    FillChecksum(&txPacket);
    if (!ProteusIII_Transparent_Transmit((uint8_t*)&txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD))
    {
        return false;
    }

    /* wait for cnf */
    if (Wait4CNF(CMD_WAIT_TIME, PROTEUSIII_CMD_GPIO_REMOTE_READ_CNF, CMD_Status_Success, true))
    {
        uint16_t length = rxPacket.Length;

        *numberOfControlsP = 0;
        uint8_t* uartP = &rxPacket.Data[1];
        ProteusIII_GPIOControlBlock_t* controlP_running = controlP;
        while (uartP < &rxPacket.Data[length])
        {
            if (*uartP == 2)
            {
                controlP_running->gpioId = *(uartP + 1);
                controlP_running->value.ratio = *(uartP + 2);

                controlP_running++;
                *numberOfControlsP += 1;
            }
            uartP += *uartP + 1;
        }
        return true;
    }

    return false;
}

bool ProteusIII_GetBonds(ProteusIII_BondDatabase_t* bondDatabaseP)
{
    if (bondDatabaseP == NULL)
    {
        return false;
    }

    txPacket.Cmd = PROTEUSIII_CMD_GET_BONDS_REQ;
    txPacket.Length = 0;

    FillChecksum(&txPacket);
    if (!ProteusIII_Transparent_Transmit((uint8_t*)&txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD))
    {
        return false;
    }

    /* wait for cnf */
    if (!Wait4CNF(CMD_WAIT_TIME, PROTEUSIII_CMD_GET_BONDS_CNF, CMD_Status_Success, true))
    {
        return false;
    }

    bondDatabaseP->nrOfDevices = rxPacket.Data[1];
    if (bondDatabaseP->nrOfDevices > PROTEUSIII_MAX_BOND_DEVICES)
    {
        bondDatabaseP->nrOfDevices = PROTEUSIII_MAX_BOND_DEVICES;
    }

    for (uint8_t i = 0; i < bondDatabaseP->nrOfDevices; i++)
    {
        uint8_t offset = 2 + i * 8;
        bondDatabaseP->devices[i].id = ((uint16_t)rxPacket.Data[offset] << 0) + ((uint16_t)rxPacket.Data[offset + 1] << 8);
        memcpy(bondDatabaseP->devices[i].btMac, &rxPacket.Data[offset + 2], 6);
    }

    return true;
}

bool ProteusIII_DeleteBonds()
{
    txPacket.Cmd = PROTEUSIII_CMD_DELETE_BONDS_REQ;
    txPacket.Length = 0;

    FillChecksum(&txPacket);
    if (!ProteusIII_Transparent_Transmit((uint8_t*)&txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD))
    {
        return false;
    }

    /* wait for cnf */
    return Wait4CNF(CMD_WAIT_TIME, PROTEUSIII_CMD_DELETE_BONDS_CNF, CMD_Status_Success, true);
}

bool ProteusIII_DeleteBond(uint8_t bondId)
{
    txPacket.Cmd = PROTEUSIII_CMD_DELETE_BONDS_REQ;
    txPacket.Length = 2;
    txPacket.Data[0] = bondId;
    txPacket.Data[1] = 0;

    FillChecksum(&txPacket);
    if (!ProteusIII_Transparent_Transmit((uint8_t*)&txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD))
    {
        return false;
    }

    /* wait for cnf */
    return Wait4CNF(CMD_WAIT_TIME, PROTEUSIII_CMD_DELETE_BONDS_CNF, CMD_Status_Success, true);
}

bool ProteusIII_AllowUnbondedConnections()
{
    txPacket.Cmd = PROTEUSIII_CMD_ALLOWUNBONDEDCONNECTIONS_REQ;
    txPacket.Length = 0;

    FillChecksum(&txPacket);
    if (!ProteusIII_Transparent_Transmit((uint8_t*)&txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD))
    {
        return false;
    }

    /* wait for cnf */
    return Wait4CNF(CMD_WAIT_TIME, PROTEUSIII_CMD_ALLOWUNBONDEDCONNECTIONS_CNF, CMD_Status_Success, true);
}

bool ProteusIII_DTMEnable()
{
    txPacket.Cmd = PROTEUSIII_CMD_DTMSTART_REQ;
    txPacket.Length = 0;

    FillChecksum(&txPacket);
    if (!ProteusIII_Transparent_Transmit((uint8_t*)&txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD))
    {
        return false;
    }

    /* wait for cnf */
    if (Wait4CNF(CMD_WAIT_TIME, PROTEUSIII_CMD_GETSTATE_CNF, CMD_Status_NoStatus, true))
    {
        if ((rxPacket.Data[0] == (uint8_t)ProteusIII_BLE_Role_DTM) && (rxPacket.Data[1] == (uint8_t)ProteusIII_BLE_Action_DTM))
        {
            return true;
        }
    }
    return false;
}

bool ProteusIII_DTMRun(ProteusIII_DTMCommand_t command, uint8_t channel_vendoroption, uint8_t length_vendorcmd, uint8_t payload)
{
    txPacket.Cmd = PROTEUSIII_CMD_DTM_REQ;
    txPacket.Length = 4;
    txPacket.Data[0] = command;
    txPacket.Data[1] = channel_vendoroption;
    txPacket.Data[2] = length_vendorcmd;
    txPacket.Data[3] = payload;

    FillChecksum(&txPacket);
    if (!ProteusIII_Transparent_Transmit((uint8_t*)&txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD))
    {
        return false;
    }

    /* wait for cnf */
    return Wait4CNF(CMD_WAIT_TIME, PROTEUSIII_CMD_DTM_CNF, CMD_Status_Success, true);
}

bool ProteusIII_DTMStartTX(uint8_t channel, uint8_t length, ProteusIII_DTMTXPattern_t pattern) { return ProteusIII_DTMRun(ProteusIII_DTMCommand_StartTX, channel, length, (uint8_t)pattern); }

bool ProteusIII_DTMStartTXCarrier(uint8_t channel) { return ProteusIII_DTMRun(ProteusIII_DTMCommand_StartTX, channel, 0x00, 0x03); }

bool ProteusIII_DTMStop() { return ProteusIII_DTMRun(ProteusIII_DTMCommand_Stop, 0x00, 0x00, 0x01); }

bool ProteusIII_DTMSetPhy(ProteusIII_Phy_t phy) { return ProteusIII_DTMRun(ProteusIII_DTMCommand_Setup, 0x02, (uint8_t)phy, 0x00); }

bool ProteusIII_DTMSetTXPower(ProteusIII_TXPower_t power) { return ProteusIII_DTMRun(ProteusIII_DTMCommand_StartTX, (uint8_t)power, 0x02, 0x03); }
