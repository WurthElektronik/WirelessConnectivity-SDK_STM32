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
 * @file ATHTTP.c
 * @brief AT commands for HTTP client functionality.
 */
#include <Calypso/ATCommands/ATHTTP.h>
#include <Calypso/Calypso.h>
#include <global/ATCommands.h>

static const char* Calypso_ATHTTP_ConnectFlags_Strings[Calypso_ATHTTP_ConnectFlags_NumberOfValues] = {"ignore_proxy", "host_exist"};

static const char* Calypso_ATHTTP_Method_Strings[Calypso_ATHTTP_Method_NumberOfValues] = {"get", "post", "head", "options", "put", "del", "connect"};

static const char* Calypso_ATHTTP_RequestFlags_Strings[Calypso_ATHTTP_RequestFlags_NumberOfValues] = {"chunk_start", "chunk_end", "drop_body"};

static const char* Calypso_ATHTTP_HeaderField_Strings[Calypso_ATHTTP_HeaderField_NumberOfValues] = {"res_age",
                                                                                                    "res_allow",
                                                                                                    "res_cache_control",
                                                                                                    "res_connection",
                                                                                                    "res_content_encoding",
                                                                                                    "res_content_language",
                                                                                                    "res_content_length",
                                                                                                    "res_content_location",
                                                                                                    "res_content_range",
                                                                                                    "res_content_type",
                                                                                                    "res_date",
                                                                                                    "res_etag",
                                                                                                    "res_expires",
                                                                                                    "res_last_modified",
                                                                                                    "res_location",
                                                                                                    "res_proxy_auth",
                                                                                                    "res_retry_after",
                                                                                                    "res_server",
                                                                                                    "res_set_cookie",
                                                                                                    "res_trailer",
                                                                                                    "res_tx_encoding",
                                                                                                    "res_upgrade",
                                                                                                    "res_vary",
                                                                                                    "res_via",
                                                                                                    "res_www_auth",
                                                                                                    "res_warning",
                                                                                                    "req_accept",
                                                                                                    "req_accept_charset",
                                                                                                    "req_accept_encoding",
                                                                                                    "req_accept_language",
                                                                                                    "req_allow",
                                                                                                    "req_auth",
                                                                                                    "req_cache_control",
                                                                                                    "req_connection",
                                                                                                    "req_content_encoding",
                                                                                                    "req_content_language",
                                                                                                    "req_content_location",
                                                                                                    "req_content_type",
                                                                                                    "req_cookie",
                                                                                                    "req_date",
                                                                                                    "req_expect",
                                                                                                    "req_forwarded",
                                                                                                    "req_from",
                                                                                                    "req_host",
                                                                                                    "req_if_match",
                                                                                                    "req_if_modified_since",
                                                                                                    "req_if_none_match",
                                                                                                    "req_if_range",
                                                                                                    "req_if_unmodified_since",
                                                                                                    "req_origin",
                                                                                                    "req_proxy_auth",
                                                                                                    "req_range",
                                                                                                    "req_te",
                                                                                                    "req_tx_encoding",
                                                                                                    "req_upgrade",
                                                                                                    "req_user_agent",
                                                                                                    "req_via",
                                                                                                    "req_warning"};

static const char* Calypso_ATHTTP_HeaderPersistency_Strings[Calypso_ATHTTP_HeaderPersistency_NumberOfValues] = {"not_persistent", "persistent"};

bool Calypso_ATHTTP_Create(uint8_t* clientHandle)
{
    if (!Calypso_SendRequest("AT+httpCreate\r\n"))
    {
        return false;
    }

    char* pRespondCommand = AT_commandBuffer;
    if (!Calypso_WaitForConfirm(Calypso_GetTimeout(Calypso_Timeout_General), Calypso_CNFStatus_Success, pRespondCommand))
    {
        return false;
    }

    const char* cmd = "+httpcreate:";
    const size_t cmdLength = strlen(cmd);

    if (0 != strncmp(pRespondCommand, cmd, cmdLength))
    {
        return false;
    }

    pRespondCommand += cmdLength;

    return ATCommand_GetNextArgumentInt(&pRespondCommand, clientHandle, ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC, ATCOMMAND_STRING_TERMINATE);
}

