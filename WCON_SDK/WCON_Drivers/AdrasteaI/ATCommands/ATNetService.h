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
 * @file ATNetService.h
 * @brief AT commands for Net Service functionality.
 */

#ifndef ADRASTEAI_AT_NETSERVICE_H_INCLUDED
#define ADRASTEAI_AT_NETSERVICE_H_INCLUDED

#include <AdrasteaI/ATCommands/ATCommon.h>
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

typedef uint32_t AdrasteaI_ATNetService_Operator_Numeric_t;

typedef char AdrasteaI_ATNetService_Operator_String_t[64];

/**
 * @brief Operator Numeric and Alpha
 */
typedef struct AdrasteaI_ATNetService_Operator_t
{
    AdrasteaI_ATNetService_Operator_Numeric_t operatorNumeric;
    AdrasteaI_ATNetService_Operator_String_t operatorString;
} AdrasteaI_ATNetService_Operator_t;

/**
 * @brief Public Land Mobile Network Selection Mode
 */
typedef enum AdrasteaI_ATNetService_PLMN_Selection_Mode_t
{
    AdrasteaI_ATNetService_PLMN_Selection_Mode_Automatic = 0,
    AdrasteaI_ATNetService_PLMN_Selection_Mode_Manual,
    AdrasteaI_ATNetService_PLMN_Selection_Mode_Deregister,
    /** @cond DOXYGEN_IGNORE */
    AdrasteaI_ATNetService_PLMN_Selection_Mode_NumberOfValues
    /** @endcond */
} AdrasteaI_ATNetService_PLMN_Selection_Mode_t;

/**
 * @brief Public Land Mobile Network Format
 */
typedef enum AdrasteaI_ATNetService_PLMN_Format_t
{
    AdrasteaI_ATNetService_PLMN_Format_Invalid = -1,
    AdrasteaI_ATNetService_PLMN_Format_Long_AlphaNumeric = 0,
    AdrasteaI_ATNetService_PLMN_Format_Short_AlphaNumeric,
    AdrasteaI_ATNetService_PLMN_Format_Numeric,
    /** @cond DOXYGEN_IGNORE */
    AdrasteaI_ATNetService_PLMN_Format_NumberOfValues
    /** @endcond */
} AdrasteaI_ATNetService_PLMN_Format_t;

/**
 * @brief Public Land Mobile Network
 */
typedef struct AdrasteaI_ATNetService_PLMN_t
{
    AdrasteaI_ATNetService_PLMN_Selection_Mode_t selectionMode;
    AdrasteaI_ATNetService_PLMN_Format_t format;
    AdrasteaI_ATNetService_Operator_t operator;
    AdrasteaI_ATCommon_AcT_t accessTechnology;
} AdrasteaI_ATNetService_PLMN_t;

typedef uint8_t AdrasteaI_ATNetService_Signal_Quality_RSSI_t;
typedef uint8_t AdrasteaI_ATNetService_Signal_Quality_Bit_Error_Rate_t;

/**
 * @brief Signal Quality
 */
typedef struct AdrasteaI_ATNetService_Signal_Quality_t
{
    AdrasteaI_ATNetService_Signal_Quality_RSSI_t rssi;
    AdrasteaI_ATNetService_Signal_Quality_Bit_Error_Rate_t ber;
} AdrasteaI_ATNetService_Signal_Quality_t;

typedef uint8_t AdrasteaI_ATNetService_Extended_Signal_Quality_rxlev_t;
typedef uint8_t AdrasteaI_ATNetService_Extended_Signal_Quality_rscp_t;
typedef uint8_t AdrasteaI_ATNetService_Extended_Signal_Quality_ecno_t;
typedef uint8_t AdrasteaI_ATNetService_Extended_Signal_Quality_rsrq_t;
typedef uint8_t AdrasteaI_ATNetService_Extended_Signal_Quality_rsrp_t;

/**
 * @brief Extended Signal Quality
 */
