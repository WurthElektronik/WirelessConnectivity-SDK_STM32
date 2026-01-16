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
 * @file ATWifi.h
 * @brief AT commands for Wifi functionality.
 */

#ifndef STEPHANOI_WIFI_H_INCLUDED
#define STEPHANOI_WIFI_H_INCLUDED

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

#define WIFI_SSID_STRINGLEN (32 + 1 + 1)
#define WIFI_PWD_STRINGLEN (64 + 1 + 1)
#define WIFI_MAC_STRINGLEN (2 + 5 + 2 * 6 + 1 + 1)
#define WIFI_IPv4_STRINGLEN (2 + 3 + 3 * 4 + 1 + 1)
#define WIFI_IPv6_STRINGLEN (2 + 7 + 4 * 8 + 1 + 1)

/**
 * @brief Wifi protocol
 */
typedef enum StephanoI_ATWifiProtocolFlags_t
{
    StephanoI_ATWifiProtocolFlags_80211b = (uint8_t)(0x01 << 0),
    StephanoI_ATWifiProtocolFlags_80211g = (uint8_t)(0x01 << 1),
    StephanoI_ATWifiProtocolFlags_80211n = (uint8_t)(0x01 << 2)
} StephanoI_ATWifiProtocolFlags_t;

/**
 * @brief Wifi DHCP
 */
typedef enum StephanoI_ATWifiDHCPFlags_t
{
    StephanoI_ATWifiDHCPFlags_Station = (uint8_t)(0x01 << 0),
    StephanoI_ATWifiDHCPFlags_SoftAP = (uint8_t)(0x01 << 1)
} StephanoI_ATWifiDHCPFlags_t;

/**
 * @brief Wifi mode type
 */
typedef enum StephanoI_ATWifiMode_t
{
    StephanoI_ATWifiMode_Disabled = (uint8_t)0,
    StephanoI_ATWifiMode_Station = (uint8_t)1,
    StephanoI_ATWifiMode_AP = (uint8_t)2,
    StephanoI_ATWifiMode_APplusStation = (uint8_t)3,
} StephanoI_ATWifiMode_t;

/**
 * @brief Wifi encryption method
 */
typedef enum StephanoI_ATWifiEncryptionMethod_t
{
    StephanoI_ATWifiEncryptionMethod_Open = (uint8_t)0,
    StephanoI_ATWifiEncryptionMethod_WEP = (uint8_t)1,
    StephanoI_ATWifiEncryptionMethod_WPAPSK = (uint8_t)2,
    StephanoI_ATWifiEncryptionMethod_WPA2PSK = (uint8_t)3,
    StephanoI_ATWifiEncryptionMethod_WPA_WAP2PSK = (uint8_t)4,
    StephanoI_ATWifiEncryptionMethod_WPA2_ENTERPRISE = (uint8_t)5,
    StephanoI_ATWifiEncryptionMethod_WPA3_PSK = (uint8_t)6,
    StephanoI_ATWifiEncryptionMethod_WPA2_WPA3PSK = (uint8_t)7,
    StephanoI_ATWifiEncryptionMethod_WAPI_PSK = (uint8_t)8,
} StephanoI_ATWifiEncryptionMethod_t;

/**
 * @brief Wifi scan type
 */
typedef enum StephanoI_ATWifiScan_t
{
    StephanoI_ATWifiScan_Active = (uint8_t)0,
    StephanoI_ATWifiScan_Passive = (uint8_t)1,
} StephanoI_ATWifiScan_t;

/**
 * @brief Wifi cipher type
 */
typedef enum StephanoI_ATWifiCipher_t
{
    StephanoI_ATWifiCipher_None = (uint8_t)0,
    StephanoI_ATWifiCipher_WEP40 = (uint8_t)1,
    StephanoI_ATWifiCipher_WEP104 = (uint8_t)2,
    StephanoI_ATWifiCipher_TKIP = (uint8_t)3,
    StephanoI_ATWifiCipher_CCMP = (uint8_t)4,
    StephanoI_ATWifiCipher_TKIPnCCMP = (uint8_t)5,
    StephanoI_ATWifiCipher_AESCMAC128 = (uint8_t)6,
    StephanoI_ATWifiCipher_Unknown = (uint8_t)7,
} StephanoI_ATWifiCipher_t;

