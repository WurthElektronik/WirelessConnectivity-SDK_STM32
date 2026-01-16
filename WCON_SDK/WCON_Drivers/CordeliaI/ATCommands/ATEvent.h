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
 * @file ATEvent.h
 * @brief AT event definitions.
 */

#ifndef CORDELIAI_AT_EVENTS_H_INCLUDED
#define CORDELIAI_AT_EVENTS_H_INCLUDED
#include <CordeliaI/CordeliaI.h>
#include <global/ATCommands.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief AT event IDs.
 */
typedef enum CordeliaI_ATEvent_t
{
    CordeliaI_ATEvent_Invalid = (uint16_t)0,
    CordeliaI_ATEvent_Startup,
    CordeliaI_ATEvent_GeneralResetRequest,
    CordeliaI_ATEvent_GeneralError,
    CordeliaI_ATEvent_WlanConnect,
    CordeliaI_ATEvent_WlanDisconnect,
    CordeliaI_ATEvent_WlanStaAdded,
    CordeliaI_ATEvent_WlanStaRemoved,
    CordeliaI_ATEvent_SocketTxFailed,
    CordeliaI_ATEvent_SocketAsyncEvent,
    CordeliaI_ATEvent_NetappIP4Acquired,
    CordeliaI_ATEvent_MQTTError,
    CordeliaI_ATEvent_MQTTRecv,
    CordeliaI_ATEvent_MQTTInfo,
    CordeliaI_ATEvent_IoTInfo,
    CordeliaI_ATEvent_IoTError,
    CordeliaI_ATEvent_FatalErrorDeviceAbort,
    CordeliaI_ATEvent_FatalErrorDriverAbort,
    CordeliaI_ATEvent_FatalErrorSyncLost,
    CordeliaI_ATEvent_FatalErrorNoCmdAck,
    CordeliaI_ATEvent_FatalErrorCmdTimeout,
    /** @cond DOXYGEN_IGNORE */
    CordeliaI_ATEvent_NumberOfValues,
    CordeliaI_ATEvent_Max = UINT16_MAX
    /** @endcond */
} CordeliaI_ATEvent_t;

/**
 * @brief Custom event IDs (first argument of "+eventcustom" event).
 */
typedef enum CordeliaI_ATEvent_CustomEventID_t
{
    CordeliaI_ATEvent_CustomEventID_GPIO,
    CordeliaI_ATEvent_CustomEventID_HTTPPost,
    /** @cond DOXYGEN_IGNORE */
    CordeliaI_ATEvent_CustomEventID_NumberOfValues
    /** @endcond */
} CordeliaI_ATEvent_CustomEventID_t;

/**
 * @brief Parameters of startup event (CordeliaI_ATEvent_Startup).
 */
typedef struct CordeliaI_ATEvent_Startup_t
{
    char articleNr[16];
    char chipID[12];
    char MACAddress[18];
    uint8_t firmwareVersion[3];
} CordeliaI_ATEvent_Startup_t;

/**
 * @brief Parameters of IoT event (CordeliaI_ATEvent_Startup).
 */
typedef struct CordeliaI_ATEvent_IoT_t
{
    char statusMessage[32];
    uint16_t statusCode;
} CordeliaI_ATEvent_IoT_t;

/**
 * @brief Parameters of MQTT event (CordeliaI_ATEvent_Startup).
 */
typedef struct CordeliaI_ATEvent_MQTT_t
{
    char statusMessage[32];
    uint16_t statusCode;
} CordeliaI_ATEvent_MQTT_t;

/**
 * @brief Parameters of MQTT recv event (CordeliaI_ATEvent_Startup).
 */
typedef struct CordeliaI_ATEvent_MQTTRecv_t
{
    char topic[32];
    char qos[8];
    char message[128];
} CordeliaI_ATEvent_MQTTRecv_t;

/**
 * @brief Parameters of IPv4 acquired event (CordeliaI_ATEvent_NetappIP4Acquired).
 */
typedef struct CordeliaI_ATEvent_NetappIP4Acquired_t
{
    char address[16];
    char gateway[16];
    char DNS[16];
} CordeliaI_ATEvent_NetappIP4Acquired_t;

/**
 * @brief Parses the received AT command and returns the corresponding CordeliaI_ATEvent_t.
 *
 * @param[in,out] pAtCommand: AT command starting with '+'
 * @param[out] pEvent: CordeliaI_ATEvent_t representing the event
 *
 * @return True if parsed successfully, false otherwise
 */
extern bool CordeliaI_ATEvent_ParseEventType(char** pAtCommand, CordeliaI_ATEvent_t* pEvent);

/**
 * @brief Parses the values of the startup event arguments.
 *
 * @param[in,out] pEventArguments: String containing arguments of the AT command
 * @param[out] CordeliaI_Examples_startupEvent: The parsed startup event data
 *
 * @return True if parsed successfully, false otherwise
 */
extern bool CordeliaI_ATEvent_ParseStartUpEvent(char** pEventArguments, CordeliaI_ATEvent_Startup_t* CordeliaI_Examples_startupEvent);

/**
 * @brief Parses the values of the IoT event arguments.
 *
 * @param[in,out] pEventArguments: String containing arguments of the AT command.
 * @param[out] CordeliaI_Examples_IoTEvent: The parsed IoT event data.
 *
 * @return True if parsed successfully, false otherwise.
 */
extern bool CordeliaI_ATEvent_ParseIoTEvent(char** pEventArguments, CordeliaI_ATEvent_IoT_t* CordeliaI_Examples_IoTEvent);

/**
 * @brief Parses the values of the MQTT event arguments.
 *
 * @param[in,out] pEventArguments: String containing arguments of the AT command.
 * @param[out] CordeliaI_Examples_MQTTEvent: The parsed MQTT event data.
 *
 * @return True if parsed successfully, false otherwise.
 */
extern bool CordeliaI_ATEvent_ParseMQTTEvent(char** pEventArguments, CordeliaI_ATEvent_MQTT_t* CordeliaI_Examples_MQTTEvent);

/**
 * @brief Parses the values of the IPv4 acquired event arguments.
 *
 * @param[in,out] pEventArguments: String containing arguments of the AT command
 * @param[out] ipv4Event: The parsed IPv4 acquired event data
 *
 * @return True if parsed successfully, false otherwise
 */
extern bool CordeliaI_ATEvent_ParseNetappIP4AcquiredEvent(char** pEventArguments, CordeliaI_ATEvent_NetappIP4Acquired_t* ipv4Event);

/**
 * @brief Parses the values of the MQTT data received event.
 *
 * @param[in,out] pEventArguments: String containing arguments of the event
 * @param[out] rcvdEvent: The parsed mqtt received data from event
 *
 * @return True if parsed successfully, false otherwise
 */
extern bool CordeliaI_ATEvent_ParseSocketMQTTRcvdEvent(char** pEventArguments, CordeliaI_ATEvent_MQTTRecv_t* rcvdEvent);

#ifdef __cplusplus
}
#endif

#endif // CORDELIAI_AT_EVENTS_H_INCLUDED
