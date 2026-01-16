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
 * @file ATHTTP.h
 * @brief AT commands for HTTP client functionality.
 */

#ifndef CALYPSO_AT_HTTP_H_INCLUDED
#define CALYPSO_AT_HTTP_H_INCLUDED
#include <Calypso/ATCommands/ATSocket.h>
#include <Calypso/Calypso.h>
#include <global/ATCommands.h>
#include <stdbool.h>
#include <stdint.h>

#define Calypso_ATHTTP_RECEIVE_BUFFER_SIZE CALYPSO_RECEIVE_BUFFER_SIZE
#define Calypso_ATHTTP_RECEIVE_HEADER_SIZE CALYPSO_RECEIVE_BUFFER_SIZE

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief Connection flags. Used as parameter for Calypso_ATHTTP_Connect().
 */
typedef enum Calypso_ATHTTP_ConnectFlags_t
{
    Calypso_ATHTTP_ConnectFlags_None = 0,
    Calypso_ATHTTP_ConnectFlags_IgnoreProxy = (1 << 0),
    Calypso_ATHTTP_ConnectFlags_HostExist = (1 << 1),
    Calypso_ATHTTP_ConnectFlags_NumberOfValues = 2
} Calypso_ATHTTP_ConnectFlags_t;

/**
 * @brief HTTP methods. Used as parameter for Calypso_ATHTTP_SendRequest().
 */
typedef enum Calypso_ATHTTP_Method_t
{
    Calypso_ATHTTP_Method_Get,
    Calypso_ATHTTP_Method_Post,
    Calypso_ATHTTP_Method_Head,
    Calypso_ATHTTP_Method_Options,
    Calypso_ATHTTP_Method_Put,
    Calypso_ATHTTP_Method_Delete,
    Calypso_ATHTTP_Method_Connect,
    /** @cond DOXYGEN_IGNORE */
    Calypso_ATHTTP_Method_NumberOfValues
    /** @endcond */
} Calypso_ATHTTP_Method_t;

/**
 * @brief HTTP request flags. Used as parameter for Calypso_ATHTTP_SendRequest().
 */
typedef enum Calypso_ATHTTP_RequestFlags_t
{
    Calypso_ATHTTP_RequestFlags_None = 0,
    Calypso_ATHTTP_RequestFlags_ChunkStart = (1 << 0),
    Calypso_ATHTTP_RequestFlags_ChunkEnd = (1 << 1),
    Calypso_ATHTTP_RequestFlags_DropBody = (1 << 2),
    Calypso_ATHTTP_RequestFlags_NumberOfValues = 3
} Calypso_ATHTTP_RequestFlags_t;

/**
 * @brief HTTP header fields. Used as parameter for Calypso_ATHTTP_SetHeader() and Calypso_ATHTTP_GetHeader().
 */