/**
 * @brief Wifi WPS flags
 */
typedef enum StephanoI_ATWPSFlags_t
{
    StephanoI_ATWPSFlags_Off = (uint8_t)0x00,
    StephanoI_ATWPSFlags_Enabled = (uint8_t)(0x01 << 0),
} StephanoI_ATWPSFlags_t;

/**
 * @brief Wifi WPA2 security flags
 */
typedef enum StephanoI_ATWPA2SecurityFlags_t
{
    StephanoI_ATWPA2SecurityFlags_ClientCertificate = (uint8_t)(0x01 << 0),
    StephanoI_ATWPA2SecurityFlags_ServerCertificate = (uint8_t)(0x01 << 1),
} StephanoI_ATWPA2SecurityFlags_t;

/**
 * @brief Wifi state
 */
typedef enum StephanoI_ATWifiState_t
{
    StephanoI_ATWifiState_NotConnected = (uint8_t)0,
    StephanoI_ATWifiState_ConnectedNoIP = (uint8_t)1,
    StephanoI_ATWifiState_ConnectednIP = (uint8_t)2,
    StephanoI_ATWifiState_Connecting = (uint8_t)3,
    StephanoI_ATWifiState_Disconnected = (uint8_t)4
} StephanoI_ATWifiState_t;

/**
 * @brief Wifi WPA2 method
 */
typedef enum StephanoI_ATWifiWPA2Method_t
{
    StephanoI_ATWifiWPA2Method_EAP_TLS = (uint8_t)0,
    StephanoI_ATWifiWPA2Method_EAP_PEAP = (uint8_t)1,
    StephanoI_ATWifiWPA2Method_EAP_TTLS = (uint8_t)2,
} StephanoI_ATWifiWPA2Method_t;

/**
 * @brief Wifi WPA2 method
 */
typedef enum StephanoI_ATWifiCountryCodePolicy_t
{
    StephanoI_ATWifiCountryCodePolicy_ChangetoAP = (uint8_t)0,
    StephanoI_ATWifiCountryCodePolicy_Keep = (uint8_t)1
} StephanoI_ATWifiCountryCodePolicy_t;

/**
 * @brief Wifi channel number
 */
typedef enum StephanoI_ATWifiChannel_t
{
    StephanoI_ATWifiChannel_1 = (uint8_t)1,
    StephanoI_ATWifiChannel_2 = (uint8_t)2,
    StephanoI_ATWifiChannel_3 = (uint8_t)3,
    StephanoI_ATWifiChannel_4 = (uint8_t)4,
    StephanoI_ATWifiChannel_5 = (uint8_t)5,
    StephanoI_ATWifiChannel_6 = (uint8_t)6,
    StephanoI_ATWifiChannel_7 = (uint8_t)7,
    StephanoI_ATWifiChannel_8 = (uint8_t)8,
    StephanoI_ATWifiChannel_9 = (uint8_t)9,
    StephanoI_ATWifiChannel_10 = (uint8_t)10,
    StephanoI_ATWifiChannel_11 = (uint8_t)11,
    StephanoI_ATWifiChannel_12 = (uint8_t)12,
    StephanoI_ATWifiChannel_13 = (uint8_t)13,
    StephanoI_ATWifiChannel_14 = (uint8_t)14
} StephanoI_ATWifiChannel_t;

/**
 * @brief Parameters of Wifi country code type
 */
typedef struct StephanoI_ATWifiCountryCode_t
{
    StephanoI_ATWifiCountryCodePolicy_t policy;
    char country_code[6];
    StephanoI_ATWifiChannel_t start_channel;
    uint8_t total_channel_count;
} StephanoI_ATWifiCountryCode_t;

/**
 * @brief Parameters of Wifi Scan Event
 */
typedef struct StephanoI_ATWiFi_Scan_t
{
    StephanoI_ATWifiEncryptionMethod_t enc;
    char ssid[WIFI_SSID_STRINGLEN];
    int8_t rssi;
    char MAC[WIFI_MAC_STRINGLEN];
    uint8_t channel;
    int32_t freq_offset;
    int32_t freqcal_val;
    StephanoI_ATWifiCipher_t pairwise_cipher;
    StephanoI_ATWifiCipher_t group_cipher;
    uint8_t bgn;
    StephanoI_ATWPSFlags_t wps_flags;
} StephanoI_ATWiFi_Scan_t;

