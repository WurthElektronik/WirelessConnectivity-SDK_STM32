/*
 ***************************************************************************************************
 * This file is part of WIRELESS CONNECTIVITY SDK:
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
 * @brief Proteus-IV driver source file.
 */

#include <ProteusIV/ProteusIV.h>
#include <global/global.h>
#include <stdio.h>
#include <string.h>

ProteusIV_ConnectionList_t ProteusIV_connection_list[PROTEUSIV_MAX_NUMBER_OF_CONNECTEDDEVICES];

#define CMD_WAIT_TIME 500
#define CNFINVALID 255

#define LENGTH_CMD_OVERHEAD (uint16_t)5
#define LENGTH_CMD_OVERHEAD_WITHOUT_CRC (uint16_t)(LENGTH_CMD_OVERHEAD - 1)

typedef struct
{
    const uint8_t Stx;
    uint8_t Cmd;
    uint16_t Length;
    uint8_t Data[PROTEUSIV_MAX_CMD_PAYLOAD_LENGTH + 1]; /* +1 from CS */

} ProteusIV_CMD_Frame_t;

#define CMD_STX 0x02

#define PROTEUSIV_CMD_FLAG_REQ (uint8_t)(0 << 6)
#define PROTEUSIV_CMD_FLAG_CNF (uint8_t)(1 << 6)
#define PROTEUSIV_CMD_FLAG_IND (uint8_t)(2 << 6)
#define PROTEUSIV_CMD_FLAG_RSP (uint8_t)(3 << 6)

#define PROTEUSIV_CMD_RESET (uint8_t)0x00
#define PROTEUSIV_CMD_RESET_REQ (PROTEUSIV_CMD_RESET | PROTEUSIV_CMD_FLAG_REQ)
#define PROTEUSIV_CMD_RESET_CNF (PROTEUSIV_CMD_RESET | PROTEUSIV_CMD_FLAG_CNF)

#define PROTEUSIV_CMD_GETSTATE (uint8_t)0x01
#define PROTEUSIV_CMD_GETSTATE_REQ (PROTEUSIV_CMD_GETSTATE | PROTEUSIV_CMD_FLAG_REQ)
#define PROTEUSIV_CMD_GETSTATE_CNF (PROTEUSIV_CMD_GETSTATE | PROTEUSIV_CMD_FLAG_CNF)
#define PROTEUSIV_CMD_STARTUP_IND (PROTEUSIV_CMD_GETSTATE | PROTEUSIV_CMD_FLAG_IND)

#define PROTEUSIV_CMD_SLEEP (unsigned char)0x02
#define PROTEUSIV_CMD_SLEEP_REQ (PROTEUSIV_CMD_SLEEP | PROTEUSIV_CMD_FLAG_REQ)
#define PROTEUSIV_CMD_SLEEP_CNF (PROTEUSIV_CMD_SLEEP | PROTEUSIV_CMD_FLAG_CNF)

#define PROTEUSIV_CMD_DATA (unsigned char)0x04
#define PROTEUSIV_CMD_DATA_REQ (PROTEUSIV_CMD_DATA | PROTEUSIV_CMD_FLAG_REQ)
#define PROTEUSIV_CMD_DATA_CNF (PROTEUSIV_CMD_DATA | PROTEUSIV_CMD_FLAG_CNF)
#define PROTEUSIV_CMD_DATA_IND (PROTEUSIV_CMD_DATA | PROTEUSIV_CMD_FLAG_IND)
#define PROTEUSIV_CMD_TXCOMPLETE_RSP (PROTEUSIV_CMD_DATA | PROTEUSIV_CMD_FLAG_RSP)

#define PROTEUSIV_CMD_CONNECT (unsigned char)0x06
#define PROTEUSIV_CMD_CONNECT_REQ (PROTEUSIV_CMD_CONNECT | PROTEUSIV_CMD_FLAG_REQ)
#define PROTEUSIV_CMD_CONNECT_CNF (PROTEUSIV_CMD_CONNECT | PROTEUSIV_CMD_FLAG_CNF)
#define PROTEUSIV_CMD_CONNECT_IND (PROTEUSIV_CMD_CONNECT | PROTEUSIV_CMD_FLAG_IND)

#define PROTEUSIV_CMD_LINKOPEN_RSP (PROTEUSIV_CMD_CONNECT | PROTEUSIV_CMD_FLAG_RSP)
#define PROTEUSIV_CMD_LINKOPEN_RSP_PAYLOAD_CLOSED 0x00
#define PROTEUSIV_CMD_LINKOPEN_RSP_PAYLOAD_OPEN 0x01

#define PROTEUSIV_CMD_DISCONNECT (unsigned char)0x07
#define PROTEUSIV_CMD_DISCONNECT_REQ (PROTEUSIV_CMD_DISCONNECT | PROTEUSIV_CMD_FLAG_REQ)
#define PROTEUSIV_CMD_DISCONNECT_CNF (PROTEUSIV_CMD_DISCONNECT | PROTEUSIV_CMD_FLAG_CNF)
#define PROTEUSIV_CMD_DISCONNECT_IND (PROTEUSIV_CMD_DISCONNECT | PROTEUSIV_CMD_FLAG_IND)

#define PROTEUSIV_CMD_SECURITY (unsigned char)0x08
#define PROTEUSIV_CMD_SECURITY_IND (PROTEUSIV_CMD_SECURITY | PROTEUSIV_CMD_FLAG_IND)

#define PROTEUSIV_CMD_SCAN (unsigned char)0x09
#define PROTEUSIV_CMD_SCAN_REQ (PROTEUSIV_CMD_SCAN | PROTEUSIV_CMD_FLAG_REQ)
#define PROTEUSIV_CMD_SCAN_CNF (PROTEUSIV_CMD_SCAN | PROTEUSIV_CMD_FLAG_CNF)
#define PROTEUSIV_CMD_SCAN_IND (PROTEUSIV_CMD_SCAN | PROTEUSIV_CMD_FLAG_IND)

#define PROTEUSIV_CMD_PASSKEY (unsigned char)0x0D
#define PROTEUSIV_CMD_PASSKEY_REQ (PROTEUSIV_CMD_PASSKEY | PROTEUSIV_CMD_FLAG_REQ)
#define PROTEUSIV_CMD_PASSKEY_CNF (PROTEUSIV_CMD_PASSKEY | PROTEUSIV_CMD_FLAG_CNF)
#define PROTEUSIV_CMD_PASSKEY_IND (PROTEUSIV_CMD_PASSKEY | PROTEUSIV_CMD_FLAG_IND)

#define PROTEUSIV_CMD_DELETEBONDS (unsigned char)0x0E
#define PROTEUSIV_CMD_DELETEBONDS_REQ (PROTEUSIV_CMD_DELETEBONDS | PROTEUSIV_CMD_FLAG_REQ)
#define PROTEUSIV_CMD_DELETEBONDS_CNF (PROTEUSIV_CMD_DELETEBONDS | PROTEUSIV_CMD_FLAG_CNF)

#define PROTEUSIV_CMD_GETBONDS (unsigned char)0x0F
#define PROTEUSIV_CMD_GETBONDS_REQ (PROTEUSIV_CMD_GETBONDS | PROTEUSIV_CMD_FLAG_REQ)
#define PROTEUSIV_CMD_GETBONDS_CNF (PROTEUSIV_CMD_GETBONDS | PROTEUSIV_CMD_FLAG_CNF)

#define PROTEUSIV_CMD_GET (unsigned char)0x10
#define PROTEUSIV_CMD_GET_REQ (PROTEUSIV_CMD_GET | PROTEUSIV_CMD_FLAG_REQ)
#define PROTEUSIV_CMD_GET_CNF (PROTEUSIV_CMD_GET | PROTEUSIV_CMD_FLAG_CNF)

#define PROTEUSIV_CMD_SET (unsigned char)0x11
#define PROTEUSIV_CMD_SET_REQ (PROTEUSIV_CMD_SET | PROTEUSIV_CMD_FLAG_REQ)
#define PROTEUSIV_CMD_SET_CNF (PROTEUSIV_CMD_SET | PROTEUSIV_CMD_FLAG_CNF)

#define PROTEUSIV_CMD_ADVERTISING (unsigned char)0x12
#define PROTEUSIV_CMD_ADVERTISING_REQ (PROTEUSIV_CMD_ADVERTISING | PROTEUSIV_CMD_FLAG_REQ)
#define PROTEUSIV_CMD_ADVERTISING_CNF (PROTEUSIV_CMD_ADVERTISING | PROTEUSIV_CMD_FLAG_CNF)

#define PROTEUSIV_CMD_MAXPAYLOAD (unsigned char)0x13
#define PROTEUSIV_CMD_MAXPAYLOAD_IND (PROTEUSIV_CMD_MAXPAYLOAD | PROTEUSIV_CMD_FLAG_IND)

#define PROTEUSIV_CMD_GETRAM (unsigned char)0x20
#define PROTEUSIV_CMD_GETRAM_REQ (PROTEUSIV_CMD_GETRAM | PROTEUSIV_CMD_FLAG_REQ)
#define PROTEUSIV_CMD_GETRAM_CNF (PROTEUSIV_CMD_GETRAM | PROTEUSIV_CMD_FLAG_CNF)

