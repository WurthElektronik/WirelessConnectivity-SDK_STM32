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
 * @brief AT commands for socket functionality.
 */

#ifndef STEPHANOI_SOCKET_H_INCLUDED
#define STEPHANOI_SOCKET_H_INCLUDED

#include <StephanoI/ATCommands/ATEvent.h>
#include <StephanoI/ATCommands/ATWifi.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

#define SOCKET_DATALEN_MAX (1024 * 16)
#define SOCKET_TYPE_LEN (5 + 2 + 1 + 1)

/**
 * @brief Parameters of Socket get state Event
 */
typedef struct StephanoI_ATSocket_State_t
{
    uint8_t link_ID;
    char type[SOCKET_TYPE_LEN];
    char remote_ip[WIFI_IPv6_STRINGLEN];
    uint32_t remote_port;
    uint32_t local_port;
    uint8_t tetype;
} StephanoI_ATSocket_State_t;

/**
 * @brief Parameters of Socket ping Event
 */
typedef uint32_t StephanoI_ATSocket_Ping_t;

typedef char StephanoI_ATSocket_DomainResolved_t[WIFI_IPv6_STRINGLEN];

typedef struct StephanoI_ATSocket_SendState_t
{
    uint32_t had_sent_len;
    uint32_t port_rcv_len;
} StephanoI_ATSocket_SendState_t;

typedef struct StephanoI_ATSocket_Receive_t
{
    uint8_t link_ID;
    uint32_t length;
    uint8_t data[SOCKET_DATALEN_MAX];
} StephanoI_ATSocket_Receive_t;

typedef struct StephanoI_ATSocket_ReceiveData_t
{
    uint32_t actual_len;
    char remote_ip[WIFI_IPv6_STRINGLEN];
    uint32_t remote_port;
    uint8_t data[SOCKET_DATALEN_MAX];
} StephanoI_ATSocket_ReceiveData_t;

typedef struct StephanoI_ATSocket_ReceiveLen_t
{
    uint16_t datalen_link0;
    uint16_t datalen_link1;
    uint16_t datalen_link2;
    uint16_t datalen_link3;
    uint16_t datalen_link4;
} StephanoI_ATSocket_ReceiveLen_t;

typedef enum StephanoI_ATSocket_ReceiveMode_t
{
    StephanoI_ATSocket_ReceiveMode_Active = 0,
    StephanoI_ATSocket_ReceiveMode_Passive
} StephanoI_ATSocket_ReceiveMode_t;

/**
 * @brief Wifi socket type
 */
typedef enum StephanoI_ATSocket_Type_t
{
    StephanoI_ATSocket_Type_TCP,
    StephanoI_ATSocket_Type_TCPv6,
    StephanoI_ATSocket_Type_UDP,
    StephanoI_ATSocket_Type_UDPv6,
    StephanoI_ATSocket_Type_SSL,
    StephanoI_ATSocket_Type_SSLv6
} StephanoI_ATSocket_Type_t;

/**
 * @brief Wifi time zone type
 */
