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
 * @brief CordeliaI examples.
 *
 * Comment/uncomment lines in CordeliaI_Examples() to start the desired example.
 */
#include <CordeliaI/ATCommands/ATEvent.h>
#include <CordeliaI/CordeliaI.h>
#include <CordeliaI/CordeliaI_Examples.h>
#include <CordeliaI/CordeliaI_Quarklink_Example.h>
#include <global/global_types.h>
#include <global_platform_types.h>
#include <stdio.h>
#include <string.h>

/**
 * @brief Contains information on last startup event (if any)
 */
CordeliaI_ATEvent_Startup_t CordeliaI_Examples_startupEvent = {0};

/**
 * @brief Contains information on IoT event (if any)
 */
CordeliaI_ATEvent_IoT_t CordeliaI_Examples_IoTEvent = {0};

/**
 * @brief Contains information on MQTT event (if any)
 */
CordeliaI_ATEvent_MQTT_t CordeliaI_Examples_MQTTEvent = {0};

/**
 * @brief Contains information on MQTT recv event (if any)
 */
CordeliaI_ATEvent_MQTTRecv_t CordeliaI_Examples_MQTTRecvEvent = {0};

/**
 * @brief Is set to true when a startup event is received
 */
bool CordeliaI_Examples_startupEventReceived = false;

/**
 * @brief Is set to true when a IoT info event with complete status received
 */
bool CordeliaI_Examples_IoTEventEnrolComplete = false;

/**
 * @brief Is set to true when MQTT info event with CONNACK status received
 */
bool CordeliaI_Examples_IoTConnect = false;

/**
 * @brief Is set to true when an IPv4 acquired event is received
 */
bool CordeliaI_Examples_ip4Acquired = false;

CordeliaI_Pins_t CordeliaI_pins = {
    .CordeliaI_Pin_Reset = WE_PIN((void*)&WE_STM32_PIN(GPIOA, GPIO_PIN_10)),
    .CordeliaI_Pin_WakeUp = WE_PIN((void*)&WE_STM32_PIN(GPIOA, GPIO_PIN_9)),
    .CordeliaI_Pin_Boot = WE_PIN((void*)&WE_STM32_PIN(GPIOA, GPIO_PIN_7)),
    .CordeliaI_Pin_AppMode0 = WE_PIN((void*)&WE_STM32_PIN(GPIOA, GPIO_PIN_0)),
    .CordeliaI_Pin_AppMode1 = WE_PIN((void*)&WE_STM32_PIN(GPIOA, GPIO_PIN_1)),
    .CordeliaI_Pin_StatusInd0 = WE_PIN((void*)&WE_STM32_PIN(GPIOB, GPIO_PIN_8)),
    .CordeliaI_Pin_StatusInd1 = WE_PIN((void*)&WE_STM32_PIN(GPIOB, GPIO_PIN_9)),
};

WE_UART_t CordeliaI_uart;

/**
 * @brief Waits for the startup event to be received.
 */
bool CordeliaI_Examples_WaitForStartup(uint32_t timeoutMs)
{
    uint32_t t0 = WE_GetTick();
    CordeliaI_Examples_startupEventReceived = false;
    while (false == CordeliaI_Examples_startupEventReceived && (WE_GetTick() - t0) < timeoutMs)
    {
    }
    return CordeliaI_Examples_startupEventReceived;
}

/**
 * @brief Waits for IoT Enrol info event to be received with status complete.
 */
bool CordeliaI_Examples_WaitForIoTEnrolCompleteEvent(uint32_t timeoutMs)
{
    uint32_t t0 = WE_GetTick();
    CordeliaI_Examples_IoTEventEnrolComplete = false;
    while (false == CordeliaI_Examples_IoTEventEnrolComplete && (WE_GetTick() - t0) < timeoutMs)
    {
    }
    return CordeliaI_Examples_IoTEventEnrolComplete;
}

/**
 * @brief Waits for MQTT info connect event to be received with status CONNACK.
 */
bool CordeliaI_Examples_WaitForIoTConnectEvent(uint32_t timeoutMs)
{
    uint32_t t0 = WE_GetTick();
    CordeliaI_Examples_IoTConnect = false;
    while (false == CordeliaI_Examples_IoTConnect && (WE_GetTick() - t0) < timeoutMs)
    {
        // Optionally add a small delay here if needed to avoid tight looping
    }
    return CordeliaI_Examples_IoTConnect;
}
/**
 * @brief Waits for an IPv4 acquired event to be received.
 */
bool CordeliaI_Examples_WaitForIPv4Acquired(uint32_t timeoutMs)
{
    uint32_t t0 = WE_GetTick();
    CordeliaI_Examples_ip4Acquired = false;
    while (false == CordeliaI_Examples_ip4Acquired && (WE_GetTick() - t0) < timeoutMs)
    {
    }
    return CordeliaI_Examples_ip4Acquired;
}

/**
 * @brief Runs CordeliaI examples.
 *
 * Comment/uncomment lines in this function to start the desired example.
 */
void CordeliaI_Examples(void)
{

    CordeliaI_uart.baudrate = 115200;
    CordeliaI_uart.flowControl = WE_FlowControl_NoFlowControl;
    CordeliaI_uart.parity = WE_Parity_None;
    CordeliaI_uart.uartInit = WE_UART1_Init;
    CordeliaI_uart.uartDeinit = WE_UART1_DeInit;
    CordeliaI_uart.uartTransmit = WE_UART1_Transmit;

    CordeliaI_Quarklink_Example();

    return;
}