#define PROTEUSIV_CMD_SETRAM (unsigned char)0x21
#define PROTEUSIV_CMD_SETRAM_REQ (PROTEUSIV_CMD_SETRAM | PROTEUSIV_CMD_FLAG_REQ)
#define PROTEUSIV_CMD_SETRAM_CNF (PROTEUSIV_CMD_SETRAM | PROTEUSIV_CMD_FLAG_CNF)

#define PROTEUSIV_CMD_NUMERIC_COMP (unsigned char)0x24
#define PROTEUSIV_CMD_NUMERIC_COMP_REQ (PROTEUSIV_CMD_NUMERIC_COMP | PROTEUSIV_CMD_FLAG_REQ)
#define PROTEUSIV_CMD_NUMERIC_COMP_CNF (PROTEUSIV_CMD_NUMERIC_COMP | PROTEUSIV_CMD_FLAG_CNF)
#define PROTEUSIV_CMD_DISPLAY_PASSKEY_IND (PROTEUSIV_CMD_NUMERIC_COMP | PROTEUSIV_CMD_FLAG_IND)

#define PROTEUSIV_CMD_PHYUPDATE (unsigned char)0x1A
#define PROTEUSIV_CMD_PHYUPDATE_REQ (PROTEUSIV_CMD_PHYUPDATE | PROTEUSIV_CMD_FLAG_REQ)
#define PROTEUSIV_CMD_PHYUPDATE_CNF (PROTEUSIV_CMD_PHYUPDATE | PROTEUSIV_CMD_FLAG_CNF)
#define PROTEUSIV_CMD_PHYUPDATE_IND (PROTEUSIV_CMD_PHYUPDATE | PROTEUSIV_CMD_FLAG_IND)

#define PROTEUSIV_CMD_UARTDISABLE (unsigned char)0x1B
#define PROTEUSIV_CMD_UARTDISABLE_REQ (PROTEUSIV_CMD_UARTDISABLE | PROTEUSIV_CMD_FLAG_REQ)
#define PROTEUSIV_CMD_UARTDISABLE_CNF (PROTEUSIV_CMD_UARTDISABLE | PROTEUSIV_CMD_FLAG_CNF)
#define PROTEUSIV_CMD_UARTENABLE_IND (PROTEUSIV_CMD_UARTDISABLE | PROTEUSIV_CMD_FLAG_IND)

#define PROTEUSIV_CMD_FACTORYRESET (unsigned char)0x1C
#define PROTEUSIV_CMD_FACTORYRESET_REQ (PROTEUSIV_CMD_FACTORYRESET | PROTEUSIV_CMD_FLAG_REQ)
#define PROTEUSIV_CMD_FACTORYRESET_CNF (PROTEUSIV_CMD_FACTORYRESET | PROTEUSIV_CMD_FLAG_CNF)

#define PROTEUSIV_CMD_CONNECTIONINFO (unsigned char)0x22
#define PROTEUSIV_CMD_CONNECTIONINFO_REQ (PROTEUSIV_CMD_CONNECTIONINFO | PROTEUSIV_CMD_FLAG_REQ)
#define PROTEUSIV_CMD_CONNECTIONINFO_CNF (PROTEUSIV_CMD_CONNECTIONINFO | PROTEUSIV_CMD_FLAG_CNF)

/**
 * @brief Type used to check the response, when a command was sent to the Proteus-IV
 */
typedef enum ProteusIV_CMD_Status_t
{
    CMD_Status_Success = (uint8_t)0x00,
    CMD_Status_Failed = (uint8_t)0x01,
    CMD_Status_Failed_2 = (uint8_t)0x02,
    CMD_Status_Failed_3 = (uint8_t)0x03,
    CMD_Status_Error,
    CMD_Status_Invalid,
    CMD_Status_Reset,
    CMD_Status_NoStatus,
    CMD_Status_NotPermitted = (uint8_t)0xFF,
} ProteusIV_CMD_Status_t;

/**
 * @brief Command confirmation.
 */
typedef struct
{
    uint8_t cmd;                   /**< Variable to check if correct CMD has been confirmed */
    ProteusIV_CMD_Status_t status; /**< Variable used to check the response (*_CNF), when a request (*_REQ) was sent to the Proteus-IV */
} ProteusIV_CMD_Confirmation_t;

/**************************************
 *          Static variables          *
 **************************************/
static ProteusIV_CMD_Frame_t txPacket = {.Stx = CMD_STX, .Length = 0}; /* request to be sent to the module */
static ProteusIV_CMD_Frame_t rxPacket = {.Stx = CMD_STX, .Length = 0}; /* received packet that has been sent by the module */

#define CMDCONFIRMATIONARRAY_LENGTH 10
static ProteusIV_CMD_Confirmation_t cmdConfirmationArray[CMDCONFIRMATIONARRAY_LENGTH];
static ProteusIV_OperationMode_t operationMode = ProteusIV_OperationMode_CommandMode;
static ProteusIV_ScanResult_t* ProteusIV_ScanDevicesP = NULL;
static ProteusIV_ConnectedDevices_t* ProteusIV_ConnectedDevicesP = NULL;
static ProteusIV_BondDatabase_t* ProteusIV_BondDatabaseP = NULL;

#define OPMODE_CHECK(valid_modes) (((uint8_t)(valid_modes) & (uint8_t)operationMode) == (uint8_t)operationMode)

/**
 * @brief Pin configuration struct pointer.
 */
static ProteusIV_Pins_t* ProteusIV_pinsP = NULL;

/**
 * @brief Uart configuration struct pointer.
 */
static WE_UART_t* ProteusIV_uartP = NULL;

static ProteusIV_CallbackConfig_t callbacks;
static WE_UART_HandleRxByte_t byteRxCallback = NULL;
static uint8_t checksum = 0;
static uint16_t rxByteCounter = 0;
static uint16_t bytesToReceive = 0;
static uint8_t rxBuffer[sizeof(ProteusIV_CMD_Frame_t)]; /* For UART RX from module */

/**************************************
 *         Static functions           *
 **************************************/

