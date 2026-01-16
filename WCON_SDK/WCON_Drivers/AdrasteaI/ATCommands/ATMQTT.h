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
 * @file ATMQTT.h
 * @brief AT commands for MQTT functionality.
 */

#ifndef ADRASTEAI_AT_MQTT_H_INCLUDED
#define ADRASTEAI_AT_MQTT_H_INCLUDED

#include <AdrasteaI/ATCommands/ATCommon.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief MQTT Connection IDs
 */
typedef enum AdrasteaI_ATMQTT_Conn_ID_t
{
    AdrasteaI_ATMQTT_Conn_ID_Invalid = -1,
    AdrasteaI_ATMQTT_Conn_ID_Single_Connectivity_Mode,
    AdrasteaI_ATMQTT_Conn_ID_1,
    AdrasteaI_ATMQTT_Conn_ID_2,
    AdrasteaI_ATMQTT_Conn_ID_3,
    AdrasteaI_ATMQTT_Conn_ID_4,
    AdrasteaI_ATMQTT_Conn_ID_5,
    /** @cond DOXYGEN_IGNORE */
    AdrasteaI_ATMQTT_Conn_ID_NumberOfValues
    /** @endcond */
} AdrasteaI_ATMQTT_Conn_ID_t;

typedef char AdrasteaI_ATMQTT_Client_ID_t[64];

typedef enum AdrasteaI_ATMQTT_IP_Session_ID_t
{
    AdrasteaI_ATMQTT_IP_Session_ID_Invalid = -1,
    AdrasteaI_ATMQTT_IP_Session_ID_Default_Data_PDN,
    AdrasteaI_ATMQTT_IP_Session_ID_Max_Value_in_NP_Config,
    /** @cond DOXYGEN_IGNORE */
    AdrasteaI_ATMQTT_IP_Session_ID_NumberOfValues
    /** @endcond */
} AdrasteaI_ATMQTT_IP_Session_ID_t;

/**
 * @brief MQTT Will Message Presence
 */
typedef enum AdrasteaI_ATMQTT_WILL_Presence_t
{
    AdrasteaI_ATMQTT_WILL_Presence_Invalid = -1,
    AdrasteaI_ATMQTT_WILL_Presence_Disable,
    AdrasteaI_ATMQTT_WILL_Presence_Enable,
    /** @cond DOXYGEN_IGNORE */
    AdrasteaI_ATMQTT_WILL_Presence_NumberOfValues
    /** @endcond */
} AdrasteaI_ATMQTT_WILL_Presence_t;

/**
 * @brief MQTT Quality of Service
 */
typedef enum AdrasteaI_ATMQTT_QoS_t
{
    AdrasteaI_ATMQTT_QoS_Invalid = -1,
    AdrasteaI_ATMQTT_QoS_At_Most_Once,
    AdrasteaI_ATMQTT_QoS_At_Least_Once,
    AdrasteaI_ATMQTT_QoS_Exactly_Once,
    /** @cond DOXYGEN_IGNORE */
    AdrasteaI_ATMQTT_QoS_NumberOfValues
    /** @endcond */
} AdrasteaI_ATMQTT_QoS_t;

/**
 * @brief MQTT Retain after disconnect
 */
typedef enum AdrasteaI_ATMQTT_Retain_t
{
    AdrasteaI_ATMQTT_Retain_Invalid = -1,
    AdrasteaI_ATMQTT_Retain_Not_Retained,
    AdrasteaI_ATMQTT_Retain_Retained,
    /** @cond DOXYGEN_IGNORE */
    AdrasteaI_ATMQTT_Retain_NumberOfValues
    /** @endcond */
} AdrasteaI_ATMQTT_Retain_t;

typedef uint16_t AdrasteaI_ATMQTT_Keep_Alive_t;

#define AdrasteaI_ATMQTT_Keep_Alive_No_Timeout 0

/**
 * @brief MQTT Clean Session
 */