typedef enum StephanoI_ATSocket_TimeZone_t
{
    StephanoI_ATSocket_TimeZone_UTCminus12 = -12,
    StephanoI_ATSocket_TimeZone_UTCminus11 = -11,
    StephanoI_ATSocket_TimeZone_UTCminus10 = -10,
    StephanoI_ATSocket_TimeZone_UTCminus9 = -9,
    StephanoI_ATSocket_TimeZone_UTCminus8 = -8,
    StephanoI_ATSocket_TimeZone_UTCminus7 = -7,
    StephanoI_ATSocket_TimeZone_UTCminus6 = -6,
    StephanoI_ATSocket_TimeZone_UTCminus5 = -5,
    StephanoI_ATSocket_TimeZone_UTCminus4 = -4,
    StephanoI_ATSocket_TimeZone_UTCminus3 = -3,
    StephanoI_ATSocket_TimeZone_UTCminus2 = -2,
    StephanoI_ATSocket_TimeZone_UTCminus1 = -1,
    StephanoI_ATSocket_TimeZone_UTC = 0,
    StephanoI_ATSocket_TimeZone_UTCPlus1 = 1,
    StephanoI_ATSocket_TimeZone_UTCPlus2 = 2,
    StephanoI_ATSocket_TimeZone_UTCPlus3 = 3,
    StephanoI_ATSocket_TimeZone_UTCPlus4 = 4,
    StephanoI_ATSocket_TimeZone_UTCPlus5 = 5,
    StephanoI_ATSocket_TimeZone_UTCPlus6 = 6,
    StephanoI_ATSocket_TimeZone_UTCPlus7 = 7,
    StephanoI_ATSocket_TimeZone_UTCPlus8 = 8,
    StephanoI_ATSocket_TimeZone_UTCPlus9 = 9,
    StephanoI_ATSocket_TimeZone_UTCPlus10 = 10,
    StephanoI_ATSocket_TimeZone_UTCPlus11 = 11,
    StephanoI_ATSocket_TimeZone_UTCPlus12 = 12,
    StephanoI_ATSocket_TimeZone_UTCPlus13 = 13,
    StephanoI_ATSocket_TimeZone_UTCPlus14 = 14,
} StephanoI_ATSocket_TimeZone_t;

/**
 * @brief Wifi SSL auth mode
 */
typedef enum StephanoI_ATSocket_SSLAuthMode_t
{
    StephanoI_ATSocket_SSLAuthMode_NoAuth = 0,
    StephanoI_ATSocket_SSLAuthMode_ClientHasCert = 1,
    StephanoI_ATSocket_SSLAuthMode_ClientRequestsCert = 2,
    StephanoI_ATSocket_SSLAuthMode_Mutual = 3,
} StephanoI_ATSocket_SSLAuthMode_t;

typedef char StephanoI_ATSocket_SNTP_Time_t[64];

typedef struct StephanoI_ATSocket_SSLClient_t
{
    uint8_t link_ID;
    StephanoI_ATSocket_SSLAuthMode_t auth_mode;
    uint8_t pki_number;
    uint8_t ca_number;
} StephanoI_ATSocket_SSLClient_t;

typedef struct StephanoI_ATSocket_SSLClientName_t
{
    uint8_t link_ID;
    char common_name[64 + 2];
} StephanoI_ATSocket_SSLClientName_t;

typedef struct StephanoI_ATSocket_SSLClientServerNameIndication_t
{
    uint8_t link_ID;
    char sni[64 + 2];
} StephanoI_ATSocket_SSLClientServerNameIndication_t;

typedef struct StephanoI_ATSocket_SSLClientALPN_t
{
    uint8_t link_ID;
    uint8_t counts;
    char alpn[64 + 2];
    char alpn1[64 + 2];
    char alpn2[64 + 2];
    char alpn3[64 + 2];
    char alpn4[64 + 2];
    char alpn5[64 + 2];
} StephanoI_ATSocket_SSLClientALPN_t;

typedef struct StephanoI_ATSocket_SSLClientPSK_t
{
    uint8_t link_ID;
    char psk[32 + 2];
    char hint[32 + 2];
} StephanoI_ATSocket_SSLClientPSK_t;

/**
 * @brief Enable IPv6 network
 *
 * @param[in] enable: Enable IPv6
 *
 * @return True if successful, false otherwise
 */
extern bool StephanoI_ATSocket_SetEnableIPv6(bool enable);

/**
 * @brief Read if IPv6 network is enabled
 *
 * @param[in] enable: Enable IPv6
 *
 * @return True if successful, false otherwise
 */
extern bool StephanoI_ATSocket_GetEnableIPv6(bool* enable);

/**
 * @brief Enable multiple socket connections
 *
 * @param[in] multiple: Multiple enabled
 *
 * @return True if successful, false otherwise
 */