static void ResetConnectionList()
{
    for (int conn_ID = 0; conn_ID < PROTEUSIV_MAX_NUMBER_OF_CONNECTEDDEVICES; conn_ID++)
    {
        ProteusIV_connection_list[conn_ID].max_payload = 0;
        ProteusIV_connection_list[conn_ID].is_connected = false;
        ProteusIV_connection_list[conn_ID].is_linkopen = false;
    }
    return;
}

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
    ProteusIV_CMD_Confirmation_t cmdConfirmation;
    cmdConfirmation.cmd = CNFINVALID;
    cmdConfirmation.status = CMD_Status_Invalid;

    uint16_t cmdLength = ((ProteusIV_CMD_Frame_t*)prxBuffer)->Length;
    memcpy(&rxPacket, prxBuffer, cmdLength + LENGTH_CMD_OVERHEAD);

    switch (rxPacket.Cmd)
    {
        case PROTEUSIV_CMD_CONNECTIONINFO_CNF:
        {
            cmdConfirmation.cmd = rxPacket.Cmd;
            cmdConfirmation.status = rxPacket.Data[0];
            if ((cmdConfirmation.status == CMD_Status_Success) && (ProteusIV_ConnectedDevicesP != NULL))
            {
                uint8_t size = rxPacket.Data[1];
                if (size >= PROTEUSIV_MAX_NUMBER_OF_CONNECTEDDEVICES)
                {
                    size = PROTEUSIV_MAX_NUMBER_OF_CONNECTEDDEVICES;
                }
                ProteusIV_ConnectedDevicesP->numberOfDevices = size;

                uint16_t len = 2;
                for (uint8_t i = 0; i < ProteusIV_ConnectedDevicesP->numberOfDevices; i++)
                {
                    ProteusIV_ConnectedDevicesP->devices[i].conn_ID = rxPacket.Data[len];
                    ProteusIV_ConnectedDevicesP->devices[i].addr_type = rxPacket.Data[len + 1];
                    memcpy(&ProteusIV_ConnectedDevicesP->devices[i].btMac[0], &rxPacket.Data[len + 2], PROTEUSIV_BTMAC_LENGTH);
                    ProteusIV_ConnectedDevicesP->devices[i].is_linkopen = (rxPacket.Data[len + 8] == 0x01);
                    memcpy(&ProteusIV_ConnectedDevicesP->devices[i].max_payload, &rxPacket.Data[len + 9], 2);
                    memcpy(&ProteusIV_ConnectedDevicesP->devices[i].connection_interval, &rxPacket.Data[len + 11], 2);
                    ProteusIV_ConnectedDevicesP->devices[i].phy_rx = rxPacket.Data[len + 13];
                    ProteusIV_ConnectedDevicesP->devices[i].phy_tx = rxPacket.Data[len + 14];
                    len += 15;
                }
            }
            break;
        }
        case PROTEUSIV_CMD_GETBONDS_CNF:
        {
            cmdConfirmation.cmd = rxPacket.Cmd;
            cmdConfirmation.status = rxPacket.Data[0];
            if ((cmdConfirmation.status == CMD_Status_Success) && (ProteusIV_BondDatabaseP != NULL))
            {
                uint8_t size = rxPacket.Data[1];
                if (size >= PROTEUSIV_MAX_BOND_DEVICES)
                {
                    size = PROTEUSIV_MAX_BOND_DEVICES;
                }
                ProteusIV_BondDatabaseP->numberOfDevices = size;

                uint16_t len = 2;
                for (uint8_t i = 0; i < ProteusIV_BondDatabaseP->numberOfDevices; i++)
                {
                    ProteusIV_BondDatabaseP->devices[i].addr_type = rxPacket.Data[len];
                    memcpy(&ProteusIV_BondDatabaseP->devices[i].btMac[0], &rxPacket.Data[len + 1], PROTEUSIV_BTMAC_LENGTH);
                    len += (1 + PROTEUSIV_BTMAC_LENGTH);
                }
            }
            break;
        }

        case PROTEUSIV_CMD_RESET_CNF:
        case PROTEUSIV_CMD_SCAN_CNF:
        case PROTEUSIV_CMD_ADVERTISING_CNF:
        case PROTEUSIV_CMD_GET_CNF:
        case PROTEUSIV_CMD_SET_CNF:
        case PROTEUSIV_CMD_GETRAM_CNF:
        case PROTEUSIV_CMD_SETRAM_CNF:
        case PROTEUSIV_CMD_PASSKEY_CNF:
        case PROTEUSIV_CMD_PHYUPDATE_CNF:
        case PROTEUSIV_CMD_CONNECT_CNF:
        case PROTEUSIV_CMD_DATA_CNF:
        case PROTEUSIV_CMD_DISCONNECT_CNF:
        case PROTEUSIV_CMD_DELETEBONDS_CNF:
        case PROTEUSIV_CMD_FACTORYRESET_CNF:
        case PROTEUSIV_CMD_SLEEP_CNF:
        case PROTEUSIV_CMD_TXCOMPLETE_RSP:
        case PROTEUSIV_CMD_NUMERIC_COMP_CNF:
        case PROTEUSIV_CMD_GETSTATE_CNF:
        case PROTEUSIV_CMD_UARTDISABLE_CNF:
        case PROTEUSIV_CMD_UARTENABLE_IND:
        {
            cmdConfirmation.cmd = rxPacket.Cmd;
            cmdConfirmation.status = rxPacket.Data[0];
            break;
        }

        case PROTEUSIV_CMD_STARTUP_IND:
        {
            cmdConfirmation.cmd = rxPacket.Cmd;
            cmdConfirmation.status = CMD_Status_Success;

            ResetConnectionList();
            break;
        }

        case PROTEUSIV_CMD_LINKOPEN_RSP:
        {
            uint8_t conn_ID = rxPacket.Data[0];
            bool link_open = (rxPacket.Data[1] == 0x01);

            if (conn_ID < PROTEUSIV_MAX_NUMBER_OF_CONNECTEDDEVICES)
            {
                ProteusIV_connection_list[conn_ID].is_linkopen = link_open;
            }

            /* Payload of LINKOPEN_RSP: conn_ID, enabled/disabled */
            if (callbacks.linkOpenCb != NULL)
            {
                callbacks.linkOpenCb(conn_ID, link_open);
            }
            break;
        }

        case PROTEUSIV_CMD_CONNECT_IND:
        {
            uint8_t conn_ID = rxPacket.Data[0];

            if ((conn_ID < PROTEUSIV_MAX_NUMBER_OF_CONNECTEDDEVICES) && (conn_ID != PROTEUSIV_MULTICONN_INVALID_ID))
            {
                ProteusIV_connection_list[conn_ID].is_connected = true;
            }

            if (callbacks.connectCb != NULL)
            {
                callbacks.connectCb(conn_ID, rxPacket.Data[1] == 0x00, rxPacket.Data[2], &rxPacket.Data[3]);
            }
            break;
        }

        case PROTEUSIV_CMD_MAXPAYLOAD_IND:
        {
            uint8_t conn_ID = rxPacket.Data[0];

            uint16_t max_payload = rxPacket.Data[2] << 8;
            max_payload |= rxPacket.Data[1];

            if (conn_ID < PROTEUSIV_MAX_NUMBER_OF_CONNECTEDDEVICES)
            {
                ProteusIV_connection_list[conn_ID].max_payload = max_payload;
            }

            if (callbacks.maxPayloadCb != NULL)
            {
                callbacks.maxPayloadCb(conn_ID, max_payload);
            }
            break;
        }

        case PROTEUSIV_CMD_DISCONNECT_IND:
        {
            uint8_t conn_ID = rxPacket.Data[0];

            if (conn_ID < PROTEUSIV_MAX_NUMBER_OF_CONNECTEDDEVICES)
            {
                ProteusIV_connection_list[conn_ID].is_connected = false;
                ProteusIV_connection_list[conn_ID].is_linkopen = false;
                ProteusIV_connection_list[conn_ID].max_payload = 0;
            }

            if (callbacks.disconnectCb != NULL)
            {
                ProteusIV_DisconnectReason_t reason = ProteusIV_DisconnectReason_Unknown;
                switch (rxPacket.Data[1])
                {
                    case 0x08:
                        reason = ProteusIV_DisconnectReason_ConnectionTimeout;
                        break;

                    case 0x13:
                        reason = ProteusIV_DisconnectReason_UserTerminatedConnection;
                        break;

                    case 0x16:
                        reason = ProteusIV_DisconnectReason_HostTerminatedConnection;
                        break;

                    case 0x3B:
                        reason = ProteusIV_DisconnectReason_ConnectionIntervalUnacceptable;
                        break;

                    case 0x3D:
                        reason = ProteusIV_DisconnectReason_MicFailure;
                        break;

                    case 0x3E:
                        reason = ProteusIV_DisconnectReason_ConnectionSetupFailed;
                        break;
                    default:
                        break;
                }
                callbacks.disconnectCb(conn_ID, reason);
            }
            break;
        }

        case PROTEUSIV_CMD_DATA_IND:
        {
            if (callbacks.rxCb != NULL)
            {
                callbacks.rxCb(rxPacket.Data[0], &rxPacket.Data[2], rxPacket.Length - 2, rxPacket.Data[1]);
            }
            break;
        }

        case PROTEUSIV_CMD_SECURITY_IND:
        {
            if (callbacks.securityCb != NULL)
            {
                ProteusIV_SecurityState_t sec;
                sec.success = rxPacket.Data[1];
                sec.level = rxPacket.Data[2];
                callbacks.securityCb(rxPacket.Data[0], sec);
            }
            break;
        }

        case PROTEUSIV_CMD_PASSKEY_IND:
        {
            if (callbacks.passkeyCb != NULL)
            {
                callbacks.passkeyCb(rxPacket.Data[0]);
            }
            break;
        }

        case PROTEUSIV_CMD_DISPLAY_PASSKEY_IND:
        {
            if (callbacks.displayPasskeyCb != NULL)
            {
                callbacks.displayPasskeyCb(rxPacket.Data[0], (ProteusIV_DisplayPasskeyAction_t)rxPacket.Data[1], &rxPacket.Data[2]);
            }
            break;
        }

        case PROTEUSIV_CMD_PHYUPDATE_IND:
        {
            if (callbacks.phyUpdateCb != NULL)
            {
                callbacks.phyUpdateCb(rxPacket.Data[0], (ProteusIV_Phy_t)rxPacket.Data[1], (ProteusIV_Phy_t)rxPacket.Data[2]);
            }
            break;
        }

        case PROTEUSIV_CMD_SCAN_IND:
        {
            uint8_t* typeP = &rxPacket.Data[0];
            uint8_t* MacP = &rxPacket.Data[1];
            int8_t* RssiP = (int8_t*)&rxPacket.Data[7];
            uint8_t* NameLenP = &rxPacket.Data[8];
            char* NameP = (char*)&rxPacket.Data[9];

            if (*NameLenP > PROTEUSIV_DEVICENAME_LENGTH)
            {
                /* error */
                *NameLenP = PROTEUSIV_DEVICENAME_LENGTH;
            }
            *(NameP + *NameLenP) = '\0';

            if (ProteusIV_ScanDevicesP != NULL)
            {
                bool updated_entry = false;
                for (int i = 0; i < PROTEUSIV_MAX_NUMBER_OF_SCANDEVICES; i++)
                {
                    if (ProteusIV_ScanDevicesP->devices[i].inUse && (0 == memcmp(ProteusIV_ScanDevicesP->devices[i].btMac, MacP, PROTEUSIV_BTMAC_LENGTH)))
                    {
                        /* update entry */
                        ProteusIV_ScanDevicesP->devices[i].rssi = *RssiP;
                        updated_entry = true;
                        break;
                    }
                }
                if (!updated_entry)
                {
                    for (int i = 0; i < PROTEUSIV_MAX_NUMBER_OF_SCANDEVICES; i++)
                    {
                        if (false == ProteusIV_ScanDevicesP->devices[i].inUse)
                        {
                            /* add new entry */
                            ProteusIV_ScanDevicesP->devices[i].inUse = true;
                            ProteusIV_ScanDevicesP->devices[i].rssi = *RssiP;
                            ProteusIV_ScanDevicesP->devices[i].addr_type = *typeP;
                            memcpy(ProteusIV_ScanDevicesP->devices[i].btMac, MacP, PROTEUSIV_BTMAC_LENGTH);
                            memcpy(ProteusIV_ScanDevicesP->devices[i].deviceName, NameP, *NameLenP + 1);
                            break;
                        }
                    }
                }
            }

            if (callbacks.scanCb != NULL)
            {
                callbacks.scanCb(*typeP, MacP, *RssiP, NameP);
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

static void ProteusIV_HandleRxByte(uint8_t* dataP, size_t size)
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
 * @brief Function that waits for the return value of Proteus-IV (*_CNF),
 * when a command (*_REQ) was sent before.
 */
static ProteusIV_CMD_Status_t Wait4CNF(uint32_t maxTimeMs, uint8_t expectedCmdConfirmation, bool resetConfirmState)
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
                return cmdConfirmationArray[i].status;
            }
        }

        if (count >= maxCount)
        {
            /* received no correct response within timeout */
            return CMD_Status_Invalid;
        }

        /* wait */
        count++;
        WE_Delay(timeStepMs);
    }

    /* we should never come to this place */
    return CMD_Status_Invalid;
}

