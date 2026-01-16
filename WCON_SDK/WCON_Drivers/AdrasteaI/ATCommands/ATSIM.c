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
 * @file ATSIM.c
 * @brief AT commands for SIM functionality.
 */
#include <AdrasteaI/ATCommands/ATSIM.h>
#include <AdrasteaI/AdrasteaI.h>
#include <global/ATCommands.h>
#include <stdio.h>

static const char* AdrasteaI_ATSIM_Facility_Strings[AdrasteaI_ATSIM_Facility_NumberOfValues] = {
    "SC", "P2", "PN", "PU", "PS",
};

static const char* AdrasteaI_ATSIM_PIN_Status_Strings[AdrasteaI_ATSIM_PIN_Status_NumberOfValues] = {
    "READY", "SIM PIN", "SIM PUK", "PH-SIM PIN", "PH-FSIM PIN", "PH-FSIM PUK", "SIM PIN2", "SIM PUK2", "PH-NET PIN", "PH-NET PUK", "PH-NETSUB PIN", "PH-NETSUB PUK", "PH-SP PIN", "PH-SP PUK", "PH-CORP PIN", "PH-CORP PUK",
};

bool AdrasteaI_ATSIM_RequestInternationalMobileSubscriberIdentity(AdrasteaI_ATSIM_IMSI_t* imsiP)
{
    if (imsiP == NULL)
    {
        return false;
    }

    if (!AdrasteaI_SendRequest("AT+CIMI\r\n"))
    {
        return false;
    }

    char* pResponseCommand = AT_commandBuffer;

    if (!AdrasteaI_WaitForConfirm(AdrasteaI_GetTimeout(AdrasteaI_Timeout_SIM), AdrasteaI_CNFStatus_Success, pResponseCommand))
    {
        return false;
    }

    if (!ATCommand_GetNextArgumentString(&pResponseCommand, (char*)*imsiP, ATCOMMAND_STRING_TERMINATE, sizeof(*imsiP)))
    {
        return false;
    }

    return true;
}