typedef enum Calypso_ATHTTP_HeaderField_t
{
    Calypso_ATHTTP_HeaderField_ResponseAge,
    Calypso_ATHTTP_HeaderField_ResponseAllow,
    Calypso_ATHTTP_HeaderField_ResponseCacheControl,
    Calypso_ATHTTP_HeaderField_ResponseConnection,
    Calypso_ATHTTP_HeaderField_ResponseContentEncoding,
    Calypso_ATHTTP_HeaderField_ResponseContentLanguage,
    Calypso_ATHTTP_HeaderField_ResponseContentLength,
    Calypso_ATHTTP_HeaderField_ResponseContentLocation,
    Calypso_ATHTTP_HeaderField_ResponseContentRange,
    Calypso_ATHTTP_HeaderField_ResponseContentType,
    Calypso_ATHTTP_HeaderField_ResponseDate,
    Calypso_ATHTTP_HeaderField_ResponseEtag,
    Calypso_ATHTTP_HeaderField_ResponseExpires,
    Calypso_ATHTTP_HeaderField_ResponseLastModified,
    Calypso_ATHTTP_HeaderField_ResponseLocation,
    Calypso_ATHTTP_HeaderField_ResponseProxyAuth,
    Calypso_ATHTTP_HeaderField_ResponseRetryAfter,
    Calypso_ATHTTP_HeaderField_ResponseServer,
    Calypso_ATHTTP_HeaderField_ResponseSetCookie,
    Calypso_ATHTTP_HeaderField_ResponseTrailer,
    Calypso_ATHTTP_HeaderField_ResponseTxEncoding,
    Calypso_ATHTTP_HeaderField_ResponseUpgrade,
    Calypso_ATHTTP_HeaderField_ResponseVary,
    Calypso_ATHTTP_HeaderField_ResponseVia,
    Calypso_ATHTTP_HeaderField_ResponseWwwAuth,
    Calypso_ATHTTP_HeaderField_ResponseWarning,
    Calypso_ATHTTP_HeaderField_RequestAccept,
    Calypso_ATHTTP_HeaderField_RequestAcceptCharset,
    Calypso_ATHTTP_HeaderField_RequestAcceptEncoding,
    Calypso_ATHTTP_HeaderField_RequestAcceptLanguage,
    Calypso_ATHTTP_HeaderField_RequestAllow,
    Calypso_ATHTTP_HeaderField_RequestAuth,
    Calypso_ATHTTP_HeaderField_RequestCacheControl,
    Calypso_ATHTTP_HeaderField_RequestConnection,
    Calypso_ATHTTP_HeaderField_RequestContentEncoding,
    Calypso_ATHTTP_HeaderField_RequestContentLanguage,
    Calypso_ATHTTP_HeaderField_RequestContentLocation,
    Calypso_ATHTTP_HeaderField_RequestContentType,
    Calypso_ATHTTP_HeaderField_RequestCookie,
    Calypso_ATHTTP_HeaderField_RequestDate,
    Calypso_ATHTTP_HeaderField_RequestExpect,
    Calypso_ATHTTP_HeaderField_RequestForwarded,
    Calypso_ATHTTP_HeaderField_RequestFrom,
    Calypso_ATHTTP_HeaderField_RequestHost,
    Calypso_ATHTTP_HeaderField_RequestIfMatch,
    Calypso_ATHTTP_HeaderField_RequestIfModifiedSince,
    Calypso_ATHTTP_HeaderField_RequestIfNoneMatch,
    Calypso_ATHTTP_HeaderField_RequestIfRange,
    Calypso_ATHTTP_HeaderField_RequestIfUnmodifiedSince,
    Calypso_ATHTTP_HeaderField_RequestOrigin,
    Calypso_ATHTTP_HeaderField_RequestProxyAuth,
    Calypso_ATHTTP_HeaderField_RequestRange,
    Calypso_ATHTTP_HeaderField_RequestTe,
    Calypso_ATHTTP_HeaderField_RequestTxEncoding,
    Calypso_ATHTTP_HeaderField_RequestUpgrade,
    Calypso_ATHTTP_HeaderField_RequestUserAgent,
    Calypso_ATHTTP_HeaderField_RequestVia,
    Calypso_ATHTTP_HeaderField_RequestWarning,
    /** @cond DOXYGEN_IGNORE */
    Calypso_ATHTTP_HeaderField_NumberOfValues
    /** @endcond */
} Calypso_ATHTTP_HeaderField_t;

/**
 * @brief HTTP header persistency parameter. Used as parameter for Calypso_ATHTTP_SetHeader().
 */
typedef enum Calypso_ATHTTP_HeaderPersistency_t
{
    Calypso_ATHTTP_HeaderPersistency_NotPersistent,
    Calypso_ATHTTP_HeaderPersistency_Persistent,
    /** @cond DOXYGEN_IGNORE */
    Calypso_ATHTTP_HeaderPersistency_NumberOfValues
    /** @endcond */
} Calypso_ATHTTP_HeaderPersistency_t;

/**
 * @brief Stores HTTP response data.
 *
 * @see Calypso_ATHTTP_ReadResponseBody()
 */
typedef struct Calypso_ATHTTP_ResponseBody_t
{
    /**
     * @brief HTTP client handle.
     */
    uint8_t clientHandle;

    /**
     * @brief Indicates if there is more data to be fetched (response body).
     */
    bool hasMoreData;

    /**
     * @brief Format of the response body data.
     */
    Calypso_DataFormat_t format;

    /**
     * @brief Number of bytes in response body buffer.
     */
    uint16_t length;

    /**
     * @brief Response body.
     * Note that the response might be split into chunks. The attribute hasMoreData is
     * set to true, if the body data is fragmented and there is more data to be fetched.
     */
    char body[Calypso_ATHTTP_RECEIVE_BUFFER_SIZE];
} Calypso_ATHTTP_ResponseBody_t;

