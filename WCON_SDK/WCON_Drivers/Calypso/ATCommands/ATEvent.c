/**
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
 **/

/**
 * @file
 * @brief AT event definitions.
 */

#include "ATEvent.h"

#include "../Calypso.h"

static const char *ATEvent_Strings[ATEvent_NumberOfValues] =
{
    "invalid",
    "eventstartup",
    "eventwakeup",
    "ping",
    "reset_request",
    "error",
    "connect",
    "disconnect",
    "sta_added",
    "sta_removed",
    "p2p_connect",
    "p2p_disconnect",
    "p2p_client_added",
    "p2p_client_removed",
    "p2p_devfound",
    "p2p_request",
    "p2p_connectfail",
    "provisioning_status",
    "provisioning_profile_added",
    "tx_failed",
    "async_event",
    "connect",
    "accept",
    "recv",
    "recvfrom",
    "ipv4_acquired",
    "ipv6_acquired",
    "ip_collision",
    "dhcpv4_leased",
    "dhcpv4_released",
    "ipv4_lost",
    "dhcp_ipv4_acquire_timeout",
    "ipv6_lost",
    "operation",
    "recv",
    "disconnect",
    "filegetfilelist",
    "eventhttpget",
    "eventcustomgpio",
    "eventcustomhttppost",
    "device_abort",
    "driver_abort",
    "sync_loss",
    "no_cmd_ack",
    "cmd_timout"
};

static bool ATEvent_ParseEventSubType(const char *eventSubTypeString,
                                      ATEvent_t eventMainType,
                                      ATEvent_t *pEventSubType);

/**
 * @brief Parses the received AT command and returns the corresponding ATEvent_t.
 *
 * @param[in,out] pAtCommand AT command starting with '+'
 * @param[out] pEvent ATEvent_t representing the event
 *
 * @return true if parsed successfully, false otherwise
 */