/**
 * @brief Parameters of Wifi State Event
 */
typedef struct StephanoI_ATWiFi_State_t
{
    StephanoI_ATWifiState_t state;
    char ssid[WIFI_SSID_STRINGLEN];
} StephanoI_ATWiFi_State_t;

/**
 * @brief Parameters of Wifi GetConnectedIPs Event
 */
typedef struct StephanoI_ATWiFi_GetConnectedIPs_t
{
    char IP[WIFI_IPv6_STRINGLEN];
    char MAC[WIFI_MAC_STRINGLEN];
} StephanoI_ATWiFi_GetConnectedIPs_t;

/**
 * @brief Parameters of Wifi GetIP Event
 */
typedef struct StephanoI_ATWiFi_GetIP_t
{
    char IP[WIFI_IPv6_STRINGLEN];
    char gateway[WIFI_IPv6_STRINGLEN];
    char netmask[WIFI_IPv6_STRINGLEN];
    char ip6ll[WIFI_IPv6_STRINGLEN];
    char ip6gl[WIFI_IPv6_STRINGLEN];
} StephanoI_ATWiFi_GetIP_t;

/**
 * @brief Parameters of Wifi GetMAC Event
 */
typedef char StephanoI_ATWiFi_MAC_t[WIFI_MAC_STRINGLEN];

/**
 * @brief Parameters of Wifi AP assign IP Event
 */
typedef struct StephanoI_ATWiFi_APAssignIP_t
{
    char MAC[WIFI_MAC_STRINGLEN];
    char ip[WIFI_IPv6_STRINGLEN];
} StephanoI_ATWiFi_APAssignIP_t;

/**
 * @brief Wifi scan print options
 */
typedef enum ATWifiScanPrintOptions_t
{
    ATWifiScanPrintOptions_ecn = (uint16_t)1,
    ATWifiScanPrintOptions_ssid = (uint16_t)2,
    ATWifiScanPrintOptions_rssi = (uint16_t)4,
    ATWifiScanPrintOptions_mac = (uint16_t)8,
    ATWifiScanPrintOptions_channel = (uint16_t)16,
    ATWifiScanPrintOptions_freq_offset = (uint16_t)32,
    ATWifiScanPrintOptions_freqcal_val = (uint16_t)64,
    ATWifiScanPrintOptions_pairwise_cypher = (uint16_t)128,
    ATWifiScanPrintOptions_group_cipher = (uint16_t)256,
    ATWifiScanPrintOptions_bgn = (uint16_t)512,
    ATWifiScanPrintOptions_wps = (uint16_t)1024,
    ATWifiScanPrintOptions_all = (uint16_t)2047
} ATWifiScanPrintOptions_t;

/**
 * @brief Wifi scan authentications options
 */
typedef enum StephanoI_ATWifiScanAuthOptions_t
{
    StephanoI_ATWifiScanAuthOptions_open = (uint16_t)1,
    StephanoI_ATWifiScanAuthOptions_wep = (uint16_t)2,
    StephanoI_ATWifiScanAuthOptions_wpa_psk = (uint16_t)4,
    StephanoI_ATWifiScanAuthOptions_wpa2_psk = (uint16_t)8,
    StephanoI_ATWifiScanAuthOptions_wpa_wpa2_psk = (uint16_t)16,
    StephanoI_ATWifiScanAuthOptions_wpa2_enterprise = (uint16_t)32,
    StephanoI_ATWifiScanAuthOptions_wpa3_psk = (uint16_t)64,
    StephanoI_ATWifiScanAuthOptions_wpa2_wpa3_psk = (uint16_t)128,
    StephanoI_ATWifiScanAuthOptions_wapi_psk = (uint16_t)256,
    StephanoI_ATWifiScanAuthOptions_owe = (uint16_t)512,
    StephanoI_ATWifiScanAuthOptions_all = (uint16_t)1023
} StephanoI_ATWifiScanAuthOptions_t;

/**
 * @brief AP configuration type
 */
