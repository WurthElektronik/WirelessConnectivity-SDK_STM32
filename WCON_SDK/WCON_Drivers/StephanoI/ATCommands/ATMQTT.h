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

#ifndef STEPHANOI_MQTT_H_INCLUDED
#define STEPHANOI_MQTT_H_INCLUDED

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

#define MQTT_TOPIC_LENGTH 128

/**
 * @brief MQTT scheme
 */
typedef enum StephanoI_ATMQTT_Scheme_t
{
    StephanoI_ATMQTT_Scheme_TCP = 1,
    StephanoI_ATMQTT_Scheme_TLS_nocert = 2,
    StephanoI_ATMQTT_Scheme_TLS_verifyserver = 3,
    StephanoI_ATMQTT_Scheme_TLS_provideclient = 4,
    StephanoI_ATMQTT_Scheme_TLS_verifyserverNprovideclient = 5,
    StephanoI_ATMQTT_Scheme_WebSocket = 6,
    StephanoI_ATMQTT_Scheme_SecureWebSocket_nocert = 7,
    StephanoI_ATMQTT_Scheme_SecureWebSocket_verifyserver = 8,
    StephanoI_ATMQTT_Scheme_SecureWebSocket_provideclient = 9,
    StephanoI_ATMQTT_Scheme_SecureWebSocket_verifyserverNprovideclient = 10,
} StephanoI_ATMQTT_Scheme_t;

/**
 * @brief MQTT state
 */
typedef enum StephanoI_ATMQTT_State_t
{
    StephanoI_ATMQTT_State_Uninitialized = 0,
    StephanoI_ATMQTT_State_AlreadySet1 = 1,
    StephanoI_ATMQTT_State_AlreadySet2 = 2,
    StephanoI_ATMQTT_State_Disconnected = 3,
    StephanoI_ATMQTT_State_Connected = 4,
    StephanoI_ATMQTT_State_Unsubscribed = 5,
    StephanoI_ATMQTT_State_ConnectedNSubscribed = 6,
} StephanoI_ATMQTT_State_t;

/**
 * @brief MQTT quality of service
 */
typedef enum StephanoI_ATMQTT_QOS_t
{
    StephanoI_ATMQTT_QOS_0 = 0,
    StephanoI_ATMQTT_QOS_1 = 1,
    StephanoI_ATMQTT_QOS_2 = 2,
} StephanoI_ATMQTT_QOS_t;

/**
 * @brief MQTT connection info event
 */
typedef struct StephanoI_ATMQTT_ConnectionInfo_t
{
    uint8_t link_ID;
    StephanoI_ATMQTT_State_t state;
    StephanoI_ATMQTT_Scheme_t scheme;
    char host[128];
    uint16_t port;
    char path[128];
    uint8_t reconnect;
} StephanoI_ATMQTT_ConnectionInfo_t;

/**
 * @brief MQTT get subscriptions event
 */
typedef struct StephanoI_ATMQTT_GetSubscriptions_t
{
    uint8_t link_ID;
    StephanoI_ATMQTT_State_t state;
    char topic[MQTT_TOPIC_LENGTH];
    StephanoI_ATMQTT_QOS_t qos;
} StephanoI_ATMQTT_GetSubscriptions_t;

/**
 * @brief MQTT receive subscriptions event
 */
typedef struct StephanoI_ATMQTT_ReceiveSubscriptions_t
{
    uint8_t link_ID;
    char topic[MQTT_TOPIC_LENGTH];
    uint16_t length;
    uint8_t* data;
} StephanoI_ATMQTT_ReceiveSubscriptions_t;

/**
 * @brief MQTT User config
 *
 * @param[in] link_ID: Link ID
 * @param[in] scheme: Connection scheme
 * @param[in] client_ID: Client ID
 * @param[in] username: User name
 * @param[in] password: Password
 * @param[in] cert_key_ID: ID of the certificate
 * @param[in] CA_ID: ID of the CA
 * @param[in] path: Path to the resource
 *
 * @return True if successful, false otherwise
 */
extern bool StephanoI_ATMQTT_Userconfig(uint8_t link_ID, StephanoI_ATMQTT_Scheme_t scheme, char* client_ID, char* username, char* password, uint8_t cert_key_ID, uint8_t CA_ID, char* path);

/**
 * @brief MQTT Set client ID
 *
 * @param[in] link_ID: Link ID
 * @param[in] client_ID: Client ID
 *
 * @return True if successful, false otherwise
 */
extern bool StephanoI_ATMQTT_SetClientID(uint8_t link_ID, char* client_ID);

