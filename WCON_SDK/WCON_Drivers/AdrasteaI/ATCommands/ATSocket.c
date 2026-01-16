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
 * @file ATSocket.c
 * @brief AT commands for Socket functionality.
 */
#include <AdrasteaI/ATCommands/ATSocket.h>
#include <AdrasteaI/AdrasteaI.h>
#include <global/ATCommands.h>
#include <stdio.h>

static const char* AdrasteaI_ATSocket_State_Strings[AdrasteaI_ATSocket_State_NumberOfValues] = {"DEACTIVATED", "ACTIVATED", "LISTENING"};

static const char* AdrasteaI_ATSocket_Type_Strings[AdrasteaI_ATSocket_Type_NumberOfValues] = {"TCP", "UDP"};

static const char* AdrasteaI_ATSocket_Behaviour_Strings[AdrasteaI_ATSocket_Behaviour_NumberOfValues] = {"OPEN", "LISTEN", "LISTENP"};

bool AdrasteaI_ATSocket_ReadCreatedSocketsStates()
{
    if (!AdrasteaI_SendRequest("AT%SOCKETCMD?\r\n"))
    {
        return false;
    }
    return AdrasteaI_WaitForConfirm(AdrasteaI_GetTimeout(AdrasteaI_Timeout_Socket), AdrasteaI_CNFStatus_Success, NULL);
}