/**
 * @brief Stores HTTP header data.
 *
 * @see Calypso_ATHTTP_GetHeader()
 */
typedef struct Calypso_ATHTTP_HeaderData_t
{
    /**
     * @brief HTTP client handle.
     */
    uint8_t clientHandle;

    /**
     * @brief Format of the header data.
     */
    Calypso_DataFormat_t format;

    /**
     * @brief Number of bytes in header data buffer.
     */
    uint16_t length;

    /**
     * @brief Header data.
     */
    char data[Calypso_ATHTTP_RECEIVE_HEADER_SIZE];
} Calypso_ATHTTP_HeaderData_t;

/**
 * @brief Creates an HTTP client.
 *
 * @param[out] clientHandle: Handle (index) of the created HTTP client
 *
 * @return True if successful, false otherwise
 */
extern bool Calypso_ATHTTP_Create(uint8_t* clientHandle);

/**
 * @brief Deletes an HTTP client.
 *
 * @param[in] clientHandle: Handle of the HTTP client to be deleted
 *
 * @return True if successful, false otherwise
 */
extern bool Calypso_ATHTTP_Destroy(uint8_t clientHandle);

/**
 * @brief Establishes an HTTP connection.
 *
 * @param[in] clientHandle: Handle of the HTTP client to connect
 * @param[in] host: Name of host to connect to
 * @param[in] flags: Connection flags. See Calypso_ATHTTP_ConnectFlag_t.
 * @param[in] privateKey: Private key file name (optional, set to NULL if not used).
 * @param[in] certificate: Client certificate file name (optional, set to NULL if not used).
 * @param[in] rootCaCertificate: Root CA certificate file name (optional, set to NULL if not used).
 *
 * @return True if successful, false otherwise
 */
extern bool Calypso_ATHTTP_Connect(uint8_t clientHandle, const char* host, uint8_t flags, const char* privateKey, const char* certificate, const char* rootCaCertificate);

/**
 * @brief Closes an HTTP connection.
 *
 * @param[in] clientHandle: Handle of the HTTP client to disconnect
 *
 * @return True if successful, false otherwise
 */
extern bool Calypso_ATHTTP_Disconnect(uint8_t clientHandle);

/**
 * @brief Sets a proxy server for HTTP communication.
 *
 * @param[in] proxy: Proxy server info
 *
 * @return True if successful, false otherwise
 */
extern bool Calypso_ATHTTP_SetProxy(Calypso_ATSocket_Descriptor_t proxy);

/**
 * @brief Sends an HTTP request.
 *
 * @param[in] clientHandle: Handle of the HTTP client to be used for sending the request
 * @param[in] method: HTTP method to be used
 * @param[in] uri: HTTP server address or URL
 * @param[in] flags: HTTP Request flags (see Calypso_ATHTTP_RequestFlags_t)
 * @param[in] format: Format in which the request's data is provided. Note that setting the format to Calypso_DataFormat_Base64
 *                   causes the Calypso module to interpret the data as Base64 - the module will decode the data and
 *                   transmit the decoded data. If using Calypso_DataFormat_Base64, you either need to provide Base64 encoded
 *                   data or set encodeAsBase64 to true.
 * @param[in] encodeAsBase64: Encode the data in Base64 format before sending it to the Calypso module
 * @param[in] length: Number of bytes to send as payload of the request (see argument data)
 * @param[in] data: Payload of the request
 * @param[out] status: HTTP status code (usually 200 in case of success, else failure)
 *
 * @return True if successful, false otherwise
 */
extern bool Calypso_ATHTTP_SendRequest(uint8_t clientHandle, Calypso_ATHTTP_Method_t method, const char* uri, uint8_t flags, Calypso_DataFormat_t format, bool encodeAsBase64, uint16_t length, const char* data, uint32_t* status);

