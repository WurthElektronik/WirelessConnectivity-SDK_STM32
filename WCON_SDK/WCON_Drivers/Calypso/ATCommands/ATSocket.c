/*
 ***************************************************************************************************
 * This file is part of WIRELESS CONNECTIVITY SDK for STM32:
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
 * COPYRIGHT (c) 2022 Würth Elektronik eiSos GmbH & Co. KG
 *
 ***************************************************************************************************
 */

/**
 * @file
 * @brief AT commands for socket functionality.
 */

#include "ATSocket.h"

#include "../Calypso.h"

static const char *ATSocketFamilyString[ATSocket_Family_NumberOfValues] =
{
    "INET",
    "INET6"
};

static const char *ATSocketTypeString[ATSocket_Type_NumberOfValues] =
{
    "STREAM",
    "DGRAM"
};

static const char *ATSocketProcotolString[ATSocket_Protocol_NumberOfValues] =
{
    "TCP",
    "UDP",
    "SEC"
};

static const char *ATSocketSockOptLevelString[ATSocket_SockOptLevel_NumberOfValues] =
{
    "SOCKET",
    "IP"
};

static const char *ATSocketSockOptSocketString[ATSocket_SockOptSocket_NumberOfValues] =
{
    "KEEPALIVE",
    "KEEPALIVETIME",
    "RX_NO_IP_BOUNDARY",
    "RCVTIMEO",
    "RCVBUF",
    "NONBLOCKING",
    "SECMETHOD",
    "SECURE_MASK",
    "SECURE_FILES_CA_FILE_NAME",
    "SECURE_FILES_PRIVATE_KEY_FILE_NAME",
    "SECURE_FILES_CERTIFICATE_FILE_NAME",
    "SECURE_FILES_DH_KEY_FILE_NAME",
    "SECURE_DOMAIN_NAME_VERIFICATION",
    "DISABLE_CERTIFICATE_STORE"
};

static const char *ATSocketSockOptIPString[ATSocket_SockOptIP_NumberOfValues] =
{
    "MULTICAST_TTL",
    "ADD_MEMBERSHIP",
    "DROP_MEMBERSHIP"
};

static const char *AtSocketSockOptSecMethod[ATSocket_SockOptSecMethod_NumberOfValues] =
{
    "SSLV3",
    "TLSV1",
    "TLSV1_1",
    "TLSV1_2",
    "SSLV3_TLSV1_2"
};

static const char *ATSocket_CipherStrings[ATSocket_Cipher_NumberOfValues] =
{
    "SSL_RSA_WITH_RC4_128_SHA",
    "SSL_RSA_WITH_RC4_128_MD5",
    "TLS_RSA_WITH_AES_256_CBC_SHA",
    "TLS_DHE_RSA_WITH_AES_256_CBC_SHA",
    "TLS_ECDHE_RSA_WITH_AES_256_CBC_SHA",
    "TLS_ECDHE_RSA_WITH_RC4_128_SHA",
    "TLS_RSA_WITH_AES_128_CBC_SHA256",
    "TLS_RSA_WITH_AES_256_CBC_SHA256",
    "TLS_ECDHE_ECDSA_WITH_AES_128_CBC_SHA256",
    "TLS_ECDHE_ECDSA_WITH_AES_128_CBC_SHA",
    "TLS_ECDHE_ECDSA_WITH_AES_256_CBC_SHA",
    "TLS_RSA_WITH_AES_128_GCM_SHA256",
    "TLS_RSA_WITH_AES_256_GCM_SHA384",
    "TLS_DHE_RSA_WITH_AES_128_GCM_SHA256",
    "TLS_DHE_RSA_WITH_AES_256_GCM_SHA384",
    "TLS_ECDHE_RSA_WITH_AES_128_GCM_SHA256",
    "TLS_ECDHE_RSA_WITH_AES_256_GCM_SHA384",
    "TLS_ECDHE_ECDSA_WITH_AES_128_GCM_SHA256",
    "TLS_ECDHE_ECDSA_WITH_AES_256_GCM_SHA384",
    "TLS_ECDHE_ECDSA_WITH_CHACHA20_POLY1305_SHA256",
    "TLS_ECDHE_RSA_WITH_CHACHA20_POLY1305_SHA256",
    "TLS_DHE_RSA_WITH_CHACHA20_POLY1305_SHA256"
};

static bool ATSocket_AddArgumentsCreate(char *pAtCommand,
                                        ATSocket_Family_t family,
                                        ATSocket_Type_t type,
                                        ATSocket_Protocol_t protocol);
static bool ATSocket_AddArgumentsBindConnect(char *pAtCommand, uint8_t socketID, ATSocket_Descriptor_t socket);
static bool ATSocket_AddArgumentsRecv(char *pAtCommand, uint8_t socketID, Calypso_DataFormat_t format, uint16_t length);
static bool ATSocket_AddArgumentsRecvFrom(char *pAtCommand,
                                          uint8_t socketID,
                                          ATSocket_Descriptor_t socketFrom,
                                          Calypso_DataFormat_t format,
                                          uint16_t length);
static bool ATSocket_AddArgumentsSendTo(char *pAtCommand,
                                        uint8_t socketID,
                                        ATSocket_Descriptor_t *remoteSocket,
                                        Calypso_DataFormat_t format,
                                        uint16_t length,
                                        char *data);
static bool ATSocket_AddArgumentsSetSockOpt(char *pAtCommand,
                                            uint8_t socketID,
                                            ATSocket_SockOptLevel_t level,
                                            uint8_t option,
                                            ATSocket_Options_t *data);


static bool ATSocket_ParseResponseCreate(char **pAtCommand, uint8_t *pOutSocketID);
static bool ATSocket_ParseResponseGetOptions(ATSocket_SockOptLevel_t level,
                                             uint8_t option,
                                             char **pAtCommand,
                                             ATSocket_Options_t *pValues);

/**
 * @brief Creates a socket (using the AT+socket command).
 *
 * @param[in] family Family of the socket. See ATSocket_Family_t
 * @param[in] type Type of the socket. See ATSocket_Type_t
 * @param[in] protocol Protocol of the socket. See ATSocket_Protocol_t
 * @param[out] socketID ID (descriptor) assigned to the new socket. Can be used to access the socket from other functions.
 *
 * @return true if successful, false otherwise
 */

