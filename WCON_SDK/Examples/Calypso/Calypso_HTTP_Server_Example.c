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
 * COPYRIGHT (c) 2025 Würth Elektronik eiSos GmbH & Co. KG
 *
 ***************************************************************************************************
 */

/**
 * @file
 * @brief Calypso HTTP server example.
 */
#include <Calypso/ATCommands/ATDevice.h>
#include <Calypso/ATCommands/ATFile.h>
#include <Calypso/ATCommands/ATHTTP.h>
#include <Calypso/ATCommands/ATNetCfg.h>
#include <Calypso/ATCommands/ATWLAN.h>
#include <Calypso/Calypso_Examples.h>
#include <Calypso/Calypso_HTTP_Server_Example.h>
#include <stdio.h>

void Calypso_HTTP_Example_EventCallback(char* eventText);

/**
 * @brief Is set to true when a HTTP POST request with ID "quit" has been received.
 */
static bool quitRequested = false;

/**
 * @brief Is set to true when a HTTP GET request has been received.
 */
static bool getRequestReceived = false;

/**
 * @brief Stores the ID of the last HTTP GET request.
 */
static char getRequestId[64];

void Calypso_HTTP_Server_Example(void)
{
    WE_DEBUG_PRINT("*** Start of Calypso HTTP server example ***\r\n");

    bool ret = false;

    if (!Calypso_Init(&Calypso_uart, &Calypso_pins, &Calypso_HTTP_Example_EventCallback))
    {
        WE_DEBUG_PRINT("Initialization error\r\n");
        return;
    }

    Calypso_PinReset();

    Calypso_Examples_WaitForStartup(5000);

    WE_Delay(1000);

    /* Get version info. This retrieves Calypso's firmware version (amongst other version info) and
	 * stores the firmware version in Calypso_firmwareVersionMajor, Calypso_firmwareVersionMinor and
	 * Calypso_firmwareVersionPatch for later use. */
    Calypso_ATDevice_Value_t deviceValue;
    ret = Calypso_ATDevice_Get(Calypso_ATDevice_GetId_General, Calypso_ATDevice_GetGeneral_Version, &deviceValue);
    Calypso_Examples_Print("Get device version", ret);

    Calypso_ATWLAN_Disconnect();

    /* Set IPv4 address method DHCP */
    Calypso_ATNetCfg_IPv4Config_t newIpV4Config = {0};
    newIpV4Config.method = Calypso_ATNetCfg_IPv4Method_Dhcp;
    ret = Calypso_ATNetCfg_SetIPv4AddressStation(&newIpV4Config);
    Calypso_Examples_Print("Set IPv4 DHCP", ret);

    /* No automatic connection */
    ret = Calypso_ATWLAN_SetConnectionPolicy(Calypso_ATWLAN_PolicyConnection_None);
    Calypso_Examples_Print("Set no WLAN connection policy", ret);

    /* WLAN station mode */
    ret = Calypso_ATWLAN_SetMode(Calypso_ATWLAN_SetMode_Station);
    Calypso_Examples_Print("Set WLAN station mode", ret);

    ret = Calypso_ATDevice_Restart(0);
    Calypso_Examples_Print("Restart network processor", ret);

    WE_Delay(1000);

    /* Connect to WLAN */
    Calypso_ATWLAN_ConnectionArguments_t connectArgs;
    memset(&connectArgs, 0, sizeof(connectArgs));
    strcpy(connectArgs.SSID, Calypso_Examples_wlanSSID);
    connectArgs.securityParams.securityType = Calypso_ATWLAN_SecurityType_WPA_WPA2;
    strcpy(connectArgs.securityParams.securityKey, Calypso_Examples_wlanKey);

    ret = Calypso_ATWLAN_Connect(connectArgs);
    Calypso_Examples_Print("Connect to WLAN", ret);

    WE_Delay(2000);

    WE_DEBUG_PRINT("Will now wait for asynchronous events until receiving HTTP POST request with id=\"quit\".\r\n");
    while (!quitRequested)
    {
        if (getRequestReceived)
        {
            /* An HTTP GET request has been received. */
            char response[256];
            sprintf(response, "This is Calypso's response to the HTTP GET request with id=\"%s\".", getRequestId);
            Calypso_ATHTTP_SendCustomResponse(Calypso_DataFormat_Base64, true, strlen(response), response);

            getRequestReceived = false;
        }
    }

    WE_DEBUG_PRINT("Quit command received, will now halt.\r\n");

    Calypso_Deinit();
}