typedef struct AdrasteaI_ATNetService_Extended_Signal_Quality_t
{
    AdrasteaI_ATNetService_Extended_Signal_Quality_rxlev_t rxlev;
    AdrasteaI_ATNetService_Signal_Quality_Bit_Error_Rate_t ber;
    AdrasteaI_ATNetService_Extended_Signal_Quality_rscp_t rscp;
    AdrasteaI_ATNetService_Extended_Signal_Quality_ecno_t ecno;
    AdrasteaI_ATNetService_Extended_Signal_Quality_rsrq_t rsrq;
    AdrasteaI_ATNetService_Extended_Signal_Quality_rsrp_t rsrp;
} AdrasteaI_ATNetService_Extended_Signal_Quality_t;

/**
 * @brief Power Saving Mode
 */
typedef enum AdrasteaI_ATNetService_Power_Saving_Mode_State_t
{
    AdrasteaI_ATNetService_Power_Saving_Mode_State_Disable,
    AdrasteaI_ATNetService_Power_Saving_Mode_State_Enable,
    /** @cond DOXYGEN_IGNORE */
    AdrasteaI_ATNetService_Power_Saving_Mode_State_NumberOfValues
    /** @endcond */
} AdrasteaI_ATNetService_Power_Saving_Mode_State_t;

typedef uint8_t AdrasteaI_ATNetService_Power_Saving_Mode_Periodic_TAU_Value_t;

/**
 * @brief Power Saving Mode Periodic TAU Units
 */
typedef enum AdrasteaI_ATNetService_Power_Saving_Mode_Periodic_TAU_Unit_t
{
    AdrasteaI_ATNetService_Power_Saving_Mode_Periodic_TAU_Unit_10m,
    AdrasteaI_ATNetService_Power_Saving_Mode_Periodic_TAU_Unit_1h,
    AdrasteaI_ATNetService_Power_Saving_Mode_Periodic_TAU_Unit_10h,
    AdrasteaI_ATNetService_Power_Saving_Mode_Periodic_TAU_Unit_2s,
    AdrasteaI_ATNetService_Power_Saving_Mode_Periodic_TAU_Unit_30s,
    AdrasteaI_ATNetService_Power_Saving_Mode_Periodic_TAU_Unit_1m,
    AdrasteaI_ATNetService_Power_Saving_Mode_Periodic_TAU_Unit_320h,
    AdrasteaI_ATNetService_Power_Saving_Mode_Periodic_TAU_Deactivated,
} AdrasteaI_ATNetService_Power_Saving_Mode_Periodic_TAU_Unit_t;

typedef uint8_t AdrasteaI_ATNetService_Power_Saving_Mode_Active_Time_Value_t;

/**
 * @brief Power Saving Mode Active Time Units
 */
typedef enum AdrasteaI_ATNetService_Power_Saving_Mode_Active_Time_Unit_t
{
    AdrasteaI_ATNetService_Power_Saving_Mode_Active_Time_Unit_2s,
    AdrasteaI_ATNetService_Power_Saving_Mode_Active_Time_Unit_1m,
    AdrasteaI_ATNetService_Power_Saving_Mode_Active_Time_Unit_Decih,
    AdrasteaI_ATNetService_Power_Saving_Mode_Active_Time_Deactivated = 7,
} AdrasteaI_ATNetService_Power_Saving_Mode_Active_Time_Unit_t;

/**
 * @brief Power Saving Mode Periodic TAU
 */
typedef union AdrasteaI_ATNetService_Power_Saving_Mode_Periodic_TAU_t
{
    uint8_t periodicTAU;
    struct
    {
        AdrasteaI_ATNetService_Power_Saving_Mode_Periodic_TAU_Value_t value : 5;
        AdrasteaI_ATNetService_Power_Saving_Mode_Periodic_TAU_Unit_t unit : 3;
    } periodicTAUValues;
} AdrasteaI_ATNetService_Power_Saving_Mode_Periodic_TAU_t;

/**
 * @brief Power Saving Mode Active Time
 */
typedef union AdrasteaI_ATNetService_Power_Saving_Mode_Active_Time_t
{
    uint8_t activeTime;
    struct
    {
        AdrasteaI_ATNetService_Power_Saving_Mode_Active_Time_Value_t value : 5;
        AdrasteaI_ATNetService_Power_Saving_Mode_Active_Time_Unit_t unit : 3;
    } activeTimeValues;
} AdrasteaI_ATNetService_Power_Saving_Mode_Active_Time_t;