extern bool StephanoI_ATSocket_SetMultiple(bool multiple);

/**
 * @brief Check if multiple socket connections are enabled
 *
 * @param[out] multipleP: Pointer to multiple enabled
 *
 * @return True if successful, false otherwise
 */
extern bool StephanoI_ATSocket_GetMultiple(bool* multipleP);

/**
 * @brief Open a TCP socket connection
 * @param[in] multiple_connections: Multiple connections are enabled or not
 * @param[in] link_ID: Link ID in case of multiple connections
 * @param[in] type: Socket type, TCP, TCPv6
 * @param[in] remote_IP: Remote IP
 * @param[in] remote_port: Remote port
 *
 * @return True if successful, false otherwise
 */
extern bool StephanoI_ATSocket_OpenTCPSocket(bool multiple_connections, uint8_t link_ID, StephanoI_ATSocket_Type_t type, char* remote_IP, uint32_t remote_port);

/**
 * @brief Open a UDP socket connection
 * @param[in] multiple_connections: Multiple connections are enabled or not
 * @param[in] link_ID: Link ID in case of multiple connections
 * @param[in] type: Socket type, UPD, UPDv6
 * @param[in] remote_IP: Remote IP
 * @param[in] remote_port: Remote port
 * @param[in] local_port: Remote port
 *
 * @return True if successful, false otherwise
 */
extern bool StephanoI_ATSocket_OpenUDPSocket(bool multiple_connections, uint8_t link_ID, StephanoI_ATSocket_Type_t type, char* remote_IP, uint32_t remote_port, uint32_t local_port);

/**
 * @brief Open a socket connection
 *
 * @param[in] type: Socket type, UPD, UPDv6,TCP, TCPv6
 * @param[in] remote_IP: Remote IP
 * @param[in] remote_port: Remote port
 *
 * @return True if successful, false otherwise
 */
extern bool StephanoI_ATSocket_OpenSocketEx(StephanoI_ATSocket_Type_t type, char* remote_IP, uint32_t remote_port);

/**
 * @brief Close a socket connection
 *
 * @param[in] multiple_connections: Multiple connections are enabled or not
 * @param[in] link_ID: Link ID in case of multiple connections
 *
 * @return True if successful, false otherwise
 */
extern bool StephanoI_ATSocket_CloseSocket(bool multiple_connections, uint8_t link_ID);

/**
 * @brief Get socket state
 *
 * @param[out] t: Pointer to socket state
 *
 * @return True if successful, false otherwise
 */
extern bool StephanoI_ATSocket_GetState(StephanoI_ATSocket_State_t* t);

/**
 * @brief Ping a host
 *
 * @param[in] host: Host to ping
 * @param[out] t: The parsed event data
 *
 * @return True if successful, false otherwise
 */
extern bool StephanoI_ATSocket_Ping(char* host, StephanoI_ATSocket_Ping_t* t);

/**
 * @brief Resolve domain
 *
 * @param[in] domain_name: Name to resolve
 * @param[out] domainP: Pointer to the resolved domain
 *
 * @return True if successful, false otherwise
 */
extern bool StephanoI_ATSocket_ResolveDomain(char* domain_name, StephanoI_ATSocket_DomainResolved_t* domainP);

/**
 * @brief Send data to the socket
 *
 * @param[in] multiple_connections: Multiple connections are enabled or not
 * @param[in] link_id: Link ID in case of multiple connections
 * @param[in] remote_host: Remote host for UPD only
 * @param[in] remote_port: Remote port for UPD only
 * @param[in] data: Pointer to the data
 * @param[in] length: Length of the data
 *
 * @return True if successful, false otherwise
 */
extern bool StephanoI_ATSocket_Send(bool multiple_connections, uint8_t link_id, char* remote_host, uint32_t remote_port, uint8_t* data, uint32_t length);