bool AdrasteaI_ATSocket_AllocateSocket(AdrasteaI_ATCommon_Session_ID_t sessionID, AdrasteaI_ATSocket_Type_t socketType, AdrasteaI_ATSocket_Behaviour_t socketBehaviour, AdrasteaI_ATCommon_IP_Addr_t destinationIPAddress, AdrasteaI_ATCommon_Port_Number_t destinationPortNumber, AdrasteaI_ATCommon_Port_Number_t sourcePortNumber, AdrasteaI_ATSocket_Data_Length_t packetSize,
                                       AdrasteaI_ATSocket_Timeout_t socketTimeout, AdrasteaI_ATSocket_IP_Addr_Format_t addressFormat, AdrasteaI_ATSocket_ID_t* socketIDP)
{
    if (socketIDP == NULL)
    {
        return false;
    }

    AdrasteaI_optionalParamsDelimCount = 1;

    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT%SOCKETCMD=\"ALLOCATE\",");

    if (!ATCommand_AppendArgumentInt(pRequestCommand, sessionID, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC), ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_AppendArgumentStringQuotationMarks(pRequestCommand, AdrasteaI_ATSocket_Type_Strings[socketType], ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_AppendArgumentStringQuotationMarks(pRequestCommand, AdrasteaI_ATSocket_Behaviour_Strings[socketBehaviour], ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_AppendArgumentStringQuotationMarks(pRequestCommand, destinationIPAddress, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_AppendArgumentInt(pRequestCommand, destinationPortNumber, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC), ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (sourcePortNumber != AdrasteaI_ATCommon_Port_Number_Invalid)
    {
        if (!ATCommand_AppendArgumentInt(pRequestCommand, sourcePortNumber, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC), ATCOMMAND_ARGUMENT_DELIM))
        {
            return false;
        }
        AdrasteaI_optionalParamsDelimCount = 1;
    }
    else
    {
        if (!ATCommand_AppendArgumentString(pRequestCommand, ATCOMMAND_STRING_EMPTY, ATCOMMAND_ARGUMENT_DELIM))
        {
            return false;
        }
        AdrasteaI_optionalParamsDelimCount++;
    }

    if (packetSize != AdrasteaI_ATSocket_Data_Length_Automatic)
    {
        if (!ATCommand_AppendArgumentInt(pRequestCommand, packetSize, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC), ATCOMMAND_ARGUMENT_DELIM))
        {
            return false;
        }
        AdrasteaI_optionalParamsDelimCount = 1;
    }
    else
    {
        if (!ATCommand_AppendArgumentString(pRequestCommand, ATCOMMAND_STRING_EMPTY, ATCOMMAND_ARGUMENT_DELIM))
        {
            return false;
        }
        AdrasteaI_optionalParamsDelimCount++;
    }

    if (socketTimeout != AdrasteaI_ATSocket_Timeout_Invalid)
    {
        if (!ATCommand_AppendArgumentInt(pRequestCommand, socketTimeout, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC), ATCOMMAND_ARGUMENT_DELIM))
        {
            return false;
        }
        AdrasteaI_optionalParamsDelimCount = 1;
    }
    else
    {
        if (!ATCommand_AppendArgumentString(pRequestCommand, ATCOMMAND_STRING_EMPTY, ATCOMMAND_ARGUMENT_DELIM))
        {
            return false;
        }
        AdrasteaI_optionalParamsDelimCount++;
    }

    if (addressFormat != AdrasteaI_ATSocket_IP_Addr_Format_Invalid)
    {
        if (!ATCommand_AppendArgumentInt(pRequestCommand, addressFormat, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC), ATCOMMAND_STRING_TERMINATE))
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

    if (!AdrasteaI_WaitForConfirm(AdrasteaI_GetTimeout(AdrasteaI_Timeout_Socket), AdrasteaI_CNFStatus_Success, pResponseCommand))
    {
        return false;
    }

    if (!ATCommand_GetNextArgumentInt(&pResponseCommand, socketIDP, ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }

    return true;
}

bool AdrasteaI_ATSocket_ActivateSocket(AdrasteaI_ATSocket_ID_t socketID, AdrasteaI_ATCommon_Session_ID_t SSLSessionID)
{
    AdrasteaI_optionalParamsDelimCount = 1;

    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT%SOCKETCMD=\"ACTIVATE\",");

    if (!ATCommand_AppendArgumentInt(pRequestCommand, socketID, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC), ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (SSLSessionID != AdrasteaI_ATCommon_Session_ID_Invalid)
    {
        if (!ATCommand_AppendArgumentInt(pRequestCommand, SSLSessionID, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC), ATCOMMAND_STRING_TERMINATE))
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

    if (!AdrasteaI_WaitForConfirm(AdrasteaI_GetTimeout(AdrasteaI_Timeout_Socket), AdrasteaI_CNFStatus_Success, NULL))
    {
        return false;
    }

    return true;
}

bool AdrasteaI_ATSocket_ReadSocketInfo(AdrasteaI_ATSocket_ID_t socketID, AdrasteaI_ATSocket_Info_t* infoP)
{
    if (infoP == NULL)
    {
        return false;
    }

    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT%SOCKETCMD=\"INFO\",");

    if (!ATCommand_AppendArgumentInt(pRequestCommand, socketID, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC), ATCOMMAND_STRING_TERMINATE))
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

    if (!AdrasteaI_WaitForConfirm(AdrasteaI_GetTimeout(AdrasteaI_Timeout_Socket), AdrasteaI_CNFStatus_Success, pResponseCommand))
    {
        return false;
    }

    if (!ATCommand_GetNextArgumentEnumWithoutQuotationMarks(&pResponseCommand, (uint8_t*)&infoP->socketState, AdrasteaI_ATSocket_State_Strings, AdrasteaI_ATSocket_State_NumberOfValues, 30, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_GetNextArgumentEnumWithoutQuotationMarks(&pResponseCommand, (uint8_t*)&infoP->socketType, AdrasteaI_ATSocket_Type_Strings, AdrasteaI_ATSocket_Type_NumberOfValues, 30, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_GetNextArgumentStringWithoutQuotationMarks(&pResponseCommand, infoP->sourceIPAddress, ATCOMMAND_ARGUMENT_DELIM, sizeof(infoP->sourceIPAddress)))
    {
        return false;
    }

    if (!ATCommand_GetNextArgumentStringWithoutQuotationMarks(&pResponseCommand, infoP->destinationIPAddress, ATCOMMAND_ARGUMENT_DELIM, sizeof(infoP->sourceIPAddress)))
    {
        return false;
    }

    if (!ATCommand_GetNextArgumentInt(&pResponseCommand, &infoP->sourcePortNumber, ATCOMMAND_INTFLAGS_SIZE16 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    switch (ATCommand_CountArgs(pResponseCommand))
    {
        case 1:
        {
            if (!ATCommand_GetNextArgumentInt(&pResponseCommand, &infoP->destinationPortNumber, ATCOMMAND_INTFLAGS_SIZE16 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_STRING_TERMINATE))
            {
                return false;
            }

            infoP->tcpSocketDirection = AdrasteaI_ATSocket_Direction_Invalid;

            infoP->socketTimeout = AdrasteaI_ATSocket_Timeout_Invalid;

            break;
        }
        case 3:
        {
            if (!ATCommand_GetNextArgumentInt(&pResponseCommand, &infoP->destinationPortNumber, ATCOMMAND_INTFLAGS_SIZE16 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_ARGUMENT_DELIM))
            {
                return false;
            }

            if (!ATCommand_GetNextArgumentInt(&pResponseCommand, &infoP->tcpSocketDirection, ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_ARGUMENT_DELIM))
            {
                return false;
            }

            if (!ATCommand_GetNextArgumentInt(&pResponseCommand, &infoP->socketTimeout, ATCOMMAND_INTFLAGS_SIZE16 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_STRING_TERMINATE))
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

bool AdrasteaI_ATSocket_DeactivateSocket(AdrasteaI_ATSocket_ID_t socketID)
{
    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT%SOCKETCMD=\"DEACTIVATE\",");

    if (!ATCommand_AppendArgumentInt(pRequestCommand, socketID, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC), ATCOMMAND_STRING_TERMINATE))
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

    if (!AdrasteaI_WaitForConfirm(AdrasteaI_GetTimeout(AdrasteaI_Timeout_Socket), AdrasteaI_CNFStatus_Success, NULL))
    {
        return false;
    }

    return true;
}

bool AdrasteaI_ATSocket_SetSocketOptions(AdrasteaI_ATSocket_ID_t socketID, AdrasteaI_ATSocket_Aggregation_Time_t aggregationTime, AdrasteaI_ATSocket_Aggregation_Buffer_Size_t aggregationBufferSize, AdrasteaI_ATSocket_TCP_Idle_Time_t idleTime)
{
    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT%SOCKETCMD=\"SETOPT\",");

    if (!ATCommand_AppendArgumentInt(pRequestCommand, socketID, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC), ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_AppendArgumentInt(pRequestCommand, aggregationTime, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC), ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_AppendArgumentInt(pRequestCommand, aggregationBufferSize, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC), ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_AppendArgumentInt(pRequestCommand, idleTime, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC), ATCOMMAND_STRING_TERMINATE))
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

    if (!AdrasteaI_WaitForConfirm(AdrasteaI_GetTimeout(AdrasteaI_Timeout_Socket), AdrasteaI_CNFStatus_Success, NULL))
    {
        return false;
    }

    return true;
}

bool AdrasteaI_ATSocket_DeleteSocket(AdrasteaI_ATSocket_ID_t socketID)
{
    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT%SOCKETCMD=\"DELETE\",");

    if (!ATCommand_AppendArgumentInt(pRequestCommand, socketID, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC), ATCOMMAND_STRING_TERMINATE))
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

    if (!AdrasteaI_WaitForConfirm(AdrasteaI_GetTimeout(AdrasteaI_Timeout_Socket), AdrasteaI_CNFStatus_Success, NULL))
    {
        return false;
    }

    return true;
}