/**
 * @brief Power Saving Mode
 */
typedef struct AdrasteaI_ATNetService_Power_Saving_Mode_t
{
    AdrasteaI_ATNetService_Power_Saving_Mode_State_t state;
    AdrasteaI_ATNetService_Power_Saving_Mode_Periodic_TAU_t periodicTAU;
    AdrasteaI_ATNetService_Power_Saving_Mode_Active_Time_t activeTime;
} AdrasteaI_ATNetService_Power_Saving_Mode_t;

/**
 * @brief eDRX Access Technology
 */
typedef enum AdrasteaI_ATNetService_eDRX_AcT_t
{
    AdrasteaI_ATNetService_eDRX_AcT_Invalid = -1,
    AdrasteaI_ATNetService_eDRX_AcT_NotUsingeDRX = 0,
    AdrasteaI_ATNetService_eDRX_AcT_EC_GSM_IoT_AGb = 1,
    AdrasteaI_ATNetService_eDRX_AcT_GSM_AGb = 2,
    AdrasteaI_ATNetService_eDRX_AcT_UTRAN_LU = 3,
    AdrasteaI_ATNetService_eDRX_AcT_E_UTRAN_WB_S1 = 4,
    AdrasteaI_ATNetService_eDRX_AcT_E_UTRAN_NB_S1 = 5,
    /** @cond DOXYGEN_IGNORE */
    AdrasteaI_ATNetService_eDRX_AcT_NumberOfValues
    /** @endcond */
} AdrasteaI_ATNetService_eDRX_AcT_t;

/**
 * @brief eDRX Value
 */
typedef enum AdrasteaI_ATNetService_eDRX_Value_t
{
    AdrasteaI_ATNetService_eDRX_Value_Invalid = -1,
    AdrasteaI_ATNetService_eDRX_Value_5s_120ms,
    AdrasteaI_ATNetService_eDRX_Value_10s_240ms,
    AdrasteaI_ATNetService_eDRX_Value_20s_480ms,
    AdrasteaI_ATNetService_eDRX_Value_40s_960ms,
    AdrasteaI_ATNetService_eDRX_Value_61s_440ms,
    AdrasteaI_ATNetService_eDRX_Value_81s_920ms,
    AdrasteaI_ATNetService_eDRX_Value_102s_400ms,
    AdrasteaI_ATNetService_eDRX_Value_122s_880ms,
    AdrasteaI_ATNetService_eDRX_Value_143s_360ms,
    AdrasteaI_ATNetService_eDRX_Value_163s_840ms,
    AdrasteaI_ATNetService_eDRX_Value_327s_680ms,
    AdrasteaI_ATNetService_eDRX_Value_655s_360ms,
    AdrasteaI_ATNetService_eDRX_Value_1310s_720ms,
    AdrasteaI_ATNetService_eDRX_Value_2621s_440ms,
    AdrasteaI_ATNetService_eDRX_Value_5242s_880ms,
    AdrasteaI_ATNetService_eDRX_Value_10485s_760ms,
    /** @cond DOXYGEN_IGNORE */
    AdrasteaI_ATNetService_eDRX_Value_NumberOfValues
    /** @endcond */
} AdrasteaI_ATNetService_eDRX_Value_t;

/**
 * @brief eDRX Mode
 */
typedef enum AdrasteaI_ATNetService_eDRX_Mode_t
{
    AdrasteaI_ATNetService_eDRX_Mode_Disable,
    AdrasteaI_ATNetService_eDRX_Mode_Enable,
    AdrasteaI_ATNetService_eDRX_Mode_Disable_Reset,
    /** @cond DOXYGEN_IGNORE */
    AdrasteaI_ATNetService_eDRX_Mode_NumberOfValues
    /** @endcond */
} AdrasteaI_ATNetService_eDRX_Mode_t;

typedef uint8_t AdrasteaI_ATNetService_eDRX_Paging_Time_Window_t;

/**
 * @brief eDRX
 */
