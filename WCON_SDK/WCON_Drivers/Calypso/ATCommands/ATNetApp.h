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
 * @file ATNetApp.h
 * @brief AT commands for network application control.
 */

#ifndef CALYPSO_AT_NETAPP_H_INCLUDED
#define CALYPSO_AT_NETAPP_H_INCLUDED
#include <Calypso/ATCommands/ATSocket.h>
#include <Calypso/Calypso.h>
#include <global/ATCommands.h>
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief Network applications (flags).
 * Used to start/stop network applications via Calypso_ATNetApp_StartApplications() / Calypso_ATNetApp_StopApplications()
 * as well as for configuring the applications via Calypso_ATNetApp_Set() / Calypso_ATNetApp_Get().
 */
typedef enum Calypso_ATNetApp_Application_t
{
    Calypso_ATNetApp_Application_None = 0,
    Calypso_ATNetApp_Application_HttpServer = (1 << 0), /**< HTTP server */
    Calypso_ATNetApp_Application_DhcpServer = (1 << 1), /**< DHCP server */
    Calypso_ATNetApp_Application_mDns = (1 << 2),       /**< mDNS (multicast-DNS) */
    Calypso_ATNetApp_Application_DnsServer = (1 << 3),  /**< DNS server */
    Calypso_ATNetApp_Application_SntpClient = (1 << 4), /**< SNTP client */
    Calypso_ATNetApp_Application_DnsClient = (1 << 5),  /**< DNS client (can't be started/stopped, configuration only) */
    Calypso_ATNetApp_Application_Device = (1 << 6),     /**< Device (can't be started/stopped, configuration only) */
    Calypso_ATNetApp_Application_NumberOfValues = 7
} Calypso_ATNetApp_Application_t;

/**
 * @brief HTTP server options.
 * See Calypso_ATNetApp_Set() / Calypso_ATNetApp_Get() and Calypso_ATNetApp_Application_HttpServer.
 */
typedef enum Calypso_ATNetApp_HttpOption_t
{
    Calypso_ATNetApp_HttpOption_PrimPortNum,
    Calypso_ATNetApp_HttpOption_AuthCheck,
    Calypso_ATNetApp_HttpOption_AuthName,
    Calypso_ATNetApp_HttpOption_AuthPassword,
    Calypso_ATNetApp_HttpOption_AuthRealm,
    Calypso_ATNetApp_HttpOption_RomPagesAccess,
    Calypso_ATNetApp_HttpOption_SecondPortNum,
    Calypso_ATNetApp_HttpOption_SecondPortEn,
    Calypso_ATNetApp_HttpOption_PrimPortSecEn,
    Calypso_ATNetApp_HttpOption_PrivKeyFile,
    Calypso_ATNetApp_HttpOption_DevCertFile,
    Calypso_ATNetApp_HttpOption_CaCertFile,
    Calypso_ATNetApp_HttpOption_TmpRegisterService,
    Calypso_ATNetApp_HttpOption_TmpUnregisterService,
    /** @cond DOXYGEN_IGNORE */
    Calypso_ATNetApp_HttpOption_NumberOfValues
    /** @endcond */
} Calypso_ATNetApp_HttpOption_t;

/**
 * @brief DHCP server options.
 * See Calypso_ATNetApp_Set() / Calypso_ATNetApp_Get() and Calypso_ATNetApp_Application_DhcpServer.
 */
typedef enum Calypso_ATNetApp_DhcpOption_t
{
    Calypso_ATNetApp_DhcpOption_Basic,
    /** @cond DOXYGEN_IGNORE */
    Calypso_ATNetApp_DhcpOption_NumberOfValues
    /** @endcond */
} Calypso_ATNetApp_DhcpOption_t;

/**
 * @brief mDNS options.
 * See Calypso_ATNetApp_Set() / Calypso_ATNetApp_Get() and Calypso_ATNetApp_Application_mDns.
 */
typedef enum Calypso_ATNetApp_mDnsOption_t
{
    Calypso_ATNetApp_mDnsOption_ContQuery,
    Calypso_ATNetApp_mDnsOption_QeventMask,
    Calypso_ATNetApp_mDnsOption_TimingParams,
    /** @cond DOXYGEN_IGNORE */
    Calypso_ATNetApp_mDnsOption_NumberOfValues
    /** @endcond */
} Calypso_ATNetApp_mDnsOption_t;

/**
 * @brief mDns event mask (flags).
 * See Calypso_ATNetApp_Set() / Calypso_ATNetApp_Get() and Calypso_ATNetApp_Application_mDns / Calypso_ATNetApp_mDnsOption_QeventMask.
 */