typedef enum AdrasteaI_ATMQTT_Clean_Session_t
{
    AdrasteaI_ATMQTT_Clean_Session_Invalid = -1,
    AdrasteaI_ATMQTT_Clean_Session_Store_Subscriptions_After_Disconnect,
    AdrasteaI_ATMQTT_Clean_Session_Discard_After_Disconnect,
    /** @cond DOXYGEN_IGNORE */
    AdrasteaI_ATMQTT_Clean_Session_NumberOfValues
    /** @endcond */
} AdrasteaI_ATMQTT_Clean_Session_t;

/**
 * @brief MQTT Events
 */
typedef enum AdrasteaI_ATMQTT_Event_t
{
    AdrasteaI_ATMQTT_Event_Invalid = -1,
    AdrasteaI_ATMQTT_Event_Connect_Confirmation,
    AdrasteaI_ATMQTT_Event_Disconnect_Confirmation,
    AdrasteaI_ATMQTT_Event_Subscribe_Confirmation,
    AdrasteaI_ATMQTT_Event_Unsubscribe_Confirmation,
    AdrasteaI_ATMQTT_Event_Publication_Confirmation,
    AdrasteaI_ATMQTT_Event_Publication_Receive,
    AdrasteaI_ATMQTT_Event_Connect_Failure,
    AdrasteaI_ATMQTT_Event_All,
    /** @cond DOXYGEN_IGNORE */
    AdrasteaI_ATMQTT_Event_NumberOfValues
    /** @endcond */
} AdrasteaI_ATMQTT_Event_t;

/**
 * @brief MQTT Event Result Codes
 */
typedef enum AdrasteaI_ATMQTT_Event_Result_Code_t
{
    AdrasteaI_ATMQTT_Event_Result_Code_Invalid = -1,
    AdrasteaI_ATMQTT_Event_Result_Code_Success,
    AdrasteaI_ATMQTT_Event_Result_Code_Fail,
    /** @cond DOXYGEN_IGNORE */
    AdrasteaI_ATMQTT_Event_Result_Code_NumberOfValues
    /** @endcond */
} AdrasteaI_ATMQTT_Event_Result_Code_t;

/**
 * @brief MQTT Event Error Codes
 */
typedef enum AdrasteaI_ATMQTT_Event_Error_Code_t
{
    AdrasteaI_ATMQTT_Event_Error_Code_Invalid = -1,
    AdrasteaI_ATMQTT_Event_Error_Code_No_Error,
    AdrasteaI_ATMQTT_Event_Error_Code_Error,
    /** @cond DOXYGEN_IGNORE */
    AdrasteaI_ATMQTT_Event_Error_Code_NumberOfValues
    /** @endcond */
} AdrasteaI_ATMQTT_Event_Error_Code_t;

/**
 * @brief MQTT Connection Result
 */
typedef struct AdrasteaI_ATMQTT_Connection_Result_t
{
    AdrasteaI_ATMQTT_Conn_ID_t connID;
    AdrasteaI_ATMQTT_Event_Result_Code_t resultCode;
    AdrasteaI_ATMQTT_Event_Error_Code_t errorCode;
} AdrasteaI_ATMQTT_Connection_Result_t;

typedef uint16_t AdrasteaI_ATMQTT_Message_ID_t;

/**
 * @brief MQTT Subscription/Publication Confirmation Result
 */
typedef struct
{
    AdrasteaI_ATMQTT_Conn_ID_t connID;
    AdrasteaI_ATMQTT_Message_ID_t msgID;
    AdrasteaI_ATMQTT_Event_Result_Code_t resultCode;
    AdrasteaI_ATMQTT_Event_Error_Code_t errorCode;
} AdrasteaI_ATMQTT_Subscription_Result_t, AdrasteaI_ATMQTT_Publication_Confirmation_Result_t;

typedef char AdrasteaI_ATMQTT_Topic_Name_t[128];

typedef uint8_t AdrasteaI_ATMQTT_Payload_Size_t;

/**
 * @brief MQTT Publication Received Result
 */
typedef struct AdrasteaI_ATMQTT_Publication_Received_Result_t
{
    AdrasteaI_ATMQTT_Conn_ID_t connID;
    AdrasteaI_ATMQTT_Message_ID_t msgID;
    AdrasteaI_ATMQTT_Topic_Name_t topicName;
    AdrasteaI_ATMQTT_Payload_Size_t payloadSize;
    char* payload;
    uint16_t payloadMaxBufferSize; //Buffer size need to be specified before passing to function
} AdrasteaI_ATMQTT_Publication_Received_Result_t;

