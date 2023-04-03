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
 * @brief Calypso examples.
 *
 * Comment/uncomment lines in Calypso_Examples() to start the desired example.
 */

#include "Calypso_Examples.h"

#include <stdio.h>
#include <string.h>

#include <Calypso/Calypso.h>

#include <Calypso/ATCommands/ATEvent.h>

#include "Calypso_Device_Example.h"
#include "Calypso_Provisioning_Example.h"
#include "Calypso_File_Example.h"
#include "Calypso_GPIO_Example.h"
#include "Calypso_HTTP_Client_Example.h"
#include "Calypso_HTTP_Server_Example.h"
#include "Calypso_MQTT_Example.h"
#include "Calypso_NetApp_Example.h"
#include "Calypso_NetCfg_Example.h"
#include "Calypso_P2P_Example.h"
#include "Calypso_Socket_Example.h"
#include "Calypso_TransparentMode_Example.h"
#include "Calypso_WLAN_Example.h"
#include "Calypso_Azure_PnP_Example/Calypso_Azure_PnP_Example.h"

/**
 * @brief UART baud rate used for communication with Calypso module.
 */
const uint32_t Calypso_Examples_baudRate = 921600;

/**
 * @brief UART flow control setting used for communication with Calypso module.
 */
const WE_FlowControl_t Calypso_Examples_flowControl = WE_FlowControl_NoFlowControl;

/**
 * @brief UART parity used for communication with Calypso module.
 */
const WE_Parity_t Calypso_Examples_parity = WE_Parity_Even;

/**
 * @brief SSID of WLAN (used in several examples)
 */
const char *Calypso_Examples_wlanSSID = "calypso_CAFFEE123456";

/**
 * @brief Key of WLAN (used in several examples)
 */
const char *Calypso_Examples_wlanKey = "calypsowlan";

/**
 * @brief Contains information on last startup event (if any)
 */
ATEvent_Startup_t Calypso_Examples_startupEvent = {0};

/**
 * @brief Is set to true when a startup event is received
 */
bool Calypso_Examples_startupEventReceived = false;

/**
 * @brief Is set to true when an IPv4 acquired event is received
 */
bool Calypso_Examples_ip4Acquired = false;

/**
 * @brief Waits for the startup event to be received.
 */
bool Calypso_Examples_WaitForStartup(uint32_t timeoutMs)
{
    uint32_t t0 = WE_GetTick();
    Calypso_Examples_startupEventReceived = false;
    while (false == Calypso_Examples_startupEventReceived && (WE_GetTick() - t0) < timeoutMs)
    {
    }
    return Calypso_Examples_startupEventReceived;
}

/**
 * @brief Waits for an IPv4 acquired event to be received.
 */
bool Calypso_Examples_WaitForIPv4Acquired(uint32_t timeoutMs)
{
    uint32_t t0 = WE_GetTick();
    Calypso_Examples_ip4Acquired = false;
    while (false == Calypso_Examples_ip4Acquired && (WE_GetTick() - t0) < timeoutMs)
    {
    }
    return Calypso_Examples_ip4Acquired;
}

/**
 * @brief Runs Calypso examples.
 *
 * Comment/uncomment lines in this function to start the desired example.
 */
void Calypso_Examples(void)
{
    /* Initialize platform (peripherals, flash interface, Systick, system clock) */
    WE_Platform_Init();

#ifdef WE_DEBUG
    WE_Debug_Init();
#endif

    uint8_t version[3];
    WE_GetDriverVersion(version);
    printf("Wuerth Elektronik eiSos Wireless Connectivity SDK version %d.%d.%d\r\n", version[0], version[1], version[2]);

    /* The example to be started can be picked from the list below by calling the corresponding function. */

    Calypso_Device_Example();
//    Calypso_Provisioning_Example();
//    Calypso_WLAN_Example();
//    Calypso_NetCfg_Example();
//    Calypso_TCPServer_Example();
//    Calypso_TCPClient_Example();
//    Calypso_UDPReceive_Example();
//    Calypso_UDPTransmit_Example();
//    Calypso_P2P_Example();
//    Calypso_File_Example();
//    Calypso_MQTT_Example();
//    Calypso_GPIO_Example();
//    Calypso_HTTP_Client_Example();
//    Calypso_HTTP_Server_Example();
//    Calypso_NetApp_Example();
//    Calypso_TransparentMode_Example();
//    Calypso_Azure_PnP_Example();

    printf("*** End of example ***\r\n");

    while (1)
    {
        WE_Delay(500);
    }
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
void Calypso_Examples_EventCallback(char* eventText)
{
    ATEvent_t event;
    ATEvent_ParseEventType(&eventText, &event);

    if (ATEvent_Invalid == event)
    {
        return;
    }

    char eventName[32];
    ATEvent_GetEventName(event, eventName);

    printf("EVENT of type \"%s\": %s\r\n", eventName, eventText);

    switch (event)
    {
    case ATEvent_Startup:
        if (ATEvent_ParseStartUpEvent(&eventText, &Calypso_Examples_startupEvent))
        {
            printf("Startup event received. "
                    "Article nr: %s, "
                    "Chip ID: %s, "
                    "MAC address: %s, "
                    "Firmware version: %d.%d.%d\r\n",
                    Calypso_Examples_startupEvent.articleNr,
                    Calypso_Examples_startupEvent.chipID,
                    Calypso_Examples_startupEvent.MACAddress,
                    Calypso_Examples_startupEvent.firmwareVersion[0],
                    Calypso_Examples_startupEvent.firmwareVersion[1],
                    Calypso_Examples_startupEvent.firmwareVersion[2]);
        }
        Calypso_Examples_startupEventReceived = true;
        break;

    case ATEvent_NetappIP4Acquired:
        Calypso_Examples_ip4Acquired = true;
        break;

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

/**
 * @brief Prints the supplied string, prefixed with OK or NOK (depending on the success parameter).
 *
 * @param str String to print
 * @param success Variable indicating if action was ok
 */
void Calypso_Examples_Print(char* str, bool success)
{
    printf("%s%s\r\n", success ? "OK    " : "NOK   ", str);
}
