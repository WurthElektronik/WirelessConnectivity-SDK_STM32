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
 * @file ATGNSS.h
 * @brief AT commands for GNSS functionality.
 */

#ifndef ADRASTEAI_AT_GNSS_H_INCLUDED
#define ADRASTEAI_AT_GNSS_H_INCLUDED

#include <AdrasteaI/ATCommands/ATCommon.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief Supported GNSS Start Modes
 */
typedef enum AdrasteaI_ATGNSS_Start_Mode_t
{
    AdrasteaI_ATGNSS_Start_Mode_Invalid = -1, /**< Invalid start mode */
    AdrasteaI_ATGNSS_Start_Mode_Cold = 1,     /**< Cold start mode */
    AdrasteaI_ATGNSS_Start_Mode_Hot = 2,      /**< Hot start mode */
    /** @cond DOXYGEN_IGNORE */
    AdrasteaI_ATGNSS_Start_Mode_NumberOfValues
    /** @endcond */
} AdrasteaI_ATGNSS_Start_Mode_t;

/**
 * @brief GNSS Tolerance
 */
typedef uint32_t AdrasteaI_ATGNSS_Tolerance_t;

/**
 * @brief GNSS Active Modes
 */
typedef enum AdrasteaI_ATGNSS_Active_Mode_t
{
    AdrasteaI_ATGNSS_Active_Mode_Invalid = -1, /**< Invalid active mode */
    AdrasteaI_ATGNSS_Active_Mode_Not_Active,   /**< Mode not active */
    AdrasteaI_ATGNSS_Active_Mode_Active,       /**< Mode active */

    /** @cond DOXYGEN_IGNORE */
    AdrasteaI_ATGNSS_Active_Mode_NumberOfValues
    /** @endcond */
} AdrasteaI_ATGNSS_Active_Mode_t;

/**
 * @brief Runtime Mode States
 */
typedef enum AdrasteaI_ATGNSS_Runtime_Mode_State_t
{
    AdrasteaI_ATGNSS_Runtime_Mode_State_Not_Set, /**< State not set */
    AdrasteaI_ATGNSS_Runtime_Mode_State_Set,     /**< State set */
} AdrasteaI_ATGNSS_Runtime_Mode_State_t;

/**
 * @brief Supported GNSS Satellite Systems
 */
typedef union AdrasteaI_ATGNSS_Satellite_Systems_t
{
    /** @cond DOXYGEN_IGNORE */
    uint8_t satSystemsStates;
    /** @endcond */
    struct
    {
        AdrasteaI_ATGNSS_Runtime_Mode_State_t GPS : 1;     /**< GPS state */
        AdrasteaI_ATGNSS_Runtime_Mode_State_t GLONASS : 1; /**< GLONASS state */
    } systems;
} AdrasteaI_ATGNSS_Satellite_Systems_t;

/**
 * @brief Supported GNSS NMEA Sentences
 */
typedef union AdrasteaI_ATGNSS_NMEA_Sentences_t
{
    /** @cond DOXYGEN_IGNORE */
    uint16_t nmeaSentencesStates;
    /** @endcond */
    struct
    {
        AdrasteaI_ATGNSS_Runtime_Mode_State_t GGA : 1;  /**< GGA state */
        AdrasteaI_ATGNSS_Runtime_Mode_State_t GLL : 1;  /**< GLL state */
        AdrasteaI_ATGNSS_Runtime_Mode_State_t GSA : 1;  /**< GSA state */
        AdrasteaI_ATGNSS_Runtime_Mode_State_t GSV : 1;  /**< GSV state */
        AdrasteaI_ATGNSS_Runtime_Mode_State_t GNS : 1;  /**< GNS state */
        AdrasteaI_ATGNSS_Runtime_Mode_State_t RMC : 1;  /**< RMC state */
        AdrasteaI_ATGNSS_Runtime_Mode_State_t PIDX : 1; /**< PIDX state */
        AdrasteaI_ATGNSS_Runtime_Mode_State_t VTG : 1;  /**< VTG state */
        AdrasteaI_ATGNSS_Runtime_Mode_State_t ZDA : 1;  /**< ZDA state */
        AdrasteaI_ATGNSS_Runtime_Mode_State_t GST : 1;  /**< GST state */
    } sentences;
} AdrasteaI_ATGNSS_NMEA_Sentences_t;

/**
 * @brief GNSS Satellite Count
 */
typedef uint8_t AdrasteaI_ATGNSS_Satellite_Count_t;