bool ATSocket_Create(ATSocket_Family_t family, ATSocket_Type_t type, ATSocket_Protocol_t protocol, uint8_t *socketID)
{
    bool ret = false;

    char *pRequestCommand = AT_commandBuffer;
    char *pRespondCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+socket=");

    ret = ATSocket_AddArgumentsCreate(pRequestCommand, family, type, protocol);

    if (ret)
    {
        if (!Calypso_SendRequest(pRequestCommand))
        {
            return false;
        }
        ret = Calypso_WaitForConfirm(Calypso_GetTimeout(Calypso_Timeout_General), Calypso_CNFStatus_Success, pRespondCommand);
    }

    if (ret)
    {
        ret = ATSocket_ParseResponseCreate(&pRespondCommand, socketID);
    }

    return ret;
}

/**
 * @brief Closes a socket (using the AT+close command).
 *
 * @param[in] socketID ID of the socket to be closed
 *
 * @return true if successful, false otherwise
 */
bool ATSocket_Close(uint8_t socketID)
{
    bool ret = false;

    char *pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+close=");

    ret = Calypso_AppendArgumentInt(pRequestCommand, socketID, (CALYPSO_INTFLAGS_NOTATION_DEC | CALYPSO_INTFLAGS_UNSIGNED), CALYPSO_STRING_TERMINATE);

    if (ret)
    {
        ret = Calypso_AppendArgumentString(pRequestCommand, CALYPSO_CRLF, CALYPSO_STRING_TERMINATE);
    }

    if (ret)
    {
        if (!Calypso_SendRequest(pRequestCommand))
        {
            return false;
        }
        ret = Calypso_WaitForConfirm(Calypso_GetTimeout(Calypso_Timeout_General), Calypso_CNFStatus_Success, NULL);
    }

    return ret;
}

/**
 * @brief Binds a socket to an IP address and a port (using the AT+bind command).
 *
 * @param[in] socketID ID of the socket to be bound
 * @param[in] socket Socket configuration. See ATSocket_Descriptor_t.
 *
 * @return true if successful, false otherwise
 */
bool ATSocket_Bind(uint8_t socketID, ATSocket_Descriptor_t socket)
{
    bool ret = false;

    char *pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+bind=");

    ret = ATSocket_AddArgumentsBindConnect(pRequestCommand, socketID, socket);

    if (ret)
    {
        if (!Calypso_SendRequest(pRequestCommand))
        {
            return false;
        }
        ret = Calypso_WaitForConfirm(Calypso_GetTimeout(Calypso_Timeout_General), Calypso_CNFStatus_Success, NULL);
    }

    return ret;
}

/**
 * Starts listening on the supplied socket (using the AT+listen command).
 *
 * @param[in] socketID ID of the socket on which to start listening
 * @param[in] backlog Max length of connect request queue
 *
 * @return true if successful, false otherwise
 */
bool ATSocket_Listen(uint8_t socketID, uint16_t backlog)
{
    bool ret = false;

    char *pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+listen=");


    ret = Calypso_AppendArgumentInt(pRequestCommand, socketID, (CALYPSO_INTFLAGS_NOTATION_DEC | CALYPSO_INTFLAGS_UNSIGNED), CALYPSO_ARGUMENT_DELIM);

    if (ret)
    {
        ret = Calypso_AppendArgumentInt(pRequestCommand, backlog, (CALYPSO_INTFLAGS_NOTATION_DEC | CALYPSO_INTFLAGS_UNSIGNED), CALYPSO_STRING_TERMINATE);
    }

    if (ret)
    {
        ret = Calypso_AppendArgumentString(pRequestCommand, CALYPSO_CRLF, CALYPSO_STRING_TERMINATE);
    }

    if (ret)
    {
        if (!Calypso_SendRequest(pRequestCommand))
        {
            return false;
        }
        ret = Calypso_WaitForConfirm(Calypso_GetTimeout(Calypso_Timeout_General), Calypso_CNFStatus_Success, NULL);
    }

    return ret;
}

/**
 * @brief Connects to a remote socket (using the AT+connect command).
 *
 * @param[in] socketID ID of the local socket to be connected
 * @param[in] remoteSocket Remote socket to connect to. See ATSocket_Descriptor_t
 *
 * @return true if successful, false otherwise
 */
bool ATSocket_Connect(uint8_t socketID, ATSocket_Descriptor_t remoteSocket)
{
    bool ret = false;

    char *pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+connect=");

    ret = ATSocket_AddArgumentsBindConnect(pRequestCommand, socketID, remoteSocket);

    if (ret)
    {
        if (!Calypso_SendRequest(pRequestCommand))
        {
            return false;
        }
        ret = Calypso_WaitForConfirm(Calypso_GetTimeout(Calypso_Timeout_General), Calypso_CNFStatus_Success, NULL);
    }

    return ret;
}

/**
 * @brief Accepts incoming connections on a socket (using the AT+accept command).
 *
 * @param[in] socketID ID of the local socket to accept connections for
 * @param[in] family Family of connections to accept
 *
 * @return true if successful, false otherwise
 */
bool ATSocket_Accept(uint8_t socketID, ATSocket_Family_t family)
{
    bool ret = false;

    char *pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+accept=");

    ret = Calypso_AppendArgumentInt(pRequestCommand, socketID, (CALYPSO_INTFLAGS_NOTATION_DEC | CALYPSO_INTFLAGS_UNSIGNED), CALYPSO_ARGUMENT_DELIM);

    if (ret)
    {
        ret = Calypso_AppendArgumentString(pRequestCommand, ATSocketFamilyString[family], CALYPSO_STRING_TERMINATE);
    }

    if (ret)
    {
        ret = Calypso_AppendArgumentString(pRequestCommand, CALYPSO_CRLF, CALYPSO_STRING_TERMINATE);
    }

    if (ret)
    {
        if (!Calypso_SendRequest(pRequestCommand))
        {
            return false;
        }
        ret = Calypso_WaitForConfirm(Calypso_GetTimeout(Calypso_Timeout_General), Calypso_CNFStatus_Success, NULL);
    }

    return ret;
}

