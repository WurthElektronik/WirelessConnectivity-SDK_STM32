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
 * @file ATDevice.h
 * @brief AT commands for basic device functionality.
 */

#ifndef ADRASTEAI_AT_DEVICE_H_INCLUDED
#define ADRASTEAI_AT_DEVICE_H_INCLUDED

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief Manufacturer Identity
 */
typedef char AdrasteaI_ATDevice_Manufacturer_Identity_t[64];

/**
 * @brief Model Identity
 */
typedef char AdrasteaI_ATDevice_Model_Identity_t[64];

/**
 * @brief Revision Identity Major
 */
typedef uint8_t AdrasteaI_ATDevice_Revision_Identity_Major_t;

/**
 * @brief Revision Identity Minor
 */
typedef uint16_t AdrasteaI_ATDevice_Revision_Identity_Minor_t;

/**
 * @brief Revision Identity
 */
typedef struct AdrasteaI_ATDevice_Revision_Identity_t
{
    AdrasteaI_ATDevice_Revision_Identity_Major_t major;
    AdrasteaI_ATDevice_Revision_Identity_Minor_t minor;
} AdrasteaI_ATDevice_Revision_Identity_t;

/**
 * @brief Serial Number
 */
typedef char AdrasteaI_ATDevice_Serial_Number_t[10];

/**
 * @brief IMEI
 */
typedef char AdrasteaI_ATDevice_IMEI_t[16];

/**
 * @brief IMEISV
 */
typedef char AdrasteaI_ATDevice_IMEISV_t[17];

/**
 * @brief SVN
 */
typedef char AdrasteaI_ATDevice_SVN_t[3];

/**
 * @brief Supported TE Character Sets
 */
typedef enum AdrasteaI_ATDevice_Character_Set_t
{
    AdrasteaI_ATDevice_Character_Set_Invalid = -1,
    AdrasteaI_ATDevice_Character_Set_UCS2,
    AdrasteaI_ATDevice_Character_Set_8859n,
    AdrasteaI_ATDevice_Character_Set_IRA,
    AdrasteaI_ATDevice_Character_Set_HEX,
    AdrasteaI_ATDevice_Character_Set_PCCP437,
    /** @cond DOXYGEN_IGNORE */
    AdrasteaI_ATDevice_Character_Set_NumberOfValues
    /** @endcond */
} AdrasteaI_ATDevice_Character_Set_t;

/**
 * @brief Phone Functionality Options
 */
typedef enum AdrasteaI_ATDevice_Phone_Functionality_t
{
    AdrasteaI_ATDevice_Phone_Functionality_Invalid = -1,
    AdrasteaI_ATDevice_Phone_Functionality_Min,
    AdrasteaI_ATDevice_Phone_Functionality_Full,
    AdrasteaI_ATDevice_Phone_Functionality_Disable_Transmit_Only,
    AdrasteaI_ATDevice_Phone_Functionality_Disable_Receive_Only,
    AdrasteaI_ATDevice_Phone_Functionality_Disable_Transmit_Receive,
    /** @cond DOXYGEN_IGNORE */
    AdrasteaI_ATDevice_Phone_Functionality_NumberOfValues
    /** @endcond */
} AdrasteaI_ATDevice_Phone_Functionality_t;

/**
 * @brief Reset type when changing phone functionality
 */
typedef enum AdrasteaI_ATDevice_Phone_Functionality_Reset_t
{
    AdrasteaI_ATDevice_Phone_Functionality_Reset_Invalid = -1,
    AdrasteaI_ATDevice_Phone_Functionality_Reset_Do_Not_Reset,
    AdrasteaI_ATDevice_Phone_Functionality_Reset_Reset,
    /** @cond DOXYGEN_IGNORE */
    AdrasteaI_ATDevice_Phone_Functionality_Reset_NumberOfValues
    /** @endcond */
} AdrasteaI_ATDevice_Phone_Functionality_Reset_t;

/**
 * @brief Result code formats
 */
typedef enum AdrasteaI_ATDevice_Result_Code_Format_t
{
    AdrasteaI_ATDevice_Result_Code_Format_Invalid = -1,
    AdrasteaI_ATDevice_Result_Code_Format_Numeric,
    AdrasteaI_ATDevice_Result_Code_Format_Verbose,
    /** @cond DOXYGEN_IGNORE */
    AdrasteaI_ATDevice_Result_Code_Format_NumberOfValues
    /** @endcond */
} AdrasteaI_ATDevice_Result_Code_Format_t;

/**
 * @brief Tests the connection to the wireless module (using the AT command).
 *
 * @return True if successful, false otherwise
 */
extern bool AdrasteaI_ATDevice_Test();

/**
 * @brief Request Manufacturer Identity (using the AT+CGMI command).
 *
 * @param[out] manufacturerIdentityP: Manufacturer Identity is returned in this argument.
 *
 * @return True if successful, false otherwise
 */
extern bool AdrasteaI_ATDevice_RequestManufacturerIdentity(AdrasteaI_ATDevice_Manufacturer_Identity_t* manufacturerIdentityP);