/**
 * @brief Function to add the checksum at the end of the data packet.
 */
static bool FillChecksum(ProteusIV_CMD_Frame_t* cmd)
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

bool ProteusIV_Internal_Transmit(const uint8_t* data, uint16_t dataLength)
{
    if ((data == NULL) || (dataLength == 0))
    {
        return false;
    }
    return ProteusIV_uartP->uartTransmit((uint8_t*)data, dataLength);
}

/**************************************
 *         Global functions           *
 **************************************/

bool ProteusIV_Transparent_Transmit(const uint8_t* payloadP, uint16_t length, uint16_t ETX)
{
    if (!OPMODE_CHECK(ProteusIV_OperationMode_TransparentMode) || (payloadP == NULL) || (length == 0))
    {
        return false;
    }

    if (!ProteusIV_Internal_Transmit(payloadP, length))
    {
        return false;
    }

    uint8_t etx[2];
    etx[0] = (uint8_t)((0xFF00 & ETX) >> 8);
    etx[1] = (uint8_t)(0x00FF & ETX);
    if (!ProteusIV_Internal_Transmit(etx, sizeof(etx)))
    {
        return false;
    }

    /* add 2 ms delay to trigger UART interpretation */
    WE_Delay(2);

    return true;
}

bool ProteusIV_Init(WE_UART_t* uartP, ProteusIV_Pins_t* pinoutP, ProteusIV_OperationMode_t opMode, ProteusIV_CallbackConfig_t callbackConfig)
{
    operationMode = opMode;

    if ((pinoutP == NULL) || (uartP == NULL) || (uartP->uartInit == NULL) || (uartP->uartDeinit == NULL) || (uartP->uartTransmit == NULL))
    {
        return false;
    }

    ProteusIV_pinsP = pinoutP;
    ProteusIV_pinsP->ProteusIV_Pin_Reset.type = WE_Pin_Type_Output;
    ProteusIV_pinsP->ProteusIV_Pin_Reset.initial_value.output = WE_Pin_Level_High;
    ProteusIV_pinsP->ProteusIV_Pin_Led0.type = WE_Pin_Type_Input;
    ProteusIV_pinsP->ProteusIV_Pin_Led0.initial_value.input_pull = WE_Pin_PullType_No;
    ProteusIV_pinsP->ProteusIV_Pin_Led1.type = WE_Pin_Type_Input;
    ProteusIV_pinsP->ProteusIV_Pin_Led1.initial_value.input_pull = WE_Pin_PullType_No;
    ProteusIV_pinsP->ProteusIV_UART_Enable.type = WE_Pin_Type_Output;
    ProteusIV_pinsP->ProteusIV_UART_Enable.initial_value.output = WE_Pin_Level_High;

    ProteusIV_pinsP->ProteusIV_Pin_Mode0.type = WE_Pin_Type_Output;
    ProteusIV_pinsP->ProteusIV_Pin_Mode1.type = WE_Pin_Type_Output;

    switch (opMode)
    {
        case ProteusIV_OperationMode_CommandMode:
        {
            ProteusIV_pinsP->ProteusIV_Pin_Mode0.initial_value.output = WE_Pin_Level_Low;
            ProteusIV_pinsP->ProteusIV_Pin_Mode1.initial_value.output = WE_Pin_Level_Low;
            break;
        }
        case ProteusIV_OperationMode_TransparentMode:
        {
            ProteusIV_pinsP->ProteusIV_Pin_Mode0.initial_value.output = WE_Pin_Level_Low;
            ProteusIV_pinsP->ProteusIV_Pin_Mode1.initial_value.output = WE_Pin_Level_High;
            break;
        }
        case ProteusIV_OperationMode_ConfigMode:
        {
            ProteusIV_pinsP->ProteusIV_Pin_Mode0.initial_value.output = WE_Pin_Level_High;
            ProteusIV_pinsP->ProteusIV_Pin_Mode1.initial_value.output = WE_Pin_Level_Low;
            break;
        }
        case ProteusIV_OperationMode_BootMode:
        {
            ProteusIV_pinsP->ProteusIV_Pin_Mode0.initial_value.output = WE_Pin_Level_High;
            ProteusIV_pinsP->ProteusIV_Pin_Mode1.initial_value.output = WE_Pin_Level_High;
            break;
        }
        default:
        {
            return false;
        }
    }

    WE_Pin_t pins[sizeof(ProteusIV_Pins_t) / sizeof(WE_Pin_t)];
    uint8_t pin_count = 0;
    memcpy(&pins[pin_count++], &ProteusIV_pinsP->ProteusIV_Pin_Reset, sizeof(WE_Pin_t));
    memcpy(&pins[pin_count++], &ProteusIV_pinsP->ProteusIV_Pin_Mode0, sizeof(WE_Pin_t));
    memcpy(&pins[pin_count++], &ProteusIV_pinsP->ProteusIV_Pin_Mode1, sizeof(WE_Pin_t));
    memcpy(&pins[pin_count++], &ProteusIV_pinsP->ProteusIV_Pin_Led0, sizeof(WE_Pin_t));
    memcpy(&pins[pin_count++], &ProteusIV_pinsP->ProteusIV_Pin_Led1, sizeof(WE_Pin_t));
    memcpy(&pins[pin_count++], &ProteusIV_pinsP->ProteusIV_UART_Enable, sizeof(WE_Pin_t));

    if (!WE_InitPins(pins, pin_count))
    {
        /* error */
        return false;
    }

    /* set callback functions */
    callbacks = callbackConfig;
    byteRxCallback = (opMode == ProteusIV_OperationMode_TransparentMode) ? callbacks.transparentRxCb : ProteusIV_HandleRxByte;
    if (NULL == byteRxCallback)
    {
        return false;
    }

    ProteusIV_uartP = uartP;
    if (false == ProteusIV_uartP->uartInit(ProteusIV_uartP->baudrate, ProteusIV_uartP->flowControl, ProteusIV_uartP->parity, &byteRxCallback))
    {
        return false;
    }
    WE_Delay(10);

    ResetConnectionList();

    /* reset module */
    if (IS_WE_PIN_UNDEFINED(ProteusIV_pinsP->ProteusIV_Pin_Reset) && (opMode == ProteusIV_OperationMode_TransparentMode))
    {
        WE_DEBUG_PRINT_INFO("Reset pin needed to reset\r\n");
        ProteusIV_Deinit();
        return false;
    }
    else if (!(IS_WE_PIN_UNDEFINED(ProteusIV_pinsP->ProteusIV_Pin_Reset) ? ProteusIV_Reset() : ProteusIV_PinReset()))
    {
        WE_DEBUG_PRINT_INFO("Reset failed\r\n");
        ProteusIV_Deinit();
        return false;
    }

    ProteusIV_ScanDevicesP = NULL;
    ProteusIV_ConnectedDevicesP = NULL;
    ProteusIV_BondDatabaseP = NULL;

    return true;
}

bool ProteusIV_Deinit()
{
    WE_Pin_t pins[sizeof(ProteusIV_Pins_t) / sizeof(WE_Pin_t)];
    uint8_t pin_count = 0;
    memcpy(&pins[pin_count++], &ProteusIV_pinsP->ProteusIV_Pin_Reset, sizeof(WE_Pin_t));
    memcpy(&pins[pin_count++], &ProteusIV_pinsP->ProteusIV_Pin_Mode0, sizeof(WE_Pin_t));
    memcpy(&pins[pin_count++], &ProteusIV_pinsP->ProteusIV_Pin_Mode1, sizeof(WE_Pin_t));
    memcpy(&pins[pin_count++], &ProteusIV_pinsP->ProteusIV_Pin_Led0, sizeof(WE_Pin_t));
    memcpy(&pins[pin_count++], &ProteusIV_pinsP->ProteusIV_Pin_Led1, sizeof(WE_Pin_t));
    memcpy(&pins[pin_count++], &ProteusIV_pinsP->ProteusIV_UART_Enable, sizeof(WE_Pin_t));

    /* deinit pins */
    if (!WE_DeinitPins(pins, pin_count))
    {
        return false;
    }

    /* reset callbacks */
    memset(&callbacks, 0, sizeof(callbacks));

    /* make sure any bytes remaining in receive buffer are discarded */
    ClearReceiveBuffers();

    return ProteusIV_uartP->uartDeinit();
}

