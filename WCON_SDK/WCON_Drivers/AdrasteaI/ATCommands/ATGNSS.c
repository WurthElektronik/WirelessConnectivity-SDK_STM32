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
 * @file ATGNSS.c
 * @brief AT commands for GNSS functionality.
 */
#include <AdrasteaI/ATCommands/ATGNSS.h>
#include <AdrasteaI/AdrasteaI.h>
#include <global/ATCommands.h>
#include <stdio.h>

#define AdrasteaI_ATGNSS_Satellite_Systems_Strings_NumberOfValues 2

#define AdrasteaI_ATGNSS_NMEA_Sentences_Strings_NumberOfValues 10

static const char* AdrasteaI_ATGNSS_Satellite_Systems_Strings[AdrasteaI_ATGNSS_Satellite_Systems_Strings_NumberOfValues] = {
    "GPS",
    "GLONASS",
};

static const char* AdrasteaI_ATGNSS_NMEA_Sentences_Strings[AdrasteaI_ATGNSS_NMEA_Sentences_Strings_NumberOfValues] = {
    "GGA", "GLL", "GSA", "GSV", "GNS", "RMC", "PIDX", "VTG", "ZDA", "GST",
};

static const char* AdrasteaI_ATGNSS_Ephemeris_Strings[AdrasteaI_ATGNSS_Ephemeris_NumberOfValues] = {
    "B",
    "C",
};

static const char* AdrasteaI_ATGNSS_Event_Strings[AdrasteaI_ATGNSS_Event_NumberOfValues] = {
    "NMEA",
    "SESSIONSTAT",
    "ALLOWSTAT",
};

static const char* AdrasteaI_ATGNSS_Deletion_Option_Strings[AdrasteaI_ATGNSS_Deletion_Option_NumberOfValues] = {
    "0", "0001", "0002", "0004", "0008",
};

bool AdrasteaI_ATGNSS_StopGNSS()
{
    if (!AdrasteaI_SendRequest("AT%IGNSSACT=0\r\n"))
    {
        return false;
    }

    if (!AdrasteaI_WaitForConfirm(AdrasteaI_GetTimeout(AdrasteaI_Timeout_GNSS), AdrasteaI_CNFStatus_Success, NULL))
    {
        return false;
    }

    return true;
}

bool AdrasteaI_ATGNSS_StartGNSS(AdrasteaI_ATGNSS_Start_Mode_t startMode)
{
    AdrasteaI_optionalParamsDelimCount = 1;

    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT%IGNSSACT=1,");

    if (startMode != AdrasteaI_ATGNSS_Start_Mode_Invalid)
    {
        if (!ATCommand_AppendArgumentInt(pRequestCommand, startMode, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC), ATCOMMAND_STRING_TERMINATE))
        {
            return false;
        }
        AdrasteaI_optionalParamsDelimCount = 0;
    }

    pRequestCommand[strlen(pRequestCommand) - AdrasteaI_optionalParamsDelimCount] = ATCOMMAND_STRING_TERMINATE;

    if (!ATCommand_AppendArgumentString(pRequestCommand, ATCOMMAND_CRLF, ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }

    if (!AdrasteaI_SendRequest(pRequestCommand))
    {
        return false;
    }

    if (!AdrasteaI_WaitForConfirm(AdrasteaI_GetTimeout(AdrasteaI_Timeout_GNSS), AdrasteaI_CNFStatus_Success, NULL))
    {
        return false;
    }

    return true;
}

