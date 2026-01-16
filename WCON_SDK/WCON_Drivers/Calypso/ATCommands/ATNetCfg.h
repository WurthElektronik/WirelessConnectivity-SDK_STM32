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
 * @file ATNetCfg.h
 * @brief AT commands for network configuration.
 */

#ifndef CALYPSO_AT_NETCFG_H_INCLUDED
#define CALYPSO_AT_NETCFG_H_INCLUDED
#include <Calypso/Calypso.h>
#include <global/ATCommands.h>
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief Network interface mode flags used by Calypso_ATNetCfg_SetInterfaceModes().
 */
typedef enum Calypso_ATNetCfg_InterfaceMode_t
{
    Calypso_ATNetCfg_InterfaceMode_None = 0,                      /**< None (all disabled) */
    Calypso_ATNetCfg_InterfaceMode_IPv6StationLocal = 1 << 0,     /**< Enable IPv6 local */
    Calypso_ATNetCfg_InterfaceMode_IPv6StationGlobal = 1 << 1,    /**< Enable IPv6 global */
    Calypso_ATNetCfg_InterfaceMode_DisableIPv4Dhcp = 1 << 2,      /**< Disable IPv4 DHCP */
    Calypso_ATNetCfg_InterfaceMode_IPv6LocalStatic = 1 << 3,      /**< Enable IPv6 local static */
    Calypso_ATNetCfg_InterfaceMode_IPv6LocalStateless = 1 << 4,   /**< Enable IPv6 local stateless */
    Calypso_ATNetCfg_InterfaceMode_IPv6LocalStateful = 1 << 5,    /**< Enable IPv6 local stateful */
    Calypso_ATNetCfg_InterfaceMode_IPv6GlobalStatic = 1 << 6,     /**< Enable IPv6 global static */
    Calypso_ATNetCfg_InterfaceMode_IPv6GlobalStateful = 1 << 7,   /**< Enable IPv6 global stateful */
    Calypso_ATNetCfg_InterfaceMode_DisableIPv4Lla = 1 << 8,       /**< Disable LLA feature */
    Calypso_ATNetCfg_InterfaceMode_EnableDhcpRelease = 1 << 9,    /**< Enable DHCP release */
    Calypso_ATNetCfg_InterfaceMode_IPv6GlobalStateless = 1 << 10, /**< Enable IPv6 global stateless */
    Calypso_ATNetCfg_InterfaceMode_DisableFastRenew = 1 << 11,    /**< Fast renew disabled */
    Calypso_ATNetCfg_InterfaceMode_NumberOfValues = 12            /**< Number of flags in this enumeration */
} Calypso_ATNetCfg_InterfaceMode_t;

/**
 * @brief Network configuration IPv4 method
 */
typedef enum Calypso_ATNetCfg_IPv4Method_t
{
    Calypso_ATNetCfg_IPv4Method_Unknown,
    Calypso_ATNetCfg_IPv4Method_Static,
    Calypso_ATNetCfg_IPv4Method_Dhcp,
    Calypso_ATNetCfg_IPv4Method_DhcpLla,
    Calypso_ATNetCfg_IPv4Method_ReleaseIPSet,
    Calypso_ATNetCfg_IPv4Method_ReleaseIPOff,
    /** @cond DOXYGEN_IGNORE */
    Calypso_ATNetCfg_IPv4Method_NumberOfValues
    /** @endcond */
} Calypso_ATNetCfg_IPv4Method_t;

/**
 * @brief Network configuration IPv6 method
 */
typedef enum Calypso_ATNetCfg_IPv6Method_t
{
    Calypso_ATNetCfg_IPv6Method_Unknown,
    Calypso_ATNetCfg_IPv6Method_Static,
    Calypso_ATNetCfg_IPv6Method_Stateless,
    Calypso_ATNetCfg_IPv6Method_Stateful,
    /** @cond DOXYGEN_IGNORE */
    Calypso_ATNetCfg_IPv6Method_NumberOfValues
    /** @endcond */
} Calypso_ATNetCfg_IPv6Method_t;

/**
 * @brief IPv4 configuration structure
 */
typedef struct Calypso_ATNetCfg_IPv4Config_t
{
    Calypso_ATNetCfg_IPv4Method_t method;
    char ipAddress[16];
    char subnetMask[16];
    char gatewayAddress[16];
    char dnsAddress[16];
} Calypso_ATNetCfg_IPv4Config_t;

/**
 * @brief IPv6 configuration structure
 */
typedef struct Calypso_ATNetCfg_IPv6Config_t
{
    Calypso_ATNetCfg_IPv6Method_t method;
    char ipAddress[CALYPSO_MAX_IP_ADDRESS_LENGTH];
    char dnsAddress[CALYPSO_MAX_IP_ADDRESS_LENGTH];
} Calypso_ATNetCfg_IPv6Config_t;

/**
 * @brief Enables or disables network interface modes.
 *
 * @param[in] modes: Flags of modes to enable. See Calypso_ATNetCfg_InterfaceMode_t. Set to Calypso_ATNetCfg_InterfaceMode_None to disable all modes.
 *
 * @return True if successful, false otherwise
 */
extern bool Calypso_ATNetCfg_SetInterfaceModes(uint16_t modes);