bool Calypso_ATHTTP_Destroy(uint8_t clientHandle)
{
    char* pRequestCommand = AT_commandBuffer;
    strcpy(pRequestCommand, "AT+httpDestroy=");
    if (!ATCommand_AppendArgumentInt(pRequestCommand, clientHandle, ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC, ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentString(pRequestCommand, ATCOMMAND_CRLF, ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }
    if (!Calypso_SendRequest(pRequestCommand))
    {
        return false;
    }
    return Calypso_WaitForConfirm(Calypso_GetTimeout(Calypso_Timeout_General), Calypso_CNFStatus_Success, NULL);
}

bool Calypso_ATHTTP_Connect(uint8_t clientHandle, const char* host, uint8_t flags, const char* privateKey, const char* certificate, const char* rootCaCertificate)
{
    char* pRequestCommand = AT_commandBuffer;
    strcpy(pRequestCommand, "AT+httpConnect=");

    if (!ATCommand_AppendArgumentInt(pRequestCommand, clientHandle, ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentString(pRequestCommand, host, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentBitmask(pRequestCommand, Calypso_ATHTTP_ConnectFlags_Strings, Calypso_ATHTTP_ConnectFlags_NumberOfValues, flags, ATCOMMAND_ARGUMENT_DELIM, AT_MAX_COMMAND_BUFFER_SIZE))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentString(pRequestCommand, privateKey, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentString(pRequestCommand, certificate, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentString(pRequestCommand, rootCaCertificate, ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentString(pRequestCommand, ATCOMMAND_CRLF, ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }

    if (!Calypso_SendRequest(pRequestCommand))
    {
        return false;
    }
    return Calypso_WaitForConfirm(Calypso_GetTimeout(Calypso_Timeout_HttpConnect), Calypso_CNFStatus_Success, NULL);
}

bool Calypso_ATHTTP_Disconnect(uint8_t clientHandle)
{
    char* pRequestCommand = AT_commandBuffer;
    strcpy(pRequestCommand, "AT+httpDisconnect=");
    if (!ATCommand_AppendArgumentInt(pRequestCommand, clientHandle, ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC, ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentString(pRequestCommand, ATCOMMAND_CRLF, ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }
    if (!Calypso_SendRequest(pRequestCommand))
    {
        return false;
    }
    return Calypso_WaitForConfirm(Calypso_GetTimeout(Calypso_Timeout_General), Calypso_CNFStatus_Success, NULL);
}

bool Calypso_ATHTTP_SetProxy(Calypso_ATSocket_Descriptor_t proxy)
{
    char* pRequestCommand = AT_commandBuffer;
    strcpy(pRequestCommand, "AT+httpSetProxy=");
    if (!Calypso_ATSocket_AppendSocketDescriptor(pRequestCommand, proxy, ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentString(pRequestCommand, ATCOMMAND_CRLF, ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }
    if (!Calypso_SendRequest(pRequestCommand))
    {
        return false;
    }
    return Calypso_WaitForConfirm(Calypso_GetTimeout(Calypso_Timeout_General), Calypso_CNFStatus_Success, NULL);
}

bool Calypso_ATHTTP_SendRequest(uint8_t clientHandle, Calypso_ATHTTP_Method_t method, const char* uri, uint8_t flags, Calypso_DataFormat_t format, bool encodeAsBase64, uint16_t length, const char* data, uint32_t* status)
{
    *status = 0;

    if (Calypso_ATHTTP_Method_Head == method)
    {
        /* HEAD request is always without body. Must use this flag, otherwise the
         * next call to Calypso_ATHTTP_ReadResponseBody will cause the module to hang. */
        flags |= Calypso_ATHTTP_RequestFlags_DropBody;
    }

    if (encodeAsBase64)
    {
        /* Encode as Base64 */
        uint32_t lengthEncoded;
        if (!Base64_GetEncBufSize(length, &lengthEncoded))
        {
            return false;
        }
        char base64Buffer[lengthEncoded];
        if (!Base64_Encode((uint8_t*)data, length, (uint8_t*)base64Buffer, &lengthEncoded))
        {
            return false;
        }

        /* Recursively call Calypso_ATHTTP_SendRequest() with the encoded binary data (excluding '\0') */
        return Calypso_ATHTTP_SendRequest(clientHandle, method, uri, flags, format, false, lengthEncoded, base64Buffer, status);
    }

    char* pRequestCommand = AT_commandBuffer;
    char* pRespondCommand = AT_commandBuffer;
    strcpy(pRequestCommand, "AT+httpSendReq=");
    if (!ATCommand_AppendArgumentInt(pRequestCommand, clientHandle, ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentString(pRequestCommand, Calypso_ATHTTP_Method_Strings[method], ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentString(pRequestCommand, uri, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentBitmask(pRequestCommand, Calypso_ATHTTP_RequestFlags_Strings, Calypso_ATHTTP_RequestFlags_NumberOfValues, flags, ATCOMMAND_ARGUMENT_DELIM, AT_MAX_COMMAND_BUFFER_SIZE))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentInt(pRequestCommand, format, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentInt(pRequestCommand, length, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentBytes(pRequestCommand, data, length, ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentString(pRequestCommand, ATCOMMAND_CRLF, ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }
    if (!Calypso_SendRequest(pRequestCommand))
    {
        return false;
    }
    if (!Calypso_WaitForConfirm(Calypso_GetTimeout(Calypso_Timeout_HttpRequest), Calypso_CNFStatus_Success, pRespondCommand))
    {
        return false;
    }

    const char* cmd = "+httpsendreq:";
    const size_t cmdLength = strlen(cmd);

    if (0 != strncmp(pRespondCommand, cmd, cmdLength))
    {
        return false;
    }

    pRespondCommand += cmdLength;

    return ATCommand_GetNextArgumentInt(&pRespondCommand, status, ATCOMMAND_INTFLAGS_SIZE32 | ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC, ATCOMMAND_STRING_TERMINATE);
}

bool Calypso_ATHTTP_ReadResponseBody(uint8_t clientHandle, Calypso_DataFormat_t format, bool decodeBase64, uint16_t length, Calypso_ATHTTP_ResponseBody_t* responseBody)
{
    responseBody->clientHandle = 0;
    responseBody->format = Calypso_DataFormat_Binary;
    responseBody->hasMoreData = false;
    responseBody->length = 0;

    char* pRequestCommand = AT_commandBuffer;
    char* pRespondCommand = AT_commandBuffer;
    strcpy(pRequestCommand, "AT+httpReadResBody=");
    if (!ATCommand_AppendArgumentInt(pRequestCommand, clientHandle, ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentInt(pRequestCommand, format, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentInt(pRequestCommand, length, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentString(pRequestCommand, ATCOMMAND_CRLF, ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }
    if (!Calypso_SendRequest(pRequestCommand))
    {
        return false;
    }
    if (!Calypso_WaitForConfirm(Calypso_GetTimeout(Calypso_Timeout_HttpRequest), Calypso_CNFStatus_Success, pRespondCommand))
    {
        return false;
    }

    const char* cmd = "+httpreadresbody:";
    const size_t cmdLength = strlen(cmd);

    if (0 != strncmp(pRespondCommand, cmd, cmdLength))
    {
        return false;
    }

    pRespondCommand += cmdLength;

    if (!ATCommand_GetNextArgumentInt(&pRespondCommand, &responseBody->clientHandle, ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_GetNextArgumentInt(&pRespondCommand, &responseBody->hasMoreData, ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_GetNextArgumentInt(&pRespondCommand, &responseBody->format, ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_GetNextArgumentInt(&pRespondCommand, &responseBody->length, ATCOMMAND_INTFLAGS_SIZE16 | ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (responseBody->length > 0)
    {
        if (decodeBase64)
        {
            uint32_t decodedSize;
            if (!Base64_GetDecBufSize((uint8_t*)pRespondCommand, responseBody->length, &decodedSize))
            {
                return false;
            }

            /* we are expecting only 'length' of decoded data */
            if (decodedSize > length)
            {
                return false;
            }

            uint32_t encodedSize = responseBody->length;
            decodedSize = sizeof(responseBody->body) - 1 /*'\0'*/;
            if (false == Base64_Decode((uint8_t*)pRespondCommand, encodedSize, (uint8_t*)responseBody->body, &decodedSize))
            {
                return false;
            }
            /* add string termination character needed by the Calypso functions */
            responseBody->body[decodedSize] = '\0';

            responseBody->length = decodedSize;
            return true;
        }
        else
        {
            if (responseBody->length > length)
            {
                return false;
            }

            return ATCommand_GetNextArgumentString(&pRespondCommand, responseBody->body, ATCOMMAND_STRING_TERMINATE, sizeof(responseBody->body));
        }
    }

    responseBody->body[0] = '\0';
    return true;
}

bool Calypso_ATHTTP_SetHeader(uint8_t clientHandle, Calypso_ATHTTP_HeaderField_t field, Calypso_ATHTTP_HeaderPersistency_t persistency, Calypso_DataFormat_t format, bool encodeAsBase64, uint16_t length, const char* data)
{
    if (encodeAsBase64)
    {
        /* Encode as Base64 */
        uint32_t lengthEncoded;
        if (!Base64_GetEncBufSize(length, &lengthEncoded))
        {
            return false;
        }
        char base64Buffer[lengthEncoded];
        if (!Base64_Encode((uint8_t*)data, length, (uint8_t*)base64Buffer, &lengthEncoded))
        {
            return false;
        }

        /* Recursively call Calypso_ATHTTP_SendRequest() with the encoded binary data (excluding '\0') */
        return Calypso_ATHTTP_SetHeader(clientHandle, field, persistency, format, false, lengthEncoded, base64Buffer);
    }

    char* pRequestCommand = AT_commandBuffer;
    strcpy(pRequestCommand, "AT+httpSetHeader=");
    if (!ATCommand_AppendArgumentInt(pRequestCommand, clientHandle, ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentString(pRequestCommand, Calypso_ATHTTP_HeaderField_Strings[field], ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentString(pRequestCommand, Calypso_ATHTTP_HeaderPersistency_Strings[persistency], ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentInt(pRequestCommand, format, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentInt(pRequestCommand, length, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentBytes(pRequestCommand, data, length, ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentString(pRequestCommand, ATCOMMAND_CRLF, ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }
    if (!Calypso_SendRequest(pRequestCommand))
    {
        return false;
    }
    return Calypso_WaitForConfirm(Calypso_GetTimeout(Calypso_Timeout_General), Calypso_CNFStatus_Success, NULL);
}

bool Calypso_ATHTTP_GetHeader(uint8_t clientHandle, Calypso_ATHTTP_HeaderField_t field, Calypso_DataFormat_t format, bool decodeBase64, uint16_t length, Calypso_ATHTTP_HeaderData_t* header)
{
    char* pRequestCommand = AT_commandBuffer;
    char* pRespondCommand = AT_commandBuffer;
    strcpy(pRequestCommand, "AT+httpGetHeader=");
    if (!ATCommand_AppendArgumentInt(pRequestCommand, clientHandle, ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentString(pRequestCommand, Calypso_ATHTTP_HeaderField_Strings[field], ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentInt(pRequestCommand, format, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentInt(pRequestCommand, length, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentString(pRequestCommand, ATCOMMAND_CRLF, ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }
    if (!Calypso_SendRequest(pRequestCommand))
    {
        return false;
    }
    if (!Calypso_WaitForConfirm(Calypso_GetTimeout(Calypso_Timeout_HttpRequest), Calypso_CNFStatus_Success, pRespondCommand))
    {
        return false;
    }

    const char* cmd = "+httpgetheader:";
    const size_t cmdLength = strlen(cmd);

    if (0 != strncmp(pRespondCommand, cmd, cmdLength))
    {
        return false;
    }

    pRespondCommand += cmdLength;

    if (!ATCommand_GetNextArgumentInt(&pRespondCommand, &header->clientHandle, ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_GetNextArgumentInt(&pRespondCommand, &header->format, ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_GetNextArgumentInt(&pRespondCommand, &header->length, ATCOMMAND_INTFLAGS_SIZE16 | ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (header->length > 0)
    {
        if (decodeBase64)
        {
            uint32_t decodedSize;
            if (!Base64_GetDecBufSize((uint8_t*)pRespondCommand, header->length, &decodedSize))
            {
                return false;
            }

            /* we are expecting only 'length' of decoded data */
            if (decodedSize > length)
            {
                return false;
            }

            uint32_t encodedSize = header->length;
            decodedSize = sizeof(header->data) - 1 /*'\0'*/;
            if (false == Base64_Decode((uint8_t*)pRespondCommand, encodedSize, (uint8_t*)header->data, &decodedSize))
            {
                return false;
            }
            /* add string termination character needed by the Calypso functions */
            header->data[decodedSize] = '\0';

            header->length = decodedSize;
            return true;
        }
        else
        {
            if (header->length > length)
            {
                return false;
            }

            return ATCommand_GetNextArgumentString(&pRespondCommand, header->data, ATCOMMAND_STRING_TERMINATE, sizeof(header->data));
        }
    }

    header->data[0] = '\0';
    return true;
}

bool Calypso_ATHTTP_SendCustomResponse(Calypso_DataFormat_t format, bool encodeAsBase64, uint16_t length, const char* data)
{
    if (encodeAsBase64)
    {
        /* Encode as Base64 */
        uint32_t lengthEncoded;
        if (!Base64_GetEncBufSize(length, &lengthEncoded))
        {
            return false;
        }
        char base64Buffer[lengthEncoded];
        if (!Base64_Encode((uint8_t*)data, length, (uint8_t*)base64Buffer, &lengthEncoded))
        {
            return false;
        }

        /* Recursively call Calypso_ATHTTP_SendCustomResponse() with the encoded binary data (excluding '\0') */
        return Calypso_ATHTTP_SendCustomResponse(format, false, lengthEncoded, base64Buffer);
    }

    char* pRequestCommand = AT_commandBuffer;
    strcpy(pRequestCommand, "AT+httpCustomResponse=");
    if (!ATCommand_AppendArgumentInt(pRequestCommand, format, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentInt(pRequestCommand, length, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentBytes(pRequestCommand, data, length, ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentString(pRequestCommand, ATCOMMAND_CRLF, ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }
    if (!Calypso_SendRequest(pRequestCommand))
    {
        return false;
    }
    return Calypso_WaitForConfirm(Calypso_GetTimeout(Calypso_Timeout_General), Calypso_CNFStatus_Success, NULL);
}