typedef enum Calypso_ATNetApp_mDnsEventMask_t
{
    Calypso_ATNetApp_mDnsEventMask_IPP = (1 << 0),
    Calypso_ATNetApp_mDnsEventMask_DeviceInfo = (1 << 1),
    Calypso_ATNetApp_mDnsEventMask_HTTP = (1 << 2),
    Calypso_ATNetApp_mDnsEventMask_HTTPS = (1 << 3),
    Calypso_ATNetApp_mDnsEventMask_Workstation = (1 << 4),
    Calypso_ATNetApp_mDnsEventMask_GUID = (1 << 5),
    Calypso_ATNetApp_mDnsEventMask_H323 = (1 << 6),
    Calypso_ATNetApp_mDnsEventMask_NTP = (1 << 7),
    Calypso_ATNetApp_mDnsEventMask_Objective = (1 << 8),
    Calypso_ATNetApp_mDnsEventMask_RDP = (1 << 9),
    Calypso_ATNetApp_mDnsEventMask_Remote = (1 << 10),
    Calypso_ATNetApp_mDnsEventMask_RTSP = (1 << 11),
    Calypso_ATNetApp_mDnsEventMask_SIP = (1 << 12),
    Calypso_ATNetApp_mDnsEventMask_SMB = (1 << 13),
    Calypso_ATNetApp_mDnsEventMask_SOAP = (1 << 14),
    Calypso_ATNetApp_mDnsEventMask_SSH = (1 << 15),
    Calypso_ATNetApp_mDnsEventMask_Telnet = (1 << 16),
    Calypso_ATNetApp_mDnsEventMask_TFTP = (1 << 17),
    Calypso_ATNetApp_mDnsEventMask_XMPP = (1 << 18),
    Calypso_ATNetApp_mDnsEventMask_RAOP = (1 << 19),
    Calypso_ATNetApp_mDnsEventMask_NumberOfValues = 20
} Calypso_ATNetApp_mDnsEventMask_t;

/**
 * @brief SNTP client options.
 * See Calypso_ATNetApp_Set() / Calypso_ATNetApp_Get() and Calypso_ATNetApp_Application_SntpClient.
 */
typedef enum Calypso_ATNetApp_SntpOption_t
{
    Calypso_ATNetApp_SntpOption_Enable,
    Calypso_ATNetApp_SntpOption_UpdateInterval,
    Calypso_ATNetApp_SntpOption_TimeZone,
    Calypso_ATNetApp_SntpOption_Servers,
    /** @cond DOXYGEN_IGNORE */
    Calypso_ATNetApp_SntpOption_NumberOfValues
    /** @endcond */
} Calypso_ATNetApp_SntpOption_t;

/**
 * @brief DNS client options
 * See Calypso_ATNetApp_Set() / Calypso_ATNetApp_Get() and Calypso_ATNetApp_Application_DnsClient.
 */
typedef enum Calypso_ATNetApp_DnsClientOption_t
{
    Calypso_ATNetApp_DnsClientOption_Time,
    /** @cond DOXYGEN_IGNORE */
    Calypso_ATNetApp_DnsClientOption_NumberOfValues
    /** @endcond */
} Calypso_ATNetApp_DnsClientOption_t;

/**
 * @brief Device options
 * See Calypso_ATNetApp_Set() / Calypso_ATNetApp_Get() and Calypso_ATNetApp_Application_Device.
 */
typedef enum Calypso_ATNetApp_DeviceOption_t
{
    Calypso_ATNetApp_DeviceOption_URN,
    Calypso_ATNetApp_DeviceOption_Domain,
    /** @cond DOXYGEN_IGNORE */
    Calypso_ATNetApp_DeviceOption_NumberOfValues
    /** @endcond */
} Calypso_ATNetApp_DeviceOption_t;

/**
 * @brief HTTP server option values.
 */
typedef union Calypso_ATNetApp_HttpOptionValue_t
{
    uint16_t primPortNum;          /**< Primary port number */
    uint8_t authCheck;             /**< Authentication check (0=disabled, 1=enabled) */
    char authName[20];             /**< Authentication name */
    char authPassword[20];         /**< Authentication password */
    char authRealm[20];            /**< Authorization realm */
    uint8_t romPagesAccess;        /**< ROM pages access (0=disabled, 1=enabled) */
    uint16_t secondPortNum;        /**< Secondary port number */
    uint8_t secondPortEn;          /**< Secondary port enable (0=disabled, 1=enabled) */
    uint8_t primPortSecEn;         /**< Primary port security enable (0=disabled, 1=enabled) */
    char privKeyFile[96];          /**< Private key file */
    char devCertFile[96];          /**< Device certificate file */
    char caCertFile[96];           /**< CA certificate file */
    char tmpRegisterService[80];   /**< Unregister service name for mDNS */
    char tmpUnregisterService[80]; /**< Register service name for mDNS */
} Calypso_ATNetApp_HttpOptionValue_t;