/**
 * @brief Send data configuration
 *
 * @param[in] report_size: Transport size
 * @param[in] transmit_size: Transmit size
 *
 * @return True if successful, false otherwise
 */
extern bool StephanoI_ATSocket_SendConfiguration(uint32_t report_size, uint32_t transmit_size);

/**
 * @brief Create/Delete the TCP/SSL server
 *
 * @param[in] multiple_connections: Multiple connections are enabled or not
 * @param[in] mode: Create or delete the server
 * @param[in] param2: Depends on mode
 * @param[in] type: Type TCP, TCPv6, SSL, SSLv6
 * @param[in] CA_enable: Enable CA
 *
 * @return True if successful, false otherwise
 */
extern bool StephanoI_ATSocket_EnableTCPSSLServer(bool multiple_connections, bool mode, uint16_t param2, StephanoI_ATSocket_Type_t type, bool CA_enable);

/**
 * @brief Set the max connections of the TCP/SSL server
 *
 * @param[in] max_num: number of connections to be set
 *
 * @return True if successful, false otherwise
 */
extern bool StephanoI_ATSocket_SetTCPSSLServerMaxConnections(uint8_t max_num);

/**
 * @brief Get the max connections of the TCP/SSL server
 *
 * @param[out] max_num: number of connections
 * @return True if successful, false otherwise
 */
extern bool StephanoI_ATSocket_GetTCPSSLServerMaxConnections(uint8_t* max_num);

/**
 * @brief Set the TCP server timeout
 *
 * @param[in] time: Time out in s [0, 7200]
 *
 * @return True if successful, false otherwise
 */
extern bool StephanoI_ATSocket_SetTCPSSLServerTimeout(uint16_t time);

/**
 * @brief Get the timeout of the TCP/SSL server
 *
 * @param[out] time:  Time out in s [0, 7200]
 * @return True if successful, false otherwise
 */
extern bool StephanoI_ATSocket_GetTCPSSLServerTimeout(uint16_t* time);

/**
 * @brief Enable/Disable the SNTP
 *
 * @param[in] enable: Enable or disable the SNTP
 * @param[in] timezone: Timezone
 * @param[in] server1: SNTP server address
 *
 * @return True if successful, false otherwise
 */
extern bool StephanoI_ATSocket_EnableSNTPServer(bool enable, StephanoI_ATSocket_TimeZone_t timezone, char* server1);

/**
 * @brief Read the SNTP time
 *
 * @param[out] t: The parsed event data
 * @return True if successful, false otherwise
 */
extern bool StephanoI_ATSocket_ReadSNTPTime(StephanoI_ATSocket_SNTP_Time_t* t);

/**
 * @brief Set SNTP time interval
 *
 * @param[in] time: Time interval in s [15,4294967]
 *
 * @return True if successful, false otherwise
 */
extern bool StephanoI_ATSocket_SetSNTPTimeInterval(uint32_t time);

/**
 * @brief Get SNTP time interval
 *
 * @param[out] time: Pointer to the time interval in s
 *
 * @return True if successful, false otherwise
 */
extern bool StephanoI_ATSocket_GetSNTPTimeInterval(uint32_t* time);

/**
 * @brief Set the SSL client
 *
 * @param[in] multiple_connections: Multiple connections are enabled or not
 * @param[in] client: SSL client data
 *
 * @return True if successful, false otherwise
 */
extern bool StephanoI_ATSocket_SetSSLClient(bool multiple_connections, StephanoI_ATSocket_SSLClient_t client);

/**
 * @brief Get name of the SSL client
 *
 * @return True if successful, false otherwise
 */
extern bool StephanoI_ATSocket_GetSSLClient();

/**
 * @brief Set name of the SSL client
 *
 * @param[in] multiple_connections: Multiple connections are enabled or not
 * @param[in] name: Name
 *
 * @return True if successful, false otherwise
 */