bool AdrasteaI_ATSocket_AddSSLtoSocket(AdrasteaI_ATSocket_ID_t socketID, AdrasteaI_ATCommon_SSL_Auth_Mode_t authMode, AdrasteaI_ATCommon_SSL_Profile_ID_t profileID)
{
    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT%SOCKETCMD=\"SSLALLOC\",");

    if (!ATCommand_AppendArgumentInt(pRequestCommand, socketID, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC), ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_AppendArgumentInt(pRequestCommand, authMode, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC), ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_AppendArgumentInt(pRequestCommand, profileID, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC), ATCOMMAND_STRING_TERMINATE))
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

    if (!AdrasteaI_WaitForConfirm(AdrasteaI_GetTimeout(AdrasteaI_Timeout_Socket), AdrasteaI_CNFStatus_Success, NULL))
    {
        return false;
    }

    return true;
}

bool AdrasteaI_ATSocket_ReadSocketLastError(AdrasteaI_ATSocket_ID_t socketID, AdrasteaI_ATSocket_Error_Code_t* errorCodeP)
{
    if (errorCodeP == NULL)
    {
        return false;
    }

    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT%SOCKETCMD=\"LASTERROR\",");

    if (!ATCommand_AppendArgumentInt(pRequestCommand, socketID, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC), ATCOMMAND_STRING_TERMINATE))
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

    if (!AdrasteaI_WaitForConfirm(AdrasteaI_GetTimeout(AdrasteaI_Timeout_Socket), AdrasteaI_CNFStatus_Success, pResponseCommand))
    {
        return false;
    }

    if (!ATCommand_GetNextArgumentInt(&pResponseCommand, errorCodeP, ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }

    return true;
}