/**
 * @brief DHCP server basic option values.
 */
typedef struct Calypso_ATNetApp_DhcpBasicOption_t
{
    uint32_t leaseTime;                                 /**< Lease time in seconds */
    char firstIpAddress[CALYPSO_MAX_IP_ADDRESS_LENGTH]; /**< First IP address for allocation */
    char lastIpAddress[CALYPSO_MAX_IP_ADDRESS_LENGTH];  /**< Last IP address for allocation */
} Calypso_ATNetApp_DhcpBasicOption_t;

/**
 * @brief DHCP server option values.
 */
typedef union Calypso_ATNetApp_DhcpOptionValue_t
{
    Calypso_ATNetApp_DhcpBasicOption_t basic; /**< Basic DHCP server parameters */
} Calypso_ATNetApp_DhcpOptionValue_t;

/**
 * @brief mDNS timing parameter values.
 */
typedef struct Calypso_ATNetApp_mDnsTimingParams_t
{
    uint32_t period;                 /**< Period in ticks (100 ticks = 1 second) */
    uint32_t repetitions;            /**< Repetitions */
    uint32_t telescopicFactor;       /**< Telescopic factor */
    uint32_t retransmissionInterval; /**< Retransmission interval */
    uint32_t maxPeriodInterval;      /**< Maximum period interval */
    uint32_t maxTime;                /**< Maximum time */

} Calypso_ATNetApp_mDnsTimingParams_t;

/**
 * @brief mDNS option values.
 */
typedef union Calypso_ATNetApp_mDnsOptionValue_t
{
    char contQuery[81];                               /**< Service name */
    uint32_t qeventMask;                              /**< Event mask */
    Calypso_ATNetApp_mDnsTimingParams_t timingParams; /**< Timing parameters */

} Calypso_ATNetApp_mDnsOptionValue_t;

/**
 * @brief SNTP client option values.
 */
typedef union Calypso_ATNetApp_SntpOptionValue_t
{
    uint8_t enable;                                /**< Enables or disables the SNTP client (0=disabled, 1=enabled) */
    uint32_t updateInterval;                       /**< Minimum update interval in seconds */
    int16_t timeZone;                              /**< UTC +/- minutes */
    char servers[3][CALYPSO_MAX_HOST_NAME_LENGTH]; /**< List of three server addresses / URLS */
} Calypso_ATNetApp_SntpOptionValue_t;

/**
 * @brief DNS client time option values.
 */
typedef struct Calypso_ATNetApp_DnsClientTime_t
{
    uint32_t maxResponseTime; /**< Maximum response time in milliseconds */
    uint16_t numberOfRetries; /**< Number of retries */
} Calypso_ATNetApp_DnsClientTime_t;

/**
 * @brief DNS client option values.
 */
typedef union Calypso_ATNetApp_DnsClientOptionValue_t
{
    Calypso_ATNetApp_DnsClientTime_t time; /**< Time parameters */
} Calypso_ATNetApp_DnsClientOptionValue_t;

/**
 * @brief Device option values.
 */
typedef union Calypso_ATNetApp_DeviceOptionValue_t
{
    char urn[33];    /**< Device name */
    char domain[63]; /**< Domain name */
} Calypso_ATNetApp_DeviceOptionValue_t;

/**
 * @brief Option value struct.
 * Used by Calypso_ATNetApp_Set() and Calypso_ATNetApp_Get().
 */
typedef union Calypso_ATNetApp_OptionValue_t
{
    Calypso_ATNetApp_HttpOptionValue_t http;           /**< HTTP server options */
    Calypso_ATNetApp_DhcpOptionValue_t dhcp;           /**< DHCP server options */
    Calypso_ATNetApp_mDnsOptionValue_t mDns;           /**< mDNS options */
    Calypso_ATNetApp_SntpOptionValue_t sntp;           /**< SNTP client options */
    Calypso_ATNetApp_DnsClientOptionValue_t dnsClient; /**< DNS client options */
    Calypso_ATNetApp_DeviceOptionValue_t device;       /**< Device options */
} Calypso_ATNetApp_OptionValue_t;

/**
 * @brief Ping mode.
 */
typedef enum Calypso_ATNetApp_PingMode_t
{
    Calypso_ATNetApp_PingMode_ReportWhenComplete,   /**< Report once all pings are done */
    Calypso_ATNetApp_PingMode_ReportAfterEveryPing, /**< Report after every ping */
    Calypso_ATNetApp_PingMode_StopAfterFirstPing,   /**< Stop after first successful ping */
                                                    /** @cond DOXYGEN_IGNORE */
    Calypso_ATNetApp_PingMode_NumberOfValues
    /** @endcond */
} Calypso_ATNetApp_PingMode_t;

