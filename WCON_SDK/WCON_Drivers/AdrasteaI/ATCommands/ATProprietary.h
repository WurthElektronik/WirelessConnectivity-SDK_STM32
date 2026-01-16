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
 * @file ATProprietary.h
 * @brief AT commands for Proprietary functionality.
 */

#ifndef ADRASTEAI_AT_PROPRIETARY_H_INCLUDED
#define ADRASTEAI_AT_PROPRIETARY_H_INCLUDED

#include <AdrasteaI/ATCommands/ATCommon.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief Network Attachment State
 */
typedef enum AdrasteaI_ATProprietary_Network_Attachment_State_t
{
    AdrasteaI_ATProprietary_Network_State_Invalid = -1,
    AdrasteaI_ATProprietary_Network_State_Detach,
    AdrasteaI_ATProprietary_Network_State_Attach,
    /** @cond DOXYGEN_IGNORE */
    AdrasteaI_ATProprietary_Network_State_NumberOfValues
    /** @endcond */
} AdrasteaI_ATProprietary_Network_Attachment_State_t;

typedef uint8_t AdrasteaI_ATProprietary_PIN_Attempts_t, AdrasteaI_ATProprietary_PUK_Attempts_t, AdrasteaI_ATProprietary_PIN2_Attempts_t, AdrasteaI_ATProprietary_PUK2_Attempts_t;

/**
 * @brief PIN PUK Attempts
 */
typedef struct AdrasteaI_ATProprietary_PIN_PUK_Attempts_t
{
    AdrasteaI_ATProprietary_PIN_Attempts_t pinAttempts;
    AdrasteaI_ATProprietary_PUK_Attempts_t pukAttempts;
    AdrasteaI_ATProprietary_PIN2_Attempts_t pin2Attempts;
    AdrasteaI_ATProprietary_PUK2_Attempts_t puk2Attempts;
} AdrasteaI_ATProprietary_PIN_PUK_Attempts_t;

/**
 * @brief RAT
 */
typedef enum AdrasteaI_ATProprietary_RAT_t
{
    AdrasteaI_ATProprietary_RAT_Invalid = -1,
    AdrasteaI_ATProprietary_RAT_DEFAULT,
    AdrasteaI_ATProprietary_RAT_CATM,
    AdrasteaI_ATProprietary_RAT_NB_IOT,
    /** @cond DOXYGEN_IGNORE */
    AdrasteaI_ATProprietary_RAT_NumberOfValues
    /** @endcond */
} AdrasteaI_ATProprietary_RAT_t;

/**
 * @brief RAT Storage
 */
typedef enum AdrasteaI_ATProprietary_RAT_Storage_t
{
    AdrasteaI_ATProprietary_RAT_Storage_Invalid = -1,
    AdrasteaI_ATProprietary_RAT_Storage_Non_Persistant,
    AdrasteaI_ATProprietary_RAT_Storage_Persistant,
    /** @cond DOXYGEN_IGNORE */
    AdrasteaI_ATProprietary_RAT_Storage_NumberOfValues
    /** @endcond */
} AdrasteaI_ATProprietary_RAT_Storage_t;

/**
 * @brief RAT Source
 */
typedef enum AdrasteaI_ATProprietary_RAT_Source_t
{
    AdrasteaI_ATProprietary_RAT_Source_Invalid = -1,
    AdrasteaI_ATProprietary_RAT_Source_None,
    AdrasteaI_ATProprietary_RAT_Source_Host,
    /** @cond DOXYGEN_IGNORE */
    AdrasteaI_ATProprietary_RAT_Source_NumberOfValues
    /** @endcond */
} AdrasteaI_ATProprietary_RAT_Source_t;

/**
 * @brief RAT Mode
 */
typedef enum AdrasteaI_ATProprietary_RAT_Mode_t
{
    AdrasteaI_ATProprietary_RAT_Mode_Invalid = -1,
    AdrasteaI_ATProprietary_RAT_Mode_Single,
    AdrasteaI_ATProprietary_RAT_Mode_Multiple,
    /** @cond DOXYGEN_IGNORE */
    AdrasteaI_ATProprietary_RAT_Mode_NumberOfValues
    /** @endcond */
} AdrasteaI_ATProprietary_RAT_Mode_t;