/**
 * @brief Trigger receiving of data via the supplied local socket (using the AT+recv command).
 *
 * Note that any received data is provided asynchronously in events of type ATEvent_SocketRcvd.
 *
 * @param[in] socketID ID of the local socket which should receive
 * @param[in] format Format in which the data is to be provided. Setting the format to Calypso_DataFormat_Base64
 *                   causes the Calypso module to encode the data as Base64 before sending it to this device via UART.
 * @param[in] length Max number of bytes to receive
 *
 * @return true if successful, false otherwise
 */
bool ATSocket_Receive(uint8_t socketID, Calypso_DataFormat_t format, uint16_t length)
{
    bool ret = false;

    char *pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+recv=");

    ret = ATSocket_AddArgumentsRecv(pRequestCommand, socketID, format, length);

    if (ret)
    {
        if (!Calypso_SendRequest(pRequestCommand))
        {
            return false;
        }
        ret = Calypso_WaitForConfirm(Calypso_GetTimeout(Calypso_Timeout_General), Calypso_CNFStatus_Success, NULL);
    }

    return ret;
}

/**
 * Trigger receiving of data from the supplied remote socket via the supplied local socket (using the AT+recvFrom command).
 *
 * Note that any received data is provided asynchronously in events of type ATEvent_SocketRcvdFrom.
 *
 * @param[in] socketID ID of the local socket which should receive
 * @param[in] remoteSocket Remote socket from which the data should be received
 * @param[in] format Format in which the data is to be provided. Setting the format to Calypso_DataFormat_Base64
 *                   causes the Calypso module to encode the data as Base64 before sending it to this device via UART.
 * @param[in] length Max number of bytes to receive
 *
 * @return true if successful, false otherwise
 */
bool ATSocket_ReceiveFrom(uint8_t socketID,
                          ATSocket_Descriptor_t remoteSocket,
                          Calypso_DataFormat_t format,
                          uint16_t length)
{
    bool ret = false;

    char *pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+recvFrom=");

    ret = ATSocket_AddArgumentsRecvFrom(pRequestCommand, socketID, remoteSocket, format, length);

    if (ret)
    {
        if (!Calypso_SendRequest(pRequestCommand))
        {
            return false;
        }
        ret = Calypso_WaitForConfirm(Calypso_GetTimeout(Calypso_Timeout_General), Calypso_CNFStatus_Success, NULL);
    }

    return ret;
}

/**
 * @brief Sends data via the supplied local socket (using the AT+send command).
 *
 * @param[in] socketID ID of the local socket via which the data should be sent
 * @param[in] format Format in which the data is provided. Note that setting the format to Calypso_DataFormat_Base64
 *                   causes the Calypso module to interpret the data as Base64 - the module will decode the data and
 *                   transmit the decoded data via the socket. If using Calypso_DataFormat_Base64, you either need to
 *                   provide Base64 encoded data or set encodeAsBase64 to true.
 * @param[in] encodeAsBase64 Encode the data in Base64 format before sending it to the Calypso module
 * @param[in] length Number of bytes to be sent
 * @param[in] data Data to be sent
 * @param[out] bytesSent The number of bytes that have been sent. Note that in case
 *                       encodeAsBase64 is true, the actual number of (encoded) bytes sent might be higher.
 *
 * @return true if successful, false otherwise
 */
bool ATSocket_Send(uint8_t socketID,
                   Calypso_DataFormat_t format,
                   bool encodeAsBase64,
                   uint16_t length,
                   char *data,
                   uint16_t *bytesSent)
{
    return ATSocket_SendTo(socketID,
                           NULL,
                           format,
                           encodeAsBase64,
                           length,
                           data,
                           bytesSent);
}

/**
 * Sends data via the supplied local socket (using the AT+send or AT+sendTo command).
 *
 * The AT+sendTo command is used if a remote socket is supplied (UDP). Otherwise,
 * the AT+send command is used (TCP).
 *
 * @param[in] socketID ID of the local socket via which the data should be sent
 * @param[in] remoteSocket Remote socket to which the data should be sent (optional)
 * @param[in] format Format in which the data is provided. Note that setting the format to Calypso_DataFormat_Base64
 *                   causes the Calypso module to interpret the data as Base64 - the module will decode the data and
 *                   transmit the decoded data via the socket. If using Calypso_DataFormat_Base64, you either need to
 *                   provide Base64 encoded data or set encodeAsBase64 to true.
 * @param[in] encodeAsBase64 Encode the data in Base64 format before sending it to the Calypso module
 * @param[in] length Number of bytes to be sent
 * @param[in] data Data to be sent
 * @param[out] bytesSent The number of bytes that have been sent. Note that in case
 *                       encodeAsBase64 is true, the actual number of (encoded) bytes sent might be higher.
 *
 * @return true if successful, false otherwise
 */