bool ProteusIV_PinReset()
{
    /* set to output mode */
    if (!WE_SetPin(ProteusIV_pinsP->ProteusIV_Pin_Reset, WE_Pin_Level_Low))
    {
        return false;
    }
    WE_Delay(5);
    /* make sure any bytes remaining in receive buffer are discarded */
    ClearReceiveBuffers();
    if (!WE_SetPin(ProteusIV_pinsP->ProteusIV_Pin_Reset, WE_Pin_Level_High))
    {
        return false;
    }

    if (operationMode == ProteusIV_OperationMode_TransparentMode)
    {
        WE_Delay(PROTEUSIV_BOOT_DURATION);
        /* peripheral only mode is ready (the module doesn't send a "ready for operation" message in peripheral only mode) */
        return true;
    }

    return CMD_Status_Success == Wait4CNF(CMD_WAIT_TIME, PROTEUSIV_CMD_STARTUP_IND, true);
}

bool ProteusIV_Reset()
{
    if (!OPMODE_CHECK(ProteusIV_OperationMode_ConfigMode | ProteusIV_OperationMode_CommandMode | ProteusIV_OperationMode_BootMode))
    {
        return false;
    }

    txPacket.Cmd = PROTEUSIV_CMD_RESET_REQ;
    txPacket.Length = 0;

    FillChecksum(&txPacket);
    if (!ProteusIV_Internal_Transmit((uint8_t*)&txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD))
    {
        return false;
    }

    if (CMD_Status_Success != Wait4CNF(CMD_WAIT_TIME, PROTEUSIV_CMD_RESET_CNF, true))
    {
        return false;
    }

    return CMD_Status_Success == Wait4CNF(PROTEUSIV_BOOT_DURATION, PROTEUSIV_CMD_STARTUP_IND, false);
}

bool ProteusIV_UartPinEnable()
{
    if (!OPMODE_CHECK(ProteusIV_OperationMode_CommandMode))
    {
        return false;
    }

    /* make sure any bytes remaining in receive buffer are discarded */
    ClearReceiveBuffers();

    /* set to output mode */
    if (!WE_SetPin(ProteusIV_pinsP->ProteusIV_UART_Enable, WE_Pin_Level_Low))
    {
        return false;
    }
    WE_Delay(1);
    if (!WE_SetPin(ProteusIV_pinsP->ProteusIV_UART_Enable, WE_Pin_Level_High))
    {
        return false;
    }

    return CMD_Status_Success == Wait4CNF(CMD_WAIT_TIME, PROTEUSIV_CMD_UARTENABLE_IND, false);
}

bool ProteusIV_Disconnect(uint8_t conn_ID)
{
    if (!OPMODE_CHECK(ProteusIV_OperationMode_CommandMode | ProteusIV_OperationMode_BootMode))
    {
        return false;
    }

    txPacket.Cmd = PROTEUSIV_CMD_DISCONNECT_REQ;
    txPacket.Length = 1;
    txPacket.Data[0] = conn_ID;

    FillChecksum(&txPacket);
    if (!ProteusIV_Internal_Transmit((uint8_t*)&txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD))
    {
        return false;
    }

    /* Confirmation is sent before performing the disconnect. After disconnect, the module sends a disconnect indication */
    return CMD_Status_Success == Wait4CNF(CMD_WAIT_TIME, PROTEUSIV_CMD_DISCONNECT_CNF, true);
}

bool ProteusIV_Sleep()
{
    if (!OPMODE_CHECK(ProteusIV_OperationMode_CommandMode))
    {
        return false;
    }

    txPacket.Cmd = PROTEUSIV_CMD_SLEEP_REQ;
    txPacket.Length = 0;

    FillChecksum(&txPacket);
    if (!ProteusIV_Internal_Transmit((uint8_t*)&txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD))
    {
        return false;
    }

    return CMD_Status_Success == Wait4CNF(CMD_WAIT_TIME, PROTEUSIV_CMD_SLEEP_CNF, true);
}

bool ProteusIV_UartDisable()
{

    if (!OPMODE_CHECK(ProteusIV_OperationMode_CommandMode))
    {
        return false;
    }

    txPacket.Cmd = PROTEUSIV_CMD_UARTDISABLE_REQ;
    txPacket.Length = 0;

    FillChecksum(&txPacket);
    if (!ProteusIV_Internal_Transmit((uint8_t*)&txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD))
    {
        return false;
    }

    return CMD_Status_Success == Wait4CNF(CMD_WAIT_TIME, PROTEUSIV_CMD_UARTDISABLE_CNF, true);
}

bool ProteusIV_Transmit(uint8_t conn_ID, uint8_t* payloadP, uint16_t length)
{
    if (!OPMODE_CHECK(ProteusIV_OperationMode_CommandMode))
    {
        return false;
    }

    if ((payloadP == NULL) || (length == 0) || (length > PROTEUSIV_MAX_RADIO_PAYLOAD_LENGTH))
    {
        return false;
    }

    txPacket.Cmd = PROTEUSIV_CMD_DATA_REQ;
    txPacket.Length = length + 1;
    txPacket.Data[0] = conn_ID;
    memcpy(&txPacket.Data[1], payloadP, length);

    FillChecksum(&txPacket);
    if (!ProteusIV_Internal_Transmit((uint8_t*)&txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD))
    {
        return false;
    }

    return CMD_Status_Success == Wait4CNF(CMD_WAIT_TIME, PROTEUSIV_CMD_DATA_CNF, true);
}

bool ProteusIV_FactoryReset()
{
    if (!OPMODE_CHECK(ProteusIV_OperationMode_CommandMode | ProteusIV_OperationMode_ConfigMode))
    {
        return false;
    }

    txPacket.Cmd = PROTEUSIV_CMD_FACTORYRESET_REQ;
    txPacket.Length = 0;

    FillChecksum(&txPacket);
    if (!ProteusIV_Internal_Transmit((uint8_t*)&txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD))
    {
        return false;
    }

    if (CMD_Status_Success != Wait4CNF(CMD_WAIT_TIME, PROTEUSIV_CMD_FACTORYRESET_CNF, true))
    {
        return false;
    }

    return CMD_Status_Success == Wait4CNF(CMD_WAIT_TIME, PROTEUSIV_CMD_STARTUP_IND, false);
}

bool ProteusIV_Set(ProteusIV_UserSettings_t userSetting, uint8_t* valueP, uint8_t length)
{
    if (!OPMODE_CHECK(ProteusIV_OperationMode_CommandMode | ProteusIV_OperationMode_ConfigMode))
    {
        return false;
    }

    if ((valueP == NULL) || (length == 0))
    {
        return false;
    }

    txPacket.Cmd = PROTEUSIV_CMD_SET_REQ;
    txPacket.Length = 1 + length;
    txPacket.Data[0] = userSetting;
    memcpy(&txPacket.Data[1], valueP, length);

    FillChecksum(&txPacket);
    if (!ProteusIV_Internal_Transmit((uint8_t*)&txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD))
    {
        return false;
    }

    switch (Wait4CNF(CMD_WAIT_TIME, PROTEUSIV_CMD_SET_CNF, true))
    {
        case CMD_Status_Success:
        {
            /* wait for startup */
            if ((operationMode == ProteusIV_OperationMode_ConfigMode) || (userSetting != ProteusIV_USERSETTING_UART_CONFIGINDEX))
            {
                return CMD_Status_Success == Wait4CNF(CMD_WAIT_TIME, PROTEUSIV_CMD_STARTUP_IND, false);
            }
            else
            {
                /* Since this will cause a different UART configuration
                 * to be applied we can't wait for the start up indication */
                WE_Delay(PROTEUSIV_BOOT_DURATION);
                return true;
            }
        }
        case CMD_Status_Failed_3:
        {
            /* value is already set */
            return true;
        }
        case CMD_Status_Failed:
        default:
        {
            return false;
        }
    }
}

bool ProteusIV_SetDeviceName(uint8_t* deviceNameP, uint8_t nameLength) { return ProteusIV_Set(ProteusIV_USERSETTING_RF_DEVICENAME, deviceNameP, nameLength); }

bool ProteusIV_SetCFGFlags(uint16_t cfgFlags)
{
    uint8_t help[2];
    memcpy(help, (uint8_t*)&cfgFlags, 2);
    return ProteusIV_Set(ProteusIV_USERSETTING_CFG_FLAGS, help, 2);
}

bool ProteusIV_SetTXPower(ProteusIV_TXPower_t txPower) { return ProteusIV_Set(ProteusIV_USERSETTING_RF_TXPOWER, (uint8_t*)&txPower, 1); }

bool ProteusIV_SetAdvertisingInterval(uint16_t min, uint16_t max)
{
    uint8_t help[4];
    memcpy(&help[0], (uint8_t*)&min, 2);
    memcpy(&help[2], (uint8_t*)&max, 2);
    return ProteusIV_Set(ProteusIV_USERSETTING_RF_ADVERTISINGINTERVAL, help, 4);
}

bool ProteusIV_SetConnectionInterval(uint16_t min, uint16_t max)
{
    uint8_t help[4];
    memcpy(&help[0], (uint8_t*)&min, 2);
    memcpy(&help[2], (uint8_t*)&max, 2);
    return ProteusIV_Set(ProteusIV_USERSETTING_RF_CONNECTIONINTERVAL, help, 4);
}