typedef uint8_t AdrasteaI_ATGNSS_Satellite_PRN_t, AdrasteaI_ATGNSS_Satellite_Elevation_t, AdrasteaI_ATGNSS_Satellite_SNR_t;

typedef uint16_t AdrasteaI_ATGNSS_Satellite_Azimuth_t;

/**
 * @brief GNSS Satellite
 */
typedef struct AdrasteaI_ATGNSS_Satellite_t
{
    AdrasteaI_ATGNSS_Satellite_PRN_t prn;
    AdrasteaI_ATGNSS_Satellite_Elevation_t elevation;
    AdrasteaI_ATGNSS_Satellite_Azimuth_t azimuth;
    AdrasteaI_ATGNSS_Satellite_SNR_t snr;
} AdrasteaI_ATGNSS_Satellite_t;

/**
 * @brief GNSS Fix Relevancy
 */
typedef enum AdrasteaI_ATGNSS_Fix_Relavancy_t
{
    AdrasteaI_ATGNSS_Fix_Relavancy_Invalid = -1, /**< Invalid fix relevancy */
    AdrasteaI_ATGNSS_Fix_Relavancy_Current,      /**< Relevant to current fix */
    AdrasteaI_ATGNSS_Fix_Relavancy_Last,         /**<  Relevant to last fix */

    /** @cond DOXYGEN_IGNORE */
    AdrasteaI_ATGNSS_Fix_Relavancy_NumberOfValues
    /** @endcond */
} AdrasteaI_ATGNSS_Fix_Relavancy_t;

/**
 * @brief GNSS Fix Types
 */
typedef enum AdrasteaI_ATGNSS_Fix_Type_t
{
    AdrasteaI_ATGNSS_Fix_Type_Invalid = -1, /**< Invalid fix */
    AdrasteaI_ATGNSS_Fix_Type_No_Fix,       /**< No fix */
    AdrasteaI_ATGNSS_Fix_Type_MSA,          /**< MSA fix */
    AdrasteaI_ATGNSS_Fix_Type_MSB,          /**< MSB fix */

    /** @cond DOXYGEN_IGNORE */
    AdrasteaI_ATGNSS_Fix_Type_NumberOfValues
    /** @endcond */
} AdrasteaI_ATGNSS_Fix_Type_t;

typedef float AdrasteaI_ATGNSS_Radius_Accuracy_t, AdrasteaI_ATGNSS_Speed_t;

typedef enum AdrasteaI_ATGNSS_Ephemeris_t
{
    AdrasteaI_ATGNSS_Ephemeris_Invalid = -1,
    AdrasteaI_ATGNSS_Ephemeris_BEP,
    AdrasteaI_ATGNSS_Ephemeris_CEP,
    /** @cond DOXYGEN_IGNORE */
    AdrasteaI_ATGNSS_Ephemeris_NumberOfValues
    /** @endcond */
} AdrasteaI_ATGNSS_Ephemeris_t;

/**
 * @brief GNSS Fix
 */
typedef struct AdrasteaI_ATGNSS_Fix_t
{
    AdrasteaI_ATGNSS_Fix_Type_t fixType;
    AdrasteaI_ATCommon_Time_t time;
    AdrasteaI_ATCommon_Date_t date;
    AdrasteaI_ATCommon_Latitude_t latitude;
    AdrasteaI_ATCommon_Longitude_t longitude;
    AdrasteaI_ATCommon_Altitude_t altitude;
    AdrasteaI_ATCommon_UTC_Timestamp_t utcTimestamp;
    AdrasteaI_ATGNSS_Radius_Accuracy_t accuracy;
    AdrasteaI_ATGNSS_Speed_t speed;
    AdrasteaI_ATGNSS_Ephemeris_t ephType;
} AdrasteaI_ATGNSS_Fix_t;

typedef float AdrasteaI_ATGNSS_TTFF_t;

/**
 * @brief GNSS Ephemeris Status
 */
typedef enum AdrasteaI_ATGNSS_Ephemeris_Status_t
{
    AdrasteaI_ATGNSS_Ephemeris_Status_Invalid = -1,
    AdrasteaI_ATGNSS_Ephemeris_Status_Not_Valid,
    AdrasteaI_ATGNSS_Ephemeris_Status_Valid,
    /** @cond DOXYGEN_IGNORE */
    AdrasteaI_ATGNSS_Ephemeris_Status_NumberOfValues
    /** @endcond */
} AdrasteaI_ATGNSS_Ephemeris_Status_t;

/**
 * @brief GNSS Event Types
 */