typedef struct AdrasteaI_ATNetService_eDRX_t
{
    AdrasteaI_ATNetService_eDRX_AcT_t AcT;
    AdrasteaI_ATNetService_eDRX_Value_t requestedValue;
    AdrasteaI_ATNetService_eDRX_Value_t networkProvidedValue;
    AdrasteaI_ATNetService_eDRX_Paging_Time_Window_t pagingTime;
} AdrasteaI_ATNetService_eDRX_t;

/**
 * @brief Coverage Enhancement Access Technology
 */
typedef enum AdrasteaI_ATNetService_CES_AcT_t
{
    AdrasteaI_ATNetService_CES_AcT_Invalid = -1,
    AdrasteaI_ATNetService_CES_AcT_No_Enhancement,
    AdrasteaI_ATNetService_CES_AcT_E_UTRAN,
    AdrasteaI_ATNetService_CES_AcT_EC_GSM_IoT_AGb,
    AdrasteaI_ATNetService_CES_AcT_E_UTRAN_NB_S1,
    /** @cond DOXYGEN_IGNORE */
    AdrasteaI_ATNetService_CES_AcT_NumberOfValues
    /** @endcond */
} AdrasteaI_ATNetService_CES_AcT_t;

/**
 * @brief Coverage Enhancement Level
 */
typedef enum AdrasteaI_ATNetService_CES_CEL_t
{
    AdrasteaI_ATNetService_CES_CEL_Invalid = -1,
    AdrasteaI_ATNetService_CES_CEL_No_Enhancement,
    AdrasteaI_ATNetService_CES_CEL_Level0,
    AdrasteaI_ATNetService_CES_CEL_Level1,
    AdrasteaI_ATNetService_CES_CEL_Level2,
    AdrasteaI_ATNetService_CES_CEL_Level3,
    /** @cond DOXYGEN_IGNORE */
    AdrasteaI_ATNetService_CES_CEL_NumberOfValues
    /** @endcond */
} AdrasteaI_ATNetService_CES_CEL_t;

/**
 * @brief Coverage Enhancement Class
 */
typedef enum AdrasteaI_ATNetService_CES_CC_t
{
    AdrasteaI_ATNetService_CES_CC_Invalid = -1,
    AdrasteaI_ATNetService_CES_CC_No_Coverage_Class,
    AdrasteaI_ATNetService_CES_CC_Coverage_Class1,
    AdrasteaI_ATNetService_CES_CC_Coverage_Class2,
    AdrasteaI_ATNetService_CES_CC_Coverage_Class3,
    AdrasteaI_ATNetService_CES_CC_Coverage_Class4,
    AdrasteaI_ATNetService_CES_CC_Coverage_Class5,
    /** @cond DOXYGEN_IGNORE */
    AdrasteaI_ATNetService_CES_CC_NumberOfValues
    /** @endcond */
} AdrasteaI_ATNetService_CES_CC_t;

/**
 * @brief Coverage Enhancement
 */
typedef struct AdrasteaI_ATNetService_CES_t
{
    AdrasteaI_ATNetService_CES_AcT_t AcT;
    AdrasteaI_ATNetService_CES_CEL_t coverageEnhacementLevel;
    AdrasteaI_ATNetService_CES_CC_t coverageClass;
} AdrasteaI_ATNetService_CES_t;

/**
 * @brief Read Operators from memory (using the AT+COPN command).
 *
 * @return True if successful, false otherwise
 */
extern bool AdrasteaI_ATNetService_ReadOperators();

/**
 * @brief Parses the value of Operator Read event arguments.
 *
 * @param[in] pEventArguments: String containing arguments of the AT command
 * @param[out] dataP: the operator is returned in this argument. See AdrasteaI_ATNetService_Operator_t.
 *
 * @return True if successful, false otherwise
 */
extern bool AdrasteaI_ATNetService_ParseOperatorReadEvent(char* pEventArguments, AdrasteaI_ATNetService_Operator_t* dataP);

/**
 * @brief Set Public Land Mobile Network read format (using the AT+COPS command).
 *
 * @param[in] format: PLMN format. See AdrasteaI_ATNetService_PLMN_Format_t.
 *
 * @return True if successful, false otherwise
 */