bool ProteusIV_SetETXConfig(uint8_t etxconfig) { return ProteusIV_Set(ProteusIV_USERSETTING_UART_TRANSP_ETX_CFG, &etxconfig, 1); }

bool ProteusIV_SetReceiveETX(uint16_t etx) { return ProteusIV_Set(ProteusIV_USERSETTING_UART_RECEIVE_TRANSP_ETX, (uint8_t*)&etx, 2); }

bool ProteusIV_SetTransmitETX(uint16_t etx) { return ProteusIV_Set(ProteusIV_USERSETTING_UART_TRANSMIT_TRANSP_ETX, (uint8_t*)&etx, 2); }

bool ProteusIV_SetSecFlags(ProteusIV_SecFlags_t secFlags) { return ProteusIV_Set(ProteusIV_USERSETTING_RF_SECFLAGS, (uint8_t*)&secFlags, 1); }

bool ProteusIV_SetMaxPeripheralConnections(uint8_t max) { return ProteusIV_Set(ProteusIV_USERSETTING_RF_MAXPERIPHERALCONNECTIONS, (uint8_t*)&max, 1); }

bool ProteusIV_SetBaudrateIndex(ProteusIV_BaudRate_t baudrate, ProteusIV_UartParity_t parity, bool flowControlEnable)
{
    uint8_t baudrateIndex = (uint8_t)baudrate;

    /* If flow control is to be enabled UART index has to be increased by one in regard to base value */
    if (flowControlEnable)
    {
        baudrateIndex++;
    }

    /* If parity bit is even, UART index has to be increased by 64 in regard of base value*/
    if (ProteusIV_UartParity_Even == parity)
    {
        baudrateIndex += 64;
    }

    return ProteusIV_Set(ProteusIV_USERSETTING_UART_CONFIGINDEX, (uint8_t*)&baudrateIndex, 1);
}

bool ProteusIV_SetBTMAC(uint8_t* btMacP) { return ProteusIV_Set(ProteusIV_USERSETTING_FS_BTMAC, btMacP, PROTEUSIV_BTMAC_LENGTH); }

bool ProteusIV_SetStaticPasskey(uint8_t* staticPasskeyP) { return ProteusIV_Set(ProteusIV_USERSETTING_RF_STATICPASSKEY, staticPasskeyP, PROTEUSIV_PASSKEY_LENGTH); }

bool ProteusIV_SetAppearance(uint16_t appearance)
{
    uint8_t help[2];
    memcpy(help, (uint8_t*)&appearance, 2);
    return ProteusIV_Set(ProteusIV_USERSETTING_RF_APPEARANCE, help, 2);
}

bool ProteusIV_SetSppServiceUuid(uint8_t* uuidP) { return ProteusIV_Set(ProteusIV_USERSETTING_RF_SPPSERVICEUUID, uuidP, PROTEUSIV_UUID_LENGTH); }

bool ProteusIV_SetSppRxUuid(uint8_t* uuidP) { return ProteusIV_Set(ProteusIV_USERSETTING_RF_SPPRXUUID, uuidP, PROTEUSIV_UUID_LENGTH); }

bool ProteusIV_SetSppTxUuid(uint8_t* uuidP) { return ProteusIV_Set(ProteusIV_USERSETTING_RF_SPPTXUUID, uuidP, PROTEUSIV_UUID_LENGTH); }

bool ProteusIV_SetAdvertisingData(uint8_t* dataP, uint16_t length) { return ProteusIV_Set(ProteusIV_USERSETTING_RF_ADVERTISINGDATA, dataP, length); }

bool ProteusIV_SetScanResponseData(uint8_t* dataP, uint16_t length) { return ProteusIV_Set(ProteusIV_USERSETTING_RF_SCANRESPONSEDATA, dataP, length); }

bool ProteusIV_Get(ProteusIV_UserSettings_t userSetting, uint8_t* responseP, uint16_t* responseLengthP)
{
    if (!OPMODE_CHECK(ProteusIV_OperationMode_CommandMode | ProteusIV_OperationMode_ConfigMode | ProteusIV_OperationMode_BootMode) || (responseP == NULL) || (responseLengthP == NULL))
    {
        return false;
    }

    txPacket.Cmd = PROTEUSIV_CMD_GET_REQ;
    txPacket.Length = 1;
    txPacket.Data[0] = userSetting;

    FillChecksum(&txPacket);
    if (!ProteusIV_Internal_Transmit((uint8_t*)&txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD))
    {
        return false;
    }

    if (CMD_Status_Success == Wait4CNF(CMD_WAIT_TIME, PROTEUSIV_CMD_GET_CNF, true))
    {
        uint16_t length = rxPacket.Length;

        if (*responseLengthP < length - 1)
        {
            /* provided buffer is too small */
            *responseLengthP = length - 1;
            return false;
        }

        *responseLengthP = length - 1;
        memcpy(responseP, &rxPacket.Data[1], *responseLengthP); /* First Data byte is status, following bytes response */
        return true;
    }

    return false;
}

bool ProteusIV_GetFWVersion(uint8_t* versionP)
{
    uint16_t length = 3;
    return ProteusIV_Get(ProteusIV_USERSETTING_FS_FWVERSION, versionP, &length);
}

bool ProteusIV_GetDeviceInfo(ProteusIV_DeviceInfo_t* deviceInfoP)
{
    if (deviceInfoP == NULL)
    {
        return false;
    }

    uint8_t help[13];
    uint16_t length = sizeof(help);
    if (!ProteusIV_Get(ProteusIV_USERSETTING_FS_DEVICEINFO, help, &length))
    {
        return false;
    }
    deviceInfoP->osVersion = 0;
    memcpy(&deviceInfoP->osVersion, help, 3);
    memcpy(&deviceInfoP->buildCode, help + 3, 4);
    memcpy(&deviceInfoP->packageVariant, help + 7, 2);
    memcpy(&deviceInfoP->chipId, help + 9, 4);
    return true;
}

bool ProteusIV_GetSerialNumber(uint8_t* serialNumberP)
{
    uint16_t length = 3;
    if (!ProteusIV_Get(ProteusIV_USERSETTING_FS_SERIALNUMBER, serialNumberP, &length))
    {
        return false;
    }
    return true;
}

bool ProteusIV_GetDeviceName(uint8_t* deviceNameP, uint16_t* nameLengthP) { return ProteusIV_Get(ProteusIV_USERSETTING_RF_DEVICENAME, deviceNameP, nameLengthP); }

bool ProteusIV_GetMAC(uint8_t* macP)
{
    uint16_t length = 8;
    return ProteusIV_Get(ProteusIV_USERSETTING_FS_MAC, macP, &length);
}

bool ProteusIV_GetBTMAC(uint8_t* btMacP)
{
    uint16_t length = PROTEUSIV_BTMAC_LENGTH;
    return ProteusIV_Get(ProteusIV_USERSETTING_FS_BTMAC, btMacP, &length);
}

bool ProteusIV_GetTXPower(ProteusIV_TXPower_t* txPowerP)
{
    uint16_t length = 1;
    return ProteusIV_Get(ProteusIV_USERSETTING_RF_TXPOWER, (uint8_t*)txPowerP, &length);
}

bool ProteusIV_GetAdvertisingInterval(uint16_t* minP, uint16_t* maxP)
{
    if ((minP == NULL) || (maxP == NULL))
    {
        return false;
    }

    uint8_t help[4];
    uint16_t length = sizeof(help);
    if (true == ProteusIV_Get(ProteusIV_USERSETTING_RF_ADVERTISINGINTERVAL, help, &length))
    {
        memcpy((uint8_t*)minP, &help[0], 2);
        memcpy((uint8_t*)maxP, &help[2], 2);
        return true;
    }
    return false;
}

bool ProteusIV_GetConnectionInterval(uint16_t* minP, uint16_t* maxP)
{
    if ((minP == NULL) || (maxP == NULL))
    {
        return false;
    }

    uint8_t help[4];
    uint16_t length = sizeof(help);
    if (true == ProteusIV_Get(ProteusIV_USERSETTING_RF_CONNECTIONINTERVAL, help, &length))
    {
        memcpy((uint8_t*)minP, &help[0], 2);
        memcpy((uint8_t*)maxP, &help[2], 2);
        return true;
    }
    return false;
}

bool ProteusIV_GetETXConfig(uint8_t* etxconfigP)
{
    uint16_t length = 1;
    return ProteusIV_Get(ProteusIV_USERSETTING_UART_TRANSP_ETX_CFG, etxconfigP, &length);
}

bool ProteusIV_GetReceiveETX(uint16_t* etxP)
{
    uint16_t length = 2;
    return ProteusIV_Get(ProteusIV_USERSETTING_UART_RECEIVE_TRANSP_ETX, (uint8_t*)etxP, &length);
}

bool ProteusIV_GetTransmitETX(uint16_t* etxP)
{
    uint16_t length = 2;
    return ProteusIV_Get(ProteusIV_USERSETTING_UART_TRANSMIT_TRANSP_ETX, (uint8_t*)etxP, &length);
}

bool ProteusIV_GetSecFlags(ProteusIV_SecFlags_t* secFlagsP)
{
    uint16_t length = 1;
    return ProteusIV_Get(ProteusIV_USERSETTING_RF_SECFLAGS, (uint8_t*)secFlagsP, &length);
}