bool AdrasteaI_ATSocket_ReadSocketSSLInfo(AdrasteaI_ATSocket_ID_t socketID, AdrasteaI_ATSocket_SSL_Info_t* infoP)
{
    if (infoP == NULL)
    {
        return false;
    }

    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT%SOCKETCMD=\"SSLINFO\",");

    if (!ATCommand_AppendArgumentInt(pRequestCommand, socketID, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC), ATCOMMAND_STRING_TERMINATE))
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

    if (!AdrasteaI_WaitForConfirm(AdrasteaI_GetTimeout(AdrasteaI_Timeout_Socket), AdrasteaI_CNFStatus_Success, pResponseCommand))
    {
        return false;
    }

    if (!ATCommand_GetNextArgumentInt(&pResponseCommand, &infoP->SSLMode, ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_GetNextArgumentInt(&pResponseCommand, &infoP->profileID, ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }

    return true;
}

bool AdrasteaI_ATSocket_KeepSocketSSLSession(AdrasteaI_ATSocket_ID_t socketID)
{
    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT%SOCKETCMD=\"SSLKEEP\",");

    if (!ATCommand_AppendArgumentInt(pRequestCommand, socketID, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC), ATCOMMAND_STRING_TERMINATE))
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

    if (!AdrasteaI_WaitForConfirm(AdrasteaI_GetTimeout(AdrasteaI_Timeout_Socket), AdrasteaI_CNFStatus_Success, NULL))
    {
        return false;
    }

    return true;
}

bool AdrasteaI_ATSocket_DeleteSocketSSLSession(AdrasteaI_ATSocket_ID_t socketID)
{
    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT%SOCKETCMD=\"SSLDEL\",");

    if (!ATCommand_AppendArgumentInt(pRequestCommand, socketID, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC), ATCOMMAND_STRING_TERMINATE))
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

    if (!AdrasteaI_WaitForConfirm(AdrasteaI_GetTimeout(AdrasteaI_Timeout_Socket), AdrasteaI_CNFStatus_Success, NULL))
    {
        return false;
    }

    return true;
}