extern bool AdrasteaI_ATNetService_SetPLMNReadFormat(AdrasteaI_ATNetService_PLMN_Format_t format);

/**
 * @brief Read currently selected plmn (using the AT+COPS command).
 *
 * @param[in] plmnP: Currently selected plmn is returned in this argument. See AdrasteaI_ATNetService_PLMN_t.
 *
 * @return True if successful, false otherwise
 */
extern bool AdrasteaI_ATNetService_ReadPLMN(AdrasteaI_ATNetService_PLMN_t* plmnP);

/**
 * @brief Set Public Land Mobile Network (using the AT+COPS command).
 *
 * @param[in] plmn: either choose a specific plmn or automatic. See AdrasteaI_ATNetService_PLMN_t.
 *
 * @return True if successful, false otherwise
 */
extern bool AdrasteaI_ATNetService_SetPLMN(AdrasteaI_ATNetService_PLMN_t plmn);

/**
 * @brief Read Signal Quality (using the AT+CSQ command).
 *
 * @param[out] sq: Signal Quality is returned in this argument. See AdrasteaI_ATNetService_Signal_Quality_t.
 *
 * @return True if successful, false otherwise
 */
extern bool AdrasteaI_ATNetService_ReadSignalQuality(AdrasteaI_ATNetService_Signal_Quality_t* sq);

/**
 * @brief Read Extended Signal Quality (using the AT+CESQ command).
 *
 * @param[out] esq: Extended Signal Quality is returned in this argument. See AdrasteaI_ATNetService_Extended_Signal_Quality_t.
 *
 * @return True if successful, false otherwise
 */
extern bool AdrasteaI_ATNetService_ReadExtendedSignalQuality(AdrasteaI_ATNetService_Extended_Signal_Quality_t* esq);

/**
 * @brief Read Power Saving Mode (using the AT+CPSMS command).
 *
 * @param[out] psmP: Power Saving Mode state and timers are returned in this argument. See AdrasteaI_ATNetService_Power_Saving_Mode_t.
 *
 * @return True if successful, false otherwise
 */
extern bool AdrasteaI_ATNetService_ReadPowerSavingMode(AdrasteaI_ATNetService_Power_Saving_Mode_t* psmP);

/**
 * @brief Set Power Saving Mode (using the AT+CPSMS command).
 *
 * @param[in] psm: Power Saving Mode state and timers. See AdrasteaI_ATNetService_Power_Saving_Mode_t.
 *
 * @return True if successful, false otherwise
 */
extern bool AdrasteaI_ATNetService_SetPowerSavingMode(AdrasteaI_ATNetService_Power_Saving_Mode_t psm);

/**
 * @brief Set eDRX Settings (using the AT+CEDRXS command).
 *
 * @param[in] mode: eDRX State. See AdrasteaI_ATNetService_eDRX_Mode_t.
 *
 * @param[in] edrx: Configuration. See AdrasteaI_ATNetService_eDRX_t.
 *
 * @return True if successful, false otherwise
 */
extern bool AdrasteaI_ATNetService_SeteDRXDynamicParameters(AdrasteaI_ATNetService_eDRX_Mode_t mode, AdrasteaI_ATNetService_eDRX_t edrx);

/**
 * @brief Set eDRX Settings (using the AT+CEDRXRDP command).
 *
 * @param[out] edrxP: eDRX Configuration is returned in this argument. See AdrasteaI_ATNetService_eDRX_t.
 *
 * @return True if successful, false otherwise
 */
extern bool AdrasteaI_ATNetService_ReadeDRXDynamicParameters(AdrasteaI_ATNetService_eDRX_t* edrxP);

/**
 * @brief Read Coverage Enhancement Status (using the AT+CRCES command).
 *
 * @param[out] cesP: Coverage Enhancement Status is returned in this argument. See AdrasteaI_ATNetService_CES_t.
 *
 * @return True if successful, false otherwise
 */
extern bool AdrasteaI_ATNetService_ReadCoverageEnhancementStatus(AdrasteaI_ATNetService_CES_t* cesP);

#ifdef __cplusplus
}
#endif

#endif /* ADRASTEAI_AT_NETSERVICE_H_INCLUDED */