bool ATEvent_ParseEventType(char **pAtCommand, ATEvent_t *pEvent)
{
    bool ret = false;
    char cmdName[32];
    char option[64];

    *pEvent = ATEvent_Invalid;
    ret = Calypso_GetCmdName(pAtCommand, cmdName, CALYPSO_EVENT_DELIM, CALYPSO_STRING_TERMINATE);
    if (ret)
    {
        if (0 == strcasecmp(cmdName, "+eventgeneral"))
        {
            ret = Calypso_GetNextArgumentString(pAtCommand, option, CALYPSO_ARGUMENT_DELIM, sizeof(option));
            if (ret)
            {
                ATEvent_ParseEventSubType(option, ATEvent_General, pEvent);
            }
        }
        else if (0 == strcasecmp(cmdName, "+eventwlan"))
        {
            ret = Calypso_GetNextArgumentString(pAtCommand, option, CALYPSO_ARGUMENT_DELIM, sizeof(option));
            if (ret)
            {
                ATEvent_ParseEventSubType(option, ATEvent_Wlan, pEvent);
            }
        }
        else if (0 == strcasecmp(cmdName, "+eventsocket"))
        {
            ret = Calypso_GetNextArgumentString(pAtCommand, option, CALYPSO_ARGUMENT_DELIM, sizeof(option));
            if (ret)
            {
                ATEvent_ParseEventSubType(option, ATEvent_Socket, pEvent);
            }
        }
        else if (0 == strcasecmp(cmdName, "+eventnetapp"))
        {
            ret = Calypso_GetNextArgumentString(pAtCommand, option, CALYPSO_ARGUMENT_DELIM, sizeof(option));
            if (ret)
            {
                ATEvent_ParseEventSubType(option, ATEvent_Netapp, pEvent);
            }
        }
        else if (0 == strcasecmp(cmdName, "+eventmqtt"))
        {
            ret = Calypso_GetNextArgumentString(pAtCommand, option, CALYPSO_ARGUMENT_DELIM, sizeof(option));
            if (ret)
            {
                ATEvent_ParseEventSubType(option, ATEvent_MQTT, pEvent);
            }
        }
        else if (0 == strcasecmp(cmdName, "+eventfatalerror"))
        {
            ret = Calypso_GetNextArgumentString(pAtCommand, option, CALYPSO_ARGUMENT_DELIM, sizeof(option));
            if (ret)
            {
                ATEvent_ParseEventSubType(option, ATEvent_FatalError, pEvent);
            }
        }
        else if (0 == strcasecmp(cmdName, "+eventstartup"))
        {
            *pEvent = ATEvent_Startup;
        }
        else if (0 == strcasecmp(cmdName, "+eventwakeup"))
        {
            *pEvent = ATEvent_WakeUp;
        }
        else if (0 == strcasecmp(cmdName, "+recv"))
        {
            *pEvent = ATEvent_SocketRcvd;
        }
        else if (0 == strcasecmp(cmdName, "+recvfrom"))
        {
            *pEvent = ATEvent_SocketRcvdFrom;
        }
        else if (0 == strcasecmp(cmdName, "+connect"))
        {
            *pEvent = ATEvent_SocketTCPConnect;
        }
        else if (0 == strcasecmp(cmdName, "+accept"))
        {
            *pEvent = ATEvent_SocketTCPAccept;
        }
        else if (0 == strcasecmp(cmdName, "+eventhttpget"))
        {
            *pEvent = ATEvent_HTTPGet;
        }
        else if (0 == strcasecmp(cmdName, "+eventcustom"))
        {
            uint8_t customEventId;
            ret = Calypso_GetNextArgumentInt(pAtCommand,
                                             &customEventId,
                                             CALYPSO_INTFLAGS_SIZE8 | CALYPSO_INTFLAGS_UNSIGNED | CALYPSO_INTFLAGS_NOTATION_DEC,
                                             CALYPSO_ARGUMENT_DELIM);
            if (ret)
            {
                switch (customEventId)
                {
                case ATEvent_CustomEventID_GPIO:
                    *pEvent = ATEvent_CustomGPIO;
                    break;

                case ATEvent_CustomEventID_HTTPPost:
                    *pEvent = ATEvent_CustomHTTPPost;
                    break;

                default:
                    ret = false;
                }
            }
        }
        else if (0 == strcasecmp(cmdName, "+netappping"))
        {
            *pEvent = ATEvent_Ping;
        }
        else if (0 == strcasecmp(cmdName, "+filegetfilelist"))
        {
            *pEvent = ATEvent_FileListEntry;
        }
        else
        {
            ret = false;
        }
    }
    return ret;
}

/**
 * @brief Returns the name of an event.
 *
 * @param[in] event Type of event
 * @param[out] pEventName Event name
 *
 * @return true if successful, false otherwise
 */
bool ATEvent_GetEventName(ATEvent_t event, char* pEventName)
{
    strcpy(pEventName, ATEvent_Strings[event]);
    return true;
}

/**
 * @brief Parses the values of the startup event arguments.
 *
 * @param[in,out] pEventArguments String containing arguments of the AT command
 * @param[out] Calypso_Examples_startupEvent The parsed startup event data
 *
 * @return true if parsed successfully, false otherwise
 */
bool ATEvent_ParseStartUpEvent(char **pEventArguments, ATEvent_Startup_t *Calypso_Examples_startupEvent)
{
    bool ret = false;

    ret = Calypso_GetNextArgumentString(pEventArguments, Calypso_Examples_startupEvent->articleNr, CALYPSO_ARGUMENT_DELIM, sizeof(Calypso_Examples_startupEvent->articleNr));

    if (ret)
    {
        ret = Calypso_GetNextArgumentString(pEventArguments, Calypso_Examples_startupEvent->chipID, CALYPSO_ARGUMENT_DELIM, sizeof(Calypso_Examples_startupEvent->chipID));
    }

    if (ret)
    {
        ret = Calypso_GetNextArgumentString(pEventArguments, Calypso_Examples_startupEvent->MACAddress, CALYPSO_ARGUMENT_DELIM, sizeof(Calypso_Examples_startupEvent->MACAddress));
    }

    if (ret)
    {
        ret = Calypso_GetNextArgumentInt(pEventArguments, &(Calypso_Examples_startupEvent->firmwareVersion[0]), CALYPSO_INTFLAGS_SIZE8 | CALYPSO_INTFLAGS_UNSIGNED, '.');
    }

    if (ret)
    {
        ret = Calypso_GetNextArgumentInt(pEventArguments, &(Calypso_Examples_startupEvent->firmwareVersion[1]), CALYPSO_INTFLAGS_SIZE8 | CALYPSO_INTFLAGS_UNSIGNED,'.');
    }

    if (ret)
    {
        ret = Calypso_GetNextArgumentInt(pEventArguments, &(Calypso_Examples_startupEvent->firmwareVersion[2]), CALYPSO_INTFLAGS_SIZE8 | CALYPSO_INTFLAGS_UNSIGNED, CALYPSO_STRING_TERMINATE);
    }

    return ret;
}

