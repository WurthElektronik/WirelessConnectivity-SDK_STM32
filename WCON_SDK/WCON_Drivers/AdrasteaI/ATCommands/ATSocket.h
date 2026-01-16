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
 * @file ATSocket.h
 * @brief AT commands for Socket functionality.
 */

#ifndef ADRASTEAI_AT_SOCKET_H_INCLUDED
#define ADRASTEAI_AT_SOCKET_H_INCLUDED

#include <AdrasteaI/ATCommands/ATCommon.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief Socket Events
 */
typedef enum AdrasteaI_ATSocket_Event_t
{
    AdrasteaI_ATSocket_Event_Invalid = -1,
    AdrasteaI_ATSocket_Event_All,
    AdrasteaI_ATSocket_Event_Data_Received,
    AdrasteaI_ATSocket_Event_Deactivated_Idle_Timer,
    AdrasteaI_ATSocket_Event_Terminated_By_Peer,
    AdrasteaI_ATSocket_Event_New_Socket_Accepted,
    /** @cond DOXYGEN_IGNORE */
    AdrasteaI_ATSocket_Event_NumberOfValues
    /** @endcond */
} AdrasteaI_ATSocket_Event_t;

typedef uint8_t AdrasteaI_ATSocket_ID_t;

#define AdrasteaI_ATSocket_ID_Invalid 0

/**
 * @brief Socket States
 */
typedef enum AdrasteaI_ATSocket_State_t
{
    AdrasteaI_ATSocket_State_Invalid = -1,
    AdrasteaI_ATSocket_State_Deactivated,
    AdrasteaI_ATSocket_State_Activated,
    AdrasteaI_ATSocket_State_Listening,
    /** @cond DOXYGEN_IGNORE */
    AdrasteaI_ATSocket_State_NumberOfValues
    /** @endcond */
} AdrasteaI_ATSocket_State_t;

/**
 * @brief Socket Read Result
 */
typedef struct AdrasteaI_ATSocket_Read_Result_t
{
    AdrasteaI_ATSocket_ID_t socketID;
    AdrasteaI_ATSocket_State_t socketState;
} AdrasteaI_ATSocket_Read_Result_t;

typedef uint8_t AdrasteaI_ATSocket_Error_Code_t;

/**
 * @brief Socket Types
 */
typedef enum AdrasteaI_ATSocket_Type_t
{
    AdrasteaI_ATSocket_Type_Invalid = -1,
    AdrasteaI_ATSocket_Type_TCP,
    AdrasteaI_ATSocket_Type_UDP,
    /** @cond DOXYGEN_IGNORE */
    AdrasteaI_ATSocket_Type_NumberOfValues
    /** @endcond */
} AdrasteaI_ATSocket_Type_t;

/**
 * @brief Socket Direction
 */
typedef enum AdrasteaI_ATSocket_Direction_t
{
    AdrasteaI_ATSocket_Direction_Invalid = -1,
    AdrasteaI_ATSocket_Direction_Not_Set,
    AdrasteaI_ATSocket_Direction_Dialer,
    AdrasteaI_ATSocket_Direction_Listener,
    /** @cond DOXYGEN_IGNORE */
    AdrasteaI_ATSocket_Direction_NumberOfValues
    /** @endcond */
} AdrasteaI_ATSocket_Direction_t;

typedef uint16_t AdrasteaI_ATSocket_Timeout_t;

#define AdrasteaI_ATSocket_Timeout_Invalid 0

/**
 * @brief Socket Info
 */
typedef struct AdrasteaI_ATSocket_Info_t
{
    AdrasteaI_ATSocket_State_t socketState;
    AdrasteaI_ATSocket_Type_t socketType;
    AdrasteaI_ATCommon_IP_Addr_t sourceIPAddress;
    AdrasteaI_ATCommon_IP_Addr_t destinationIPAddress;
    AdrasteaI_ATCommon_Port_Number_t sourcePortNumber;
    AdrasteaI_ATCommon_Port_Number_t destinationPortNumber;
    AdrasteaI_ATSocket_Direction_t tcpSocketDirection;
    AdrasteaI_ATSocket_Timeout_t socketTimeout;
} AdrasteaI_ATSocket_Info_t;