/**
 * @brief RAT Status
 */
typedef struct AdrasteaI_ATProprietary_RAT_Status_t
{
    AdrasteaI_ATProprietary_RAT_t rat;
    AdrasteaI_ATProprietary_RAT_Mode_t mode;
    AdrasteaI_ATProprietary_RAT_Source_t source;
} AdrasteaI_ATProprietary_RAT_Status_t;

typedef uint8_t AdrasteaI_ATProprietary_Boot_Delay_t;

typedef int16_t AdrasteaI_ATProprietary_Ping_Packet_Count_t, AdrasteaI_ATProprietary_Ping_Packet_Size_t, AdrasteaI_ATProprietary_Ping_Timeout_t;

#define AdrasteaI_ATProprietary_Ping_Packet_Count_Invalid -1

#define AdrasteaI_ATProprietary_Ping_Packet_Size_Invalid -1

#define AdrasteaI_ATProprietary_Ping_Timeout_Invalid -1

typedef uint8_t AdrasteaI_ATProprietary_Ping_ID_t;

typedef uint16_t AdrasteaI_ATProprietary_Ping_TTL_t, AdrasteaI_ATProprietary_Ping_RTT_t;

/**
 * @brief Ping Result
 */
typedef struct AdrasteaI_ATProprietary_Ping_Result_t
{
    AdrasteaI_ATProprietary_Ping_ID_t id;
    AdrasteaI_ATCommon_IP_Addr_t addr;
    AdrasteaI_ATProprietary_Ping_TTL_t ttl;
    AdrasteaI_ATProprietary_Ping_RTT_t rtt;
} AdrasteaI_ATProprietary_Ping_Result_t;

typedef char AdrasteaI_ATProprietary_Domain_Name_t[128];

/**
 * @brief Proprietary IP Address format
 */
typedef enum AdrasteaI_ATProprietary_IP_Addr_Format_t
{
    AdrasteaI_ATProprietary_IP_Addr_Format_Invalid = -1,
    AdrasteaI_ATProprietary_IP_Addr_Format_IPv4,
    AdrasteaI_ATProprietary_IP_Addr_Format_IPv6,
    AdrasteaI_ATProprietary_IP_Addr_Format_IPv4v6,
    /** @cond DOXYGEN_IGNORE */
    AdrasteaI_ATProprietary_IP_Addr_Format_NumberOfValues
    /** @endcond */
} AdrasteaI_ATProprietary_IP_Addr_Format_t;

/**
 * @brief Domain Name Result
 */
typedef struct AdrasteaI_ATProprietary_Domain_Name_Resolve_Result_t
{
    AdrasteaI_ATProprietary_IP_Addr_Format_t format;
    AdrasteaI_ATCommon_IP_Addr_t addr;
} AdrasteaI_ATProprietary_Domain_Name_Resolve_Result_t;

typedef char AdrasteaI_ATProprietary_File_Name_t[64];

/**
 * @brief Credential Format
 */
typedef enum AdrasteaI_ATProprietary_Credential_Format_t
{
    AdrasteaI_ATProprietary_Credential_Format_Invalid = -1,
    AdrasteaI_ATProprietary_Credential_Format_Certificate,
    AdrasteaI_ATProprietary_Credential_Format_Private_Key,
    AdrasteaI_ATProprietary_Credential_Format_PSK_ID,
    AdrasteaI_ATProprietary_Credential_Format_PSK_Key,
    /** @cond DOXYGEN_IGNORE */
    AdrasteaI_ATProprietary_Credential_Format_NumberOfValues
    /** @endcond */
} AdrasteaI_ATProprietary_Credential_Format_t;

typedef uint8_t AdrasteaI_ATProprietary_File_Name_Count_t;