/**
 * @brief Parses the values of the ping event arguments.
 *
 * @param[in,out] pEventArguments String containing arguments of the AT command
 * @param[out] pingEvent The parsed ping event data
 *
 * @return true if parsed successfully, false otherwise
 */
bool ATEvent_ParsePingEvent(char **pEventArguments, ATEvent_Ping_t *pingEvent)
{
    bool ret = false;

    ret = Calypso_GetNextArgumentInt(pEventArguments, &(pingEvent->packetsSent), CALYPSO_INTFLAGS_SIZE16 | CALYPSO_INTFLAGS_NOTATION_DEC | CALYPSO_INTFLAGS_UNSIGNED, CALYPSO_ARGUMENT_DELIM);

    if (ret)
    {
        ret = Calypso_GetNextArgumentInt(pEventArguments, &(pingEvent->packetsReceived), CALYPSO_INTFLAGS_SIZE16 | CALYPSO_INTFLAGS_NOTATION_DEC | CALYPSO_INTFLAGS_UNSIGNED, CALYPSO_ARGUMENT_DELIM);
    }

    if (ret)
    {
        ret = Calypso_GetNextArgumentInt(pEventArguments, &(pingEvent->roundTripTimeMs), CALYPSO_INTFLAGS_SIZE16 | CALYPSO_INTFLAGS_NOTATION_DEC | CALYPSO_INTFLAGS_UNSIGNED, CALYPSO_STRING_TERMINATE);
    }

    return ret;
}

/**
 * @brief Parses the values of the TCP connect event arguments.
 *
 * @param[in,out] pEventArguments String containing arguments of the AT command
 * @param[out] connectEvent The parsed TCP connect event data
 *
 * @return true if parsed successfully, false otherwise
 */
bool ATEvent_ParseSocketTCPConnectEvent(char **pEventArguments, ATEvent_SocketTCPConnect_t* connectEvent)
{
    bool ret = Calypso_GetNextArgumentInt(pEventArguments, &(connectEvent->serverPort), CALYPSO_INTFLAGS_SIZE16 | CALYPSO_INTFLAGS_UNSIGNED, CALYPSO_ARGUMENT_DELIM);
    if (ret)
    {
        ret = Calypso_GetNextArgumentString(pEventArguments, connectEvent->serverAddress, CALYPSO_STRING_TERMINATE, sizeof(connectEvent->serverAddress));
    }
    return ret;
}

/**
 * @brief Parses the values of the TCP accept event arguments.
 *
 * @param[in,out] pEventArguments String containing arguments of the AT command
 * @param[out] acceptEvent The parsed TCP accept event data
 *
 * @return true if parsed successfully, false otherwise
 */
bool ATEvent_ParseSocketTCPAcceptEvent(char **pEventArguments, ATEvent_SocketTCPAccept_t* acceptEvent)
{
    char temp[12];

    bool ret = Calypso_GetNextArgumentInt(pEventArguments, &(acceptEvent->socketID), CALYPSO_INTFLAGS_SIZE8 | CALYPSO_INTFLAGS_UNSIGNED, CALYPSO_ARGUMENT_DELIM);

    if (ret)
    {
        ret = Calypso_GetNextArgumentString(pEventArguments, temp, CALYPSO_ARGUMENT_DELIM, sizeof(temp));
        if (ret)
        {
            ret = ATSocket_ParseSocketFamily(temp, &(acceptEvent->family));
        }
    }

    if (ret)
    {
        ret = Calypso_GetNextArgumentInt(pEventArguments, &(acceptEvent->clientPort), CALYPSO_INTFLAGS_SIZE16 | CALYPSO_INTFLAGS_UNSIGNED, CALYPSO_ARGUMENT_DELIM);
    }

    if (ret)
    {
        ret = Calypso_GetNextArgumentString(pEventArguments, acceptEvent->clientAddress, CALYPSO_STRING_TERMINATE, sizeof(acceptEvent->clientAddress));
    }

    return ret;
}