/**
 * @brief MQTT IP Address Format
 */
typedef enum AdrasteaI_ATMQTT_IP_Addr_Format_t
{
    AdrasteaI_ATMQTT_IP_Addr_Format_Invalid = -1,
    AdrasteaI_ATMQTT_IP_Addr_Format_IPv4v6,
    AdrasteaI_ATMQTT_IP_Addr_Format_IPv4,
    AdrasteaI_ATMQTT_IP_Addr_Format_IPv6,
    /** @cond DOXYGEN_IGNORE */
    AdrasteaI_ATMQTT_IP_Addr_Format_NumberOfValues
    /** @endcond */
} AdrasteaI_ATMQTT_IP_Addr_Format_t;

/**
 * @brief AWSIOT Quality of Service
 */
typedef enum AdrasteaI_ATMQTT_AWSIOT_QoS_t
{
    AdrasteaI_ATMQTT_AWSIOT_QoS_Invalid = -1,
    AdrasteaI_ATMQTT_AWSIOT_QoS_No_Confirmation,
    AdrasteaI_ATMQTT_AWSIOT_QoS_Confirmed,
    /** @cond DOXYGEN_IGNORE */
    AdrasteaI_ATMQTT_AWSIOT_QoS_NumberOfValues
    /** @endcond */
} AdrasteaI_ATMQTT_AWSIOT_QoS_t;

/**
 * @brief AWSIOT Subscription/Publication Confirmation Result
 */
typedef struct
{
    AdrasteaI_ATMQTT_Message_ID_t msgID;
    AdrasteaI_ATMQTT_Event_Result_Code_t resultCode;
} AdrasteaI_ATMQTT_AWSIOT_Subscription_Result_t, AdrasteaI_ATMQTT_AWSIOT_Publication_Confirmation_Result_t;

/**
 * @brief AWSIOT Publication Received Result
 */
typedef struct AdrasteaI_ATMQTT_AWSIOT_Publication_Received_Result_t
{
    AdrasteaI_ATMQTT_Topic_Name_t topicName;
    char* payload;
    uint16_t payloadMaxBufferSize; //Buffer size need to be specified before passing to function
} AdrasteaI_ATMQTT_AWSIOT_Publication_Received_Result_t;

/**
 * @brief Configure Nodes (using the AT%MQTTCFG command).
 *
 * @param[in] connID: MQTT Connection. See AdrasteaI_ATMQTT_Conn_ID_t.
 *
 * @param[in] clientID: Unique client ID to connect to broker.
 *
 * @param[in] addr: Broker ip address or URL.
 *
 * @param[in] username: Username if authentication is required (optional pass empty string to skip).
 *
 * @param[in] password: Password if authentication is required (optional pass empty string to skip).
 *
 * @return True if successful, false otherwise
 */
extern bool AdrasteaI_ATMQTT_ConfigureNodes(AdrasteaI_ATMQTT_Conn_ID_t connID, AdrasteaI_ATMQTT_Client_ID_t clientID, AdrasteaI_ATCommon_IP_Addr_t addr, AdrasteaI_ATCommon_Auth_Username_t username, AdrasteaI_ATCommon_Auth_Password_t password);

/**
 * @brief Configure TLS (using the AT%MQTTCFG command).
 *
 * @param[in] connID: MQTT Connection. See AdrasteaI_ATMQTT_Conn_ID_t.
 *
 * @param[in] authMode: Authentication mode of TLS.
 *
 * @param[in] profileID: Profile ID of TLS configuration.
 *
 * @return True if successful, false otherwise
 */
extern bool AdrasteaI_ATMQTT_ConfigureTLS(AdrasteaI_ATMQTT_Conn_ID_t connID, AdrasteaI_ATCommon_TLS_Auth_Mode_t authMode, AdrasteaI_ATCommon_TLS_Profile_ID_t profileID);