/**
 * @brief Returns the device's MAC address.
 *
 * @param[out] macAddress: The returned MAC address. Must be a byte array of size 6.
 *
 * @return True if successful, false otherwise
 */
extern bool Calypso_ATNetCfg_GetMacAddress(uint8_t macAddress[6]);

/**
 * @brief Sets the device's MAC address.
 *
 * @param[in] macAddress: The MAC address to be set. Must be a byte array of size 6.
 *
 * @return True if successful, false otherwise
 */
extern bool Calypso_ATNetCfg_SetMacAddress(uint8_t macAddress[6]);

/**
 * @brief Returns the device's IPv4 station address and other IPv4 info (subnet mask, gateway, DNS).
 *
 * This is the configuration used when the device is used as a WLAN station
 * (use Calypso_ATNetCfg_GetIPv4AddressAP() for getting AP configuration).
 *
 * @param[out] ipConfig: The returned IPv4 configuration.
 *
 * @return True if successful, false otherwise
 */
extern bool Calypso_ATNetCfg_GetIPv4AddressStation(Calypso_ATNetCfg_IPv4Config_t* ipConfig);

/**
 * @brief Sets the device's IPv4 station address and other IPv4 info (subnet mask, gateway, DNS).
 *
 * This is the configuration used when the device is used as a WLAN station
 * (use Calypso_ATNetCfg_SetIPv4AddressAP() for setting AP configuration).
 *
 * @param[in] ipConfig: The IPv4 configuration to be set.
 *
 * @return True if successful, false otherwise
 */
extern bool Calypso_ATNetCfg_SetIPv4AddressStation(Calypso_ATNetCfg_IPv4Config_t* ipConfig);

/**
 * @brief Returns the device's IPv4 access point address and other IPv4 info (subnet mask, gateway, DNS).
 *
 * This is the configuration used when the device is used as a WLAN access point
 * (use Calypso_ATNetCfg_GetIPv4AddressStation() for getting station configuration).
 *
 * @param[out] ipConfig: The returned IPv4 configuration.
 *
 * @return True if successful, false otherwise
 */
extern bool Calypso_ATNetCfg_GetIPv4AddressAP(Calypso_ATNetCfg_IPv4Config_t* ipConfig);

/**
 * @brief Sets the device's IPv4 access point address and other IPv4 info (subnet mask, gateway, DNS).
 *
 * This is the configuration used when the device is used as a WLAN access point
 * (use Calypso_ATNetCfg_SetIPv4AddressStation() for setting station configuration).
 *
 * @param[in] ipConfig: The IPv4 configuration to be set.
 *
 * @return True if successful, false otherwise
 */
extern bool Calypso_ATNetCfg_SetIPv4AddressAP(Calypso_ATNetCfg_IPv4Config_t* ipConfig);

/**
 * @brief Returns the device's local IPv6 configuration.
 *
 * @param[out] ipConfig: The returned IPv6 configuration
 *
 * @return True if successful, false otherwise
 */
extern bool Calypso_ATNetCfg_GetIPv6AddressLocal(Calypso_ATNetCfg_IPv6Config_t* ipConfig);

/**
 * @brief Sets the device's local IPv6 configuration.
 *
 * @param[in] ipConfig: The IPv6 configuration to be set
 *
 * @return True if successful, false otherwise
 */
extern bool Calypso_ATNetCfg_SetIPv6AddressLocal(Calypso_ATNetCfg_IPv6Config_t* ipConfig);

/**
 * @brief Returns the device's global IPv6 configuration.
 *
 * @param[out] ipConfig: The returned IPv6 configuration
 *
 * @return True if successful, false otherwise
 */
extern bool Calypso_ATNetCfg_GetIPv6AddressGlobal(Calypso_ATNetCfg_IPv6Config_t* ipConfig);

/**
 * @brief Sets the device's global IPv6 configuration.
 *
 * @param[in] ipConfig: The IPv6 configuration to be set
 *
 * @return True if successful, false otherwise
 */
extern bool Calypso_ATNetCfg_SetIPv6AddressGlobal(Calypso_ATNetCfg_IPv6Config_t* ipConfig);

/**
 * @brief Disconnects the station with the supplied MAC address. Only works in AP mode.
 *
 * @param[in] macAddress: MAC address of the station to disconnect from. Must be a byte array of size 6.
 *
 * @return True if successful, false otherwise
 */
extern bool Calypso_ATNetCfg_DisconnectApStation(uint8_t macAddress[6]);

/**
 * @brief Returns the secondary DNS server address.
 *
 * @param[out] dns2ndServerAddress: The returned secondary DNS server address.
 *
 * @return True if successful, false otherwise
 */
extern bool Calypso_ATNetCfg_GetIPv4DnsClient(char dns2ndServerAddress[16]);

/**
 * @brief Sets the secondary DNS server address.
 *
 * @param[out] dns2ndServerAddress: The secondary DNS server address to be set.
 *
 * @return True if successful, false otherwise
 */
extern bool Calypso_ATNetCfg_SetIPv4DnsClient(const char dns2ndServerAddress[16]);

#ifdef __cplusplus
}
#endif

#endif /* CALYPSO_AT_NETCFG_H_INCLUDED */