/**
 * @brief Fetches the response to the last HTTP request from the Calypso module.
 *
 * @param[in] clientHandle: HTTP client handle
 * @param[in] format: Format in which the data is to be provided. Setting the format to Calypso_DataFormat_Base64
 *                   causes the Calypso module to encode the data as Base64 before sending it to this device via UART.
 * @param[in] decodeBase64: Enables decoding of received Base64 data
 * @param[in] length: Number of bytes to fetch
 * @param[out] responseBody: The returned HTTP response data. The payload of the response is contained in the body attribute
 *                          of responseBody. Note that, depending on the length argument, the response might be split into
 *                          chunks. The hasMoreData attribute of responseBody is set to true, if the body data is fragmented
 *                          and there is more data to be fetched.
 *
 * @return True if successful, false otherwise
 */
extern bool Calypso_ATHTTP_ReadResponseBody(uint8_t clientHandle, Calypso_DataFormat_t format, bool decodeBase64, uint16_t length, Calypso_ATHTTP_ResponseBody_t* responseBody);

/**
 * @brief Sets HTTP header data for the next HTTP request(s).
 *
 * When used with a request field (prefix "Calypso_ATHTTP_HeaderField_Request"), the field's value is set.
 * When used with a response field (prefix "Calypso_ATHTTP_HeaderField_Response"), a response filter is set for the field.
 *
 * @param[in] clientHandle: HTTP client handle
 * @param[in] field: Header field to be set
 * @param[in] persistency: Specifies if the supplied header field value is persistent or not persistent
 * @param[in] format: Format in which the header field value is provided. Note that setting the format to Calypso_DataFormat_Base64
 *                   causes the Calypso module to interpret the data as Base64 - the module will decode the data and
 *                   transmit the decoded data. If using Calypso_DataFormat_Base64, you either need to provide Base64 encoded
 *                   data or set encodeAsBase64 to true.
 * @param[in] encodeAsBase64: Encode the data in Base64 format before sending it to the Calypso module
 * @param[in] length: Number of bytes in header field value. Set to zero to remove a request header.
 * @param[in] data: Header field value
 *
 * @return True if successful, false otherwise
 */
extern bool Calypso_ATHTTP_SetHeader(uint8_t clientHandle, Calypso_ATHTTP_HeaderField_t field, Calypso_ATHTTP_HeaderPersistency_t persistency, Calypso_DataFormat_t format, bool encodeAsBase64, uint16_t length, const char* data);

/**
 * @brief Gets HTTP header data.
 *
 * @param[in] clientHandle: HTTP client handle
 * @param[in] field: Header field to be fetched
 * @param[in] format: Format in which the header data is to be provided. Setting the format to Calypso_DataFormat_Base64
 *                   causes the Calypso module to encode the data as Base64 before sending it to this device via UART.
 * @param[in] decodeBase64: Enables decoding of received Base64 data
 * @param[in] length: Number of bytes to fetch
 * @param[out] header: The returned header data
 *
 * @return True if successful, false otherwise
 */
extern bool Calypso_ATHTTP_GetHeader(uint8_t clientHandle, Calypso_ATHTTP_HeaderField_t field, Calypso_DataFormat_t format, bool decodeBase64, uint16_t length, Calypso_ATHTTP_HeaderData_t* header);

/**
 * @brief Sends a custom HTTP response in reply to a client's HTTP GET request.
 *
 * @param[in] format: Format in which the response data is provided. Note that setting the format to Calypso_DataFormat_Base64
 *                   causes the Calypso module to interpret the data as Base64 - the module will decode the data and
 *                   transmit the decoded data. If using Calypso_DataFormat_Base64, you either need to provide Base64 encoded
 *                   data or set encodeAsBase64 to true.
 * @param[in] encodeAsBase64: Encode the data in Base64 format before sending it to the Calypso module
 * @param[in] length: Number of bytes in response data.
 * @param[in] data: Response content
 *
 * @return True if successful, false otherwise
 */
extern bool Calypso_ATHTTP_SendCustomResponse(Calypso_DataFormat_t format, bool encodeAsBase64, uint16_t length, const char* data);

#ifdef __cplusplus
}
#endif

#endif /* CALYPSO_AT_HTTP_H_INCLUDED */
