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
 * @brief Calypso network applications example.
 */

#include "Calypso_NetApp_Example.h"

#include <stdio.h>

#include <Calypso/ATCommands/ATDevice.h>
#include <Calypso/ATCommands/ATNetApp.h>
#include <Calypso/ATCommands/ATNetCfg.h>
#include <Calypso/ATCommands/ATWLAN.h>

#include "Calypso_Examples.h"

static void Calypso_NetApp_Example_EventCallback(char* eventText);

/* Used to keep track of number of expected ping responses remaining */
static uint8_t nrOfPingResponsesRemaining = 0;

void Calypso_NetApp_Example(void)
{
    printf("*** Start of Calypso ATNetApp example ***\r\n");

    bool ret = false;

    if (!Calypso_Init(Calypso_Examples_baudRate, Calypso_Examples_flowControl, Calypso_Examples_parity, &Calypso_NetApp_Example_EventCallback, NULL))
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

    /* Get IPv4 configuration (station) */
    ATNetCfg_IPv4Config_t ipV4Config;
    ret = ATNetCfg_GetIPv4AddressStation(&ipV4Config);
    Calypso_Examples_Print("Get IPv4 config", ret);
    if (ret)
    {
        printf("*** Station IPv4 configuration ***\r\n");
        printf("IPv4 address: %s\r\n", ipV4Config.ipAddress);
        printf("Subnet mask: %s\r\n", ipV4Config.subnetMask);
        printf("Gateway address: %s\r\n", ipV4Config.gatewayAddress);
        printf("DNS address: %s\r\n", ipV4Config.dnsAddress);
    }


    /* Host / IP lookup example */
    ATNetApp_GetHostByNameResult_t lookupResult;
    ret = ATNetApp_GetHostByName("www.google.com", ATSocket_Family_INET, &lookupResult);
    Calypso_Examples_Print("Get host by name", ret);
    printf("IP lookup result: host=\"%s\", IP=\"%s\"\r\n", lookupResult.hostName, lookupResult.hostAddress);


    /* SNTP client example */
    ret = ATNetApp_StartApplications(ATNetApp_Application_SntpClient);
    Calypso_Examples_Print("Start SNTP client", ret);

    ATNetApp_OptionValue_t value;
    memset(&value, 0, sizeof(value));
    value.sntp.updateInterval = 1;
    ret = ATNetApp_Set(ATNetApp_Application_SntpClient, ATNetApp_SntpOption_UpdateInterval, &value);
    Calypso_Examples_Print("Set SNTP update interval", ret);

    memset(&value, 0, sizeof(value));
    value.sntp.timeZone = 60;
    ret = ATNetApp_Set(ATNetApp_Application_SntpClient, ATNetApp_SntpOption_TimeZone, &value);
    Calypso_Examples_Print("Set SNTP time zone", ret);

    memset(&value, 0, sizeof(value));
    strcpy(value.sntp.servers[0], "pool.ntp.org");
    strcpy(value.sntp.servers[1], "europe.pool.ntp.org");
    strcpy(value.sntp.servers[2], "");
    ret = ATNetApp_Set(ATNetApp_Application_SntpClient, ATNetApp_SntpOption_Servers, &value);
    Calypso_Examples_Print("Set SNTP servers", ret);

    WE_Delay(2000);

    ret = ATNetApp_UpdateTime();
    Calypso_Examples_Print("Update device time", ret);

    ret = ATDevice_Get(ATDevice_GetId_General, ATDevice_GetGeneral_Time, &deviceValue);
    Calypso_Examples_Print("Get device time", ret);
    if (ret)
    {
        printf("date(dd:mm:yy): %u.%u.%u time(hh:mm:ss): %u:%u:%u\r\n",
               deviceValue.general.time.day,
               deviceValue.general.time.month,
               deviceValue.general.time.year,
               deviceValue.general.time.hour,
               deviceValue.general.time.minute,
               deviceValue.general.time.second);
    }

    ret = ATNetApp_StopApplications(ATNetApp_Application_SntpClient);
    Calypso_Examples_Print("Disable SNTP client", ret);

    WE_Delay(500);


    /* Ping the gateway. Ping response is reported in the form of events. */
    ATNetApp_PingParameters_t pingParams;
    pingParams.family = ATSocket_Family_INET;
    strcpy(pingParams.destination, ipV4Config.gatewayAddress);
    pingParams.size = 8;
    pingParams.delayMs = 500;
    pingParams.timeoutMs = 1000;
    pingParams.maxNumberOfPings = 4;
    pingParams.mode = ATNetApp_PingMode_ReportAfterEveryPing;
    nrOfPingResponsesRemaining = pingParams.maxNumberOfPings;
    ret = ATNetApp_Ping(&pingParams);
    Calypso_Examples_Print("Ping gateway", ret);

    /* Wait for all ping responses requested above to arrive */
    while (nrOfPingResponsesRemaining > 0)
    {
    }

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
void Calypso_NetApp_Example_EventCallback(char* eventText)
{
    Calypso_Examples_EventCallback(eventText);

    ATEvent_t event;
    ATEvent_ParseEventType(&eventText, &event);

    if (ATEvent_Invalid == event)
    {
        return;
    }

    switch (event)
    {
    case ATEvent_Ping:
    {
        ATEvent_Ping_t pingEvent;
        if (ATEvent_ParsePingEvent(&eventText, &pingEvent))
        {
            printf("Ping event - "
                    "Packets received: %d, "
                    "packets sent: %d, "
                    "roundtrip time: %d ms\r\n",
                    pingEvent.packetsSent,
                    pingEvent.packetsReceived,
                    pingEvent.roundTripTimeMs);
        }
        if (nrOfPingResponsesRemaining > 0)
        {
            nrOfPingResponsesRemaining--;
        }
        break;
    }

    case ATEvent_Startup:
    case ATEvent_WakeUp:
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
    case ATEvent_HTTPGet:
    case ATEvent_CustomGPIO:
    case ATEvent_CustomHTTPPost:
    case ATEvent_FatalErrorDeviceAbort:
    case ATEvent_FatalErrorDriverAbort:
    case ATEvent_FatalErrorSyncLost:
    case ATEvent_FatalErrorNoCmdAck:
    case ATEvent_FatalErrorCmdTimeout:
    default:
        break;
    }
}