typedef struct StephanoI_ATWiFi_APConfiguration_t
{
    char ssid[WIFI_SSID_STRINGLEN];
    char pwd[WIFI_PWD_STRINGLEN];
    StephanoI_ATWifiChannel_t channel;
    StephanoI_ATWifiEncryptionMethod_t ecn;
    uint8_t max_connections;
    bool ssid_hidden;
} StephanoI_ATWiFi_APConfiguration_t;

typedef char StephanoI_ATWiFi_Hostname_t[32];

/**
 * @brief AP IP range type
 */
typedef struct StephanoI_ATWiFi_IPRange_t
{
    uint16_t lease_time;
    char start_ip[WIFI_IPv6_STRINGLEN];
    char end_ip[WIFI_IPv6_STRINGLEN];
} StephanoI_ATWiFi_IPRange_t;

/**
 * @brief Enable/disable the Wifi
 *
 * @param[in] enable: Enable or disable
 *
 * @return True if successful, false otherwise
 */
extern bool StephanoI_ATWifi_Init(bool enable);

/**
 * @brief Read the Wifi driver state
 *
 * @param[in] enableP: Enable or disable
 *
 * @return True if successful, false otherwise
 */
extern bool StephanoI_ATWifi_GetInit(bool* enableP);

/**
 * @brief Set the Wifi mode
 *
 * @param[in] mode: Mode of the Wifi device
 * @param[in] autoconnect: Autoconnect
 *
 * @return True if successful, false otherwise
 */
extern bool StephanoI_ATWifi_SetMode(StephanoI_ATWifiMode_t mode, bool autoconnect);

/**
 * @brief Get the Wifi mode
 *
 * @param[out] modeP: Pointer to the wifi mode
 *
 * @return True if successful, false otherwise
 */
extern bool StephanoI_ATWifi_GetMode(StephanoI_ATWifiMode_t* modeP);

/**
 * @brief Get the Wifi state
 *
 * @param[out] stateP: Pointer to the wifi state
 *
 * @return True if successful, false otherwise
 */
extern bool StephanoI_ATWifi_GetState(StephanoI_ATWiFi_State_t* stateP);

/**
 * @brief Set the country code
 *
 * @param[in] t: Country code
 *
 * @return True if successful, false otherwise
 */
extern bool StephanoI_ATWifi_SetCountryCode(StephanoI_ATWifiCountryCode_t t);

/**
 * @brief Get the country code
 *
 * @param[out] t: Pointer to country code type
 *
 * @return True if successful, false otherwise
 */
extern bool StephanoI_ATWifi_GetCountryCode(StephanoI_ATWifiCountryCode_t* t);

/**
 * @brief Set the host name
 *
 * @param[in] name: Host name
 *
 * @return True if successful, false otherwise
 */
extern bool StephanoI_ATWifi_SetHostname(StephanoI_ATWiFi_Hostname_t name);

/**
 * @brief Get the host name
 *
 * @param[in] nameP: Pointer to the host name
 *
 * @return True if successful, false otherwise
 */
extern bool StephanoI_ATWifi_GetHostname(StephanoI_ATWiFi_Hostname_t* nameP);

/**
 * @brief Enable the DHCP
 *
 * @param[in] enable: Enable or disable
 * @param[in] mode: Mode (bit0 Station DHCP, bit1 AP DHCP)
 *
 * @return True if successful, false otherwise
 */
extern bool StephanoI_ATWifi_EnableDHCP(bool enable, StephanoI_ATWifiDHCPFlags_t mode);

/**
 * @brief Get the DHCP state
 *
 * @param[in] stateP: Pointer to the protocol standard
 *
 * @return True if successful, false otherwise
 */
extern bool StephanoI_ATWifi_GetDHCPState(uint8_t* stateP);

/**
 * @brief Enable/disable the MDNS
 *
 * @param[in] enable: Enable or disable the MDNS
 * @param[in] host_name: Host name
 * @param[in] service_name: Service name
 * @param[in] port: Port
 *
 * @return True if successful, false otherwise
 */
extern bool StephanoI_ATWifi_EnableMDNS(bool enable, char* host_name, char* service_name, uint16_t port);