/**
 * @brief Configure IP (using the AT%MQTTCFG command).
 *
 * @param[in] connID: MQTT Connection. See AdrasteaI_ATMQTT_Conn_ID_t.
 *
 * @param[in] sessionID: Session ID (optional pass AdrasteaI_ATMQTT_IP_Session_ID_Invalid to skip).
 *
 * @param[in] ipFormat: IP Address format (optional pass AdrasteaI_ATMQTT_IP_Addr_Format_Invalid to skip).
 *
 * @param[in] port: Destination Port (optional pass AdrasteaI_ATCommon_Port_Number_Invalid to skip).
 *
 * @return True if successful, false otherwise
 */
extern bool AdrasteaI_ATMQTT_ConfigureIP(AdrasteaI_ATMQTT_Conn_ID_t connID, AdrasteaI_ATMQTT_IP_Session_ID_t sessionID, AdrasteaI_ATMQTT_IP_Addr_Format_t ipFormat, AdrasteaI_ATCommon_Port_Number_t port);

/**
 * @brief Configure Will Message (using the AT%MQTTCFG command).
 *
 * @param[in] connID: MQTT Connection. See AdrasteaI_ATMQTT_Conn_ID_t.
 *
 * @param[in] presence: Configure if will message is enabled.
 *
 * @param[in] qos: Will Quality of Service.
 *
 * @param[in] retain: Whether or not the Will Message will be retained across disconnects.
 *
 * @param[in] topic: Topic Name.
 *
 * @param[in] message: Will Message.
 *
 * @return True if successful, false otherwise
 */
extern bool AdrasteaI_ATMQTT_ConfigureWillMessage(AdrasteaI_ATMQTT_Conn_ID_t connID, AdrasteaI_ATMQTT_WILL_Presence_t presence, AdrasteaI_ATMQTT_QoS_t qos, AdrasteaI_ATMQTT_Retain_t retain, AdrasteaI_ATMQTT_Topic_Name_t topic, char* message);

/**
 * @brief Configure Protocol (using the AT%MQTTCFG command).
 *
 * @param[in] connID: MQTT Connection. See AdrasteaI_ATMQTT_Conn_ID_t.
 *
 * @param[in] keepAlive: Keep Alive time in seconds.
 *
 * @param[in] cleanSession: determine if server should discard subscriptions or not after disconnect.
 *
 * @return True if successful, false otherwise
 */
extern bool AdrasteaI_ATMQTT_ConfigureProtocol(AdrasteaI_ATMQTT_Conn_ID_t connID, AdrasteaI_ATMQTT_Keep_Alive_t keepAlive, AdrasteaI_ATMQTT_Clean_Session_t cleanSession);

/**
 * @brief Set MQTT Notification Events (using the AT%MQTTEV command).
 *
 * @param[in] event: MQTT event type. See AdrasteaI_ATMQTT_Event_t.
 *
 * @param[in] state: Event State
 *
 * @return True if successful, false otherwise
 */
extern bool AdrasteaI_ATMQTT_SetMQTTUnsolicitedNotificationEvents(AdrasteaI_ATMQTT_Event_t event, AdrasteaI_ATCommon_Event_State_t state);

/**
 * @brief Parses the value of Connection Confirmation event arguments.
 *
 * @param[in] pEventArguments: String containing arguments of the AT command
 * @param[out] dataP: the connection result is returned in this argument. See AdrasteaI_ATMQTT_Connection_Result_t.
 *
 * @return True if successful, false otherwise
 */
extern bool AdrasteaI_ATMQTT_ParseConnectionConfirmationEvent(char* pEventArguments, AdrasteaI_ATMQTT_Connection_Result_t* dataP);

/**
 * @brief Parses the value of Disconnection Confirmation event arguments.
 *
 * @param[in] pEventArguments: String containing arguments of the AT command
 * @param[out] dataP: the disconnection result is returned in this argument. See AdrasteaI_ATMQTT_Connection_Result_t.
 *
 * @return True if successful, false otherwise
 */
extern bool AdrasteaI_ATMQTT_ParseDisconnectionConfirmationEvent(char* pEventArguments, AdrasteaI_ATMQTT_Connection_Result_t* dataP);