/**
 * @brief Is called when an event notification has been received.
 *
 * Note that this function is called from an interrupt - code in this function
 * should thus be kept simple.
 *
 * Note in particular that it is not possible to send AT commands to CordeliaI
 * from within this event handler.
 *
 * Also note that not all calls of this handler necessarily correspond to valid
 * events (i.e. events from CordeliaI_ATEvent_t). Some events might in fact be responses
 * to AT commands that are not included in CordeliaI_ATEvent_t.
 */
void CordeliaI_Examples_EventCallback(char* eventText)
{
    CordeliaI_ATEvent_t event;
    if (false == CordeliaI_ATEvent_ParseEventType(&eventText, &event))
    {
        return;
    }

    switch (event)
    {
        case CordeliaI_ATEvent_Startup:
            if (CordeliaI_ATEvent_ParseStartUpEvent(&eventText, &CordeliaI_Examples_startupEvent))
            {
                printf("Startup event received. "
                       "Article nr: %s, "
                       "Chip ID: %s, "
                       "MAC address: %s, "
                       "Firmware version: %d.%d.%d\r\n",
                       CordeliaI_Examples_startupEvent.articleNr, CordeliaI_Examples_startupEvent.chipID, CordeliaI_Examples_startupEvent.MACAddress, CordeliaI_Examples_startupEvent.firmwareVersion[0], CordeliaI_Examples_startupEvent.firmwareVersion[1], CordeliaI_Examples_startupEvent.firmwareVersion[2]);
            }
            CordeliaI_Examples_startupEventReceived = true;
            break;
        case CordeliaI_ATEvent_IoTInfo:
            if (CordeliaI_ATEvent_ParseIoTEvent(&eventText, &CordeliaI_Examples_IoTEvent))
            {
                printf("IoT Info event received. "
                       "Status message: %s, "
                       "Status code: %u\r\n",
                       CordeliaI_Examples_IoTEvent.statusMessage, CordeliaI_Examples_IoTEvent.statusCode);
            }
            if (strncmp(CordeliaI_Examples_IoTEvent.statusMessage, "\"ENROLMENT_COMPLETE\"", strlen("\"ENROLMENT_COMPLETE\"")) == 0)
            {
                CordeliaI_Examples_IoTEventEnrolComplete = true;
            }
            break;
        case CordeliaI_ATEvent_IoTError:
            if (CordeliaI_ATEvent_ParseIoTEvent(&eventText, &CordeliaI_Examples_IoTEvent))
            {
                printf("IoT Error event received. "
                       "Status message: %s, "
                       "Status code: %u\r\n",
                       CordeliaI_Examples_IoTEvent.statusMessage, CordeliaI_Examples_IoTEvent.statusCode);
            }
            break;
        case CordeliaI_ATEvent_NetappIP4Acquired:
            CordeliaI_Examples_ip4Acquired = true;
            break;
        case CordeliaI_ATEvent_MQTTInfo:
            if (CordeliaI_ATEvent_ParseMQTTEvent(&eventText, &CordeliaI_Examples_MQTTEvent))
            {
                printf("MQTT Info event received. "
                       "Status message: %s, "
                       "Status code: %u\r\n",
                       CordeliaI_Examples_MQTTEvent.statusMessage, CordeliaI_Examples_MQTTEvent.statusCode);
            }
            if (strncmp(CordeliaI_Examples_MQTTEvent.statusMessage, "\"CONNACK\"", strlen("\"CONNACK\"")) == 0)
            {
                CordeliaI_Examples_IoTConnect = true;
            }
            break;
        case CordeliaI_ATEvent_MQTTError:
            if (CordeliaI_ATEvent_ParseMQTTEvent(&eventText, &CordeliaI_Examples_MQTTEvent))
            {
                printf("MQTT Error event received. "
                       "Status message: %s, "
                       "Status code: %u\r\n",
                       CordeliaI_Examples_MQTTEvent.statusMessage, CordeliaI_Examples_MQTTEvent.statusCode);
            }
            break;
        case CordeliaI_ATEvent_MQTTRecv:
            if (CordeliaI_ATEvent_ParseSocketMQTTRcvdEvent(&eventText, &CordeliaI_Examples_MQTTRecvEvent))
            {
                printf("MQTT message received. "
                       "Topic: %s, "
                       "QoS: %s, "
                       "Message: %s\r\n",
                       CordeliaI_Examples_MQTTRecvEvent.topic, CordeliaI_Examples_MQTTRecvEvent.qos, CordeliaI_Examples_MQTTRecvEvent.message);
            }
            break;
        case CordeliaI_ATEvent_SocketTxFailed:
        case CordeliaI_ATEvent_SocketAsyncEvent:
        case CordeliaI_ATEvent_GeneralResetRequest:
        case CordeliaI_ATEvent_GeneralError:
        case CordeliaI_ATEvent_WlanConnect:
        case CordeliaI_ATEvent_WlanDisconnect:
        case CordeliaI_ATEvent_WlanStaAdded:
        case CordeliaI_ATEvent_WlanStaRemoved:
        case CordeliaI_ATEvent_FatalErrorDeviceAbort:
        case CordeliaI_ATEvent_FatalErrorDriverAbort:
        case CordeliaI_ATEvent_FatalErrorSyncLost:
        case CordeliaI_ATEvent_FatalErrorNoCmdAck:
        case CordeliaI_ATEvent_FatalErrorCmdTimeout:
            break;
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
void CordeliaI_Examples_Print(char* str, bool success) { printf("%s%s\r\n", success ? "OK    " : "NOK   ", str); }