bool ProteusIV_GetMaxPeripheralConnections(uint8_t* maxP)
{
    uint16_t length = 1;
    return ProteusIV_Get(ProteusIV_USERSETTING_RF_MAXPERIPHERALCONNECTIONS, (uint8_t*)maxP, &length);
}

bool ProteusIV_GetBaudrateIndex(ProteusIV_BaudRate_t* baudrateP, ProteusIV_UartParity_t* parityP, bool* flowControlEnableP)
{
    if ((baudrateP == NULL) || (parityP == NULL) || (flowControlEnableP == NULL))
    {
        return false;
    }

    uint16_t length = 1;
    uint8_t uartIndex;

    if (ProteusIV_Get(ProteusIV_USERSETTING_UART_CONFIGINDEX, (uint8_t*)&uartIndex, &length))
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
            *parityP = ProteusIV_UartParity_None;
        }
        else
        {
            *parityP = ProteusIV_UartParity_Even;
            uartIndex -= 64;
        }

        *baudrateP = (ProteusIV_BaudRate_t)uartIndex;
        return true;
    }

    return false;
}

bool ProteusIV_GetStaticPasskey(uint8_t* staticPasskeyP)
{
    uint16_t length = PROTEUSIV_PASSKEY_LENGTH;
    return ProteusIV_Get(ProteusIV_USERSETTING_RF_STATICPASSKEY, staticPasskeyP, &length);
}

bool ProteusIV_GetAppearance(uint16_t* appearanceP)
{
    if (appearanceP == NULL)
    {
        return false;
    }

    uint8_t help[2];
    uint16_t length = sizeof(help);
    if (ProteusIV_Get(ProteusIV_USERSETTING_RF_APPEARANCE, help, &length))
    {
        memcpy((uint8_t*)appearanceP, help, 2);
        return true;
    }

    return false;
}

bool ProteusIV_GetSppServiceUuid(uint8_t* uuidP)
{
    uint16_t length = PROTEUSIV_UUID_LENGTH;
    return ProteusIV_Get(ProteusIV_USERSETTING_RF_SPPSERVICEUUID, uuidP, &length);
}

bool ProteusIV_GetSppRxUuid(uint8_t* uuidP)
{
    uint16_t length = PROTEUSIV_UUID_LENGTH;
    return ProteusIV_Get(ProteusIV_USERSETTING_RF_SPPRXUUID, uuidP, &length);
}

bool ProteusIV_GetSppTxUuid(uint8_t* uuidP)
{
    uint16_t length = PROTEUSIV_UUID_LENGTH;
    return ProteusIV_Get(ProteusIV_USERSETTING_RF_SPPTXUUID, uuidP, &length);
}

bool ProteusIV_GetAdvertisingData(uint8_t* dataP, uint16_t* lengthP) { return ProteusIV_Get(ProteusIV_USERSETTING_RF_ADVERTISINGDATA, dataP, lengthP); }

bool ProteusIV_GetScanResponseData(uint8_t* dataP, uint16_t* lengthP) { return ProteusIV_Get(ProteusIV_USERSETTING_RF_SCANRESPONSEDATA, dataP, lengthP); }

bool ProteusIV_GetCFGFlags(uint16_t* cfgFlagsP)
{
    if (cfgFlagsP == NULL)
    {
        return false;
    }

    uint8_t help[2];
    uint16_t length = sizeof(help);

    if (ProteusIV_Get(ProteusIV_USERSETTING_CFG_FLAGS, help, &length))
    {
        memcpy((uint8_t*)cfgFlagsP, help, 2);
        return true;
    }

    return false;
}

bool ProteusIV_GetRAM(ProteusIV_RuntimeSettings_t runtimeSetting, uint8_t* responseP, uint16_t* responseLengthP)
{
    if (!OPMODE_CHECK(ProteusIV_OperationMode_CommandMode) || (responseP == NULL) || (responseLengthP == NULL))
    {
        return false;
    }

    txPacket.Cmd = PROTEUSIV_CMD_GETRAM_REQ;
    txPacket.Length = 1;
    txPacket.Data[0] = runtimeSetting;

    FillChecksum(&txPacket);
    if (!ProteusIV_Internal_Transmit((uint8_t*)&txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD))
    {
        return false;
    }

    if (CMD_Status_Success == Wait4CNF(CMD_WAIT_TIME, PROTEUSIV_CMD_GETRAM_CNF, true))
    {
        uint16_t length = rxPacket.Length;

        if (*responseLengthP < length - 1)
        {
            /* provided buffer is too small */
            *responseLengthP = length - 1;
            return false;
        }

        *responseLengthP = length - 1;
        memcpy(responseP, &rxPacket.Data[1], *responseLengthP); /* First Data byte is status, following bytes response */
        return true;
    }

    return false;
}

bool ProteusIV_SetRAM(ProteusIV_RuntimeSettings_t runtimeSetting, uint8_t* valueP, uint8_t length)
{
    if (!OPMODE_CHECK(ProteusIV_OperationMode_CommandMode) || (valueP == NULL) || (length == 0))
    {
        return false;
    }

    txPacket.Cmd = PROTEUSIV_CMD_SETRAM_REQ;
    txPacket.Length = 1 + length;
    txPacket.Data[0] = runtimeSetting;
    memcpy(&txPacket.Data[1], valueP, length);

    FillChecksum(&txPacket);
    if (!ProteusIV_Internal_Transmit((uint8_t*)&txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD))
    {
        return false;
    }

    return CMD_Status_Success == Wait4CNF(CMD_WAIT_TIME, PROTEUSIV_CMD_SETRAM_CNF, true);
}

bool ProteusIV_GetRAMAdvertisingData(uint8_t* dataP, uint16_t* lengthP) { return ProteusIV_GetRAM(ProteusIV_RUNTIMESETTING_RF_ADVERTISINGDATA, dataP, lengthP); }

bool ProteusIV_GetRAMScanResponseData(uint8_t* dataP, uint16_t* lengthP) { return ProteusIV_GetRAM(ProteusIV_RUNTIMESETTING_RF_SCANRESPONSEDATA, dataP, lengthP); }

bool ProteusIV_SetRAMAdvertisingData(uint8_t* dataP, uint16_t length) { return ProteusIV_SetRAM(ProteusIV_RUNTIMESETTING_RF_ADVERTISINGDATA, dataP, length); }

bool ProteusIV_SetRAMScanResponseData(uint8_t* dataP, uint16_t length) { return ProteusIV_SetRAM(ProteusIV_RUNTIMESETTING_RF_SCANRESPONSEDATA, dataP, length); }

bool ProteusIV_GetState(ProteusIV_ModuleState_t* moduleStateP)
{
    if (!OPMODE_CHECK(ProteusIV_OperationMode_CommandMode | ProteusIV_OperationMode_ConfigMode | ProteusIV_OperationMode_BootMode) || (moduleStateP == NULL))
    {
        return false;
    }

    txPacket.Cmd = PROTEUSIV_CMD_GETSTATE_REQ;
    txPacket.Length = 0;

    FillChecksum(&txPacket);
    if (!ProteusIV_Internal_Transmit((uint8_t*)&txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD))
    {
        return false;
    }

    if (CMD_Status_Success == Wait4CNF(CMD_WAIT_TIME, PROTEUSIV_CMD_GETSTATE_CNF, true))
    {
        moduleStateP->internal_mode = rxPacket.Data[1];
        moduleStateP->connection_state.raw = rxPacket.Data[2];
        return true;
    }

    return false;
}

bool ProteusIV_ScanStart(ProteusIV_ScanResult_t* scanresultP)
{
    if (!OPMODE_CHECK(ProteusIV_OperationMode_CommandMode))
    {
        return false;
    }

    if (scanresultP != NULL)
    {
        for (int i = 0; i < PROTEUSIV_MAX_NUMBER_OF_SCANDEVICES; i++)
        {
            scanresultP->devices[i].inUse = false;
        }
    }
    ProteusIV_ScanDevicesP = scanresultP;

    txPacket.Cmd = PROTEUSIV_CMD_SCAN_REQ;
    txPacket.Length = 1;
    txPacket.Data[0] = 1;

    FillChecksum(&txPacket);
    if (!ProteusIV_Internal_Transmit((uint8_t*)&txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD))
    {
        ProteusIV_ScanDevicesP = NULL;
        return false;
    }

    if (CMD_Status_Success == Wait4CNF(CMD_WAIT_TIME, PROTEUSIV_CMD_SCAN_CNF, true))
    {
        return true;
    }

    ProteusIV_ScanDevicesP = NULL;
    return false;
}

bool ProteusIV_ScanStop()
{
    if (!OPMODE_CHECK(ProteusIV_OperationMode_CommandMode))
    {
        return false;
    }

    txPacket.Cmd = PROTEUSIV_CMD_SCAN_REQ;
    txPacket.Length = 1;
    txPacket.Data[0] = 0;

    FillChecksum(&txPacket);
    if (!ProteusIV_Internal_Transmit((uint8_t*)&txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD))
    {
        return false;
    }

    if (CMD_Status_Success != Wait4CNF(CMD_WAIT_TIME, PROTEUSIV_CMD_SCAN_CNF, true))
    {
        return false;
    }

    if (ProteusIV_ScanDevicesP != NULL)
    {
        ProteusIV_ScanDevicesP->numberOfDevices = 0;
        for (int i = 0; i < PROTEUSIV_MAX_NUMBER_OF_SCANDEVICES; i++)
        {
            if (ProteusIV_ScanDevicesP->devices[i].inUse)
            {
                ProteusIV_ScanDevicesP->numberOfDevices++;
            }
        }
        ProteusIV_ScanDevicesP = NULL;
    }

    return true;
}