/**
 * @brief Parses the values of the socket receive / receive from event arguments.
 *
 * @param[in,out] pEventArguments String containing arguments of the AT command
 * @param[in] decodeBase64 Enables decoding of received Base64 data
 * @param[out] rcvdEvent The parsed socket receive / receive from event data
 *
 * @return true if parsed successfully, false otherwise
 */
bool ATEvent_ParseSocketRcvdEvent(char **pEventArguments,
                                  bool decodeBase64,
                                  ATEvent_SocketRcvd_t* rcvdEvent)
{
    if (!Calypso_GetNextArgumentInt(pEventArguments, &(rcvdEvent->socketID), CALYPSO_INTFLAGS_SIZE8 | CALYPSO_INTFLAGS_UNSIGNED, CALYPSO_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!Calypso_GetNextArgumentInt(pEventArguments, &(rcvdEvent->format), CALYPSO_INTFLAGS_SIZE8 | CALYPSO_INTFLAGS_UNSIGNED, CALYPSO_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!Calypso_GetNextArgumentInt(pEventArguments, &(rcvdEvent->length), CALYPSO_INTFLAGS_SIZE16 | CALYPSO_INTFLAGS_UNSIGNED, CALYPSO_ARGUMENT_DELIM))
    {
        return false;
    }

    if (decodeBase64)
    {
        uint32_t decodedSize = Calypso_GetBase64DecBufSize((uint8_t*) *pEventArguments, rcvdEvent->length);
        if (decodedSize - 1 > sizeof(rcvdEvent->data))
        {
            return false;
        }

        if (!Calypso_DecodeBase64((uint8_t*) *pEventArguments, rcvdEvent->length, (uint8_t*) rcvdEvent->data, &decodedSize))
        {
            return false;
        }
        rcvdEvent->length = decodedSize - 1;
        return true;
    }

    /* Received bytes count must not exceed buffer size */
    if (rcvdEvent->length >= sizeof(rcvdEvent->data) - 1)
    {
        return false;
    }
    return Calypso_GetNextArgumentString(pEventArguments, rcvdEvent->data, CALYPSO_STRING_TERMINATE, sizeof(rcvdEvent->data));
}

/**
 * @brief Parses the values of the IPv4 acquired event arguments.
 *
 * @param[in,out] pEventArguments String containing arguments of the AT command
 * @param[out] ipv4Event The parsed IPv4 acquired event data
 *
 * @return true if parsed successfully, false otherwise
 */
bool ATEvent_ParseNetappIP4AcquiredEvent(char **pEventArguments, ATEvent_NetappIP4Acquired_t* ipv4Event)
{
    bool ret = Calypso_GetNextArgumentString(pEventArguments, ipv4Event->address, CALYPSO_ARGUMENT_DELIM, sizeof(ipv4Event->address));

    if (ret)
    {
        ret = Calypso_GetNextArgumentString(pEventArguments, ipv4Event->gateway, CALYPSO_ARGUMENT_DELIM, sizeof(ipv4Event->gateway));
    }

    if (ret)
    {
        ret = Calypso_GetNextArgumentString(pEventArguments, ipv4Event->DNS, CALYPSO_STRING_TERMINATE, sizeof(ipv4Event->DNS));
    }

    return ret;
}

/**
 * @brief Retrieves the HTTP GET event ID argument.
 *
 * @param[in,out] pEventArguments String containing arguments of the AT command
 * @param[out] id The HTTP GET event ID
 * @param[in] maxIdLength Max. length of ID (including string termination character).
 *
 * @return true if parsed successfully, false otherwise
 */
bool ATEvent_ParseHttpGetEvent(char **pEventArguments, char *id, uint16_t maxIdLength)
{
    return Calypso_GetNextArgumentString(pEventArguments, id, CALYPSO_STRING_TERMINATE, maxIdLength);
}

/**
 * @brief Parses the values of the file list entry event arguments.
 *
 * @param[in,out] pEventArguments String containing arguments of the AT command
 * @param[out] fileListEntry The parsed file list entry data
 *
 * @return true if parsed successfully, false otherwise
 */
bool ATEvent_ParseFileListEntryEvent(char **pEventArguments, ATFile_FileListEntry_t* fileListEntry)
{
    return ATFile_ParseFileListEntry(pEventArguments, fileListEntry);
}

/**
 * @brief Parses the values of the custom GPIO event arguments.
 *
 * @param[in,out] pEventArguments String containing arguments of the AT command
 * @param[out] gpioId ID of the GPIO that has been configured via the module's REST API.
 *
 * @return true if parsed successfully, false otherwise
 */
bool ATEvent_ParseCustomGPIOEvent(char **pEventArguments, uint8_t *gpioId)
{
    return Calypso_GetNextArgumentInt(pEventArguments,
                                      gpioId,
                                      CALYPSO_INTFLAGS_SIZE8 | CALYPSO_INTFLAGS_UNSIGNED | CALYPSO_INTFLAGS_NOTATION_DEC,
                                      CALYPSO_STRING_TERMINATE);
}

/**
 * @brief Parses the values of the custom HTTP POST event arguments.
 *
 * @param[in,out] pEventArguments String containing arguments of the AT command
 * @param[out] id HTTP POST event id
 * @param[out] value HTTP POST event value
 * @param[in] maxIdLength Max. length of ID (including string termination character)
 * @param[in] maxValueLength Max. length of value (including string termination character)
 *
 * @return true if parsed successfully, false otherwise
 */
bool ATEvent_ParseCustomHTTPPostEvent(char **pEventArguments,
                                      char *id,
                                      char *value,
                                      uint16_t maxIdLength,
                                      uint16_t maxValueLength)
{
    if (!Calypso_GetNextArgumentString(pEventArguments, id, CALYPSO_ARGUMENT_DELIM, maxIdLength))
    {
        return false;
    }
    return Calypso_GetNextArgumentString(pEventArguments, value, CALYPSO_STRING_TERMINATE, maxValueLength);
}

/**
 * @brief Parses an event sub type string (first argument of received event string) to ATEvent_t.
 *
 * @param[in] eventSubTypeString String containing the event's ID
 * @param[in] eventMainType Main event type category (ATEvent_General, ATEvent_Wlan, ATEvent_Socket,
 *            ATEvent_Netapp, ATEvent_MQTT or ATEvent_FatalError)
 * @param[out] pEventSubType ATEvent_t representing the event
 *
 * @return true if parsed successfully, false otherwise
 */
static bool ATEvent_ParseEventSubType(const char *eventSubTypeString,
                                      ATEvent_t eventMainType,
                                      ATEvent_t *pEventSubType)
{
    uint8_t typeCount = 0;

    switch (eventMainType)
    {
    case ATEvent_General:
        typeCount = ATEvent_General_NumberOfValues;
        break;

    case ATEvent_Wlan:
        typeCount = ATEvent_WLAN_NumberOfValues;
        break;

    case ATEvent_Socket:
        typeCount = ATEvent_Socket_NumberOfValues;
        break;

    case ATEvent_Netapp:
        typeCount = ATEvent_Netapp_NumberOfValues;
        break;

    case ATEvent_MQTT:
        typeCount = ATEvent_MQTT_NumberOfValues;
        break;

    case ATEvent_FatalError:
        typeCount = ATEvent_FatalError_NumberOfValues;
        break;

    default:
        break;
    }

    *pEventSubType = ATEvent_Invalid;

    for (int i = 0; i < typeCount; i++)
    {
        ATEvent_t event = eventMainType + (ATEvent_t) i;
        if (0 == strcasecmp(eventSubTypeString, ATEvent_Strings[event]))
        {
            *pEventSubType = event;
            return true;
        }
    }

    return false;
}