/**
 * @brief File Names List
 */
typedef struct AdrasteaI_ATProprietary_File_Names_List_t
{
    AdrasteaI_ATProprietary_File_Name_Count_t count;
    AdrasteaI_ATProprietary_File_Name_t* filenames;
} AdrasteaI_ATProprietary_File_Names_List_t;

typedef char AdrasteaI_ATProprietary_File_Path_t[128];

/**
 * @brief TLS Profile ID list
 */
typedef struct AdrasteaI_ATProprietary_TLS_Profile_ID_List_t
{
    AdrasteaI_ATCommon_TLS_Profile_ID_Count_t count;
    AdrasteaI_ATCommon_TLS_Profile_ID_t* profileIDs;
} AdrasteaI_ATProprietary_TLS_Profile_ID_List_t;

/**
 * @brief PDN Parameters
 */
typedef struct AdrasteaI_ATProprietary_PDN_Parameters_t
{
    AdrasteaI_ATCommon_Session_ID_t sessionID;
    AdrasteaI_ATCommon_APN_Name_t apnName;
    AdrasteaI_ATProprietary_IP_Addr_Format_t ipFormat;
} AdrasteaI_ATProprietary_PDN_Parameters_t;

/**
 * @brief Read Network Attachment State (using the AT%CMATT command).
 *
 * @param[out] stateP: Network Attachment State is returned in this argument. See AdrasteaI_ATProprietary_Network_Attachment_State_t.
 *
 * @return True if successful, false otherwise
 */
extern bool AdrasteaI_ATProprietary_ReadNetworkAttachmentState(AdrasteaI_ATProprietary_Network_Attachment_State_t* stateP);

/**
 * @brief Set Network Attachment State (using the AT%CMATT command).
 *
 * @param[in] state: Network Attachment State. See AdrasteaI_ATProprietary_Network_Attachment_State_t.
 *
 * @return True if successful, false otherwise
 */
extern bool AdrasteaI_ATProprietary_SetNetworkAttachmentState(AdrasteaI_ATProprietary_Network_Attachment_State_t state);

/**
 * @brief Read Remaining PIN and PUK Attempts left (using the AT%CPININFO command).
 *
 * @param[out] attemptsP: Remaining Attempts are returned in this argument. See AdrasteaI_ATProprietary_PIN_PUK_Attempts_t.
 *
 * @return True if successful, false otherwise
 */
extern bool AdrasteaI_ATProprietary_ReadRemainingPINPUKAttempts(AdrasteaI_ATProprietary_PIN_PUK_Attempts_t* attemptsP);

/**
 * @brief Switch to RAT without Full Reboot (using the AT%RATACT command).
 *
 * @param[in] rat: RAT. See AdrasteaI_ATProprietary_RAT_t.
 *
 * @param[in] storage: RAT Storage (optional pass AdrasteaI_ATProprietary_RAT_Storage_Invalid to skip). See AdrasteaI_ATProprietary_RAT_Storage_t.
 *
 * @param[in] source: RAT Source (optional pass AdrasteaI_ATProprietary_RAT_Source_Invalid to skip). See AdrasteaI_ATProprietary_RAT_Source_t.
 *
 * @return True if successful, false otherwise
 */
extern bool AdrasteaI_ATProprietary_SwitchToRATWithoutFullReboot(AdrasteaI_ATProprietary_RAT_t rat, AdrasteaI_ATProprietary_RAT_Storage_t storage, AdrasteaI_ATProprietary_RAT_Source_t source);

/**
 * @brief Read RAT Status (using the AT%RATACT command).
 *
 * @param[out] ratstatusP: RAT Status is returned in this argument. See AdrasteaI_ATProprietary_RAT_Status_t.
 *
 * @return True if successful, false otherwise
 */
extern bool AdrasteaI_ATProprietary_ReadRATStatus(AdrasteaI_ATProprietary_RAT_Status_t* ratstatusP);