/**
 * @brief Host / IP lookup result.
 * @see Calypso_ATNetApp_GetHostByName()
 */
typedef struct Calypso_ATNetApp_GetHostByNameResult_t
{
    char hostName[CALYPSO_MAX_HOST_NAME_LENGTH];     /**< Name of looked up host */
    char hostAddress[CALYPSO_MAX_IP_ADDRESS_LENGTH]; /**< Address of looked up host */
} Calypso_ATNetApp_GetHostByNameResult_t;

/**
 * @brief Ping parameters.
 * @see Calypso_ATNetApp_Ping()
 */
typedef struct Calypso_ATNetApp_PingParameters_t
{
    Calypso_ATSocket_Family_t family;                /**< INET or INET6 */
    char destination[CALYPSO_MAX_IP_ADDRESS_LENGTH]; /**< Destination IP address (0 to stop an ongoing ping) */
    uint16_t size;                                   /**< Size of ping in bytes */
    uint16_t delayMs;                                /**< Delay between pings in milliseconds */
    uint16_t timeoutMs;                              /**< Timeout for each ping in milliseconds */
    uint16_t maxNumberOfPings;                       /**< Number of pings to send (0 = forever) */
    Calypso_ATNetApp_PingMode_t mode;                /**< Ping mode. See Calypso_ATNetApp_PingMode_t. */
} Calypso_ATNetApp_PingParameters_t;

/**
 * @brief Starts one or more networking applications.
 *
 * @param[in] apps: Applications (flags) to start (see Calypso_ATNetApp_Application_t)
 *
 * @return True if successful, false otherwise
 */
extern bool Calypso_ATNetApp_StartApplications(uint8_t apps);

/**
 * @brief Stops one or more networking applications.
 *
 * @param[in] apps: Applications (flags) to stop (see Calypso_ATNetApp_Application_t)
 *
 * @return True if successful, false otherwise
 */
extern bool Calypso_ATNetApp_StopApplications(uint8_t apps);

/**
 * @brief Sets network applications parameters.
 *
 * @param[in] app: Application ID
 * @param[in] option: Parameter to be set. Type depends on the application.
 * @param[in] value: Value to be set
 *
 * @return True if successful, false otherwise
 */
extern bool Calypso_ATNetApp_Set(Calypso_ATNetApp_Application_t app, uint8_t option, Calypso_ATNetApp_OptionValue_t* value);

/**
 * @brief Gets network applications parameters.
 *
 * @param[in] app: Application ID
 * @param[in] option: Parameter to be retrieved. Type depends on the application.
 * @param[out] value: The returned value
 *
 * @return True if successful, false otherwise
 */
extern bool Calypso_ATNetApp_Get(Calypso_ATNetApp_Application_t app, uint8_t option, Calypso_ATNetApp_OptionValue_t* value);

/**
 * @brief Looks up the IP address for the supplied host name.
 *
 * @param[in] hostName: Name of host
 * @param[in] family: Network protocol family
 * @param[out] lookupResult: The lookup result containing the IP address for the supplied host
 *
 * @return True if successful, false otherwise
 */
extern bool Calypso_ATNetApp_GetHostByName(const char* hostName, Calypso_ATSocket_Family_t family, Calypso_ATNetApp_GetHostByNameResult_t* lookupResult);

/**
 * @brief Pings another device.
 *
 * This function starts pinging the supplied host at regular intervals. The value returned
 * by this function indicates if the request was successful or not. The results of the ping
 * command (packets sent/received, roundtrip time) are reported in the form of events. The
 * number of events issued depends on the value of the mode parameter.
 *
 * @param[in] parameters: Ping command parameters
 *
 * @return True if ping was requested successfully, false otherwise
 */
extern bool Calypso_ATNetApp_Ping(Calypso_ATNetApp_PingParameters_t* parameters);

/**
 * @brief Updates the device's time using the SNTP client.
 *
 * Note that the SNTP client must be started using Calypso_ATNetApp_StartApplications(Calypso_ATNetApp_Application_SntpClient)
 * and configured using Calypso_ATNetApp_Set(Calypso_ATNetApp_Application_SntpClient, ...) before calling this function.
 *
 * @return True if successful, false otherwise
 */
extern bool Calypso_ATNetApp_UpdateTime();

#ifdef __cplusplus
}
#endif

#endif /* CALYPSO_AT_NETAPP_H_INCLUDED */