/**
 * @brief Scan options
 *
 * @param[in] print_mask: Determine which parameters will be shown in the print results.
 * @param[in] rssi: Minimum rssi value. Devices with lower rssi will be filtered out.
 * @param[in] authmode_mask: Determine which devices will be filtered out by means of their supported authentication method..
 *
 * @return True if successful, false otherwise
 */
extern bool StephanoI_ATWifi_Station_ScanAPOptions(uint16_t print_mask, int8_t rssi, uint16_t authmode_mask);

/**
 * @brief Scan APs
 *
 * @param[in] ssid: SSID to scan for
 *
 * @return True if successful, false otherwise
 */
extern bool StephanoI_ATWifi_Station_ScanAPs(char* ssid);

/**
 * @brief Connect to AP
 *
 * @param[in] ssid: SSID to connect to
 * @param[in] password: Pass word
 *
 * @return True if successful, false otherwise
 */
extern bool StephanoI_ATWifi_Station_Connect(char* ssid, char* password);

/**
 * @brief Connect to WPA2 Enterprise AP
 *
 * @param[in] ssid: SSID to connect to
 * @param[in] method: Method
 * @param[in] identity: Identity
 * @param[in] username: User name
 * @param[in] password: Pass word
 * @param[in] security: Security
 *
 * @return True if successful, false otherwise
 */
extern bool StephanoI_ATWifi_Station_ConnectWPA2Enterprise(char* ssid, StephanoI_ATWifiWPA2Method_t method, char* identity, char* username, char* password, StephanoI_ATWPA2SecurityFlags_t security);

/**
 * @brief Configure the reconnection settings
 *
 * @param[in] interval_s: Reconnect interval in s
 * @param[in] repeat_count: Repeat count
 *
 * @return True if successful, false otherwise
 */
extern bool StephanoI_ATWifi_Station_ReConnectConfiguration(uint16_t interval_s, uint16_t repeat_count);

/**
 * @brief Disconnect from AP
 *
 * @return True if successful, false otherwise
 */
extern bool StephanoI_ATWifi_Station_Disconnect();

/**
 * @brief Enable the autoconnect mode
 *
 * @param[in] autoconnect: Autoconnect
 *
 * @return True if successful, false otherwise
 */
extern bool StephanoI_ATWifi_Station_AutoConnect(bool autoconnect);

/**
 * @brief Set the MAC of a station
 *
 * @param[in] mac: MAC
 *
 * @return True if successful, false otherwise
 */
extern bool StephanoI_ATWifi_Station_SetMAC(StephanoI_ATWiFi_MAC_t mac);

/**
 * @brief Get the MAC of the station
 *
 * @param[out] t: Pointer to the MAC
 *
 * @return True if successful, false otherwise
 */
extern bool StephanoI_ATWifi_Station_GetMAC(StephanoI_ATWiFi_MAC_t* t);

/**
 * @brief Set the protocol standard of the station
 *
 * @param[in] prot: Protocol standard
 *
 * @return True if successful, false otherwise
 */
extern bool StephanoI_ATWifi_Station_SetProtocol(StephanoI_ATWifiProtocolFlags_t prot);

/**
 * @brief Get the protocol standard of the station
 *
 * @param[in] protP: Pointer to the protocol standard
 *
 * @return True if successful, false otherwise
 */
extern bool StephanoI_ATWifi_Station_GetProtocol(StephanoI_ATWifiProtocolFlags_t* protP);

/**
 * @brief Set the IP of a AP
 * 
 * @param[in] ip: IPv4 to be used by the AP
 * @param[in] gateway: Corresponding gateway
 * @param[in] netmask: Corresponding net mask
 *
 * @return True if successful, false otherwise
 */
extern bool StephanoI_ATWifi_AP_SetIP(char* ip, char* gateway, char* netmask);

/**
 * @brief Get the IP of a station
 *
 * @return True if successful, false otherwise
 */
extern bool StephanoI_ATWifi_Station_GetIP();

/**
 * @brief Configure the AP
 *
 * @param[in] config: Configuration
 *
 * @return True if successful, false otherwise
 */
extern bool StephanoI_ATWifi_AP_SetConfiguration(StephanoI_ATWiFi_APConfiguration_t config);

/**
 * @brief Get the AP configuration
 *
 * @param[in] configP: Pointer to the configuration
 *
 * @return True if successful, false otherwise
 */