/**
 * @brief MQTT Set user name
 *
 * @param[in] link_ID: Link ID
 * @param[in] username: User name
 *
 * @return True if successful, false otherwise
 */
extern bool StephanoI_ATMQTT_SetUsername(uint8_t link_ID, char* username);

/**
 * @brief MQTT Set pass word
 *
 * @param[in] link_ID: Link ID
 * @param[in] password: Pass word
 *
 * @return True if successful, false otherwise
 */
extern bool StephanoI_ATMQTT_SetPassword(uint8_t link_ID, char* password);

/**
 * @brief MQTT connection config
 *
 * @param[in] link_ID: Link ID
 * @param[in] keep_alive: Keep alive time
 * @param[in] disable_clean_session: Clean session
 * @param[in] lwt_topics: LWT topic
 * @param[in] lwt_msg: LWT message
 * @param[in] qos: QOS
 * @param[in] lwt_retain: LWT retain
 *
 * @return True if successful, false otherwise
 */
extern bool StephanoI_ATMQTT_ConnectionConfig(uint8_t link_ID, uint16_t keep_alive, bool disable_clean_session, char* lwt_topics, char* lwt_msg, StephanoI_ATMQTT_QOS_t qos, bool lwt_retain);

/**
 * @brief MQTT Connection setup
 *
 * @param[in] link_ID: Link ID
 * @param[in] host: URL of the host
 * @param[in] port: Port
 * @param[in] reconnect: Reconnect
 *
 * @return True if successful, false otherwise
 */
extern bool StephanoI_ATMQTT_Connect(uint8_t link_ID, char* host, uint16_t port, bool reconnect);

/**
 * @brief MQTT Get connection Info
 *
 * @return True if successful, false otherwise
 */
extern bool StephanoI_ATMQTT_GetConnectionInfo();

/**
 * @brief MQTT Publish
 *
 * @param[in] link_ID: Link ID
 * @param[in] topic: Topic
 * @param[in] data: Data
 * @param[in] qos: Quality of service
 * @param[in] retain: Retain
 *
 * @return True if successful, false otherwise
 */
extern bool StephanoI_ATMQTT_Publish(uint8_t link_ID, char* topic, char* data, StephanoI_ATMQTT_QOS_t qos, uint8_t retain);

/**
 * @brief MQTT Subscribe
 *
 * @param[in] link_ID: Link ID
 * @param[in] topic: Topic
 * @param[in] qos: Quality of service
 *
 * @return True if successful, false otherwise
 */
extern bool StephanoI_ATMQTT_Subscribe(uint8_t link_ID, char* topic, StephanoI_ATMQTT_QOS_t qos);

/**
 * @brief MQTT Get subscriptions
 *
 * @return True if successful, false otherwise
 */
extern bool StephanoI_ATMQTT_GetSubscriptions();

/**
 * @brief MQTT Unsubscribe
 *
 * @param[in] link_ID: Link ID
 * @param[in] topic: Topic
 *
 * @return True if successful, false otherwise
 */
extern bool StephanoI_ATMQTT_Unsubscribe(uint8_t link_ID, char* topic);

/**
 * @brief MQTT Clean
 *
 * @param[in] link_ID: Link ID
 *
 * @return True if successful, false otherwise
 */
extern bool StephanoI_ATMQTT_Clean(uint8_t link_ID);

/**
 * @brief Parses the values of the MQTT connection info event arguments
 *
 * @param[in] EventArgumentsP: String containing arguments of the AT command
 * @param[out] t: The parsed event data
 *
 * @return True if parsed successfully, false otherwise
 */
extern bool StephanoI_ATMQTT_ParseConnectionInfo(char* EventArgumentsP, StephanoI_ATMQTT_ConnectionInfo_t* t);

/**
 * @brief Parses the values of the MQTT get subscriptions event arguments
 *
 * @param[in] EventArgumentsP: String containing arguments of the AT command
 * @param[out] t: The parsed event data
 *
 * @return True if parsed successfully, false otherwise
 */
extern bool StephanoI_ATMQTT_ParseGetSubscriptions(char* EventArgumentsP, StephanoI_ATMQTT_GetSubscriptions_t* t);

/**
 * @brief Parses the values of the MQTT receive subscriptions event arguments
 *
 * @param[in] EventArgumentsP: String containing arguments of the AT command
 * @param[out] t: The parsed event data
 *
 * @return True if parsed successfully, false otherwise
 */
extern bool StephanoI_ATMQTT_ParseReceiveSubscriptions(char* EventArgumentsP, StephanoI_ATMQTT_ReceiveSubscriptions_t* t);

#ifdef __cplusplus
}
#endif

#endif /* STEPHANOI_MQTT_H_INCLUDED */