/**
 * @brief Is called when an event notification has been received.
 *
 * Note that this function is called from an interrupt - code in this function
 * should thus be kept simple.
 *
 * Note in particular that it is not possible to send AT commands to Calypso
 * from within this event handler.
 *
 * Also note that not all calls of this handler necessarily correspond to valid
 * events (i.e. events from Calypso_ATEvent_t). Some events might in fact be responses
 * to AT commands that are not included in Calypso_ATEvent_t.
 */
void Calypso_HTTP_Example_EventCallback(char* eventText)
{
    Calypso_Examples_EventCallback(eventText);

    Calypso_ATEvent_t event;
    if (false == Calypso_ATEvent_ParseEventType(&eventText, &event))
    {
        return;
    }

    switch (event)
    {
        case Calypso_ATEvent_HTTPGet:
        {
            if (!getRequestReceived)
            {
                if (Calypso_ATEvent_ParseHttpGetEvent(&eventText, getRequestId, sizeof(getRequestId)))
                {
                    WE_DEBUG_PRINT("Received HTTP GET request with id=\"%s\"\r\n", getRequestId);

                    getRequestReceived = true;
                }
            }
            break;
        }

        case Calypso_ATEvent_CustomGPIO:
        {
            uint8_t gpioId;
            if (Calypso_ATEvent_ParseCustomGPIOEvent(&eventText, &gpioId))
            {
                WE_DEBUG_PRINT("GPIO %d has been remotely configured\r\n", gpioId);
            }
            break;
        }

        case Calypso_ATEvent_CustomHTTPPost:
        {
            char id[64];
            char value[1024];
            if (Calypso_ATEvent_ParseCustomHTTPPostEvent(&eventText, id, value, sizeof(id), sizeof(value)))
            {
                WE_DEBUG_PRINT("Received HTTP POST event containing id=\"%s\" and value=\"%s\"\r\n", id, value);

                if (0 == strcmp(id, "quit"))
                {
                    quitRequested = true;
                }
            }
            break;
        }

        case Calypso_ATEvent_Startup:
        case Calypso_ATEvent_WakeUp:
        case Calypso_ATEvent_Ping:
        case Calypso_ATEvent_SocketTxFailed:
        case Calypso_ATEvent_SocketAsyncEvent:
        case Calypso_ATEvent_SocketTCPConnect:
        case Calypso_ATEvent_SocketTCPAccept:
        case Calypso_ATEvent_SocketRcvd:
        case Calypso_ATEvent_SocketRcvdFrom:
        case Calypso_ATEvent_WlanP2PConnect:
        case Calypso_ATEvent_WlanP2PDisconnect:
        case Calypso_ATEvent_WlanP2PClientAdded:
        case Calypso_ATEvent_WlanP2PClientRemoved:
        case Calypso_ATEvent_WlanP2PDevFound:
        case Calypso_ATEvent_WlanP2PRequest:
        case Calypso_ATEvent_WlanP2PConnectFail:
        case Calypso_ATEvent_GeneralResetRequest:
        case Calypso_ATEvent_GeneralError:
        case Calypso_ATEvent_WlanConnect:
        case Calypso_ATEvent_WlanDisconnect:
        case Calypso_ATEvent_WlanStaAdded:
        case Calypso_ATEvent_WlanStaRemoved:
        case Calypso_ATEvent_WlanProvisioningStatus:
        case Calypso_ATEvent_WlanProvisioningProfileAdded:
        case Calypso_ATEvent_NetappIP4Acquired:
        case Calypso_ATEvent_NetappIP6Acquired:
        case Calypso_ATEvent_NetappIPCollision:
        case Calypso_ATEvent_NetappDHCPv4_leased:
        case Calypso_ATEvent_NetappDHCPv4_released:
        case Calypso_ATEvent_NetappIPv4Lost:
        case Calypso_ATEvent_NetappDHCPIPv4AcquireTimeout:
        case Calypso_ATEvent_NetappIPv6Lost:
        case Calypso_ATEvent_MQTTOperation:
        case Calypso_ATEvent_MQTTRecv:
        case Calypso_ATEvent_MQTTDisconnect:
        case Calypso_ATEvent_FileListEntry:
        case Calypso_ATEvent_FatalErrorDeviceAbort:
        case Calypso_ATEvent_FatalErrorDriverAbort:
        case Calypso_ATEvent_FatalErrorSyncLost:
        case Calypso_ATEvent_FatalErrorNoCmdAck:
        case Calypso_ATEvent_FatalErrorCmdTimeout:
        default:
            break;
    }
}