extern bool StephanoI_ATWifi_AP_GetConfiguration(StephanoI_ATWiFi_APConfiguration_t* configP);

/**
 * @brief Get the IP of the connected stations
 *
 * @return True if successful, false otherwise
 */
extern bool StephanoI_ATWifi_AP_GetConnectedStations();

/**
 * @brief Disconnect stations from AP
 *
 * @param[in] mac: MAC to disconnect
 *
 * @return True if successful, false otherwise
 */
extern bool StephanoI_ATWifi_AP_DisconnectStation(StephanoI_ATWiFi_MAC_t mac);

/**
 * @brief Set the MAC of a AP
 *
 * @param[in] mac: MAC
 *
 * @return True if successful, false otherwise
 */
extern bool StephanoI_ATWifi_AP_SetMAC(StephanoI_ATWiFi_MAC_t mac);

/**
 * @brief Get the MAC of a AP
 *
 * @param[out] t: Pointer to the MAC
 *
 * @return True if successful, false otherwise
 */
extern bool StephanoI_ATWifi_AP_GetMAC(StephanoI_ATWiFi_MAC_t* t);

/**
 * @brief Set the protocol standard of the AP
 *
 * @param[in] prot: Protocol standard
 *
 * @return True if successful, false otherwise
 */
extern bool StephanoI_ATWifi_AP_SetProtocol(StephanoI_ATWifiProtocolFlags_t prot);

/**
 * @brief Get the protocol standard of the AP
 *
 * @param[in] protP: Pointer to the protocol standard
 *
 * @return True if successful, false otherwise
 */
extern bool StephanoI_ATWifi_AP_GetProtocol(StephanoI_ATWifiProtocolFlags_t* protP);

/**
 * @brief Configure IP address range of AP
 *
 * @param[in] enable: Enable or disable
 * @param[in] range: IP range
 *
 * @return True if successful, false otherwise
 */
extern bool StephanoI_ATWifi_AP_SetIPrangeConfiguration(bool enable, StephanoI_ATWiFi_IPRange_t range);

/**
 * @brief Get the DHCP state
 *
 * @param[in] rangeP: Pointer to the IP range
 *
 * @return True if successful, false otherwise
 */
extern bool StephanoI_ATWifi_AP_GetIPrangeConfiguration(StephanoI_ATWiFi_IPRange_t* rangeP);

/**
 * @brief Get the IP of a AP
 *
 * @return True if successful, false otherwise
 */
extern bool StephanoI_ATWifi_AP_GetIP();

/**
 * @brief Parses the values of the Wifi mode arguments.
 *
 * @param[in] EventArgumentsP: String containing arguments of the AT command
 * @param[out] t: The parsed event data
 *
 * @return True if parsed successfully, false otherwise
 */
extern bool StephanoI_ATWiFi_ParseMode(char* EventArgumentsP, StephanoI_ATWifiMode_t* t);

/**
 * @brief Parses the values of the Wifi Scan arguments.
 *
 * @param[in] EventArgumentsP: String containing arguments of the AT command
 * @param[out] t: The parsed event data
 *
 * @return True if parsed successfully, false otherwise
 */
extern bool StephanoI_ATWiFi_ParseScan(char* EventArgumentsP, StephanoI_ATWiFi_Scan_t* t);

/**
 * @brief Parses the values of the Wifi state arguments.
 *
 * @param[in] EventArgumentsP: String containing arguments of the AT command
 * @param[out] t: The parsed event data
 *
 * @return True if parsed successfully, false otherwise
 */
extern bool StephanoI_ATWiFi_ParseState(char* EventArgumentsP, StephanoI_ATWiFi_State_t* t);

/**
 * @brief Parses the values of the Wifi GetconnectedIPs arguments.
 *
 * @param[in] EventArgumentsP: String containing arguments of the AT command
 * @param[out] t: The parsed event data
 *
 * @return True if parsed successfully, false otherwise
 */
extern bool StephanoI_ATWiFi_ParseGetConnectedIPs(char* EventArgumentsP, StephanoI_ATWiFi_GetConnectedIPs_t* t);

/**
 * @brief Parses the values of the Wifi GetIP arguments.
 *
 * @param[in] EventArgumentsP: String containing arguments of the AT command
 * @param[out] t: The parsed event data
 *
 * @return True if parsed successfully, false otherwise
 */