extern bool StephanoI_ATSocket_SetSSLClientName(bool multiple_connections, StephanoI_ATSocket_SSLClientName_t name);

/**
 * @brief Get name of the SSL client
 *
 * @param[out] name: Pointer to name
 *
 * @return True if successful, false otherwise
 */
extern bool StephanoI_ATSocket_GetSSLClientName(StephanoI_ATSocket_SSLClientName_t* name);

/**
 * @brief Set the SSL client server name indication
 *
 * @param[in] multiple_connections: Multiple connections are enabled or not
 * @param[in] name: Name
 *
 * @return True if successful, false otherwise
 */
extern bool StephanoI_ATSocket_SetSSLClientServerNameIndication(bool multiple_connections, StephanoI_ATSocket_SSLClientServerNameIndication_t name);

/**
 * @brief Get name of the SSL client Server Name Indication
 *
 * @param[out] name: Pointer to name
 *
 * @return True if successful, false otherwise
 */
extern bool StephanoI_ATSocket_GetSSLClientServerNameIndication(StephanoI_ATSocket_SSLClientServerNameIndication_t* name);

/**
 * @brief Set the SSL client ALPN
 *
 * @param[in] multiple_connections: Multiple connections are enabled or not
 * @param[in] alpn: ALPN
 *
 * @return True if successful, false otherwise
 */
extern bool StephanoI_ATSocket_SetSSLClientALPN(bool multiple_connections, StephanoI_ATSocket_SSLClientALPN_t alpn);

/**
 * @brief Get ALPN
 *
 * @param[out] alpn: Pointer to alpn
 *
 * @return True if successful, false otherwise
 */
extern bool StephanoI_ATSocket_GetSSLClientALPN(StephanoI_ATSocket_SSLClientALPN_t* alpn);

/**
 * @brief Set the SSL client PSK
 *
 * @param[in] multiple_connections: Multiple connections are enabled or not
 * @param[in] psk: PSK
 *
 * @return True if successful, false otherwise
 */
extern bool StephanoI_ATSocket_SetSSLClientPSK(bool multiple_connections, StephanoI_ATSocket_SSLClientPSK_t psk);

/**
 * @brief Get PSK
 *
 * @param[out] psk: Pointer to psk
 *
 * @return True if successful, false otherwise
 */
extern bool StephanoI_ATSocket_GetSSLClientPSK(StephanoI_ATSocket_SSLClientPSK_t* psk);

/**
 * @brief Set DNS server information
 *
 * @param[in] manual: Manual or auto
 * @param[in] DNS_server: DNS server
 *
 * @return True if successful, false otherwise
 */
extern bool StephanoI_ATSocket_SetDNSServerInformation(bool manual, char* DNS_server);

/**
 * @brief Set the receive mode passive
 *
 * @param[in] mode: Passive or active
 *
 * @return True if successful, false otherwise
 */
extern bool StephanoI_ATSocket_SetReceiveMode(StephanoI_ATSocket_ReceiveMode_t mode);

/**
 * @brief Get the receive mode passive
 *
 * @param[in] modeP: Pointer to the boolean that is true in case of passive
 *
 * @return True if successful, false otherwise
 */
extern bool StephanoI_ATSocket_GetReceiveMode(StephanoI_ATSocket_ReceiveMode_t* modeP);

/**
 * @brief Get the length of received data
 *
 * @param[out] t: The parsed event data
 *
 * @return True if successful, false otherwise
 */
extern bool StephanoI_ATSocket_GetReceiveLength(StephanoI_ATSocket_ReceiveLen_t* t);

/**
 * @brief Request for received data
 *
 * @param[in] multiple_connections: Multiple connections are enabled or not
 * @param[in] link_ID: Link ID in case of multiple connections
 * @param[in] len: Length of the data
 * @param[out] dataP: Pointer to the received data
 *
 * @return True if successful, false otherwise
 */