typedef enum AdrasteaI_ATGNSS_Event_t
{
    AdrasteaI_ATGNSS_Event_Invalid = -1,
    AdrasteaI_ATGNSS_Event_NMEA,
    AdrasteaI_ATGNSS_Event_Session_Status_Change,
    AdrasteaI_ATGNSS_Event_Allowed_Status_Change,
    /** @cond DOXYGEN_IGNORE */
    AdrasteaI_ATGNSS_Event_NumberOfValues
    /** @endcond */
} AdrasteaI_ATGNSS_Event_t;

/**
 * @brief GNSS Session Status
 */
typedef enum AdrasteaI_ATGNSS_Session_Status_t
{
    AdrasteaI_ATGNSS_Session_Status_Invalid = -1,
    AdrasteaI_ATGNSS_Session_Status_None,
    AdrasteaI_ATGNSS_Session_Status_Session_Begin,
    AdrasteaI_ATGNSS_Session_Status_Session_End,
    /** @cond DOXYGEN_IGNORE */
    AdrasteaI_ATGNSS_Session_Status_NumberOfValues
    /** @endcond */
} AdrasteaI_ATGNSS_Session_Status_t;

/**
 * @brief GNSS Allowed Status
 */
typedef enum AdrasteaI_ATGNSS_Allowed_Status_t
{
    AdrasteaI_ATGNSS_Allowed_Status_Invalid = -1,
    AdrasteaI_ATGNSS_Allowed_Status_Not_Allowed,
    AdrasteaI_ATGNSS_Allowed_Status_Allowed,
    AdrasteaI_ATGNSS_Allowed_Status_Started_Automatically_or_Delay,
    /** @cond DOXYGEN_IGNORE */
    AdrasteaI_ATGNSS_Allowed_Status_NumberOfValues
    /** @endcond */
} AdrasteaI_ATGNSS_Allowed_Status_t;

typedef char AdrasteaI_ATGNSS_NMEA_Sentence_t[128];

typedef enum AdrasteaI_ATGNSS_Deletion_Option_t
{
    AdrasteaI_ATGNSS_Deletion_Option_Invalid = -1,
    AdrasteaI_ATGNSS_Deletion_Option_All = 0,
    AdrasteaI_ATGNSS_Deletion_Option_Ephemeris,
    AdrasteaI_ATGNSS_Deletion_Option_Almanac,
    AdrasteaI_ATGNSS_Deletion_Option_Position,
    AdrasteaI_ATGNSS_Deletion_Option_Time,
    /** @cond DOXYGEN_IGNORE */
    AdrasteaI_ATGNSS_Deletion_Option_NumberOfValues
    /** @endcond */
} AdrasteaI_ATGNSS_Deletion_Option_t;

typedef enum AdrasteaI_ATGNSS_CEP_Number_of_Days_t
{
    AdrasteaI_ATGNSS_CEP_Number_of_Days_Invalid = -1,
    AdrasteaI_ATGNSS_CEP_Number_of_Days_1Day = 1,
    AdrasteaI_ATGNSS_CEP_Number_of_Days_2Days = 2,
    AdrasteaI_ATGNSS_CEP_Number_of_Days_3Days = 3,
    AdrasteaI_ATGNSS_CEP_Number_of_Days_7Days = 7,
    AdrasteaI_ATGNSS_CEP_Number_of_Days_14Days = 14,
    AdrasteaI_ATGNSS_CEP_Number_of_Days_28Days = 28,
    /** @cond DOXYGEN_IGNORE */
    AdrasteaI_ATGNSS_CEP_Number_of_Days_NumberOfValues
    /** @endcond */
} AdrasteaI_ATGNSS_CEP_Number_of_Days_t;

typedef enum AdrasteaI_ATGNSS_CEP_File_Validity_t
{
    AdrasteaI_ATGNSS_CEP_File_Validity_Invalid = -1,
    AdrasteaI_ATGNSS_CEP_File_Validity_Expired,
    AdrasteaI_ATGNSS_CEP_File_Validity_Valid,
    /** @cond DOXYGEN_IGNORE */
    AdrasteaI_ATGNSS_CEP_File_Validity_NumberOfValues
    /** @endcond */
} AdrasteaI_ATGNSS_CEP_File_Validity_t;

typedef uint8_t AdrasteaI_ATGNSS_CEP_Remaining_Days_t, AdrasteaI_ATGNSS_CEP_Remaining_Hours_t, AdrasteaI_ATGNSS_CEP_Remaining_Minutes_t;

/**
 * @brief CEP File Status
 */