bool AdrasteaI_ATSIM_SetFacilityLock(AdrasteaI_ATSIM_Facility_t facility, AdrasteaI_ATSIM_Lock_Mode_t mode, AdrasteaI_ATSIM_PIN_t pin)
{
    AdrasteaI_optionalParamsDelimCount = 1;

    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+CLCK=");

    if (facility == AdrasteaI_ATSIM_Facility_P2)
    {
        return false;
    }

    if (!ATCommand_AppendArgumentStringQuotationMarks(pRequestCommand, AdrasteaI_ATSIM_Facility_Strings[facility], ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_AppendArgumentInt(pRequestCommand, mode, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC), ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (strlen(pin) != 0)
    {
        if (!ATCommand_AppendArgumentStringQuotationMarks(pRequestCommand, pin, ATCOMMAND_STRING_TERMINATE))
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

    if (!AdrasteaI_WaitForConfirm(AdrasteaI_GetTimeout(AdrasteaI_Timeout_SIM), AdrasteaI_CNFStatus_Success, NULL))
    {
        return false;
    }

    return true;
}

bool AdrasteaI_ATSIM_ReadFacilityLock(AdrasteaI_ATSIM_Facility_t facility, AdrasteaI_ATSIM_Lock_Status_t* statusP)
{
    if (statusP == NULL)
    {
        return false;
    }

    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+CLCK=");

    if (facility == AdrasteaI_ATSIM_Facility_P2)
    {
        return false;
    }

    if (!ATCommand_AppendArgumentStringQuotationMarks(pRequestCommand, AdrasteaI_ATSIM_Facility_Strings[facility], ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_AppendArgumentInt(pRequestCommand, 2, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC), ATCOMMAND_STRING_TERMINATE))
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

    char* pResponseCommand = AT_commandBuffer;

    if (!AdrasteaI_WaitForConfirm(AdrasteaI_GetTimeout(AdrasteaI_Timeout_SIM), AdrasteaI_CNFStatus_Success, pResponseCommand))
    {
        return false;
    }

    pResponseCommand += 1;

    if (!ATCommand_GetNextArgumentInt(&pResponseCommand, statusP, ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }

    return true;
}

bool AdrasteaI_ATSIM_ReadSubscriberNumber()
{
    /* TODO unfinished command */
    if (!AdrasteaI_SendRequest("AT+CNUM\r\n"))
    {
        return false;
    }

    if (!AdrasteaI_WaitForConfirm(AdrasteaI_GetTimeout(AdrasteaI_Timeout_SIM), AdrasteaI_CNFStatus_Success, NULL))
    {
        return false;
    }

    return true;
}

bool AdrasteaI_ATSIM_ReadPinStatus(AdrasteaI_ATSIM_PIN_Status_t* statusP)
{
    if (statusP == NULL)
    {
        return false;
    }

    if (!AdrasteaI_SendRequest("AT+CPIN?\r\n"))
    {
        return false;
    }

    char* pResponseCommand = AT_commandBuffer;

    if (!AdrasteaI_WaitForConfirm(AdrasteaI_GetTimeout(AdrasteaI_Timeout_SIM), AdrasteaI_CNFStatus_Success, pResponseCommand))
    {
        return false;
    }

    pResponseCommand += 1;

    uint8_t status_index;

    if (!ATCommand_GetNextArgumentEnum(&pResponseCommand, &status_index, AdrasteaI_ATSIM_PIN_Status_Strings, AdrasteaI_ATSIM_PIN_Status_NumberOfValues, 30, ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }

    *statusP = (AdrasteaI_ATSIM_PIN_Status_t)status_index;

    return true;
}

bool AdrasteaI_ATSIM_EnterPin(AdrasteaI_ATSIM_PIN_t pin1, AdrasteaI_ATSIM_PIN_t pin2)
{
    AdrasteaI_optionalParamsDelimCount = 1;

    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+CPIN=");

    if (!ATCommand_AppendArgumentStringQuotationMarks(pRequestCommand, pin1, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (strlen(pin2) != 0)
    {
        if (!ATCommand_AppendArgumentStringQuotationMarks(pRequestCommand, pin2, ATCOMMAND_STRING_TERMINATE))
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

    if (!AdrasteaI_WaitForConfirm(AdrasteaI_GetTimeout(AdrasteaI_Timeout_SIM), AdrasteaI_CNFStatus_Success, NULL))
    {
        return false;
    }

    return true;
}

bool AdrasteaI_ATSIM_ChangePassword(AdrasteaI_ATSIM_Facility_t facility, AdrasteaI_ATSIM_PIN_t oldpassword, AdrasteaI_ATSIM_PIN_t newpassword)
{
    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+CPWD=");

    if (!ATCommand_AppendArgumentStringQuotationMarks(pRequestCommand, AdrasteaI_ATSIM_Facility_Strings[facility], ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_AppendArgumentStringQuotationMarks(pRequestCommand, oldpassword, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_AppendArgumentStringQuotationMarks(pRequestCommand, newpassword, ATCOMMAND_STRING_TERMINATE))
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

    if (!AdrasteaI_WaitForConfirm(AdrasteaI_GetTimeout(AdrasteaI_Timeout_SIM), AdrasteaI_CNFStatus_Success, NULL))
    {
        return false;
    }

    return true;
}

bool AdrasteaI_ATSIM_RestrictedSIMAccess(AdrasteaI_ATSIM_Restricted_Access_Command_t cmd, AdrasteaI_ATSIM_Restricted_Access_File_ID fileID, AdrasteaI_ATSIM_Restricted_Access_P1 p1, AdrasteaI_ATSIM_Restricted_Access_P2 p2, AdrasteaI_ATSIM_Restricted_Access_P3 p3, char* dataWritten, AdrasteaI_ATSIM_Restricted_Access_Response_t* cmdResponse)
{
    AdrasteaI_optionalParamsDelimCount = 1;

    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+CRSM=");

    if (!ATCommand_AppendArgumentInt(pRequestCommand, cmd, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC), ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_AppendArgumentInt(pRequestCommand, fileID, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC), ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_AppendArgumentInt(pRequestCommand, p1, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC), ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_AppendArgumentInt(pRequestCommand, p2, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC), ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_AppendArgumentInt(pRequestCommand, p3, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC), ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (dataWritten != NULL)
    {
        if (!ATCommand_AppendArgumentStringQuotationMarks(pRequestCommand, dataWritten, ATCOMMAND_STRING_TERMINATE))
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

    char* pResponseCommand = AT_commandBuffer;

    if (!AdrasteaI_WaitForConfirm(AdrasteaI_GetTimeout(AdrasteaI_Timeout_SIM), AdrasteaI_CNFStatus_Success, pResponseCommand))
    {
        return false;
    }

    pResponseCommand += 1;

    if (!ATCommand_GetNextArgumentInt(&pResponseCommand, &cmdResponse->sw1, ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    switch (ATCommand_CountArgs(pResponseCommand))
    {
        case 1:
        {
            if (!ATCommand_GetNextArgumentInt(&pResponseCommand, &cmdResponse->sw2, ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_STRING_TERMINATE))
            {
                return false;
            }
            break;
        }
        case 2:
        {
            if (!ATCommand_GetNextArgumentInt(&pResponseCommand, &cmdResponse->sw2, ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_ARGUMENT_DELIM))
            {
                return false;
            }
            if (!ATCommand_GetNextArgumentStringWithoutQuotationMarks(&pResponseCommand, cmdResponse->responseRead, ATCOMMAND_STRING_TERMINATE, cmdResponse->responseReadMaxBufferSize))
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

bool AdrasteaI_ATSIM_RequestIntegratedCircuitCardIdentifier(AdrasteaI_ATSIM_ICCID_t* iccidP)
{
    if (iccidP == NULL)
    {
        return false;
    }

    if (!AdrasteaI_SendRequest("AT%CCID\r\n"))
    {
        return false;
    }

    char* pResponseCommand = AT_commandBuffer;

    if (!AdrasteaI_WaitForConfirm(AdrasteaI_GetTimeout(AdrasteaI_Timeout_SIM), AdrasteaI_CNFStatus_Success, pResponseCommand))
    {
        return false;
    }

    pResponseCommand += 1;

    if (!ATCommand_GetNextArgumentString(&pResponseCommand, (char*)*iccidP, ATCOMMAND_STRING_TERMINATE, sizeof(*iccidP)))
    {
        return false;
    }

    return true;
}

bool AdrasteaI_ATSMS_ParseSubscriberNumberEvent(char* pEventArguments, AdrasteaI_ATSIM_Subscriber_Number_t* dataP)
{
    if (dataP == NULL || pEventArguments == NULL)
    {
        return false;
    }

    char* argumentsP = pEventArguments;

    argumentsP += 1;

    char alphaN[30];

    if (!ATCommand_GetNextArgumentString(&argumentsP, alphaN, ATCOMMAND_ARGUMENT_DELIM, sizeof(alphaN)))
    {
        return false;
    }

    if (!ATCommand_GetNextArgumentStringWithoutQuotationMarks(&argumentsP, dataP->number, ATCOMMAND_ARGUMENT_DELIM, sizeof(dataP->number)))
    {
        return false;
    }

    switch (ATCommand_CountArgs(argumentsP))
    {
        case 1:
        {
            if (!ATCommand_GetNextArgumentInt(&argumentsP, &dataP->numberType, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_SIZE8), ATCOMMAND_STRING_TERMINATE))
            {
                return false;
            }
            break;
        }
        default:
        {
            if (!ATCommand_GetNextArgumentInt(&argumentsP, &dataP->numberType, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_SIZE8), ATCOMMAND_ARGUMENT_DELIM))
            {
                return false;
            }
            break;
        }
    }

    return true;
}