/**
 * @brief Socket Behaviour
 */
typedef enum AdrasteaI_ATSocket_Behaviour_t
{
    AdrasteaI_ATSocket_Behaviour_Invalid = -1,
    AdrasteaI_ATSocket_Behaviour_Open_Connection,
    AdrasteaI_ATSocket_Behaviour_Listen,
    AdrasteaI_ATSocket_Behaviour_ListenMultiple,
    /** @cond DOXYGEN_IGNORE */
    AdrasteaI_ATSocket_Behaviour_NumberOfValues
    /** @endcond */
} AdrasteaI_ATSocket_Behaviour_t;

typedef uint16_t AdrasteaI_ATSocket_Data_Length_t;

#define AdrasteaI_ATSocket_Data_Length_Automatic 0

/**
 * @brief Socket Data Read
 */
typedef struct AdrasteaI_ATSocket_Data_Read_t
{
    AdrasteaI_ATSocket_ID_t socketID;
    AdrasteaI_ATSocket_Data_Length_t dataLength;
    char* data;
    AdrasteaI_ATSocket_Data_Length_t dataLeftLength;
    AdrasteaI_ATCommon_IP_Addr_t sourceIPAddress;
    AdrasteaI_ATCommon_Port_Number_t sourcePortNumber;
} AdrasteaI_ATSocket_Data_Read_t;

/**
 * @brief Socket IP Address Format
 */
typedef enum AdrasteaI_ATSocket_IP_Addr_Format_t
{
    AdrasteaI_ATSocket_IP_Addr_Format_Invalid = -1,
    AdrasteaI_ATSocket_IP_Addr_Format_IPv4v6,
    AdrasteaI_ATSocket_IP_Addr_Format_IPv4,
    AdrasteaI_ATSocket_IP_Addr_Format_IPv6,
    /** @cond DOXYGEN_IGNORE */
    AdrasteaI_ATSocket_IP_Addr_Format_NumberOfValues
    /** @endcond */
} AdrasteaI_ATSocket_IP_Addr_Format_t;

typedef struct AdrasteaI_ATSocket_SSL_Info_t
{
    AdrasteaI_ATCommon_SSL_Auth_Mode_t SSLMode;
    AdrasteaI_ATCommon_SSL_Profile_ID_t profileID;
} AdrasteaI_ATSocket_SSL_Info_t;

typedef uint16_t AdrasteaI_ATSocket_Aggregation_Time_t;

typedef uint16_t AdrasteaI_ATSocket_Aggregation_Buffer_Size_t;

typedef uint16_t AdrasteaI_ATSocket_TCP_Idle_Time_t;

/**
 * @brief Read Created Sockets States (using the AT%SOCKETCMD command).
 *
 * @return True if successful, false otherwise
 */
extern bool AdrasteaI_ATSocket_ReadCreatedSocketsStates();

/**
 * @brief Allocate Socket (using the AT%SOCKETCMD command).
 *
 * @param[in] sessionID: Session ID.
 *
 * @param[in] socketType: Socket Type. See AdrasteaI_ATSocket_Type_t.
 *
 * @param[in] socketBehaviour: Socket Behaviour. See AdrasteaI_ATSocket_Behaviour_t.
 *
 * @param[in] destinationIPAddress: Destination IP Address.
 *
 * @param[in] destinationPortNumber: Destination Port Number.
 *
 * @param[in] sourcePortNumber: Source Port Number (optional pass AdrasteaI_ATCommon_Port_Number_Invalid to skip).
 *
 * @param[in] packetSize: Packet Size (optional pass AdrasteaI_ATSocket_Data_Length_Automatic to skip).
 *
 * @param[in] socketTimeout: Socket Timeout in seconds (optional pass AdrasteaI_ATSocket_Timeout_Invalid to skip).
 *
 * @param[in] addressFormat: IP Address Format (optional pass AdrasteaI_ATSocket_IP_Addr_Format_Invalid to skip). See AdrasteaI_ATSocket_IP_Addr_Format_t.
 *
 * @param[out] socketIDP: Socket ID.
 *
 * @return True if successful, false otherwise
 */