/**
 * @brief Set Boot Delay (using the AT%SETBDELAY command).
 *
 * @param[in] delay: Boot Delay in seconds.
 *
 * @return True if successful, false otherwise
 */
extern bool AdrasteaI_ATProprietary_SetBootDelay(AdrasteaI_ATProprietary_Boot_Delay_t delay);

/**
 * @brief Resolve Domain Name (using the AT%DNSRSLV command).
 *
 * @param[in] sessionid: Session ID.
 *
 * @param[in] domain: Domain Name (URL).
 *
 * @param[in] format: IP Address Format (optional pass AdrasteaI_ATProprietary_IP_Addr_Format_Invalid to skip). See AdrasteaI_ATProprietary_IP_Addr_Format_t.
 *
 * @return True if successful, false otherwise
 */
extern bool AdrasteaI_ATProprietary_ResolveDomainName(AdrasteaI_ATCommon_Session_ID_t sessionid, AdrasteaI_ATProprietary_Domain_Name_t domain, AdrasteaI_ATProprietary_IP_Addr_Format_t format);

/**
 * @brief Parses the value of Resolve Domain Name event arguments.
 *
 * @param[in] pEventArguments: String containing arguments of the AT command
 * @param[out] dataP: Domain Name Resolve Result is returned in this argument. See AdrasteaI_ATPacketDomain_PDP_Context_t.
 *
 * @return True if successful, false otherwise
 */
extern bool AdrasteaI_ATProprietary_ParseResolveDomainNameEvent(char* pEventArguments, AdrasteaI_ATProprietary_Domain_Name_Resolve_Result_t* dataP);

/**
 * @brief Ping an address (using the AT%PINGCMD command).
 *
 * @param[in] format: IP Address Format. See AdrasteaI_ATProprietary_IP_Addr_Format_t.
 *
 * @param[in] destaddr: Destination IP Address.
 *
 * @param[in] packetcount: Packet Count (optional pass AdrasteaI_ATProprietary_Ping_Packet_Count_Invalid to skip).
 *
 * @param[in] packetsize: Packet Size in Bytes (optional pass AdrasteaI_ATProprietary_Ping_Packet_Size_Invalid to skip).
 *
 * @param[in] timeout: Ping Timeout (optional pass AdrasteaI_ATProprietary_Ping_Timeout_Invalid to skip).
 *
 * @return True if successful, false otherwise
 */
extern bool AdrasteaI_ATProprietary_Ping(AdrasteaI_ATProprietary_IP_Addr_Format_t format, AdrasteaI_ATCommon_IP_Addr_t destaddr, AdrasteaI_ATProprietary_Ping_Packet_Count_t packetcount, AdrasteaI_ATProprietary_Ping_Packet_Size_t packetsize, AdrasteaI_ATProprietary_Ping_Timeout_t timeout);

/**
 * @brief Parses the value of Ping Result event arguments.
 *
 * @param[in] pEventArguments: String containing arguments of the AT command
 * @param[out] dataP: Ping Result is returned in this argument. See AdrasteaI_ATPacketDomain_PDP_Context_t.
 *
 * @return True if successful, false otherwise
 */
extern bool AdrasteaI_ATProprietary_ParsePingResultEvent(char* pEventArguments, AdrasteaI_ATProprietary_Ping_Result_t* dataP);

/**
 * @brief Read a credential (using the AT%CERTCMD command).
 *
 * @param[in] filename: Name of file to read.
 *
 * @param[out] dataP: Contents of file is read to this argument.
 *
 * @param[in] dataMaxBufferSize: Size of the buffer.
 *
 * @return True if successful, false otherwise
 */
extern bool AdrasteaI_ATProprietary_ReadCredential(AdrasteaI_ATProprietary_File_Name_t filename, char* dataP, uint16_t dataMaxBufferSize);

/**
 * @brief Write a credential (using the AT%CERTCMD command).
 *
 * @param[in] filename: Name of file to write.
 *
 * @param[in] format: Format of credential. See AdrasteaI_ATProprietary_Credential_Format_t.
 *
 * @param[out] data: Data to write to file.
 *
 * @return True if successful, false otherwise
 */