bool ATSocket_SendTo(uint8_t socketID,
                     ATSocket_Descriptor_t *remoteSocket,
                     Calypso_DataFormat_t format,
                     bool encodeAsBase64,
                     uint16_t length,
                     char *data,
                     uint16_t *bytesSent)
{
    *bytesSent = 0;

    if (encodeAsBase64)
    {
        /* Base64 encoded data might exceed the max. chunk size. To limit the required buffer size,
         * the data is encoded in chunks, if necessary. */

        uint16_t maxChunkSize = (((CALYPSO_MAX_PAYLOAD_SIZE - 1) * 3) / 4) - 2;
        uint16_t chunkSize = 0;
        for (uint16_t chunkOffset = 0; chunkOffset < length; chunkOffset += chunkSize)
        {
            chunkSize = length - chunkOffset;
            if (chunkSize > maxChunkSize)
            {
                chunkSize = maxChunkSize;
            }

            /* Encode as Base64 */
            uint32_t lengthEncoded = Calypso_GetBase64EncBufSize(chunkSize);
            char base64Buffer[lengthEncoded];
            Calypso_EncodeBase64((uint8_t *) data + chunkOffset,
                                 chunkSize,
                                 (uint8_t *) base64Buffer,
                                 &lengthEncoded);

            /* Recursively call ATSocket_Send() with the encoded binary data (excluding '\0') */
            uint16_t chunkBytesWritten = 0;
            bool ok = ATSocket_SendTo(socketID,
                                      remoteSocket,
                                      format,
                                      false,
                                      lengthEncoded - 1,
                                      base64Buffer,
                                      &chunkBytesWritten);
            if (!ok)
            {
                return false;
            }

            *bytesSent += chunkSize;
        }

       return true;
    }

    /* Send data using either AT+send or AT+sendTo, splitting the payload into
     * chunks of max. CALYPSO_MAX_PAYLOAD_SIZE, if necessary. */
    uint16_t chunkBytesSent = 0;
    for (uint16_t chunkOffset = 0; chunkOffset < length; chunkOffset += chunkBytesSent)
    {
        uint16_t chunkSize = length - chunkOffset;
        if (chunkSize > CALYPSO_MAX_PAYLOAD_SIZE)
        {
            chunkSize = CALYPSO_MAX_PAYLOAD_SIZE;
        }

        char *pRequestCommand = AT_commandBuffer;

        if (NULL == remoteSocket)
        {
            /* Stream / TCP */
            strcpy(pRequestCommand, "AT+send=");
        }
        else
        {
            /* Datagram / UDP */
            strcpy(pRequestCommand, "AT+sendTo=");
        }

        if (!ATSocket_AddArgumentsSendTo(pRequestCommand,
                                         socketID,
                                         remoteSocket,
                                         format,
                                         chunkSize,
                                         data + chunkOffset))
        {
            return false;
        }

        chunkBytesSent = chunkSize;

        if (!Calypso_SendRequest(pRequestCommand))
        {
            return false;
        }
        if (!Calypso_WaitForConfirm(Calypso_GetTimeout(Calypso_Timeout_General), Calypso_CNFStatus_Success, NULL))
        {
            return false;
        }

        *bytesSent += chunkBytesSent;

#ifdef WE_DEBUG
        /* Flush debug buffer, as it may have been filled up with the written data */
        WE_Debug_Flush();
#endif
    }

    return true;
}

/**
 * @brief Sets socket options (using the AT+setSockOpt command).
 *
 * @param[in] socketID ID of the local socket for which options should be set
 * @param[in] level Option level parameter
 * @param[in] option Option parameter
 * @param[in] pValues Option value. Uses the ATSocket_Options_t member corresponding to the option being set.
 *
 * @return true if successful, false otherwise
 */
bool ATSocket_SetSocketOption(uint8_t socketID,
                              ATSocket_SockOptLevel_t level,
                              uint8_t option,
                              ATSocket_Options_t *pValues)
{
    bool ret = false;

    char *pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+setSockOpt=");

    ret = ATSocket_AddArgumentsSetSockOpt(pRequestCommand, socketID, level, option, pValues);

    if (ret)
    {
        if (!Calypso_SendRequest(pRequestCommand))
        {
            return false;
        }
        ret = Calypso_WaitForConfirm(Calypso_GetTimeout(Calypso_Timeout_General), Calypso_CNFStatus_Success, NULL);
    }

    return ret;
}

/**
 * @brief Gets socket options (using the AT+getSockOpt command).
 *
 * @param[in] socketID ID of the local socket for which options should be set
 * @param[in] level Option level parameter
 * @param[in] option Option parameter
 * @param[out] pValues Option value. Uses the ATSocket_Options_t member corresponding to the option being retrieved.
 *
 * @return true if successful, false otherwise
 */