extern bool AdrasteaI_ATSocket_AllocateSocket(AdrasteaI_ATCommon_Session_ID_t sessionID, AdrasteaI_ATSocket_Type_t socketType, AdrasteaI_ATSocket_Behaviour_t socketBehaviour, AdrasteaI_ATCommon_IP_Addr_t destinationIPAddress, AdrasteaI_ATCommon_Port_Number_t destinationPortNumber, AdrasteaI_ATCommon_Port_Number_t sourcePortNumber, AdrasteaI_ATSocket_Data_Length_t packetSize,
                                              AdrasteaI_ATSocket_Timeout_t socketTimeout, AdrasteaI_ATSocket_IP_Addr_Format_t addressFormat, AdrasteaI_ATSocket_ID_t* socketIDP);

/**
 * @brief Activate Socket (using the AT%SOCKETCMD command).
 *
 * @param[in] socketID: Socket ID.
 *
 * @param[in] SSLSessionID: SSL Session ID (optional pass AdrasteaI_ATCommon_Session_ID_Invalid to skip).
 *
 * @return True if successful, false otherwise
 */
extern bool AdrasteaI_ATSocket_ActivateSocket(AdrasteaI_ATSocket_ID_t socketID, AdrasteaI_ATCommon_Session_ID_t SSLSessionID);

/**
 * @brief Read Socket Info (using the AT%SOCKETCMD command).
 *
 * @param[in] socketID: Socket ID.
 *
 * @param[out] infoP: Socket Info is returned in this argument. See AdrasteaI_ATSocket_Info_t.
 *
 * @return True if successful, false otherwise
 */
extern bool AdrasteaI_ATSocket_ReadSocketInfo(AdrasteaI_ATSocket_ID_t socketID, AdrasteaI_ATSocket_Info_t* infoP);

/**
 * @brief Deactivate Socket (using the AT%SOCKETCMD command).
 *
 * @param[in] socketID: Socket ID.
 *
 * @return True if successful, false otherwise
 */
extern bool AdrasteaI_ATSocket_DeactivateSocket(AdrasteaI_ATSocket_ID_t socketID);

/**
 * @brief Set Socket Options (using the AT%SOCKETCMD command).
 *
 * @param[in] socketID: Socket ID.
 *
 * @param[in] aggregationTime: Aggregation Time in milliseconds.
 *
 * @param[in] aggregationBufferSize: Aggregation Buffer Size in bytes.
 *
 * @param[in] idleTime: Idle Time in seconds.
 *
 * @return True if successful, false otherwise
 */
extern bool AdrasteaI_ATSocket_SetSocketOptions(AdrasteaI_ATSocket_ID_t socketID, AdrasteaI_ATSocket_Aggregation_Time_t aggregationTime, AdrasteaI_ATSocket_Aggregation_Buffer_Size_t aggregationBufferSize, AdrasteaI_ATSocket_TCP_Idle_Time_t idleTime);

/**
 * @brief Delete Socket (using the AT%SOCKETCMD command).
 *
 * @param[in] socketID: Socket ID.
 *
 * @return True if successful, false otherwise
 */
extern bool AdrasteaI_ATSocket_DeleteSocket(AdrasteaI_ATSocket_ID_t socketID);

/**
 * @brief Read Socket Last Error (using the AT%SOCKETCMD command).
 *
 * @param[in] socketID: Socket ID.
 *
 * @param[out] errorCodeP: Socket Error Code is returned in this argument. See AdrasteaI_ATSocket_Error_Code_t.
 *
 * @return True if successful, false otherwise
 */
extern bool AdrasteaI_ATSocket_ReadSocketLastError(AdrasteaI_ATSocket_ID_t socketID, AdrasteaI_ATSocket_Error_Code_t* errorCodeP);

/**
 * @brief Add SSL to Socket (using the AT%SOCKETCMD command).
 *
 * @param[in] socketID: Socket ID.
 *
 * @param[in] authMode: SSL Authentication Mode.
 *
 * @param[in] profileID: SSL Profile ID.
 *
 * @return True if successful, false otherwise
 */
extern bool AdrasteaI_ATSocket_AddSSLtoSocket(AdrasteaI_ATSocket_ID_t socketID, AdrasteaI_ATCommon_SSL_Auth_Mode_t authMode, AdrasteaI_ATCommon_SSL_Profile_ID_t profileID);