bool AdrasteaI_ATGNSS_StartGNSSWithTolerance(AdrasteaI_ATGNSS_Tolerance_t tolerance)
{
    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT%IGNSSACT=2,");

    if (!ATCommand_AppendArgumentInt(pRequestCommand, tolerance, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC), ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }

    if (!ATCommand_AppendArgumentString(pRequestCommand, ATCOMMAND_CRLF, ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }

    if (!AdrasteaI_SendRequest(pRequestCommand))
    {
        return false;
    }

    if (!AdrasteaI_WaitForConfirm(AdrasteaI_GetTimeout(AdrasteaI_Timeout_GNSS), AdrasteaI_CNFStatus_Success, NULL))
    {
        return false;
    }

    return true;
}

bool AdrasteaI_ATGNSS_ReadGNSSActiveMode(AdrasteaI_ATGNSS_Active_Mode_t* activeModeP)
{
    if (activeModeP == NULL)
    {
        return false;
    }

    if (!AdrasteaI_SendRequest("AT%IGNSSACT?\r\n"))
    {
        return false;
    }

    char* pResponseCommand = AT_commandBuffer;

    if (!AdrasteaI_WaitForConfirm(AdrasteaI_GetTimeout(AdrasteaI_Timeout_GNSS), AdrasteaI_CNFStatus_Success, pResponseCommand))
    {
        return false;
    }

    if (!ATCommand_GetNextArgumentInt(&pResponseCommand, activeModeP, ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }

    return true;
}

bool AdrasteaI_ATGNSS_SetSatelliteSystems(AdrasteaI_ATGNSS_Satellite_Systems_t satSystems)
{
    AdrasteaI_optionalParamsDelimCount = 1;

    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT%IGNSSCFG=\"SET\",\"SAT\",");

    for (uint8_t i = 0; i < AdrasteaI_ATGNSS_Satellite_Systems_Strings_NumberOfValues; i++)
    {
        if ((satSystems.satSystemsStates & (AdrasteaI_ATGNSS_Runtime_Mode_State_Set << i)) >> i)
        {
            if (!ATCommand_AppendArgumentStringQuotationMarks(pRequestCommand, AdrasteaI_ATGNSS_Satellite_Systems_Strings[i], ATCOMMAND_ARGUMENT_DELIM))
            {
                return false;
            }
        }
    }

    pRequestCommand[strlen(pRequestCommand) - AdrasteaI_optionalParamsDelimCount] = ATCOMMAND_STRING_TERMINATE;

    if (!ATCommand_AppendArgumentString(pRequestCommand, ATCOMMAND_CRLF, ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }

    if (!AdrasteaI_SendRequest(pRequestCommand))
    {
        return false;
    }

    if (!AdrasteaI_WaitForConfirm(AdrasteaI_GetTimeout(AdrasteaI_Timeout_GNSS), AdrasteaI_CNFStatus_Success, NULL))
    {
        return false;
    }

    return true;
}

bool AdrasteaI_ATGNSS_GetSatelliteSystems(AdrasteaI_ATGNSS_Satellite_Systems_t* satSystemsP)
{
    if (satSystemsP == NULL)
    {
        return false;
    }

    if (!AdrasteaI_SendRequest("AT%IGNSSCFG=\"GET\",\"SAT\"\r\n"))
    {
        return false;
    }

    char* pResponseCommand = AT_commandBuffer;

    if (!AdrasteaI_WaitForConfirm(AdrasteaI_GetTimeout(AdrasteaI_Timeout_GNSS), AdrasteaI_CNFStatus_Success, pResponseCommand))
    {
        return false;
    }

    pResponseCommand += 1;

    satSystemsP->satSystemsStates = 0;

    uint8_t argscount = ATCommand_CountArgs(pResponseCommand);

    for (uint8_t i = 0; i < argscount; i++)
    {
        uint8_t satSystem;
        if (i == argscount - 1)
        {
            if (!ATCommand_GetNextArgumentEnumWithoutQuotationMarks(&pResponseCommand, &satSystem, AdrasteaI_ATGNSS_Satellite_Systems_Strings, AdrasteaI_ATGNSS_Satellite_Systems_Strings_NumberOfValues, 30, ATCOMMAND_STRING_TERMINATE))
            {
                return false;
            }
        }
        else
        {
            if (!ATCommand_GetNextArgumentEnumWithoutQuotationMarks(&pResponseCommand, &satSystem, AdrasteaI_ATGNSS_Satellite_Systems_Strings, AdrasteaI_ATGNSS_Satellite_Systems_Strings_NumberOfValues, 30, ATCOMMAND_ARGUMENT_DELIM))
            {
                return false;
            }
        }

        satSystemsP->satSystemsStates |= AdrasteaI_ATGNSS_Runtime_Mode_State_Set << satSystem;
    }

    return true;
}

bool AdrasteaI_ATGNSS_SetNMEASentences(AdrasteaI_ATGNSS_NMEA_Sentences_t nmeaSentences)
{
    AdrasteaI_optionalParamsDelimCount = 1;

    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT%IGNSSCFG=\"SET\",\"NMEA\"");

    for (uint8_t i = 0; i < AdrasteaI_ATGNSS_NMEA_Sentences_Strings_NumberOfValues; i++)
    {
        if ((nmeaSentences.nmeaSentencesStates & (AdrasteaI_ATGNSS_Runtime_Mode_State_Set << i)) >> i)
        {
            if (!ATCommand_AppendArgumentStringQuotationMarks(pRequestCommand, AdrasteaI_ATGNSS_NMEA_Sentences_Strings[i], ATCOMMAND_ARGUMENT_DELIM))
            {
                return false;
            }
        }
    }

    pRequestCommand[strlen(pRequestCommand) - AdrasteaI_optionalParamsDelimCount] = ATCOMMAND_STRING_TERMINATE;

    if (!ATCommand_AppendArgumentString(pRequestCommand, ATCOMMAND_CRLF, ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }

    if (!AdrasteaI_SendRequest(pRequestCommand))
    {
        return false;
    }

    if (!AdrasteaI_WaitForConfirm(AdrasteaI_GetTimeout(AdrasteaI_Timeout_GNSS), AdrasteaI_CNFStatus_Success, NULL))
    {
        return false;
    }

    return true;
}

bool AdrasteaI_ATGNSS_GetNMEASentences(AdrasteaI_ATGNSS_NMEA_Sentences_t* nmeaSentencesP)
{
    if (nmeaSentencesP == NULL)
    {
        return false;
    }

    if (!AdrasteaI_SendRequest("AT%IGNSSCFG=\"GET\",\"NMEA\"\r\n"))
    {
        return false;
    }

    char* pResponseCommand = AT_commandBuffer;

    if (!AdrasteaI_WaitForConfirm(AdrasteaI_GetTimeout(AdrasteaI_Timeout_GNSS), AdrasteaI_CNFStatus_Success, pResponseCommand))
    {
        return false;
    }

    pResponseCommand += 1;

    nmeaSentencesP->nmeaSentencesStates = 0;

    uint8_t argscount = ATCommand_CountArgs(pResponseCommand);

    for (uint8_t i = 0; i < argscount; i++)
    {
        uint8_t nmeaSentence;
        if (i == argscount - 1)
        {
            if (!ATCommand_GetNextArgumentEnumWithoutQuotationMarks(&pResponseCommand, &nmeaSentence, AdrasteaI_ATGNSS_NMEA_Sentences_Strings, AdrasteaI_ATGNSS_NMEA_Sentences_Strings_NumberOfValues, 30, ATCOMMAND_STRING_TERMINATE))
            {
                return false;
            }
        }
        else
        {
            if (!ATCommand_GetNextArgumentEnumWithoutQuotationMarks(&pResponseCommand, &nmeaSentence, AdrasteaI_ATGNSS_NMEA_Sentences_Strings, AdrasteaI_ATGNSS_NMEA_Sentences_Strings_NumberOfValues, 30, ATCOMMAND_ARGUMENT_DELIM))
            {
                return false;
            }
        }

        nmeaSentencesP->nmeaSentencesStates |= AdrasteaI_ATGNSS_Runtime_Mode_State_Set << nmeaSentence;
    }

    return true;
}

bool AdrasteaI_ATGNSS_QueryGNSSSatellites(AdrasteaI_ATGNSS_Satellite_Count_t* satCountP)
{
    if (satCountP == NULL)
    {
        return false;
    }

    if (!AdrasteaI_SendRequest("AT%IGNSSINFO=\"SAT\"\r\n"))
    {
        return false;
    }

    char* pResponseCommand = AT_commandBuffer;

    if (!AdrasteaI_WaitForConfirm(AdrasteaI_GetTimeout(AdrasteaI_Timeout_GNSS), AdrasteaI_CNFStatus_Success, pResponseCommand))
    {
        return false;
    }

    pResponseCommand += 1;

    if (!ATCommand_GetNextArgumentInt(&pResponseCommand, satCountP, ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }

    return true;
}

bool AdrasteaI_ATGNSS_ParseSatelliteQueryEvent(char* pEventArguments, AdrasteaI_ATGNSS_Satellite_t* dataP)
{
    if (dataP == NULL || pEventArguments == NULL)
    {
        return false;
    }

    char* argumentsP = pEventArguments;

    switch (ATCommand_CountArgs(argumentsP))
    {
        case 4:
        {
            if (!ATCommand_GetNextArgumentInt(&argumentsP, &dataP->prn, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_SIZE8), ATCOMMAND_ARGUMENT_DELIM))
            {
                return false;
            }

            if (!ATCommand_GetNextArgumentInt(&argumentsP, &dataP->elevation, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_SIZE8), ATCOMMAND_ARGUMENT_DELIM))
            {
                return false;
            }

            if (!ATCommand_GetNextArgumentInt(&argumentsP, &dataP->azimuth, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_SIZE16), ATCOMMAND_ARGUMENT_DELIM))
            {
                return false;
            }

            if (!ATCommand_GetNextArgumentInt(&argumentsP, &dataP->snr, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_SIZE8), ATCOMMAND_STRING_TERMINATE))
            {
                return false;
            }

            break;
        }
        default:
            return false;
            break;
    }

    return true;
}