extern bool StephanoI_ATSocket_GetReceivedData(bool multiple_connections, uint8_t link_ID, uint16_t len, StephanoI_ATSocket_ReceiveData_t* dataP);

/**
 * @brief Parses the values of the socket state arguments.
 *
 * @param[in] EventArgumentsP: String containing arguments of the AT command
 * @param[out] t: The parsed event data
 *
 * @return True if parsed successfully, false otherwise
 */
extern bool StephanoI_ATSocket_ParseState(char* EventArgumentsP, StephanoI_ATSocket_State_t* t);

/**
 * @brief Parses the values of the socket ping arguments.
 *
 * @param[in] EventArgumentsP: String containing arguments of the AT command
 * @param[out] t: The parsed event data
 *
 * @return True if parsed successfully, false otherwise
 */
extern bool StephanoI_ATSocket_ParsePing(char* EventArgumentsP, StephanoI_ATSocket_Ping_t* t);

/**
 * @brief Parses the values of the socket domain resolved arguments.
 *
 * @param[in] EventArgumentsP: String containing arguments of the AT command
 * @param[out] t: The parsed event data
 *
 * @return True if parsed successfully, false otherwise
 */
extern bool StephanoI_ATSocket_ParseDomainResolved(char* EventArgumentsP, StephanoI_ATSocket_DomainResolved_t* t);

/**
 * @brief Parses the values of the socket send state arguments.
 *
 * @param[in] EventArgumentsP: String containing arguments of the AT command
 * @param[out] t: The parsed event data
 *
 * @return True if parsed successfully, false otherwise
 */
extern bool StephanoI_ATSocket_ParseSendState(char* EventArgumentsP, StephanoI_ATSocket_SendState_t* t);

/**
 * @brief Parses the values of the socket receive event arguments.
 *
 * @param[in] EventArgumentsP:      String containing arguments of the AT command
 * @param[in] multiple_connections: Multiple connections are enabled or not
 * @param[out] t: The parsed event data
 *
 * @return True if parsed successfully, false otherwise
 */
extern bool StephanoI_ATSocket_ParseReceive(char* EventArgumentsP, bool multiple_connections, StephanoI_ATSocket_Receive_t* t);

/**
 * @brief Parses the values of the socket receive data event arguments.
 *
 * @param[in] EventArgumentsP: String containing arguments of the AT command
 * @param[out] t: The parsed event data
 *
 * @return True if parsed successfully, false otherwise
 */
extern bool StephanoI_ATSocket_ParseReceiveData(char* EventArgumentsP, StephanoI_ATSocket_ReceiveData_t* t);

/**
 * @brief Parses the values of the socket receive len event arguments.
 *
 * @param[in] EventArgumentsP: String containing arguments of the AT command
 * @param[out] t: The parsed event data
 *
 * @return True if parsed successfully, false otherwise
 */
extern bool StephanoI_ATSocket_ParseReceiveLen(char* EventArgumentsP, StephanoI_ATSocket_ReceiveLen_t* t);

/**
 * @brief Parses the values of the socket get multiple event arguments.
 *
 * @param[in] EventArgumentsP: String containing arguments of the AT command
 * @param[out] b: Is multiple enabled
 *
 * @return True if parsed successfully, false otherwise
 */
extern bool StephanoI_ATSocket_ParseGetMultiple(char* EventArgumentsP, bool* b);

/**
 * @brief Parses the values of the sntp time event arguments.
 *
 * @param[in] EventArgumentsP: String containing arguments of the AT command
 * @param[out] t: The parsed event data
 *
 * @return True if parsed successfully, false otherwise
 */
extern bool StephanoI_ATSocket_ParseSNTPTime(char* EventArgumentsP, StephanoI_ATSocket_SNTP_Time_t* t);

/**
 * @brief Parses the values of the sntp time interval event arguments.
 *
 * @param[in] EventArgumentsP: String containing arguments of the AT command
 * @param[out] t: The parsed event data
 *
 * @return True if parsed successfully, false otherwise
 */