typedef struct AdrasteaI_ATGNSS_CEP_Status_t
{
    AdrasteaI_ATGNSS_CEP_File_Validity_t validity;
    AdrasteaI_ATGNSS_CEP_Remaining_Days_t remDays;
    AdrasteaI_ATGNSS_CEP_Remaining_Hours_t remHours;
    AdrasteaI_ATGNSS_CEP_Remaining_Minutes_t remMinutes;
} AdrasteaI_ATGNSS_CEP_Status_t;

/**
 * @brief Stop GNSS (using the AT%IGNSSACT command).
 *
 * @return True if successful, false otherwise
 */
extern bool AdrasteaI_ATGNSS_StopGNSS();

/**
 * @brief Start GNSS (using the AT%IGNSSACT command).
 *
 * @param[in] startMode: GNSS Start Mode (optional pass AdrasteaI_ATGNSS_Start_Mode_Invalid to skip). See AdrasteaI_ATGNSS_Start_Mode_t.
 *
 * @return True if successful, false otherwise
 */
extern bool AdrasteaI_ATGNSS_StartGNSS(AdrasteaI_ATGNSS_Start_Mode_t startMode);

/**
 * @brief Start GNSS with Tolerance (using the AT%IGNSSACT command).
 *
 * @param[in] tolerance: Tolerance delay to start GNSS in Seconds
 *
 * @return True if successful, false otherwise
 */
extern bool AdrasteaI_ATGNSS_StartGNSSWithTolerance(AdrasteaI_ATGNSS_Tolerance_t tolerance);

/**
 * @brief Read GNSS Active (using the AT%IGNSSACT command).
 *
 * @param[out] activeModeP: GNSS Active is returned in this argument. See AdrasteaI_ATGNSS_Active_Mode_t.
 *
 * @return True if successful, false otherwise
 */
extern bool AdrasteaI_ATGNSS_ReadGNSSActiveMode(AdrasteaI_ATGNSS_Active_Mode_t* activeModeP);

/**
 * @brief Set Satellite Systems (using the AT%IGNSSCFG command).
 *
 * @param[in] satSystems: Satellite Systems used. See AdrasteaI_ATGNSS_Satellite_Systems_t.
 *
 * @return True if successful, false otherwise
 */
extern bool AdrasteaI_ATGNSS_SetSatelliteSystems(AdrasteaI_ATGNSS_Satellite_Systems_t satSystems);

/**
 * @brief Read Used Satellite Systems (using the AT%IGNSSCFG command).
 *
 * @param[out] satSystemsP: Used Satellite Systems are returned in this argument. See AdrasteaI_ATGNSS_Satellite_Systems_t.
 *
 * @return True if successful, false otherwise
 */
extern bool AdrasteaI_ATGNSS_GetSatelliteSystems(AdrasteaI_ATGNSS_Satellite_Systems_t* satSystemsP);

/**
 * @brief Set NMEA Sentences (using the AT%IGNSSCFG command).
 *
 * @param[in] nmeaSentences: NMEA Sentences. See AdrasteaI_ATGNSS_NMEA_Sentences_t.
 *
 * @return True if successful, false otherwise
 */
extern bool AdrasteaI_ATGNSS_SetNMEASentences(AdrasteaI_ATGNSS_NMEA_Sentences_t nmeaSentences);

/**
 * @brief Read Used NMEA Sentences (using the AT%IGNSSCFG command).
 *
 * @param[out] nmeaSentencesP: Used NMEA Sentences are returned in this argument. See AdrasteaI_ATGNSS_NMEA_Sentences_t.
 *
 * @return True if successful, false otherwise
 */
extern bool AdrasteaI_ATGNSS_GetNMEASentences(AdrasteaI_ATGNSS_NMEA_Sentences_t* nmeaSentencesP);

/**
 * @brief Query available GNSS Satellites (using the AT%IGNSSINFO command).
 *
 * @param[out] satCountP: Number of available Satellites is returned in this argument
 *
 * @return True if successful, false otherwise
 */
extern bool AdrasteaI_ATGNSS_QueryGNSSSatellites(AdrasteaI_ATGNSS_Satellite_Count_t* satCountP);

/**
 * @brief Parses the value of Query GNSS Satellites event arguments.
 *
 * @param[out] dataP: Satellite information is returned in this argument
 * @param[in] pEventArguments: String containing arguments of the AT command
 *
 * @return True if successful, false otherwise
 */
extern bool AdrasteaI_ATGNSS_ParseSatelliteQueryEvent(char* pEventArguments, AdrasteaI_ATGNSS_Satellite_t* dataP);