bool AdrasteaI_ATGNSS_QueryGNSSFix(AdrasteaI_ATGNSS_Fix_Relavancy_t relevancy, AdrasteaI_ATGNSS_Fix_t* fixP)
{
    if (fixP == NULL)
    {
        return false;
    }

    char* pRequestCommand = AT_commandBuffer;

    switch (relevancy)
    {
        case AdrasteaI_ATGNSS_Fix_Relavancy_Current:
            strcpy(pRequestCommand, "AT%IGNSSINFO=\"FIX\"\r\n");
            break;
        case AdrasteaI_ATGNSS_Fix_Relavancy_Last:
            strcpy(pRequestCommand, "AT%IGNSSINFO=\"LASTFIX\"\r\n");
            break;
        default:
            return false;
            break;
    }

    if (!AdrasteaI_SendRequest(pRequestCommand))
    {
        return false;
    }

    char* pResponseCommand = AT_commandBuffer;

    if (!AdrasteaI_WaitForConfirm(AdrasteaI_GetTimeout(AdrasteaI_Timeout_GNSS), AdrasteaI_CNFStatus_Success, pResponseCommand))
    {
        return false;
    }

    memset(fixP, -1, sizeof(AdrasteaI_ATGNSS_Fix_t));

    pResponseCommand += 1;

    switch (ATCommand_CountArgs(pResponseCommand))
    {
        case 1:
            if (!ATCommand_GetNextArgumentInt(&pResponseCommand, &fixP->fixType, ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_STRING_TERMINATE))
            {
                return false;
            }

            return true;
        default:
            if (!ATCommand_GetNextArgumentInt(&pResponseCommand, &fixP->fixType, ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_ARGUMENT_DELIM))
            {
                return false;
            }
    }

    char time[9];

    char* timeP = time;

    if (!ATCommand_GetNextArgumentStringWithoutQuotationMarks(&pResponseCommand, time, ATCOMMAND_ARGUMENT_DELIM, sizeof(time)))
    {
        return false;
    }

    if (!ATCommand_GetNextArgumentInt(&timeP, &fixP->time.Hours, ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ':'))
    {
        return false;
    }

    if (!ATCommand_GetNextArgumentInt(&timeP, &fixP->time.Minutes, ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ':'))
    {
        return false;
    }

    if (!ATCommand_GetNextArgumentInt(&timeP, &fixP->time.Seconds, ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }

    char date[11];

    char* dateP = date;

    if (!ATCommand_GetNextArgumentStringWithoutQuotationMarks(&pResponseCommand, date, ATCOMMAND_ARGUMENT_DELIM, sizeof(date)))
    {
        return false;
    }

    if (!ATCommand_GetNextArgumentInt(&dateP, &fixP->date.Day, ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, '/'))
    {
        return false;
    }

    if (!ATCommand_GetNextArgumentInt(&dateP, &fixP->date.Month, ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, '/'))
    {
        return false;
    }

    if (!ATCommand_GetNextArgumentInt(&dateP, &fixP->date.Year, ATCOMMAND_INTFLAGS_SIZE16 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }

    if (!ATCommand_GetNextArgumentDoubleWithoutQuotationMarks(&pResponseCommand, &fixP->latitude, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_GetNextArgumentDoubleWithoutQuotationMarks(&pResponseCommand, &fixP->longitude, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_GetNextArgumentDoubleWithoutQuotationMarks(&pResponseCommand, &fixP->altitude, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_GetNextArgumentInt(&pResponseCommand, &fixP->utcTimestamp, ATCOMMAND_INTFLAGS_SIZE64 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_GetNextArgumentFloat(&pResponseCommand, &fixP->accuracy, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_GetNextArgumentFloatWithoutQuotationMarks(&pResponseCommand, &fixP->speed, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_GetNextArgumentEnumWithoutQuotationMarks(&pResponseCommand, (uint8_t*)&fixP->ephType, AdrasteaI_ATGNSS_Ephemeris_Strings, AdrasteaI_ATGNSS_Ephemeris_NumberOfValues, 30, ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }

    return true;
}

bool AdrasteaI_ATGNSS_QueryGNSSTTFF(AdrasteaI_ATGNSS_TTFF_t* ttffP)
{
    if (ttffP == NULL)
    {
        return false;
    }

    if (!AdrasteaI_SendRequest("AT%IGNSSINFO=\"TTFF\"\r\n"))
    {
        return false;
    }

    char* pResponseCommand = AT_commandBuffer;

    if (!AdrasteaI_WaitForConfirm(AdrasteaI_GetTimeout(AdrasteaI_Timeout_GNSS), AdrasteaI_CNFStatus_Success, pResponseCommand))
    {
        return false;
    }

    pResponseCommand += 1;

    if (!ATCommand_GetNextArgumentFloatWithoutQuotationMarks(&pResponseCommand, ttffP, ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }

    return true;
}

bool AdrasteaI_ATGNSS_QueryGNSSEphemerisStatus(AdrasteaI_ATGNSS_Ephemeris_Status_t* statusP)
{
    if (statusP == NULL)
    {
        return false;
    }

    if (!AdrasteaI_SendRequest("AT%IGNSSINFO=\"EPH\"\r\n"))
    {
        return false;
    }

    char* pResponseCommand = AT_commandBuffer;

    if (!AdrasteaI_WaitForConfirm(AdrasteaI_GetTimeout(AdrasteaI_Timeout_GNSS), AdrasteaI_CNFStatus_Success, pResponseCommand))
    {
        return false;
    }

    if (!ATCommand_GetNextArgumentInt(&pResponseCommand, statusP, ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }

    return true;
}

bool AdrasteaI_ATGNSS_SetGNSSUnsolicitedNotificationEvents(AdrasteaI_ATGNSS_Event_t event, AdrasteaI_ATCommon_Event_State_t state)
{
    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT%IGNSSEV=");

    if (!ATCommand_AppendArgumentStringQuotationMarks(pRequestCommand, AdrasteaI_ATGNSS_Event_Strings[event], ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_AppendArgumentInt(pRequestCommand, state, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC), ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }

    if (!ATCommand_AppendArgumentString(pRequestCommand, ATCOMMAND_CRLF, ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }

    if (!AdrasteaI_SendRequest(pRequestCommand))
    {
        return false;
    }

    if (!AdrasteaI_WaitForConfirm(AdrasteaI_GetTimeout(AdrasteaI_Timeout_GNSS), AdrasteaI_CNFStatus_Success, NULL))
    {
        return false;
    }

    return true;
}

bool AdrasteaI_ATGNSS_ParseSessionStatusChangedEvent(char* pEventArguments, AdrasteaI_ATGNSS_Session_Status_t* dataP)
{
    if (dataP == NULL || pEventArguments == NULL)
    {
        return false;
    }

    char* argumentsP = pEventArguments;

    if (!ATCommand_GetNextArgumentInt(&argumentsP, dataP, ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }

    return true;
}

bool AdrasteaI_ATGNSS_ParseAllowedStatusChangedEvent(char* pEventArguments, AdrasteaI_ATGNSS_Allowed_Status_t* dataP)
{
    if (dataP == NULL || pEventArguments == NULL)
    {
        return false;
    }

    char* argumentsP = pEventArguments;

    if (!ATCommand_GetNextArgumentInt(&argumentsP, dataP, ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }

    return true;
}

bool AdrasteaI_ATGNSS_ParseNMEAEvent(char* pEventArguments, AdrasteaI_ATGNSS_NMEA_Sentence_t* dataP)
{
    if (dataP == NULL || pEventArguments == NULL)
    {
        return false;
    }

    strcpy(*dataP, pEventArguments + 1);

    return true;
}

bool AdrasteaI_ATGNSS_DeleteData(AdrasteaI_ATGNSS_Deletion_Option_t deleteOption)
{
    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT%IGNSSMEM=\"ERASE\",");

    if (!ATCommand_AppendArgumentStringQuotationMarks(pRequestCommand, AdrasteaI_ATGNSS_Deletion_Option_Strings[deleteOption], ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }

    if (!ATCommand_AppendArgumentString(pRequestCommand, ATCOMMAND_CRLF, ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }

    if (!AdrasteaI_SendRequest(pRequestCommand))
    {
        return false;
    }

    if (!AdrasteaI_WaitForConfirm(AdrasteaI_GetTimeout(AdrasteaI_Timeout_GNSS), AdrasteaI_CNFStatus_Success, NULL))
    {
        return false;
    }

    return true;
}

bool AdrasteaI_ATGNSS_DownloadCEPFile(AdrasteaI_ATGNSS_CEP_Number_of_Days_t numDays)
{
    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT%IGNSSCEP=\"DLD\",");

    if (!ATCommand_AppendArgumentInt(pRequestCommand, numDays, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC), ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }

    if (!ATCommand_AppendArgumentString(pRequestCommand, ATCOMMAND_CRLF, ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }

    if (!AdrasteaI_SendRequest(pRequestCommand))
    {
        return false;
    }

    if (!AdrasteaI_WaitForConfirm(AdrasteaI_GetTimeout(AdrasteaI_Timeout_GNSS), AdrasteaI_CNFStatus_Success, NULL))
    {
        return false;
    }

    return true;
}

bool AdrasteaI_ATGNSS_EraseCEPFile()
{
    if (!AdrasteaI_SendRequest("AT%IGNSSCEP=\"ERASE\"\r\n"))
    {
        return false;
    }

    if (!AdrasteaI_WaitForConfirm(AdrasteaI_GetTimeout(AdrasteaI_Timeout_GNSS), AdrasteaI_CNFStatus_Success, NULL))
    {
        return false;
    }

    return true;
}

bool AdrasteaI_ATGNSS_QueryCEPFileStatus(AdrasteaI_ATGNSS_CEP_Status_t* status)
{
    if (status == NULL)
    {
        return false;
    }

    if (!AdrasteaI_SendRequest("AT%IGNSSCEP=\"STAT\"\r\n"))
    {
        return false;
    }

    char* pResponseCommand = AT_commandBuffer;

    if (!AdrasteaI_WaitForConfirm(AdrasteaI_GetTimeout(AdrasteaI_Timeout_GNSS), AdrasteaI_CNFStatus_Success, pResponseCommand))
    {
        return false;
    }

    pResponseCommand += 1;

    switch (ATCommand_CountArgs(pResponseCommand))
    {
        case 1:
        {
            if (!ATCommand_GetNextArgumentInt(&pResponseCommand, &status->validity, ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_STRING_TERMINATE))
            {
                return false;
            }

            break;
        }
        case 4:
        {
            if (!ATCommand_GetNextArgumentInt(&pResponseCommand, &status->validity, ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_ARGUMENT_DELIM))
            {
                return false;
            }

            if (!ATCommand_GetNextArgumentInt(&pResponseCommand, &status->remDays, ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_ARGUMENT_DELIM))
            {
                return false;
            }

            if (!ATCommand_GetNextArgumentInt(&pResponseCommand, &status->remHours, ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_ARGUMENT_DELIM))
            {
                return false;
            }

            if (!ATCommand_GetNextArgumentInt(&pResponseCommand, &status->remMinutes, ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_STRING_TERMINATE))
            {
                return false;
            }

            break;
        }
        default:
            return false;
    }

    return true;
}
