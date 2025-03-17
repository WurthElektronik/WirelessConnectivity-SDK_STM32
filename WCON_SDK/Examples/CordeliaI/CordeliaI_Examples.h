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

#ifndef CORDELIAI_EXAMPLES_H_INCLUDED
#define CORDELIAI_EXAMPLES_H_INCLUDED
#include <CordeliaI/ATCommands/ATEvent.h>
#include <CordeliaI/CordeliaI.h>
#include <global/global.h>

#ifdef __cplusplus
extern "C"
{
#endif

    extern const char* CordeliaI_Examples_wlanSSID;
    extern const char* CordeliaI_Examples_wlanKey;
    extern CordeliaI_ATEvent_Startup_t CordeliaI_Examples_startupEvent;
    extern CordeliaI_ATEvent_IoT_t CordeliaI_Examples_IoTEvent;
    extern CordeliaI_ATEvent_MQTT_t CordeliaI_Examples_MQTTEvent;
    extern CordeliaI_ATEvent_MQTTRecv_t CordeliaI_Examples_MQTTRecvEvent;
    extern bool CordeliaI_Examples_startupEventReceived;
    extern bool CordeliaI_Examples_ip4Acquired;
    extern CordeliaI_Pins_t CordeliaI_pins;
    extern WE_UART_t CordeliaI_uart;
    extern void CordeliaI_Examples(void);
    extern void CordeliaI_Examples_Print(char* str, bool success);
    extern bool CordeliaI_Examples_WaitForStartup(uint32_t timeoutMs);
    extern bool CordeliaI_Examples_WaitForIoTEnrolCompleteEvent(uint32_t timeoutMs);
    extern bool CordeliaI_Examples_WaitForIoTConnectEvent(uint32_t timeoutMs);
    extern bool CordeliaI_Examples_WaitForIPv4Acquired(uint32_t timeoutMs);
    extern void CordeliaI_Examples_EventCallback(char* eventText);

#ifdef __cplusplus
}
#endif

#endif /* CORDELIAI_EXAMPLES_H_INCLUDED */