extern bool StephanoI_ATWiFi_ParseGetIP(char* EventArgumentsP, StephanoI_ATWiFi_GetIP_t* t);

/**
 * @brief Parses the values of the Wifi GetMAC arguments.
 *
 * @param[in] EventArgumentsP: String containing arguments of the AT command
 * @param[out] t: The parsed event data
 *
 * @return True if parsed successfully, false otherwise
 */
extern bool StephanoI_ATWiFi_ParseGetMAC(char* EventArgumentsP, StephanoI_ATWiFi_MAC_t* t);

/**
 * @brief Parses the values of the DHCP state
 *
 * @param[in] EventArgumentsP: String containing arguments of the AT command
 * @param[out] t: The parsed event data
 *
 * @return True if parsed successfully, false otherwise
 */
extern bool StephanoI_ATWiFi_ParseGetDHCPState(char* EventArgumentsP, uint8_t* t);

/**
 * @brief Parses the values of the Wifi StationhasConnected arguments.
 *
 * @param[in] EventArgumentsP: String containing arguments of the AT command
 * @param[out] t: The parsed event data
 *
 * @return True if parsed successfully, false otherwise
 */
extern bool StephanoI_ATWiFi_ParseStationhasConnected(char* EventArgumentsP, StephanoI_ATWiFi_MAC_t* t);

/**
 * @brief Parses the values of the Wifi AP assign IP arguments.
 *
 * @param[in] EventArgumentsP: String containing arguments of the AT command
 * @param[out] t: The parsed event data
 *
 * @return True if parsed successfully, false otherwise
 */
extern bool StephanoI_ATWiFi_ParseAPAssignIP(char* EventArgumentsP, StephanoI_ATWiFi_APAssignIP_t* t);

/**
 * @brief Parses the values of the Wifi country code.
 *
 * @param[in] EventArgumentsP: String containing arguments of the AT command
 * @param[out] t: The parsed event data
 *
 * @return True if parsed successfully, false otherwise
 */
extern bool StephanoI_ATWiFi_ParseCountryCode(char* EventArgumentsP, StephanoI_ATWifiCountryCode_t* t);

/**
 * @brief Parses the values of the protocol
 *
 * @param[in] EventArgumentsP: String containing arguments of the AT command
 * @param[out] t: The parsed event data
 *
 * @return True if parsed successfully, false otherwise
 */
extern bool StephanoI_ATWiFi_ParseProtocol(char* EventArgumentsP, StephanoI_ATWifiProtocolFlags_t* t);

/**
 * @brief Parses the values of the Wifi host name.
 *
 * @param[in] EventArgumentsP: String containing arguments of the AT command
 * @param[out] t: The parsed event data
 *
 * @return True if parsed successfully, false otherwise
 */
extern bool StephanoI_ATWiFi_ParseHostname(char* EventArgumentsP, StephanoI_ATWiFi_Hostname_t* t);

/**
 * @brief Parses the values of the Wifi driver state arguments.
 *
 * @param[in] EventArgumentsP: String containing arguments of the AT command
 * @param[out] t: The parsed event data
 *
 * @return True if parsed successfully, false otherwise
 */
extern bool StephanoI_ATWiFi_ParseInit(char* EventArgumentsP, bool* t);

/**
 * @brief Parses the values of the AP configuration.
 *
 * @param[in] EventArgumentsP: String containing arguments of the AT command
 * @param[out] t: The parsed event data
 *
 * @return True if parsed successfully, false otherwise
 */
extern bool StephanoI_ATWiFi_ParseAPConfiguration(char* EventArgumentsP, StephanoI_ATWiFi_APConfiguration_t* t);

/**
 * @brief Parses the values of the IP range
 *
 * @param[in] EventArgumentsP: String containing arguments of the AT command
 * @param[out] t: The parsed event data
 *
 * @return True if parsed successfully, false otherwise
 */
extern bool StephanoI_ATWiFi_ParseGetIPrangeConfiguration(char* EventArgumentsP, StephanoI_ATWiFi_IPRange_t* t);

#ifdef __cplusplus
}
#endif

#endif /* STEPHANOI_WIFI_H_INCLUDED */
