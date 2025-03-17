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
 * @brief Calypso examples.
 *
 * Comment/uncomment lines in Calypso_Examples() to start the desired example.
 */
#include <Calypso/ATCommands/ATEvent.h>
#include <Calypso/Calypso.h>
#include <Calypso/Calypso_Azure_PnP_Example/Calypso_Azure_PnP_Example.h>
#include <Calypso/Calypso_Device_Example.h>
#include <Calypso/Calypso_Examples.h>
#include <Calypso/Calypso_File_Example.h>
#include <Calypso/Calypso_GPIO_Example.h>
#include <Calypso/Calypso_HTTP_Client_Example.h>
#include <Calypso/Calypso_HTTP_Server_Example.h>
#include <Calypso/Calypso_MQTT_Example.h>
#include <Calypso/Calypso_NetApp_Example.h>
#include <Calypso/Calypso_NetCfg_Example.h>
#include <Calypso/Calypso_P2P_Example.h>
#include <Calypso/Calypso_Provisioning_Example.h>
#include <Calypso/Calypso_Socket_Example.h>
#include <Calypso/Calypso_TransparentMode_Example.h>
#include <Calypso/Calypso_WLAN_Example.h>
#include <global/global_types.h>
#include <global_platform_types.h>
#include <stdio.h>
#include <string.h>

/**
 * @brief SSID of WLAN (used in several examples)
 */
const char* Calypso_Examples_wlanSSID = "calypso_CAFFEE123456";

/**
 * @brief Key of WLAN (used in several examples)
 */
const char* Calypso_Examples_wlanKey = "calypsowlan";

/**
 * @brief Contains information on last startup event (if any)
 */
Calypso_ATEvent_Startup_t Calypso_Examples_startupEvent = {0};

/**
 * @brief Is set to true when a startup event is received
 */
bool Calypso_Examples_startupEventReceived = false;

/**
 * @brief Is set to true when an IPv4 acquired event is received
 */
bool Calypso_Examples_ip4Acquired = false;

Calypso_Pins_t Calypso_pins = {
    .Calypso_Pin_Reset = WE_PIN((void*)&WE_STM32_PIN(GPIOA, GPIO_PIN_10)),
    .Calypso_Pin_WakeUp = WE_PIN((void*)&WE_STM32_PIN(GPIOA, GPIO_PIN_9)),
    .Calypso_Pin_Boot = WE_PIN((void*)&WE_STM32_PIN(GPIOA, GPIO_PIN_7)),
    .Calypso_Pin_AppMode0 = WE_PIN((void*)&WE_STM32_PIN(GPIOA, GPIO_PIN_0)),
    .Calypso_Pin_AppMode1 = WE_PIN((void*)&WE_STM32_PIN(GPIOA, GPIO_PIN_1)),
    .Calypso_Pin_StatusInd0 = WE_PIN((void*)&WE_STM32_PIN(GPIOB, GPIO_PIN_8)),
    .Calypso_Pin_StatusInd1 = WE_PIN((void*)&WE_STM32_PIN(GPIOB, GPIO_PIN_9)),
};

WE_UART_t Calypso_uart;

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

    Calypso_uart.baudrate = 921600;
    Calypso_uart.flowControl = WE_FlowControl_NoFlowControl;
    Calypso_uart.parity = WE_Parity_Even;
    Calypso_uart.uartInit = WE_UART1_Init;
    Calypso_uart.uartDeinit = WE_UART1_DeInit;
    Calypso_uart.uartTransmit = WE_UART1_Transmit;

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

    return;
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
void Calypso_Examples_EventCallback(char* eventText)
{
    Calypso_ATEvent_t event;
    if (false == Calypso_ATEvent_ParseEventType(&eventText, &event))
    {
        return;
    }

    switch (event)
    {
        case Calypso_ATEvent_Startup:
            if (Calypso_ATEvent_ParseStartUpEvent(&eventText, &Calypso_Examples_startupEvent))
            {
                WE_DEBUG_PRINT("Startup event received. "
                               "Article nr: %s, "
                               "Chip ID: %s, "
                               "MAC address: %s, "
                               "Firmware version: %d.%d.%d\r\n",
                               Calypso_Examples_startupEvent.articleNr, Calypso_Examples_startupEvent.chipID, Calypso_Examples_startupEvent.MACAddress, Calypso_Examples_startupEvent.firmwareVersion[0], Calypso_Examples_startupEvent.firmwareVersion[1], Calypso_Examples_startupEvent.firmwareVersion[2]);
            }
            Calypso_Examples_startupEventReceived = true;
            break;

        case Calypso_ATEvent_NetappIP4Acquired:
            Calypso_Examples_ip4Acquired = true;
            break;

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
        case Calypso_ATEvent_NetappIP6Acquired:
        case Calypso_ATEvent_NetappIPCollision:
        case Calypso_ATEvent_NetappDHCPv4_leased:
        case Calypso_ATEvent_NetappDHCPv4_released:
        case Calypso_ATEvent_NetappIPv4Lost:
        case Calypso_ATEvent_NetappDHCPIPv4AcquireTimeout:
        case Calypso_ATEvent_NetappIPv6Lost:
        case Calypso_ATEvent_MQTTConnack:
        case Calypso_ATEvent_MQTTPuback:
        case Calypso_ATEvent_MQTTSuback:
        case Calypso_ATEvent_MQTTUnsuback:
        case Calypso_ATEvent_MQTTRecv:
        case Calypso_ATEvent_MQTTDisconnect:
        case Calypso_ATEvent_FileListEntry:
        case Calypso_ATEvent_HTTPGet:
        case Calypso_ATEvent_CustomGPIO:
        case Calypso_ATEvent_CustomHTTPPost:
        case Calypso_ATEvent_FatalErrorDeviceAbort:
        case Calypso_ATEvent_FatalErrorDriverAbort:
        case Calypso_ATEvent_FatalErrorSyncLost:
        case Calypso_ATEvent_FatalErrorNoCmdAck:
        case Calypso_ATEvent_FatalErrorCmdTimeout:
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
void Calypso_Examples_Print(char* str, bool success) { WE_DEBUG_PRINT("%s%s\r\n", success ? "OK    " : "NOK   ", str); }