/**
 * @brief Parses the value of Subscription Confirmation event arguments.
 *
 * @param[in] pEventArguments: String containing arguments of the AT command
 * @param[out] dataP: the subscription result is returned in this argument. See AdrasteaI_ATMQTT_Subscription_Result_t.
 *
 * @return True if successful, false otherwise
 */
extern bool AdrasteaI_ATMQTT_ParseSubscriptionConfirmationEvent(char* pEventArguments, AdrasteaI_ATMQTT_Subscription_Result_t* dataP);

/**
 * @brief Parses the value of Unsubscription Confirmation event arguments.
 *
 * @param[in] pEventArguments: String containing arguments of the AT command
 * @param[out] dataP: the unsubscription result is returned in this argument. See AdrasteaI_ATMQTT_Subscription_Result_t.
 *
 * @return True if successful, false otherwise
 */
extern bool AdrasteaI_ATMQTT_ParseUnsubscriptionConfirmationEvent(char* pEventArguments, AdrasteaI_ATMQTT_Subscription_Result_t* dataP);

/**
 * @brief Parses the value of Publication Confirmation event arguments.
 *
 * @param[in] pEventArguments: String containing arguments of the AT command
 * @param[out] dataP: the publication result is returned in this argument. See AdrasteaI_ATMQTT_Publication_Confirmation_Result_t.
 *
 * @return True if successful, false otherwise
 */
extern bool AdrasteaI_ATMQTT_ParsePublicationConfirmationEvent(char* pEventArguments, AdrasteaI_ATMQTT_Publication_Confirmation_Result_t* dataP);

/**
 * @brief Parses the value of Publication Received event arguments.
 *
 * @param[in] pEventArguments: String containing arguments of the AT command
 * @param[out] dataP: the received publication is returned in this argument. See AdrasteaI_ATMQTT_Publication_Confirmation_Result_t.
 *
 * @return True if successful, false otherwise
 */
extern bool AdrasteaI_ATMQTT_ParsePublicationReceivedEvent(char* pEventArguments, AdrasteaI_ATMQTT_Publication_Received_Result_t* dataP);

/**
 * @brief Connect to Broker (using the AT%MQTTCMD command).
 *
 * @param[in] connID: MQTT Connection. See AdrasteaI_ATMQTT_Conn_ID_t.
 *
 * @return True if successful, false otherwise
 */
extern bool AdrasteaI_ATMQTT_Connect(AdrasteaI_ATMQTT_Conn_ID_t connID);

/**
 * @brief Disconnect from Broker (using the AT%MQTTCMD command).
 *
 * @param[in] connID: MQTT Connection. See AdrasteaI_ATMQTT_Conn_ID_t.
 *
 * @return True if successful, false otherwise
 */
extern bool AdrasteaI_ATMQTT_Disconnect(AdrasteaI_ATMQTT_Conn_ID_t connID);

/**
 * @brief Subscribe to Topic (using the AT%MQTTCMD command).
 *
 * @param[in] connID: MQTT Connection. See AdrasteaI_ATMQTT_Conn_ID_t.
 *
 * @param[in] QoS: Quality of Service.
 *
 * @param[in] topicName: MQTT Topic Name.
 *
 * @return True if successful, false otherwise
 */
extern bool AdrasteaI_ATMQTT_Subscribe(AdrasteaI_ATMQTT_Conn_ID_t connID, AdrasteaI_ATMQTT_QoS_t QoS, AdrasteaI_ATMQTT_Topic_Name_t topicName);

/**
 * @brief Unsubscribe from Topic (using the AT%MQTTCMD command).
 *
 * @param[in] connID: MQTT Connection. See AdrasteaI_ATMQTT_Conn_ID_t.
 *
 * @param[in] topicName: MQTT Topic Name.
 *
 * @return True if successful, false otherwise
 */
extern bool AdrasteaI_ATMQTT_Unsubscribe(AdrasteaI_ATMQTT_Conn_ID_t connID, AdrasteaI_ATMQTT_Topic_Name_t topicName);