extern bool StephanoI_ATSocket_ParseSNTPTimeInterval(char* EventArgumentsP, uint32_t* t);

/**
 * @brief Parses the values of the receive mode event arguments.
 *
 * @param[in] EventArgumentsP: String containing arguments of the AT command
 * @param[out] b: The parsed event data
 *
 * @return True if parsed successfully, false otherwise
 */
extern bool StephanoI_ATSocket_ParseReceiveMode(char* EventArgumentsP, StephanoI_ATSocket_ReceiveMode_t* b);

/**
 * @brief Parses the values of the IPv6 enable event arguments.
 *
 * @param[in] EventArgumentsP: String containing arguments of the AT command
 * @param[out] t: The parsed event data
 *
 * @return True if parsed successfully, false otherwise
 */
extern bool StephanoI_ATSocket_ParseEnableIPv6(char* EventArgumentsP, bool* t);

/**
 * @brief Parses the values of the max connection event arguments.
 *
 * @param[in] EventArgumentsP: String containing arguments of the AT command
 * @param[out] t: The parsed event data
 *
 * @return True if parsed successfully, false otherwise
 */
extern bool StephanoI_ATSocket_ParseTCPSSLServerMaxConnections(char* EventArgumentsP, uint8_t* t);

/**
 * @brief Parses the values of the server timeout event arguments.
 *
 * @param[in] EventArgumentsP: String containing arguments of the AT command
 * @param[out] t: The parsed event data
 *
 * @return True if parsed successfully, false otherwise
 */
extern bool StephanoI_ATSocket_ParseTCPSSLServerTimeout(char* EventArgumentsP, uint16_t* t);

/**
 * @brief Parses the values of the ssl client event arguments.
 *
 * @param[in] EventArgumentsP: String containing arguments of the AT command
 * @param[out] t: The parsed event data
 *
 * @return True if parsed successfully, false otherwise
 */
extern bool StephanoI_ATSocket_ParseSSLClient(char* EventArgumentsP, StephanoI_ATSocket_SSLClient_t* t);

/**
 * @brief Parses the values of the ssl client name event arguments.
 *
 * @param[in] EventArgumentsP: String containing arguments of the AT command
 * @param[out] t: The parsed event data
 *
 * @return True if parsed successfully, false otherwise
 */
extern bool StephanoI_ATSocket_ParseSSLClientName(char* EventArgumentsP, StephanoI_ATSocket_SSLClientName_t* t);

/**
 * @brief Parses the values of the ssl server name indication event arguments.
 *
 * @param[in] EventArgumentsP: String containing arguments of the AT command
 * @param[out] t: The parsed event data
 *
 * @return True if parsed successfully, false otherwise
 */
extern bool StephanoI_ATSocket_ParseSSLClientServerNameIndication(char* EventArgumentsP, StephanoI_ATSocket_SSLClientServerNameIndication_t* t);

/**
 * @brief Parses the values of the ssl ALPN event arguments.
 *
 * @param[in] EventArgumentsP: String containing arguments of the AT command
 * @param[out] t: The parsed event data
 *
 * @return True if parsed successfully, false otherwise
 */
extern bool StephanoI_ATSocket_ParseSSLALPN(char* EventArgumentsP, StephanoI_ATSocket_SSLClientALPN_t* t);

/**
 * @brief Parses the values of the ssl PSK event arguments.
 *
 * @param[in] EventArgumentsP: String containing arguments of the AT command
 * @param[out] t: The parsed event data
 *
 * @return True if parsed successfully, false otherwise
 */
extern bool StephanoI_ATSocket_ParseSSLPSK(char* EventArgumentsP, StephanoI_ATSocket_SSLClientPSK_t* t);

#ifdef __cplusplus
}
#endif

#endif /* STEPHANOI_SOCKET_H_INCLUDED */