bool AdrasteaI_ATSocket_ReceiveFromSocket(AdrasteaI_ATSocket_ID_t socketID, AdrasteaI_ATSocket_Data_Read_t* dataReadP, uint16_t maxBufferLength)
{
    if (dataReadP == NULL)
    {
        return false;
    }

    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT%SOCKETDATA=\"RECEIVE\",");

    if (!ATCommand_AppendArgumentInt(pRequestCommand, socketID, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC), ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_AppendArgumentInt(pRequestCommand, maxBufferLength, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC), ATCOMMAND_STRING_TERMINATE))
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

    if (!AdrasteaI_WaitForConfirm(AdrasteaI_GetTimeout(AdrasteaI_Timeout_Socket), AdrasteaI_CNFStatus_Success, pResponseCommand))
    {
        return false;
    }

    if (!ATCommand_GetNextArgumentInt(&pResponseCommand, &dataReadP->socketID, ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_GetNextArgumentInt(&pResponseCommand, &dataReadP->dataLength, ATCOMMAND_INTFLAGS_SIZE16 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_GetNextArgumentInt(&pResponseCommand, &dataReadP->dataLeftLength, ATCOMMAND_INTFLAGS_SIZE16 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    switch (ATCommand_CountArgs(pResponseCommand))
    {
        case 1:
        {
            if (!ATCommand_GetNextArgumentStringWithoutQuotationMarks(&pResponseCommand, dataReadP->data, ATCOMMAND_STRING_TERMINATE, maxBufferLength))
            {
                return false;
            }

            strcpy(dataReadP->sourceIPAddress, "");

            dataReadP->sourcePortNumber = AdrasteaI_ATCommon_Port_Number_Invalid;

            break;
        }
        case 3:
        {
            if (!ATCommand_GetNextArgumentStringWithoutQuotationMarks(&pResponseCommand, dataReadP->data, ATCOMMAND_ARGUMENT_DELIM, maxBufferLength))
            {
                return false;
            }

            if (!ATCommand_GetNextArgumentStringWithoutQuotationMarks(&pResponseCommand, dataReadP->sourceIPAddress, ATCOMMAND_ARGUMENT_DELIM, sizeof(dataReadP->sourceIPAddress)))
            {
                return false;
            }

            if (!ATCommand_GetNextArgumentInt(&pResponseCommand, &dataReadP->sourcePortNumber, ATCOMMAND_INTFLAGS_SIZE16 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_STRING_TERMINATE))
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

bool AdrasteaI_ATSocket_SendToSocket(AdrasteaI_ATSocket_ID_t socketID, char* data, AdrasteaI_ATSocket_Data_Length_t dataLength)
{
    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT%SOCKETDATA=\"SEND\",");

    if (!ATCommand_AppendArgumentInt(pRequestCommand, socketID, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC), ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_AppendArgumentInt(pRequestCommand, dataLength, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC), ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_AppendArgumentStringQuotationMarks(pRequestCommand, data, ATCOMMAND_STRING_TERMINATE))
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

    if (!AdrasteaI_WaitForConfirm(AdrasteaI_GetTimeout(AdrasteaI_Timeout_Socket), AdrasteaI_CNFStatus_Success, NULL))
    {
        return false;
    }

    return true;
}

bool AdrasteaI_ATSocket_SetSocketUnsolicitedNotificationEvents(AdrasteaI_ATSocket_Event_t event, AdrasteaI_ATCommon_Event_State_t state)
{
    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT%SOCKETEV=");

    if (!ATCommand_AppendArgumentInt(pRequestCommand, event, (ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC), ATCOMMAND_ARGUMENT_DELIM))
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

    if (!AdrasteaI_WaitForConfirm(AdrasteaI_GetTimeout(AdrasteaI_Timeout_Socket), AdrasteaI_CNFStatus_Success, NULL))
    {
        return false;
    }

    return true;
}

bool AdrasteaI_ATSocket_ParseDataReceivedEvent(char* pEventArguments, AdrasteaI_ATSocket_ID_t* dataP)
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

bool AdrasteaI_ATSocket_ParseSocketTerminatedEvent(char* pEventArguments, AdrasteaI_ATSocket_ID_t* dataP)
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

bool AdrasteaI_ATSocket_ParseSocketsReadEvent(char* pEventArguments, AdrasteaI_ATSocket_Read_Result_t* dataP)
{
    if (dataP == NULL || pEventArguments == NULL)
    {
        return false;
    }

    char* argumentsP = pEventArguments;

    if (ATCommand_CountArgs(argumentsP) != 2)
    {
        return false;
    }

    if (!ATCommand_GetNextArgumentInt(&argumentsP, &dataP->socketID, ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_GetNextArgumentEnumWithoutQuotationMarks(&argumentsP, (uint8_t*)&dataP->socketState, AdrasteaI_ATSocket_State_Strings, AdrasteaI_ATSocket_State_NumberOfValues, 30, ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }

    return true;
}
