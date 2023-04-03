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
 * @brief Calypso HTTP server example.
 */

#include "Calypso_HTTP_Server_Example.h"

#include <stdio.h>

#include <Calypso/ATCommands/ATDevice.h>
#include <Calypso/ATCommands/ATFile.h>
#include <Calypso/ATCommands/ATHTTP.h>
#include <Calypso/ATCommands/ATNetCfg.h>
#include <Calypso/ATCommands/ATWLAN.h>

#include "Calypso_Examples.h"

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
    printf("*** Start of Calypso HTTP server example ***\r\n");

    bool ret = false;

    if (!Calypso_Init(Calypso_Examples_baudRate, Calypso_Examples_flowControl, Calypso_Examples_parity, &Calypso_HTTP_Example_EventCallback, NULL))
    {
        return;
    }

    Calypso_PinReset();

    Calypso_Examples_WaitForStartup(5000);

    WE_Delay(1000);

    /* Get version info. This retrieves Calypso's firmware version (amongst other version info) and
     * stores the firmware version in Calypso_firmwareVersionMajor, Calypso_firmwareVersionMinor and
     * Calypso_firmwareVersionPatch for later use. */
    ATDevice_Value_t deviceValue;
    ret = ATDevice_Get(ATDevice_GetId_General, ATDevice_GetGeneral_Version, &deviceValue);
    Calypso_Examples_Print("Get device version", ret);

    ATWLAN_Disconnect();

    /* Set IPv4 address method DHCP */
    ATNetCfg_IPv4Config_t newIpV4Config = {0};
    newIpV4Config.method = ATNetCfg_IPv4Method_Dhcp;
    ret = ATNetCfg_SetIPv4AddressStation(&newIpV4Config);
    Calypso_Examples_Print("Set IPv4 DHCP", ret);

    /* No automatic connection */
    ret = ATWLAN_SetConnectionPolicy(ATWLAN_PolicyConnection_None);
    Calypso_Examples_Print("Set no WLAN connection policy", ret);

    /* WLAN station mode */
    ret = ATWLAN_SetMode(ATWLAN_SetMode_Station);
    Calypso_Examples_Print("Set WLAN station mode", ret);

    ret = ATDevice_Restart(0);
    Calypso_Examples_Print("Restart network processor", ret);

    WE_Delay(1000);

    /* Connect to WLAN */
    ATWLAN_ConnectionArguments_t connectArgs;
    memset(&connectArgs, 0, sizeof(connectArgs));
    strcpy(connectArgs.SSID, Calypso_Examples_wlanSSID);
    connectArgs.securityParams.securityType = ATWLAN_SecurityType_WPA_WPA2;
    strcpy(connectArgs.securityParams.securityKey, Calypso_Examples_wlanKey);

    ret = ATWLAN_Connect(connectArgs);
    Calypso_Examples_Print("Connect to WLAN", ret);

    WE_Delay(2000);


    printf("Will now wait for asynchronous events until receiving HTTP POST request with id=\"quit\".\r\n");
    while (!quitRequested)
    {
        if (getRequestReceived)
        {
            /* An HTTP GET request has been received. */
            char response[256];
            sprintf(response, "This is Calypso's response to the HTTP GET request with id=\"%s\".", getRequestId);
            ATHTTP_SendCustomResponse(Calypso_DataFormat_Base64, true, strlen(response), response);

            getRequestReceived = false;
        }
    }

    printf("Quit command received, will now halt.\r\n");

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
 * events (i.e. events from ATEvent_t). Some events might in fact be responses
 * to AT commands that are not included in ATEvent_t.
 */
void Calypso_HTTP_Example_EventCallback(char* eventText)
{
    Calypso_Examples_EventCallback(eventText);

    ATEvent_t event;
    ATEvent_ParseEventType(&eventText, &event);

    switch (event)
    {
    case ATEvent_HTTPGet:
    {
        if (!getRequestReceived)
        {
            if (ATEvent_ParseHttpGetEvent(&eventText, getRequestId, sizeof(getRequestId)))
            {
                printf("Received HTTP GET request with id=\"%s\"\r\n", getRequestId);

                getRequestReceived = true;
            }
        }
        break;
    }

    case ATEvent_CustomGPIO:
    {
        uint8_t gpioId;
        if (ATEvent_ParseCustomGPIOEvent(&eventText, &gpioId))
        {
            printf("GPIO %d has been remotely configured\r\n", gpioId);
        }
        break;
    }

    case ATEvent_CustomHTTPPost:
    {
        char id[64];
        char value[1024];
        if (ATEvent_ParseCustomHTTPPostEvent(&eventText, id, value, sizeof(id), sizeof(value)))
        {
            printf("Received HTTP POST event containing id=\"%s\" and value=\"%s\"\r\n", id, value);

            if (0 == strcmp(id, "quit"))
            {
                quitRequested = true;
            }
        }
        break;
    }

    case ATEvent_Startup:
    case ATEvent_WakeUp:
    case ATEvent_Ping:
    case ATEvent_SocketTxFailed:
    case ATEvent_SocketAsyncEvent:
    case ATEvent_SocketTCPConnect:
    case ATEvent_SocketTCPAccept:
    case ATEvent_SocketRcvd:
    case ATEvent_SocketRcvdFrom:
    case ATEvent_WlanP2PConnect:
    case ATEvent_WlanP2PDisconnect:
    case ATEvent_WlanP2PClientAdded:
    case ATEvent_WlanP2PClientRemoved:
    case ATEvent_WlanP2PDevFound:
    case ATEvent_WlanP2PRequest:
    case ATEvent_WlanP2PConnectFail:
    case ATEvent_Invalid:
    case ATEvent_GeneralResetRequest:
    case ATEvent_GeneralError:
    case ATEvent_WlanConnect:
    case ATEvent_WlanDisconnect:
    case ATEvent_WlanStaAdded:
    case ATEvent_WlanStaRemoved:
    case ATEvent_WlanProvisioningStatus:
    case ATEvent_WlanProvisioningProfileAdded:
    case ATEvent_NetappIP4Acquired:
    case ATEvent_NetappIP6Acquired:
    case ATEvent_NetappIPCollision:
    case ATEvent_NetappDHCPv4_leased:
    case ATEvent_NetappDHCPv4_released:
    case ATEvent_NetappIPv4Lost:
    case ATEvent_NetappDHCPIPv4AcquireTimeout:
    case ATEvent_NetappIPv6Lost:
    case ATEvent_MQTTOperation:
    case ATEvent_MQTTRecv:
    case ATEvent_MQTTDisconnect:
    case ATEvent_FileListEntry:
    case ATEvent_FatalErrorDeviceAbort:
    case ATEvent_FatalErrorDriverAbort:
    case ATEvent_FatalErrorSyncLost:
    case ATEvent_FatalErrorNoCmdAck:
    case ATEvent_FatalErrorCmdTimeout:
    default:
        break;
    }
}