/**
 * @brief Request Model Identity (using the AT+CGMM command).
 *
 * @param[out] modelIdentityP: Model Identity is returned in this argument.
 *
 * @return True if successful, false otherwise
 */
extern bool AdrasteaI_ATDevice_RequestModelIdentity(AdrasteaI_ATDevice_Model_Identity_t* modelIdentityP);

/**
 * @brief Request Revision Identity (using the AT+CGMR command).
 *
 * @param[out] revisionIdentityP: Revision Identity is returned in this argument.
 *
 * @return True if successful, false otherwise
 */
extern bool AdrasteaI_ATDevice_RequestRevisionIdentity(AdrasteaI_ATDevice_Revision_Identity_t* revisionIdentityP);

/**
 * @brief Request IMEI (using the AT+CGSN command).
 *
 * @param[out] imeiP: IMEI is returned in this argument.
 *
 * @return True if successful, false otherwise
 */
extern bool AdrasteaI_ATDevice_RequestIMEI(AdrasteaI_ATDevice_IMEI_t* imeiP);

/**
 * @brief Request IMEISV (using the AT+CGSN command).
 *
 * @param[out] imeisvP: IMEISV is returned in this argument.
 *
 * @return True if successful, false otherwise
 */
extern bool AdrasteaI_ATDevice_RequestIMEISV(AdrasteaI_ATDevice_IMEISV_t* imeisvP);

/**
 * @brief Request SVN (using the AT+CGSN command).
 *
 * @param[out] svnP: SVN is returned in this argument.
 *
 * @return True if successful, false otherwise
 */
extern bool AdrasteaI_ATDevice_RequestSVN(AdrasteaI_ATDevice_SVN_t* svnP);

/**
 * @brief Request Serial Number (using the AT+GSN command).
 *
 * @param[out] serialNumberP: Serial Number is returned in this argument.
 *
 * @return True if successful, false otherwise
 */
extern bool AdrasteaI_ATDevice_RequestSerialNumber(AdrasteaI_ATDevice_Serial_Number_t* serialNumberP);

/**
 * @brief Read TE Character Set (using the AT+CSCS command).
 *
 * @param[out] charsetP: TE Character Set is returned in this argument.
 *
 * @return True if successful, false otherwise
 */
extern bool AdrasteaI_ATDevice_GetTECharacterSet(AdrasteaI_ATDevice_Character_Set_t* charsetP);

/**
 * @brief Set TE Character Set (using the AT+CSCS command).
 *
 * @param[in] charset: TE Character Set. See AdrasteaI_ATDevice_Character_Set_t.
 *
 * @return True if successful, false otherwise
 */
extern bool AdrasteaI_ATDevice_SetTECharacterSet(AdrasteaI_ATDevice_Character_Set_t charset);

/**
 * @brief Read Capabilities List (using the AT+GCAP command).
 *
 * @param[out] capListP: Capabilities List is returned in this argument.
 *
 * @param[in] maxBufferSize: Maximum size of buffer to hold data.
 *
 * @return True if successful, false otherwise
 */
extern bool AdrasteaI_ATDevice_GetCapabilitiesList(char* capListP, uint8_t maxBufferSize);

/**
 * @brief Read Phone Functionality (using the AT+CFUN command).
 *
 * @param[out] phoneFunP: Phone Functionality is returned in this argument.
 *
 * @return True if successful, false otherwise
 */
extern bool AdrasteaI_ATDevice_GetPhoneFunctionality(AdrasteaI_ATDevice_Phone_Functionality_t* phoneFunP);

/**
 * @brief Set Phone Functionality (using the AT+CFUN command).
 *
 * @param[in] phoneFun: Phone Functionality. See AdrasteaI_ATDevice_Phone_Functionality_t.
 *
 * @param[in] resetType: Reset type when changing phone functionality (optional pass AdrasteaI_ATDevice_Phone_Functionality_Reset_Invalid to skip). See AdrasteaI_ATDevice_Phone_Functionality_Reset_t.
 *
 * @return True if successful, false otherwise
 */
extern bool AdrasteaI_ATDevice_SetPhoneFunctionality(AdrasteaI_ATDevice_Phone_Functionality_t phoneFun, AdrasteaI_ATDevice_Phone_Functionality_Reset_t resetType);

/**
 * @brief Resets the device. (using the ATZ command).
 *
 * @return True if successful, false otherwise
 */
extern bool AdrasteaI_ATDevice_Reset();

/**
 * @brief Performs a factory reset of the device (using the AT&F0 command).
 *
 * @return True if successful, false otherwise
 */
extern bool AdrasteaI_ATDevice_FactoryReset();

/**
 * @brief Set Result Code Format (using the ATV command).
 *
 * @param[in] format: Result Code Format. See AdrasteaI_ATDevice_Result_Code_Format_t.
 *
 * @return True if successful, false otherwise
 */
extern bool AdrasteaI_ATDevice_SetResultCodeFormat(AdrasteaI_ATDevice_Result_Code_Format_t format);

#ifdef __cplusplus
}
#endif

#endif /* ADRASTEAI_AT_DEVICE_H_INCLUDED */