bool ATSocket_GetSocketOption(uint8_t socketID,
                              ATSocket_SockOptLevel_t level,
                              uint8_t option,
                              ATSocket_Options_t *pValues)
{
    char *pRequestCommand = AT_commandBuffer;
    char *pRespondCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+getSockOpt=");

    if (!Calypso_AppendArgumentInt(pRequestCommand, socketID, (CALYPSO_INTFLAGS_NOTATION_DEC | CALYPSO_INTFLAGS_UNSIGNED), CALYPSO_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!Calypso_AppendArgumentString(pRequestCommand, ATSocketSockOptLevelString[level], CALYPSO_ARGUMENT_DELIM))
    {
        return false;
    }

    if (ATSocket_SockOptLevel_Socket == level)
    {
        if (option == ATSocket_SockOptSocket_DisableCertificateStore)
        {
            /* Not supported */
            return false;
        }
        if (!Calypso_AppendArgumentString(pRequestCommand, ATSocketSockOptSocketString[option], CALYPSO_STRING_TERMINATE))
        {
            return false;
        }
    }
    else if (ATSocket_SockOptLevel_IP == level)
    {
        if (!Calypso_AppendArgumentString(pRequestCommand, ATSocketSockOptIPString[option], CALYPSO_STRING_TERMINATE))
        {
            return false;
        }
    }
    else
    {
        return false;
    }

    if (!Calypso_AppendArgumentString(pRequestCommand, CALYPSO_CRLF, CALYPSO_STRING_TERMINATE))
    {
        return false;
    }

    if (!Calypso_SendRequest(pRequestCommand))
    {
        return false;
    }
    if (!Calypso_WaitForConfirm(Calypso_GetTimeout(Calypso_Timeout_General), Calypso_CNFStatus_Success, pRespondCommand))
    {
        return false;
    }

    return ATSocket_ParseResponseGetOptions(level, option, &pRespondCommand, pValues);
}

/**
 * @brief Adds socket descriptor arguments to the AT command string.
 *
 * @param[out] pAtCommand The AT command string to add the arguments to
 * @param[in] socket Socket information to append
 * @param[in] lastDelim Delimiter to append after the last argument
 *
 * @return true if successful, false otherwise
 */
bool ATSocket_AppendSocketDescriptor(char *pAtCommand, ATSocket_Descriptor_t socket, char lastDelim)
{
    bool ret = false;
    if (socket.family < ATSocket_Family_NumberOfValues)
    {
        ret = Calypso_AppendArgumentString(pAtCommand, ATSocketFamilyString[socket.family], CALYPSO_ARGUMENT_DELIM);
    }

    if (ret)
    {
        ret = Calypso_AppendArgumentInt(pAtCommand, socket.port, (CALYPSO_INTFLAGS_NOTATION_DEC | CALYPSO_INTFLAGS_UNSIGNED), CALYPSO_ARGUMENT_DELIM);
    }

    if (ret)
    {
        ret = Calypso_AppendArgumentString(pAtCommand, socket.address, lastDelim);
    }

    return ret;
}

/**
 * @brief Adds arguments to the AT+socket command string.
 *
 * @param[out] pAtCommand The AT command string to add the arguments to
 * @param[in] family Family of the socket. See ATSocket_Family_t
 * @param[in] type Type of the socket. See ATSocket_Type_t
 * @param[in] protocol Protocol of the socket. See ATSocket_Protocol_t
 *
 * @return true if successful, false otherwise
 */
static bool ATSocket_AddArgumentsCreate(char *pAtCommand, ATSocket_Family_t family, ATSocket_Type_t type, ATSocket_Protocol_t protocol)
{
    bool ret = false;

    if (family < ATSocket_Family_NumberOfValues)
    {
        ret = Calypso_AppendArgumentString(pAtCommand, ATSocketFamilyString[family], CALYPSO_ARGUMENT_DELIM);
    }

    if (ret && (type < ATSocket_Type_NumberOfValues))
    {
        ret = Calypso_AppendArgumentString(pAtCommand, ATSocketTypeString[type], CALYPSO_ARGUMENT_DELIM);
    }

    if (ret && (protocol < ATSocket_Protocol_NumberOfValues))
    {
        ret = Calypso_AppendArgumentString(pAtCommand, ATSocketProcotolString[protocol], CALYPSO_STRING_TERMINATE);
    }

    if (ret)
    {
        ret = Calypso_AppendArgumentString(pAtCommand, CALYPSO_CRLF,CALYPSO_STRING_TERMINATE);
    }

    return ret;
}

/**
 * @brief Adds arguments to the AT+bind / AT+connect command string.
 *
 * @param[out] pAtCommand The AT command string to add the arguments to
 * @param[in] socketID ID of the socket to be bound
 * @param[in] socket Socket configuration. See ATSocket_Descriptor_t.
 *
 * @return true if successful, false otherwise
 */
static bool ATSocket_AddArgumentsBindConnect(char *pAtCommand, uint8_t socketID, ATSocket_Descriptor_t socket)
{
    bool ret = false;

    ret = Calypso_AppendArgumentInt(pAtCommand, socketID, (CALYPSO_INTFLAGS_NOTATION_DEC | CALYPSO_INTFLAGS_UNSIGNED), CALYPSO_ARGUMENT_DELIM);

    if (ret)
    {
        ret = ATSocket_AppendSocketDescriptor(pAtCommand, socket, CALYPSO_STRING_TERMINATE);
    }

    if (ret)
    {
        ret = Calypso_AppendArgumentString(pAtCommand, CALYPSO_CRLF, CALYPSO_STRING_TERMINATE);
    }

    return ret;
}

/**
 * @brief Adds arguments to the AT+recv command string.
 *
 * @param[out] pAtCommand The AT command string to add the arguments to
 * @param[in] socketID ID of the local socket which should receive
 * @param[in] format Format in which the data is to be provided. The module will encode the data and will provide it in this format.
 * @param[in] length Max number of bytes to receive
 *
 * @return true if successful, false otherwise
 */
static bool ATSocket_AddArgumentsRecv(char *pAtCommand,
                                      uint8_t socketID,
                                      Calypso_DataFormat_t format,
                                      uint16_t length)
{
    bool ret = false;

    if (format >= Calypso_DataFormat_NumberOfValues)
    {
        return false;
    }

    ret = Calypso_AppendArgumentInt(pAtCommand, socketID, (CALYPSO_INTFLAGS_NOTATION_DEC | CALYPSO_INTFLAGS_UNSIGNED), CALYPSO_ARGUMENT_DELIM);

    if (ret)
    {
        ret = Calypso_AppendArgumentInt(pAtCommand, format, (CALYPSO_INTFLAGS_NOTATION_DEC | CALYPSO_INTFLAGS_UNSIGNED), CALYPSO_ARGUMENT_DELIM);
    }

    if (ret)
    {
        ret = Calypso_AppendArgumentInt(pAtCommand,
                                        length < CALYPSO_MAX_PAYLOAD_SIZE ? length : CALYPSO_MAX_PAYLOAD_SIZE,
                                        (CALYPSO_INTFLAGS_NOTATION_DEC | CALYPSO_INTFLAGS_UNSIGNED),
                                        CALYPSO_STRING_TERMINATE);
    }

    if (ret)
    {
        ret = Calypso_AppendArgumentString(pAtCommand, CALYPSO_CRLF, CALYPSO_STRING_TERMINATE);
    }

    return ret;
}

/**
 * @brief Adds arguments to the AT+recvFrom command string.
 *
 * @param[out] pAtCommand The AT command string to add the arguments to
 * @param[in] socketID ID of the local socket which should receive
 * @param[in] remoteSocket Remote socket from which the data should be received
 * @param[in] format Format in which the data is to be provided. The module will encode the data and will provide it in this format.
 * @param[in] length Max number of bytes to receive
 *
 * @return true if successful, false otherwise
 */
static bool ATSocket_AddArgumentsRecvFrom(char *pAtCommand,
                                          uint8_t socketID,
                                          ATSocket_Descriptor_t socketFrom,
                                          Calypso_DataFormat_t format,
                                          uint16_t length)
{
    bool ret = false;

    ret = Calypso_AppendArgumentInt(pAtCommand, socketID, (CALYPSO_INTFLAGS_NOTATION_DEC | CALYPSO_INTFLAGS_UNSIGNED), CALYPSO_ARGUMENT_DELIM);

    if (ret)
    {
        ret = ATSocket_AppendSocketDescriptor(pAtCommand, socketFrom, CALYPSO_ARGUMENT_DELIM);
    }

    if (ret)
    {
        ret = Calypso_AppendArgumentInt(pAtCommand, format, (CALYPSO_INTFLAGS_NOTATION_DEC | CALYPSO_INTFLAGS_UNSIGNED), CALYPSO_ARGUMENT_DELIM);
    }

    if (ret)
    {
        ret = Calypso_AppendArgumentInt(pAtCommand,
                                        length < CALYPSO_MAX_PAYLOAD_SIZE ? length : CALYPSO_MAX_PAYLOAD_SIZE,
                                        (CALYPSO_INTFLAGS_NOTATION_DEC | CALYPSO_INTFLAGS_UNSIGNED),
                                        CALYPSO_STRING_TERMINATE);
    }

    if (ret)
    {
        ret = Calypso_AppendArgumentString(pAtCommand, CALYPSO_CRLF, CALYPSO_STRING_TERMINATE);
    }

    return ret;
}

/**
 * @brief Adds arguments to the AT+send or AT+sendTo command string.
 *
 * @param[out] pAtCommand The AT command string to add the arguments to
 * @param[in] socketID ID of the local socket via which the data should be sent
 * @param[in] remoteSocket Remote socket to which the data should be sent. Optional (to be used with AT+sendTo).
 * @param[in] format Format in which the data is provided
 * @param[in] length Number of bytes to be sent
 * @param[in] data Data to be sent
 *
 * @return true if successful, false otherwise
 */
static bool ATSocket_AddArgumentsSendTo(char *pAtCommand,
                                        uint8_t socketID,
                                        ATSocket_Descriptor_t *remoteSocket,
                                        Calypso_DataFormat_t format,
                                        uint16_t length,
                                        char *data)
{
    if (format >= Calypso_DataFormat_NumberOfValues)
    {
        return false;
    }

    if (!Calypso_AppendArgumentInt(pAtCommand, socketID, (CALYPSO_INTFLAGS_NOTATION_DEC | CALYPSO_INTFLAGS_UNSIGNED), CALYPSO_ARGUMENT_DELIM))
    {
        return false;
    }

    if (NULL != remoteSocket)
    {
        if (!ATSocket_AppendSocketDescriptor(pAtCommand, *remoteSocket, CALYPSO_ARGUMENT_DELIM))
        {
            return false;
        }
    }

    if (!Calypso_AppendArgumentInt(pAtCommand, format, (CALYPSO_INTFLAGS_NOTATION_DEC | CALYPSO_INTFLAGS_UNSIGNED), CALYPSO_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!Calypso_AppendArgumentInt(pAtCommand, length, (CALYPSO_INTFLAGS_NOTATION_DEC | CALYPSO_INTFLAGS_UNSIGNED), CALYPSO_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!Calypso_AppendArgumentBytes(pAtCommand, data, length, CALYPSO_STRING_TERMINATE))
    {
        return false;
    }

    return Calypso_AppendArgumentString(pAtCommand, CALYPSO_CRLF, CALYPSO_STRING_TERMINATE);
}

/**
 * @brief Adds arguments to the AT+setSockOpt command string.
 *
 * @param[out] pAtCommand The AT command string to add the arguments to
 * @param[in] socketID ID of the local socket for which options should be set
 * @param[in] level Option level parameter
 * @param[in] option Option parameter
 * @param[in] pValues Option value
 *
 * @return true if successful, false otherwise
 */
static bool ATSocket_AddArgumentsSetSockOpt(char *pAtCommand,
                                            uint8_t socketID,
                                            ATSocket_SockOptLevel_t level,
                                            uint8_t option,
                                            ATSocket_Options_t *pValues)
{
    bool ret = false;

    ret = Calypso_AppendArgumentInt(pAtCommand, socketID, (CALYPSO_INTFLAGS_NOTATION_DEC | CALYPSO_INTFLAGS_UNSIGNED), CALYPSO_ARGUMENT_DELIM);

    if (ret)
    {
        ret = Calypso_AppendArgumentString(pAtCommand, ATSocketSockOptLevelString[level], CALYPSO_ARGUMENT_DELIM);
    }

    if (ret)
    {
        if (ATSocket_SockOptLevel_Socket == level)
        {
            if (!Calypso_AppendArgumentString(pAtCommand, ATSocketSockOptSocketString[option], CALYPSO_ARGUMENT_DELIM))
            {
                return false;
            }

            switch (option)
            {
            case ATSocket_SockOptSocket_KeepAlive:
                ret = Calypso_AppendArgumentInt(pAtCommand, pValues->keepAlive, (CALYPSO_INTFLAGS_NOTATION_DEC | CALYPSO_INTFLAGS_UNSIGNED), CALYPSO_STRING_TERMINATE);
                break;

            case ATSocket_SockOptSocket_KeepAliveTime:
                ret = Calypso_AppendArgumentInt(pAtCommand, pValues->keepAliveTimeSeconds, (CALYPSO_INTFLAGS_NOTATION_DEC | CALYPSO_INTFLAGS_UNSIGNED), CALYPSO_STRING_TERMINATE);
                break;

            case ATSocket_SockOptSocket_RXNoIPBoundary:
                ret = Calypso_AppendArgumentInt(pAtCommand, pValues->rxNoIpBoundary, (CALYPSO_INTFLAGS_NOTATION_DEC | CALYPSO_INTFLAGS_UNSIGNED), CALYPSO_STRING_TERMINATE);
                break;

            case ATSocket_SockOptSocket_RCVTimeout:
                ret = Calypso_AppendArgumentInt(pAtCommand, pValues->rcvTimeout.seconds, (CALYPSO_INTFLAGS_NOTATION_DEC | CALYPSO_INTFLAGS_UNSIGNED), CALYPSO_ARGUMENT_DELIM);
                if (ret)
                {
                    ret = Calypso_AppendArgumentInt(pAtCommand, pValues->rcvTimeout.microseconds, (CALYPSO_INTFLAGS_NOTATION_DEC | CALYPSO_INTFLAGS_UNSIGNED), CALYPSO_STRING_TERMINATE);
                }
                break;

            case ATSocket_SockOptSocket_RCVBuf:
                ret = Calypso_AppendArgumentInt(pAtCommand, pValues->rcvBuf, (CALYPSO_INTFLAGS_NOTATION_DEC | CALYPSO_INTFLAGS_UNSIGNED), CALYPSO_STRING_TERMINATE);
                break;

            case ATSocket_SockOptSocket_NonBlocking:
                ret = Calypso_AppendArgumentInt(pAtCommand, pValues->nonBlocking, (CALYPSO_INTFLAGS_NOTATION_DEC | CALYPSO_INTFLAGS_UNSIGNED), CALYPSO_STRING_TERMINATE);
                break;

            case ATSocket_SockOptSocket_SecMethod:
                ret = Calypso_AppendArgumentString(pAtCommand, AtSocketSockOptSecMethod[pValues->secMethod], CALYPSO_STRING_TERMINATE);
                break;

            case ATSocket_SockOptSocket_SecureMask:
                ret = ATSocket_AppendCipherMask(pAtCommand, pValues->secureMask);
                break;

            case ATSocket_SockOptSocket_SecureFilesCAFileName:
            case ATSocket_SockOptSocket_SecureFilesPrivateKeyFileName:
            case ATSocket_SockOptSocket_SecureFilesCertificateFileName:
            case ATSocket_SockOptSocket_SecureFilesDHKeyFileName:
                /* File name strings to append */
                ret = Calypso_AppendArgumentString(pAtCommand, pValues->fileName, CALYPSO_STRING_TERMINATE);
                break;

            case ATSocket_SockOptSocket_SecureDomainNameVerification:
                ret = Calypso_AppendArgumentString(pAtCommand, pValues->secureDomainNameVerification, CALYPSO_STRING_TERMINATE);
                break;

            case ATSocket_SockOptSocket_DisableCertificateStore:
                ret = Calypso_AppendArgumentInt(pAtCommand, pValues->disableCertificateStore, (CALYPSO_INTFLAGS_NOTATION_DEC | CALYPSO_INTFLAGS_UNSIGNED), CALYPSO_STRING_TERMINATE);
                break;

            default:
            {
                /* Invalid option or not implemented */
                ret = false;
                break;
            }
            }
        }
        else if (ATSocket_SockOptLevel_IP == level)
        {
            if (!Calypso_AppendArgumentString(pAtCommand, ATSocketSockOptIPString[option], CALYPSO_ARGUMENT_DELIM))
            {
                return false;
            }

            switch (option)
            {
            case ATSocket_SockOptIP_MulticastTTL:
                ret = Calypso_AppendArgumentInt(pAtCommand, pValues->multicastTTL, (CALYPSO_INTFLAGS_NOTATION_DEC | CALYPSO_INTFLAGS_UNSIGNED), CALYPSO_STRING_TERMINATE);
                break;

            case ATSocket_SockOptIP_AddMembership:
            case ATSocket_SockOptIP_DropMembership:
                ret = Calypso_AppendArgumentString(pAtCommand, pValues->multicastGroup.ipv4Address, CALYPSO_ARGUMENT_DELIM);
                if (ret)
                {
                    ret = Calypso_AppendArgumentString(pAtCommand, pValues->multicastGroup.interfaceAddress, CALYPSO_STRING_TERMINATE);
                }
                break;

            default:
                /* Invalid option or not implemented */
                ret = false;
                break;
            }
        }
        else
        {
            ret = false;
        }
    }

    if (ret)
    {
        ret = Calypso_AppendArgumentString(pAtCommand, CALYPSO_CRLF, CALYPSO_STRING_TERMINATE);
    }

    return ret;
}

/**
 * @brief Parses the response to the AT+socket command.
 *
 * @param[in,out] pAtCommand The string received in response to the AT+socket command
 * @param[out] pOutSocketID The parsed socket ID
 *
 * @return true if successful, false otherwise
 */
static bool ATSocket_ParseResponseCreate(char **pAtCommand, uint8_t *pOutSocketID)
{
    bool ret = false;
    const char *cmd = "+socket:";
    const size_t cmdLength = strlen(cmd);

    /* Check if response is for this command */
    ret = (0 == strncmp(*pAtCommand, cmd, cmdLength));
    if (ret)
    {
        *pAtCommand += cmdLength;
        ret = Calypso_GetNextArgumentInt(pAtCommand, pOutSocketID, CALYPSO_INTFLAGS_SIZE8 | CALYPSO_INTFLAGS_UNSIGNED, CALYPSO_STRING_TERMINATE);
    }

    return ret;
}

bool ATSocket_ParseResponseGetOptions(ATSocket_SockOptLevel_t level,
                                      uint8_t option,
                                      char **pAtCommand,
                                      ATSocket_Options_t *pValues)
{
    bool ret = false;

    const char *expectedCmd = "+getsockopt:";
    const size_t cmdLength = strlen(expectedCmd);

    char tempString[50];

    /* Check if response is for getSockOpt */
    if (0 != strncmp(*pAtCommand, expectedCmd, cmdLength))
    {
        return false;
    }

    *pAtCommand += cmdLength;

    switch (level)
    {
    case ATSocket_SockOptLevel_Socket:
    {
        switch (option)
        {
        case ATSocket_SockOptSocket_KeepAlive:
            ret = Calypso_GetNextArgumentInt(pAtCommand, &pValues->keepAlive, CALYPSO_INTFLAGS_SIZE8 | CALYPSO_INTFLAGS_UNSIGNED, CALYPSO_STRING_TERMINATE);
            break;

        case ATSocket_SockOptSocket_KeepAliveTime:
            ret = Calypso_GetNextArgumentInt(pAtCommand, &pValues->keepAliveTimeSeconds, CALYPSO_INTFLAGS_SIZE32 | CALYPSO_INTFLAGS_UNSIGNED, CALYPSO_STRING_TERMINATE);
            break;

        case ATSocket_SockOptSocket_RXNoIPBoundary:
            ret = Calypso_GetNextArgumentInt(pAtCommand, &pValues->rxNoIpBoundary, CALYPSO_INTFLAGS_SIZE8 | CALYPSO_INTFLAGS_UNSIGNED, CALYPSO_STRING_TERMINATE);
            break;

        case ATSocket_SockOptSocket_RCVTimeout:
            ret = Calypso_GetNextArgumentInt(pAtCommand, &pValues->rcvTimeout.seconds, CALYPSO_INTFLAGS_SIZE32 | CALYPSO_INTFLAGS_UNSIGNED, CALYPSO_ARGUMENT_DELIM);
            if (ret)
            {
                ret = Calypso_GetNextArgumentInt(pAtCommand, &pValues->rcvTimeout.microseconds, CALYPSO_INTFLAGS_SIZE32 | CALYPSO_INTFLAGS_UNSIGNED, CALYPSO_STRING_TERMINATE);
            }
            break;

        case ATSocket_SockOptSocket_RCVBuf:
            ret = Calypso_GetNextArgumentInt(pAtCommand, &pValues->rcvBuf, CALYPSO_INTFLAGS_SIZE32 | CALYPSO_INTFLAGS_UNSIGNED, CALYPSO_STRING_TERMINATE);
            break;

        case ATSocket_SockOptSocket_NonBlocking:
            ret = Calypso_GetNextArgumentInt(pAtCommand, &pValues->nonBlocking, CALYPSO_INTFLAGS_SIZE8 | CALYPSO_INTFLAGS_UNSIGNED, CALYPSO_STRING_TERMINATE);
            break;

        case ATSocket_SockOptSocket_SecMethod:
            ret = Calypso_GetNextArgumentString(pAtCommand, tempString, CALYPSO_STRING_TERMINATE, sizeof(tempString));
            if (ret)
            {
                pValues->secMethod = Calypso_FindString(AtSocketSockOptSecMethod, ATSocket_SockOptSecMethod_NumberOfValues, tempString, ATSocket_SockOptSecMethod_SSLv3, &ret);
            }
            break;

        case ATSocket_SockOptSocket_SecureMask:
            ret = Calypso_GetNextArgumentBitmask(pAtCommand,
                                                 ATSocket_CipherStrings,
                                                 ATSocket_Cipher_NumberOfValues,
                                                 50,
                                                 &pValues->secureMask,
                                                 CALYPSO_STRING_TERMINATE);
            break;

        case ATSocket_SockOptSocket_SecureFilesCAFileName:
        case ATSocket_SockOptSocket_SecureFilesPrivateKeyFileName:
        case ATSocket_SockOptSocket_SecureFilesCertificateFileName:
        case ATSocket_SockOptSocket_SecureFilesDHKeyFileName:
            ret = Calypso_GetNextArgumentString(pAtCommand, pValues->fileName, CALYPSO_STRING_TERMINATE, sizeof(pValues->fileName));
            break;

        case ATSocket_SockOptSocket_SecureDomainNameVerification:
            ret = Calypso_GetNextArgumentString(pAtCommand, pValues->secureDomainNameVerification, CALYPSO_STRING_TERMINATE, sizeof(pValues->secureDomainNameVerification));
            break;

        case ATSocket_SockOptSocket_DisableCertificateStore:
            ret = Calypso_GetNextArgumentInt(pAtCommand, &pValues->disableCertificateStore, CALYPSO_INTFLAGS_SIZE8 | CALYPSO_INTFLAGS_UNSIGNED, CALYPSO_STRING_TERMINATE);
            break;

        default:
            ret = false;
            break;
        }
        break;
    }

    case ATSocket_SockOptLevel_IP:
        switch (option)
        {
        case ATSocket_SockOptIP_MulticastTTL:
            ret = Calypso_GetNextArgumentInt(pAtCommand, &pValues->multicastTTL, CALYPSO_INTFLAGS_SIZE32 | CALYPSO_INTFLAGS_UNSIGNED, CALYPSO_STRING_TERMINATE);
            break;

        case ATSocket_SockOptIP_AddMembership:
        case ATSocket_SockOptIP_DropMembership:
            ret = Calypso_GetNextArgumentString(pAtCommand, pValues->multicastGroup.ipv4Address, CALYPSO_ARGUMENT_DELIM, sizeof(pValues->multicastGroup.ipv4Address));
            if (ret)
            {
                ret = Calypso_GetNextArgumentString(pAtCommand, pValues->multicastGroup.interfaceAddress, CALYPSO_STRING_TERMINATE, sizeof(pValues->multicastGroup.interfaceAddress));
            }
            break;

        default:
            ret = false;
            break;
        }
        break;

    default:
        ret = false;
        break;

    }

    return ret;
}


/**
 * @brief Parses a string to ATSocket_Family_t.
 *
 * @param[in] familyString String representing the socket family
 * @param[out] pOutFamily The parsed socket family
 *
 * @return true if successful, false otherwise
 */
bool ATSocket_ParseSocketFamily(const char *familyString, ATSocket_Family_t *pOutFamily)
{
    bool ok;
    *pOutFamily = Calypso_FindString(ATSocketFamilyString, ATSocket_Family_NumberOfValues, familyString, ATSocket_Family_INET, &ok);
    return ok;
}

/**
 * @brief Returns the string representation of the supplied socket family.
 *
 * @param[in] family Socket family
 * @param[out] pOutFamilyStr String representation of the supplied socket family
 *
 * @return true if successful, false otherwise
 */
bool ATSocket_GetSocketFamilyString(ATSocket_Family_t family, char *pOutFamilyStr)
{
    if (family >= ATSocket_Family_NumberOfValues)
    {
        return false;
    }
    strcpy(pOutFamilyStr, ATSocketFamilyString[family]);
    return true;
}

/**
 * @brief Appends cipher mask flags to the supplied string.
 *
 * @param[out] pOutStr The string to which the flags should be added (must be null terminated)
 * @param[in] cipherMask Cipher mask flags (see ATSocket_Cipher_t)
 */
bool ATSocket_AppendCipherMask(char *pOutStr, uint32_t cipherMask)
{
    return Calypso_AppendArgumentBitmask(pOutStr,
                                         ATSocket_CipherStrings,
                                         ATSocket_Cipher_NumberOfValues,
                                         cipherMask,
                                         CALYPSO_STRING_TERMINATE,
                                         AT_MAX_COMMAND_BUFFER_SIZE);
}