/**
 * @brief Query GNSS Fix (using the AT%IGNSSINFO command).
 *
 * @param[in] relevancy: Determines if Last Fix or Current Fix should be queried. See AdrasteaI_ATGNSS_Fix_Relavancy_t.
 *
 * @param[out] fixP: GNSS Fix is returned in this argument
 *
 * @return True if successful, false otherwise
 */
extern bool AdrasteaI_ATGNSS_QueryGNSSFix(AdrasteaI_ATGNSS_Fix_Relavancy_t relevancy, AdrasteaI_ATGNSS_Fix_t* fixP);

/**
 * @brief Query GNSS Time To First Fix (using the AT%IGNSSINFO command).
 *
 * @param[out] ttffP: Time to first fix in milliseconds is returned in this argument
 *
 * @return True if successful, false otherwise
 */
extern bool AdrasteaI_ATGNSS_QueryGNSSTTFF(AdrasteaI_ATGNSS_TTFF_t* ttffP);

/**
 * @brief Query GNSS Ephemeris (using the AT%IGNSSINFO command).
 *
 * @param[out] statusP: Ephemeris status is returned in this argument
 *
 * @return True if successful, false otherwise
 */
extern bool AdrasteaI_ATGNSS_QueryGNSSEphemerisStatus(AdrasteaI_ATGNSS_Ephemeris_Status_t* statusP);

/**
 * @brief Set GNSS Notification Events (using the AT%IGNSSINFO command).
 *
 * @param[in] event: GNSS event type. See AdrasteaI_ATGNSS_Event_t.
 *
 * @param[in] state: Event State
 *
 * @return True if successful, false otherwise
 */
extern bool AdrasteaI_ATGNSS_SetGNSSUnsolicitedNotificationEvents(AdrasteaI_ATGNSS_Event_t event, AdrasteaI_ATCommon_Event_State_t state);

/**
 * @brief Parses the value of Session Status Change event arguments.
 *
 * @param[out] dataP: Session Status is returned in this argument
 * @param[in] pEventArguments: String containing arguments of the AT command
 *
 * @return True if successful, false otherwise
 */
extern bool AdrasteaI_ATGNSS_ParseSessionStatusChangedEvent(char* pEventArguments, AdrasteaI_ATGNSS_Session_Status_t* dataP);

/**
 * @brief Parses the value of Allowed Status Change event arguments.
 *
 * @param[out] dataP: Allowed Status is returned in this argument
 * @param[in] pEventArguments: String containing arguments of the AT command
 *
 * @return True if successful, false otherwise
 */
extern bool AdrasteaI_ATGNSS_ParseAllowedStatusChangedEvent(char* pEventArguments, AdrasteaI_ATGNSS_Allowed_Status_t* dataP);

/**
 * @brief Parses the value of NMEA event arguments.
 *
 * @param[out] dataP: NMEA Sentence is returned in this argument
 * @param[in] pEventArguments: String containing arguments of the AT command
 *
 * @return True if successful, false otherwise
 */
extern bool AdrasteaI_ATGNSS_ParseNMEAEvent(char* pEventArguments, AdrasteaI_ATGNSS_NMEA_Sentence_t* dataP);

/**
 * @brief Deletes GNSS Data (using the AT%IGNSSMEM command).
 *
 * @param[in] deleteOption: GNSS data to delete. See AdrasteaI_ATGNSS_Deletion_Option_t.
 *
 * @return True if successful, false otherwise
 */
extern bool AdrasteaI_ATGNSS_DeleteData(AdrasteaI_ATGNSS_Deletion_Option_t deleteOption);

/**
 * @brief Downloads CEP File (using the AT%IGNSSCEP command).
 *
 * @param[in] numDays: Number of days for the downlaoded CEP file. See AdrasteaI_ATGNSS_CEP_Number_of_Days_t.
 *
 * @return True if successful, false otherwise
 */
extern bool AdrasteaI_ATGNSS_DownloadCEPFile(AdrasteaI_ATGNSS_CEP_Number_of_Days_t numDays);

/**
 * @brief Erase CEP File (using the AT%IGNSSCEP command).
 *
 * @return True if successful, false otherwise
 */
extern bool AdrasteaI_ATGNSS_EraseCEPFile();

/**
 * @brief Query Information about CEP File (using the AT%IGNSSCEP command).
 *
 * @param[out] status: of CEP file is returned in this argument. See AdrasteaI_ATGNSS_CEP_Status_t.
 *
 * @return True if successful, false otherwise
 */
extern bool AdrasteaI_ATGNSS_QueryCEPFileStatus(AdrasteaI_ATGNSS_CEP_Status_t* status);

#ifdef __cplusplus
}
#endif

#endif /* ADRASTEAI_AT_GNSS_H_INCLUDED */