/**
 * @brief Publish to Topic (using the AT%MQTTCMD command).
 *
 * @param[in] connID: MQTT Connection. See AdrasteaI_ATMQTT_Conn_ID_t.
 *
 * @param[in] qos: Quality of Service.
 *
 * @param[in] retain: Whether or not the Will Message will be retained across disconnects.
 *
 * @param[in] topicName: MQTT Topic Name.
 *
 * @param[in] payload: Payload to be published to topic.
 *
 * @param[in] payloadSize: Size of the payload in bytes.
 *
 * @return True if successful, false otherwise
 */
extern bool AdrasteaI_ATMQTT_Publish(AdrasteaI_ATMQTT_Conn_ID_t connID, AdrasteaI_ATMQTT_QoS_t qos, AdrasteaI_ATMQTT_Retain_t retain, AdrasteaI_ATMQTT_Topic_Name_t topicName, char* payload, AdrasteaI_ATMQTT_Payload_Size_t payloadSize);

/**
 * @brief Configure AWS connection (using the AT%AWSIOTCFG command).
 * *
 * @param[in] url: URL of endpoint.
 *
 * @param[in] profileID: Profile ID of TLS configuration.
 *
 * @param[in] clientID: Unique Client ID.
 *
 * @return True if successful, false otherwise
 */
extern bool AdrasteaI_ATMQTT_AWSIOTConfigureConnection(AdrasteaI_ATCommon_IP_Addr_t url, AdrasteaI_ATCommon_TLS_Profile_ID_t profileID, AdrasteaI_ATMQTT_Client_ID_t clientID);

/**
 * @brief Configure AWS IP (using the AT%AWSIOTCFG command).
 * *
 * @param[in] sessionID: Session ID (optional pass AdrasteaI_ATMQTT_IP_Session_ID_Invalid to skip).
 *
 * @param[in] ipFormat: ip address format (optional pass AdrasteaI_ATMQTT_IP_Addr_Format_Invalid to skip).
 *
 * @return True if successful, false otherwise
 */
extern bool AdrasteaI_ATMQTT_AWSIOTConfigureIP(AdrasteaI_ATMQTT_IP_Session_ID_t sessionID, AdrasteaI_ATMQTT_IP_Addr_Format_t ipFormat);

/**
 * @brief Configure AWS Protocol (using the AT%AWSIOTCFG command).
 * *
 * @param[in] keepAlive: Keep Alive.
 *
 * @param[in] qos: Quality of Service.
 *
 * @return True if successful, false otherwise
 */
extern bool AdrasteaI_ATMQTT_AWSIOTConfigureProtocol(AdrasteaI_ATMQTT_Keep_Alive_t keepAlive, AdrasteaI_ATMQTT_AWSIOT_QoS_t qos);

/**
 * @brief Set AWS IoT Notification Events (using the AT%AWSIOTEV command).
 *
 * @param[in] event: MQTT event type. See AdrasteaI_ATMQTT_Event_t.
 *
 * @param[in] state: Event State
 *
 * @return True if successful, false otherwise
 */
extern bool AdrasteaI_ATMQTT_SetAWSIOTUnsolicitedNotificationEvents(AdrasteaI_ATMQTT_Event_t event, AdrasteaI_ATCommon_Event_State_t state);

/**
 * @brief Parses the value of Connection Confirmation event arguments.
 *
 * @param[in] pEventArguments: String containing arguments of the AT command
 * @param[out] dataP: the event result code is returned in this argument. See AdrasteaI_ATMQTT_Event_Result_Code_t.
 *
 * @return True if successful, false otherwise
 */
extern bool AdrasteaI_ATMQTT_ParseAWSIOTConnectionConfirmationEvent(char* pEventArguments, AdrasteaI_ATMQTT_Event_Result_Code_t* dataP);

/**
 * @brief Parses the value of Disconnection Confirmation event arguments.
 *
 * @param[in] pEventArguments: String containing arguments of the AT command
 * @param[out] dataP: the event result code is returned in this argument. See AdrasteaI_ATMQTT_Event_Result_Code_t.
 *
 * @return True if successful, false otherwise
 */
extern bool AdrasteaI_ATMQTT_ParseAWSIOTDisconnectionConfirmationEvent(char* pEventArguments, AdrasteaI_ATMQTT_Event_Result_Code_t* dataP);

