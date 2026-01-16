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
 * @file ATEvent.h
 * @brief AT event definitions.
 */

#ifndef ADRASTEAI_AT_EVENT_H_INCLUDED
#define ADRASTEAI_AT_EVENT_H_INCLUDED

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief AT event IDs.
 */
typedef enum AdrasteaI_ATEvent_t
{
    AdrasteaI_ATEvent_Invalid = (uint16_t)0, /**< Invalid event */

    AdrasteaI_ATEvent_Ready, /**< Startup event */

    AdrasteaI_ATEvent_NetService_Operator_Read,                 /**< Net Service operator read event (Use @ref AdrasteaI_ATNetService_ParseOperatorReadEvent to parse data) */
    AdrasteaI_ATEvent_Proprietary_Ping_Result,                  /**< Proprietery ping result event (Use @ref AdrasteaI_ATProprietary_ParsePingResultEvent to parse data) */
    AdrasteaI_ATEvent_Proprietary_Domain_Name_Resolve,          /**< Proprietery DNS event (Use @ref AdrasteaI_ATProprietary_ParseResolveDomainNameEvent to parse data) */
    AdrasteaI_ATEvent_PacketDomain_Network_Registration_Status, /**< Packet domain network registration event (Use @ref AdrasteaI_ATPacketDomain_ParseNetworkRegistrationStatusEvent to parse data) */
    AdrasteaI_ATEvent_PacketDomain_PDP_Context,                 /**< Packet domain PDP context event (Use @ref AdrasteaI_ATPacketDomain_ParsePDPContextEvent to parse data) */
    AdrasteaI_ATEvent_PacketDomain_PDP_Context_State,           /**< Packet domain PDP context event (Use @ref AdrasteaI_ATPacketDomain_ParsePDPContextStateEvent to parse data) */
    AdrasteaI_ATEvent_GNSS_Satellite_Query,                     /**< GNSS satelite query event (Use @ref AdrasteaI_ATGNSS_ParseSatelliteQueryEvent to parse data) */

    AdrasteaI_ATEvent_GNSS_NMEA,                  /**< GNSS NMEA event (Use @ref AdrasteaI_ATGNSS_ParseNMEAEvent to parse data) */
    AdrasteaI_ATEvent_GNSS_Session_Status_Change, /**< GNSS session status change event (Use @ref AdrasteaI_ATGNSS_ParseSessionStatusChangedEvent to parse data) */
    AdrasteaI_ATEvent_GNSS_Allowed_Status_Change, /**< GNSS allowed status change event (Use @ref AdrasteaI_ATGNSS_ParseAllowedStatusChangedEvent to parse data) */

    AdrasteaI_ATEvent_GNSS_DataFileSaved, /**< GNSS data files saved event */

    AdrasteaI_ATEvent_MQTT_Connection_Confirmation,     /**< MQTT connection confirmation event (Use @ref AdrasteaI_ATMQTT_ParseConnectionConfirmationEvent to parse data) */
    AdrasteaI_ATEvent_MQTT_Disconnection_Confirmation,  /**< MQTT disconnection confirmation event (Use @ref AdrasteaI_ATMQTT_ParseDisconnectionConfirmationEvent to parse data) */
    AdrasteaI_ATEvent_MQTT_Subscription_Confirmation,   /**< MQTT subscription confirmation event (Use @ref AdrasteaI_ATMQTT_ParseSubscriptionConfirmationEvent to parse data) */
    AdrasteaI_ATEvent_MQTT_Unsubscription_Confirmation, /**< MQTT unsubscription confirmation event (Use @ref AdrasteaI_ATMQTT_ParseUnsubscriptionConfirmationEvent to parse data) */
    AdrasteaI_ATEvent_MQTT_Publication_Confirmation,    /**< MQTT publication confirmation event (Use @ref AdrasteaI_ATMQTT_ParsePublicationConfirmationEvent to parse data) */
    AdrasteaI_ATEvent_MQTT_Publication_Received,        /**< MQTT publication received event (Use @ref AdrasteaI_ATMQTT_ParsePublicationReceivedEvent to parse data) */
    AdrasteaI_ATEvent_MQTT_Connection_Failure,          /**< MQTT connection failure event */

    AdrasteaI_ATEvent_MQTT_AWSIOT_Connection_Confirmation,     /**< AWS IoT MQTT connection confirmation event (Use @ref AdrasteaI_ATMQTT_ParseAWSIOTConnectionConfirmationEvent to parse data) */
    AdrasteaI_ATEvent_MQTT_AWSIOT_Disconnection_Confirmation,  /**< AWS IoT MQTT disconnection confirmation event (Use @ref AdrasteaI_ATMQTT_ParseAWSIOTDisconnectionConfirmationEvent to parse data) */
    AdrasteaI_ATEvent_MQTT_AWSIOT_Subscription_Confirmation,   /**< AWS IoT MQTT subscription confirmation event (Use @ref AdrasteaI_ATMQTT_ParseAWSIOTSubscriptionConfirmationEvent to parse data) */
    AdrasteaI_ATEvent_MQTT_AWSIOT_Unsubscription_Confirmation, /**< AWS IoT MQTT unsubscription confirmation event (Use @ref AdrasteaI_ATMQTT_ParseAWSIOTUnsubscriptionConfirmationEvent to parse data) */
    AdrasteaI_ATEvent_MQTT_AWSIOT_Publication_Confirmation,    /**< AWS IoT MQTT publication confirmation event (Use @ref AdrasteaI_ATMQTT_ParseAWSIOTPublicationConfirmationEvent to parse data) */
    AdrasteaI_ATEvent_MQTT_AWSIOT_Publication_Received,        /**< AWS IoT MQTT publication received event (Use @ref AdrasteaI_ATMQTT_ParseAWSIOTPublicationReceivedEvent to parse data) */
    AdrasteaI_ATEvent_MQTT_AWSIOT_Connection_Failure,          /**< AWS IoT MQTT connection failure event */

    AdrasteaI_ATEvent_HTTP_PUT_Confirmation,    /**< HTTP PUT confirmation event (Use @ref AdrasteaI_ATHTTP_ParsePUTEvent to parse data) */
    AdrasteaI_ATEvent_HTTP_POST_Confirmation,   /**< HTTP POST confirmation event (Use @ref AdrasteaI_ATHTTP_ParsePOSTEvent to parse data) */
    AdrasteaI_ATEvent_HTTP_DELETE_Confirmation, /**< HTTP DELETE confirmation event (Use @ref AdrasteaI_ATHTTP_ParseDELETEEvent to parse data) */
    AdrasteaI_ATEvent_HTTP_GET_Receive,         /**< HTTP GET receive event (Use @ref AdrasteaI_ATHTTP_ParseGETEvent to parse data) */
    AdrasteaI_ATEvent_HTTP_Session_Termination, /**< HTTP session termination event */

    AdrasteaI_ATEvent_SMS_Read_Message,     /**< SMS read message event (Use @ref AdrasteaI_ATSMS_ParseReadMessageEvent to parse data) */
    AdrasteaI_ATEvent_SMS_List_Messages,    /**< SMS list messages event (Use @ref AdrasteaI_ATSMS_ParseListMessagesEvent to parse data) */
    AdrasteaI_ATEvent_SMS_Message_Received, /**< SMS message received event (Use @ref AdrasteaI_ATSMS_ParseMessageReceivedEvent to parse data) */
    AdrasteaI_ATEvent_SMS_Error,            /**< SMS error event (Use @ref AdrasteaI_ATSMS_ParseSMSErrorEvent to parse data) */

    AdrasteaI_ATEvent_Socket_Data_Received,          /**< Socket data received event (Use @ref AdrasteaI_ATSocket_ParseDataReceivedEvent to parse data) */
    AdrasteaI_ATEvent_Socket_Deactivated_Idle_Timer, /**< Socket deactivated idle timer event */
    AdrasteaI_ATEvent_Socket_Terminated_By_Peer,     /**< Socket terminated by peer event (Use @ref AdrasteaI_ATSocket_ParseSocketTerminatedEvent to parse data) */
    AdrasteaI_ATEvent_Socket_New_Socket_Accepted,    /**< Socket accepted event */
    AdrasteaI_ATEvent_Socket_Sockets_Read,           /**< Socket read event (Use @ref AdrasteaI_ATSocket_ParseSocketsReadEvent to parse data) */

    AdrasteaI_ATEvent_SIM_Subscriber_Number, /**< SIM subscriber number event (Use @ref AdrasteaI_ATSMS_ParseSubscriberNumberEvent to parse data) */

    /** @cond DOXYGEN_IGNORE */
    AdrasteaI_ATEvent_NumberOfValues,
    AdrasteaI_ATEvent_Max = UINT16_MAX
    /** @endcond */
} AdrasteaI_ATEvent_t;

/**
 * @brief Parses the received AT command and returns the corresponding AdrasteaI_ATEvent_t.
 *
 * @param[in,out] pAtCommand: AT command starting with '+' or '%'
 * @param[out] pEvent: AdrasteaI_ATEvent_t representing the event
 *
 * @return True if parsed successfully, false otherwise
 */
extern bool AdrasteaI_ATEvent_ParseEventType(char** pAtCommand, AdrasteaI_ATEvent_t* pEvent);

#ifdef __cplusplus
}
#endif

#endif // AT_EVENTS_H_INCLUDED