bool ProteusIV_AdvertisingStart()
{
    if (!OPMODE_CHECK(ProteusIV_OperationMode_CommandMode | ProteusIV_OperationMode_BootMode))
    {
        return false;
    }

    txPacket.Cmd = PROTEUSIV_CMD_ADVERTISING_REQ;
    txPacket.Length = 1;
    txPacket.Data[0] = 1;

    FillChecksum(&txPacket);
    if (!ProteusIV_Internal_Transmit((uint8_t*)&txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD))
    {
        return false;
    }

    return CMD_Status_Success == Wait4CNF(CMD_WAIT_TIME, PROTEUSIV_CMD_ADVERTISING_CNF, true);
}

bool ProteusIV_AdvertisingStop()
{
    if (!OPMODE_CHECK(ProteusIV_OperationMode_CommandMode | ProteusIV_OperationMode_BootMode))
    {
        return false;
    }

    txPacket.Cmd = PROTEUSIV_CMD_ADVERTISING_REQ;
    txPacket.Length = 1;
    txPacket.Data[0] = 0;

    FillChecksum(&txPacket);
    if (!ProteusIV_Internal_Transmit((uint8_t*)&txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD))
    {
        return false;
    }

    return CMD_Status_Success == Wait4CNF(CMD_WAIT_TIME, PROTEUSIV_CMD_ADVERTISING_CNF, true);
}

bool ProteusIV_GetConnectionInfo(uint8_t conn_ID, ProteusIV_ConnectedDevices_t* devicesP)
{
    if (!OPMODE_CHECK(ProteusIV_OperationMode_CommandMode | ProteusIV_OperationMode_BootMode) || (devicesP == NULL))
    {
        return false;
    }

    ProteusIV_ConnectedDevicesP = devicesP;
    ProteusIV_ConnectedDevicesP->numberOfDevices = 0;

    txPacket.Cmd = PROTEUSIV_CMD_CONNECTIONINFO_REQ;
    txPacket.Length = 1;
    txPacket.Data[0] = conn_ID;

    FillChecksum(&txPacket);
    if (!ProteusIV_Internal_Transmit((uint8_t*)&txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD))
    {
        ProteusIV_ConnectedDevicesP = NULL;
        return false;
    }

    bool ret = (CMD_Status_Success == Wait4CNF(CMD_WAIT_TIME, PROTEUSIV_CMD_CONNECTIONINFO_CNF, true));
    ProteusIV_ConnectedDevicesP = NULL;
    return ret;
}

bool ProteusIV_Connect(ProteusIV_Bluetooth_Address_Type_t type, uint8_t* btMacP)
{
    if (!OPMODE_CHECK(ProteusIV_OperationMode_CommandMode) || (btMacP == NULL))
    {
        return false;
    }

    txPacket.Cmd = PROTEUSIV_CMD_CONNECT_REQ;
    txPacket.Length = PROTEUSIV_BTMAC_LENGTH + 1;
    txPacket.Data[0] = (uint8_t)type;
    memcpy(&txPacket.Data[1], btMacP, PROTEUSIV_BTMAC_LENGTH);

    FillChecksum(&txPacket);
    if (!ProteusIV_Internal_Transmit((uint8_t*)&txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD))
    {
        return false;
    }

    return CMD_Status_Success == Wait4CNF(3000, PROTEUSIV_CMD_CONNECT_CNF, true);
}

bool ProteusIV_Passkey(uint8_t conn_ID, uint8_t* passkey)
{
    if (!OPMODE_CHECK(ProteusIV_OperationMode_CommandMode | ProteusIV_OperationMode_BootMode) || (passkey == NULL))
    {
        return false;
    }

    txPacket.Cmd = PROTEUSIV_CMD_PASSKEY_REQ;
    txPacket.Length = PROTEUSIV_PASSKEY_LENGTH + 1;
    txPacket.Data[0] = conn_ID;
    memcpy(&txPacket.Data[1], passkey, PROTEUSIV_PASSKEY_LENGTH);

    FillChecksum(&txPacket);
    if (!ProteusIV_Internal_Transmit((uint8_t*)&txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD))
    {
        return false;
    }

    return CMD_Status_Success == Wait4CNF(CMD_WAIT_TIME, PROTEUSIV_CMD_PASSKEY_CNF, true);
}

bool ProteusIV_NumericCompareConfirm(uint8_t conn_ID, bool keyIsOk)
{
    if (!OPMODE_CHECK(ProteusIV_OperationMode_CommandMode | ProteusIV_OperationMode_BootMode))
    {
        return false;
    }

    txPacket.Cmd = PROTEUSIV_CMD_NUMERIC_COMP_REQ;
    txPacket.Length = 2;
    txPacket.Data[0] = conn_ID;
    txPacket.Data[1] = keyIsOk ? 0x00 : 0x01;

    FillChecksum(&txPacket);
    if (!ProteusIV_Internal_Transmit((uint8_t*)&txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD))
    {
        return false;
    }

    return CMD_Status_Success == Wait4CNF(CMD_WAIT_TIME, PROTEUSIV_CMD_NUMERIC_COMP_CNF, true);
}

bool ProteusIV_PhyUpdate(uint8_t conn_ID, ProteusIV_Phy_t phy)
{
    if (!OPMODE_CHECK(ProteusIV_OperationMode_CommandMode | ProteusIV_OperationMode_BootMode))
    {
        return false;
    }

    txPacket.Cmd = PROTEUSIV_CMD_PHYUPDATE_REQ;
    txPacket.Length = 2;
    txPacket.Data[0] = conn_ID;
    txPacket.Data[1] = (uint8_t)phy;

    FillChecksum(&txPacket);
    if (!ProteusIV_Internal_Transmit((uint8_t*)&txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD))
    {
        return false;
    }

    return CMD_Status_Success == Wait4CNF(CMD_WAIT_TIME, PROTEUSIV_CMD_PHYUPDATE_CNF, true);
}

bool ProteusIV_GetLed1PinLevel(WE_Pin_Level_t* Led1LevelP) { return WE_GetPinLevel(ProteusIV_pinsP->ProteusIV_Pin_Led1, Led1LevelP); }

bool ProteusIV_GetBonds(ProteusIV_BondDatabase_t* bondDatabaseP)
{
    if (!OPMODE_CHECK(ProteusIV_OperationMode_CommandMode | ProteusIV_OperationMode_BootMode | ProteusIV_OperationMode_ConfigMode) || (bondDatabaseP == NULL))
    {
        return false;
    }

    ProteusIV_BondDatabaseP = bondDatabaseP;
    ProteusIV_BondDatabaseP->numberOfDevices = 0;

    txPacket.Cmd = PROTEUSIV_CMD_GETBONDS_REQ;
    txPacket.Length = 0;

    FillChecksum(&txPacket);
    if (!ProteusIV_Internal_Transmit((uint8_t*)&txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD))
    {
        ProteusIV_BondDatabaseP = NULL;
        return false;
    }

    bool ret = (CMD_Status_Success == Wait4CNF(CMD_WAIT_TIME, PROTEUSIV_CMD_GETBONDS_CNF, true));
    ProteusIV_BondDatabaseP = NULL;

    return ret;
}

bool ProteusIV_DeleteBonds()
{
    if (!OPMODE_CHECK(ProteusIV_OperationMode_CommandMode | ProteusIV_OperationMode_ConfigMode))
    {
        return false;
    }

    txPacket.Cmd = PROTEUSIV_CMD_DELETEBONDS_REQ;
    txPacket.Length = 0;

    FillChecksum(&txPacket);
    if (!ProteusIV_Internal_Transmit((uint8_t*)&txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD))
    {
        return false;
    }

    return CMD_Status_Success == Wait4CNF(CMD_WAIT_TIME, PROTEUSIV_CMD_DELETEBONDS_CNF, true);
}

bool ProteusIV_DeleteBond(ProteusIV_Bluetooth_Address_Type_t type, uint8_t* btMacP)
{
    if (!OPMODE_CHECK(ProteusIV_OperationMode_CommandMode | ProteusIV_OperationMode_ConfigMode))
    {
        return false;
    }

    txPacket.Cmd = PROTEUSIV_CMD_DELETEBONDS_REQ;

    txPacket.Length = PROTEUSIV_BTMAC_LENGTH + 1;
    txPacket.Data[0] = (uint8_t)type;
    memcpy(&txPacket.Data[1], btMacP, PROTEUSIV_BTMAC_LENGTH);

    FillChecksum(&txPacket);
    if (!ProteusIV_Internal_Transmit((uint8_t*)&txPacket, txPacket.Length + LENGTH_CMD_OVERHEAD))
    {
        return false;
    }

    return CMD_Status_Success == Wait4CNF(CMD_WAIT_TIME, PROTEUSIV_CMD_DELETEBONDS_CNF, true);
}