/**
 * @brief Parses the value of Subscription Confirmation event arguments.
 *
 * @param[in] pEventArguments: String containing arguments of the AT command
 * @param[out] dataP: the subscription result is returned in this argument. See AdrasteaI_ATMQTT_AWSIOT_Subscription_Result_t.
 *
 * @return True if successful, false otherwise
 */
extern bool AdrasteaI_ATMQTT_ParseAWSIOTSubscriptionConfirmationEvent(char* pEventArguments, AdrasteaI_ATMQTT_AWSIOT_Subscription_Result_t* dataP);

/**
 * @brief Parses the value of Unsubscription Confirmation event arguments.
 *
 * @param[in] pEventArguments: String containing arguments of the AT command
 * @param[out] dataP: the unsubscription result is returned in this argument. See AdrasteaI_ATMQTT_AWSIOT_Subscription_Result_t.
 *
 * @return True if successful, false otherwise
 */
extern bool AdrasteaI_ATMQTT_ParseAWSIOTUnsubscriptionConfirmationEvent(char* pEventArguments, AdrasteaI_ATMQTT_AWSIOT_Subscription_Result_t* dataP);

/**
 * @brief Parses the value of Publication Confirmation event arguments.
 *
 * @param[in] pEventArguments: String containing arguments of the AT command
 * @param[out] dataP: the publication confirmation result is returned in this argument. See AdrasteaI_ATMQTT_AWSIOT_Publication_Confirmation_Result_t.
 *
 * @return True if successful, false otherwise
 */
extern bool AdrasteaI_ATMQTT_ParseAWSIOTPublicationConfirmationEvent(char* pEventArguments, AdrasteaI_ATMQTT_AWSIOT_Publication_Confirmation_Result_t* dataP);

/**
 * @brief Parses the value of Publication Received event arguments.
 *
 * @param[in] pEventArguments: String containing arguments of the AT command
 * @param[out] dataP: the publication received result is returned in this argument. See AdrasteaI_ATMQTT_AWSIOT_Publication_Received_Result_t.
 *
 * @return True if successful, false otherwise
 */
extern bool AdrasteaI_ATMQTT_ParseAWSIOTPublicationReceivedEvent(char* pEventArguments, AdrasteaI_ATMQTT_AWSIOT_Publication_Received_Result_t* dataP);

/**
 * @brief Connect to AWS (using the AT%AWSIOTCMD command).
 *
 * @return True if successful, false otherwise
 */
extern bool AdrasteaI_ATMQTT_AWSIOTConnect();

/**
 * @brief Disconnect from AWS (using the AT%AWSIOTCMD command).
 *
 * @return True if successful, false otherwise
 */
extern bool AdrasteaI_ATMQTT_AWSIOTDisconnect();

/**
 * @brief Subscribe to Topic (using the AT%AWSIOTCMD command).
 *
 * @param[in] topicName: MQTT Topic name. See AdrasteaI_ATMQTT_Topic_Name_t.
 *
 * @return True if successful, false otherwise
 */
extern bool AdrasteaI_ATMQTT_AWSIOTSubscribe(AdrasteaI_ATMQTT_Topic_Name_t topicName);

/**
 * @brief Unsubscribe from Topic (using the AT%AWSIOTCMD command).
 *
 * @param[in] topicName: MQTT Topic name. See AdrasteaI_ATMQTT_Topic_Name_t.
 *
 * @return True if successful, false otherwise
 */
extern bool AdrasteaI_ATMQTT_AWSIOTUnsubscribe(AdrasteaI_ATMQTT_Topic_Name_t topicName);

/**
 * @brief Publish to Topic (using the AT%AWSIOTCMD command).
 *
 * @param[in] topicName: MQTT Topic name. See AdrasteaI_ATMQTT_Topic_Name_t.
 *
 * @param[in] payload: Payload to be published. See AdrasteaI_ATMQTT_Payload_t.
 *
 * @return True if successful, false otherwise
 */
extern bool AdrasteaI_ATMQTT_AWSIOTPublish(AdrasteaI_ATMQTT_Topic_Name_t topicName, char* payload);

#ifdef __cplusplus
}
#endif

#endif /* ADRASTEAI_AT_MQTT_H_INCLUDED */