extern bool AdrasteaI_ATProprietary_WriteCredential(AdrasteaI_ATProprietary_File_Name_t filename, AdrasteaI_ATProprietary_Credential_Format_t format, char* data);

/**
 * @brief List filenames of credentials stored (using the AT%CERTCMD command).
 *
 * @param[out] filenamesList: List of credentials Stored. (Free memory from filenamesList->filenames after usage)
 *
 * @return True if successful, false otherwise
 */
extern bool AdrasteaI_ATProprietary_ListCredentials(AdrasteaI_ATProprietary_File_Names_List_t* filenamesList);

/**
 * @brief Delete a credential (using the AT%CERTCMD command).
 *
 * @param[in] filename: Name of file to delete.
 *
 * @return True if successful, false otherwise
 */
extern bool AdrasteaI_ATProprietary_DeleteCredential(AdrasteaI_ATProprietary_File_Name_t filename);

/**
 * @brief List Configured TLS Profile IDS (using the AT%CERTCFG command).
 *
 * @param[out] profileIDsList: List of TLS profile IDs. (Free memory from profileIDsList->profileIDs after usage)
 *
 * @return True if successful, false otherwise
 */
extern bool AdrasteaI_ATProprietary_ListTLSProfiles(AdrasteaI_ATProprietary_TLS_Profile_ID_List_t* profileIDsList);

/**
 * @brief Add TLS Profile (using the AT%CERTCFG command).
 *
 * @param[in] profileID: Profile ID of TLS Profile.
 *
 * @param[in] CA: Name of CA (optional pass empty string to skip).
 *
 * @param[in] CAPath: Path of CA (optional pass empty string to skip).
 *
 * @param[in] deviceCert: Name of device Certificate (optional pass empty string to skip).
 *
 * @param[in] deviceKey: Name of device Key (optional pass empty string to skip).
 *
 * @param[in] pskID: Name of PSK ID (optional pass empty string to skip).
 *
 * @param[in] pskKey: Name of PSK Key (optional pass empty string to skip).
 *
 * @return True if successful, false otherwise
 */
extern bool AdrasteaI_ATProprietary_AddTLSProfile(AdrasteaI_ATCommon_TLS_Profile_ID_t profileID, AdrasteaI_ATProprietary_File_Name_t CA, AdrasteaI_ATProprietary_File_Path_t CAPath, AdrasteaI_ATProprietary_File_Name_t deviceCert, AdrasteaI_ATProprietary_File_Name_t deviceKey, AdrasteaI_ATProprietary_File_Name_t pskID, AdrasteaI_ATProprietary_File_Name_t pskKey);

/**
 * @brief Delete TLS Profile (using the AT%CERTCFG command).
 *
 * @param[in] profileID: Profile ID of TLS Profile.
 *
 * @return True if successful, false otherwise
 */
extern bool AdrasteaI_ATProprietary_DeleteTLSProfile(AdrasteaI_ATCommon_TLS_Profile_ID_t profileID);

/**
 * @brief Set PDN Parameters (using the AT%PDNSET command).
 *
 * @param[in] parameters: PDN Parameters. See AdrasteaI_ATProprietary_PDN_Parameters_t.
 *
 * @return True if successful, false otherwise
 */
extern bool AdrasteaI_ATProprietary_SetPDNParameters(AdrasteaI_ATProprietary_PDN_Parameters_t parameters);

/**
 * @brief Read PDN Parameters (using the AT%PDNSET command).
 *
 * @param[out] parameters: PDN Parameters are returned in this argument. See AdrasteaI_ATProprietary_PDN_Parameters_t.
 *
 * @return True if successful, false otherwise
 */
extern bool AdrasteaI_ATProprietary_ReadPDNParameters(AdrasteaI_ATProprietary_PDN_Parameters_t* parameters);

#ifdef __cplusplus
}
#endif

#endif /* ADRASTEAI_AT_PROPRIETARY_H_INCLUDED */