/**
 * @brief Read Socket SSL Info (using the AT%SOCKETCMD command).
 *
 * @param[in] socketID: Socket ID.
 *
 * @param[out] infoP: SSL Info is returned in this argument. See AdrasteaI_ATSocket_SSL_Info_t.
 *
 * @return True if successful, false otherwise
 */
extern bool AdrasteaI_ATSocket_ReadSocketSSLInfo(AdrasteaI_ATSocket_ID_t socketID, AdrasteaI_ATSocket_SSL_Info_t* infoP);

/**
 * @brief Keep SSL Session for Socket (using the AT%SOCKETCMD command).
 *
 * @param[in] socketID: Socket ID.
 *
 * @return True if successful, false otherwise
 */
extern bool AdrasteaI_ATSocket_KeepSocketSSLSession(AdrasteaI_ATSocket_ID_t socketID);

/**
 * @brief Delete SSL Session for Socket (using the AT%SOCKETCMD command).
 *
 * @param[in] socketID: Socket ID.
 *
 * @return True if successful, false otherwise
 */
extern bool AdrasteaI_ATSocket_DeleteSocketSSLSession(AdrasteaI_ATSocket_ID_t socketID);

/**
 * @brief Receive from Socket (using the AT%SOCKETDATA command).
 *
 * @param[in] socketID: Socket ID.
 *
 * @param[in] maxBufferLength: Maximum data length to read.
 *
 * @param[out] dataReadP: Data read is returned in this argument.
 *
 * @return True if successful, false otherwise
 */
extern bool AdrasteaI_ATSocket_ReceiveFromSocket(AdrasteaI_ATSocket_ID_t socketID, AdrasteaI_ATSocket_Data_Read_t* dataReadP, uint16_t maxBufferLength);

/**
 * @brief Send to Socket (using the AT%SOCKETDATA command).
 *
 * @param[in] socketID: Socket ID.
 *
 * @param[in] data: Data to send.
 *
 * @param[in] dataLength: Length of data to send.
 *
 * @return True if successful, false otherwise
 */
extern bool AdrasteaI_ATSocket_SendToSocket(AdrasteaI_ATSocket_ID_t socketID, char* data, AdrasteaI_ATSocket_Data_Length_t dataLength);

/**
 * @brief Set Socket Notification Events (using the AT%SOCKETEV command).
 *
 * @param[in] event: Socket event type. See AdrasteaI_ATSocket_Event_t.
 *
 * @param[in] state: Event State
 *
 * @return True if successful, false otherwise
 */
extern bool AdrasteaI_ATSocket_SetSocketUnsolicitedNotificationEvents(AdrasteaI_ATSocket_Event_t event, AdrasteaI_ATCommon_Event_State_t state);

/**
 * @brief Parses the value of Data Received event arguments.
 *
 * @param[in] pEventArguments: String containing arguments of the AT command
 * @param[out] dataP: ID of Socket where data was received is returned in this argument.
 *
 * @return True if successful, false otherwise
 */
extern bool AdrasteaI_ATSocket_ParseDataReceivedEvent(char* pEventArguments, AdrasteaI_ATSocket_ID_t* dataP);

/**
 * @brief Parses the value of Socket Terminated event arguments.
 *
 * @param[in] pEventArguments: String containing arguments of the AT command
 * @param[out] dataP: ID of Socket that was terminated is returned in this argument.
 *
 * @return True if successful, false otherwise
 */
extern bool AdrasteaI_ATSocket_ParseSocketTerminatedEvent(char* pEventArguments, AdrasteaI_ATSocket_ID_t* dataP);

/**
 * @brief Parses the value of Socket Read event arguments.
 *
 * @param[in] pEventArguments: String containing arguments of the AT command
 * @param[out] dataP: Socket Read Result si returned in this argument. See AdrasteaI_ATSocket_Read_Result_t.
 *
 * @return True if successful, false otherwise
 */
extern bool AdrasteaI_ATSocket_ParseSocketsReadEvent(char* pEventArguments, AdrasteaI_ATSocket_Read_Result_t* dataP);

#ifdef __cplusplus
}
#endif

#endif /* ADRASTEAI_AT_SOCKET_H_INCLUDED */
