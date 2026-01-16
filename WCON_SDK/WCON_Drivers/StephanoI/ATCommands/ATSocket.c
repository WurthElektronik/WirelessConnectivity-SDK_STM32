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
 * @brief AT commands for socket functionality.
 */

#include <StephanoI/ATCommands/ATSocket.h>
#include <StephanoI/StephanoI.h>
#include <global/ATCommands.h>

bool StephanoI_ATSocket_SetEnableIPv6(bool enable)
{
    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+CIPV6=");

    if (!ATCommand_AppendArgumentInt(pRequestCommand, enable ? (uint32_t)1 : (uint32_t)0, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentString(pRequestCommand, ATCOMMAND_CRLF, ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }

    if (!StephanoI_SendRequest(pRequestCommand))
    {
        return false;
    }
    return StephanoI_WaitForConfirm(StephanoI_GetTimeout(StephanoI_Timeout_General), StephanoI_CNFStatus_Success);
}

bool StephanoI_ATSocket_GetEnableIPv6(bool* enable)
{
    char responsebuffer[2];
    if (!StephanoI_SendRequest("AT+CIPV6?\r\n"))
    {
        return false;
    }
    if (!StephanoI_WaitForConfirm_ex(StephanoI_GetTimeout(StephanoI_Timeout_General), StephanoI_CNFStatus_Success, responsebuffer, sizeof(responsebuffer)))
    {
        return false;
    }

    return StephanoI_ATSocket_ParseEnableIPv6(responsebuffer, enable);
}

bool StephanoI_ATSocket_SetMultiple(bool multiple)
{
    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+CIPMUX=");

    if (!ATCommand_AppendArgumentInt(pRequestCommand, multiple ? (uint32_t)1 : (uint32_t)0, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentString(pRequestCommand, ATCOMMAND_CRLF, ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }

    if (!StephanoI_SendRequest(pRequestCommand))
    {
        return false;
    }
    return StephanoI_WaitForConfirm(StephanoI_GetTimeout(StephanoI_Timeout_General), StephanoI_CNFStatus_Success);
}

bool StephanoI_ATSocket_GetMultiple(bool* multipleP)
{
    char responsebuffer[3];
    if (!StephanoI_SendRequest("AT+CIPMUX?\r\n"))
    {
        return false;
    }

    if (!StephanoI_WaitForConfirm_ex(StephanoI_GetTimeout(StephanoI_Timeout_General), StephanoI_CNFStatus_Success, responsebuffer, sizeof(responsebuffer)))
    {
        return false;
    }

    return StephanoI_ATSocket_ParseGetMultiple(responsebuffer, multipleP);
}

bool StephanoI_ATSocket_OpenTCPSocket(bool multiple_connections, uint8_t link_ID, StephanoI_ATSocket_Type_t type, char* remote_IP, uint32_t remote_port)
{
    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+CIPSTART=");

    if (multiple_connections)
    {
        /* AT+CIPMUX=1 */
        if (!ATCommand_AppendArgumentInt(pRequestCommand, link_ID, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_ARGUMENT_DELIM))
        {
            return false;
        }
    }
    switch (type)
    {
        case StephanoI_ATSocket_Type_TCP:
        {
            if (!ATCommand_AppendArgumentStringQuotationMarks(pRequestCommand, "TCP", ATCOMMAND_ARGUMENT_DELIM))
            {
                return false;
            }
        }
        break;
        case StephanoI_ATSocket_Type_TCPv6:
        {
            if (!ATCommand_AppendArgumentStringQuotationMarks(pRequestCommand, "TCPv6", ATCOMMAND_ARGUMENT_DELIM))
            {
                return false;
            }
        }
        break;
        case StephanoI_ATSocket_Type_UDP:
        case StephanoI_ATSocket_Type_UDPv6:
        default:
        {
            return false;
        }
    }
    if (!ATCommand_AppendArgumentStringQuotationMarks(pRequestCommand, remote_IP, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentInt(pRequestCommand, remote_port, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentString(pRequestCommand, ATCOMMAND_CRLF, ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }

    if (!StephanoI_SendRequest(pRequestCommand))
    {
        return false;
    }
    return StephanoI_WaitForConfirm(StephanoI_GetTimeout(StephanoI_Timeout_SocketOpen), StephanoI_CNFStatus_Success);
}

bool StephanoI_ATSocket_OpenUDPSocket(bool multiple_connections, uint8_t link_ID, StephanoI_ATSocket_Type_t type, char* remote_IP, uint32_t remote_port, uint32_t local_port)
{
    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+CIPSTART=");

    if (multiple_connections)
    {
        /* AT+CIPMUX=1 */
        if (!ATCommand_AppendArgumentInt(pRequestCommand, link_ID, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_ARGUMENT_DELIM))
        {
            return false;
        }
    }
    switch (type)
    {
        case StephanoI_ATSocket_Type_UDP:
        {
            if (!ATCommand_AppendArgumentStringQuotationMarks(pRequestCommand, "UDP", ATCOMMAND_ARGUMENT_DELIM))
            {
                return false;
            }
        }
        break;
        case StephanoI_ATSocket_Type_UDPv6:
        {
            if (!ATCommand_AppendArgumentStringQuotationMarks(pRequestCommand, "UDPv6", ATCOMMAND_ARGUMENT_DELIM))
            {
                return false;
            }
        }
        break;
        case StephanoI_ATSocket_Type_TCP:
        case StephanoI_ATSocket_Type_TCPv6:
        default:
        {
            return false;
        }
    }
    if (!ATCommand_AppendArgumentStringQuotationMarks(pRequestCommand, remote_IP, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentInt(pRequestCommand, remote_port, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentInt(pRequestCommand, local_port, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentString(pRequestCommand, ATCOMMAND_CRLF, ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }

    if (!StephanoI_SendRequest(pRequestCommand))
    {
        return false;
    }
    return StephanoI_WaitForConfirm(StephanoI_GetTimeout(StephanoI_Timeout_SocketOpen), StephanoI_CNFStatus_Success);
}

bool StephanoI_ATSocket_OpenSocketEx(StephanoI_ATSocket_Type_t type, char* remote_IP, uint32_t remote_port)
{
    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+CIPSTARTEX=");

    switch (type)
    {
        case StephanoI_ATSocket_Type_TCP:
        {
            if (!ATCommand_AppendArgumentStringQuotationMarks(pRequestCommand, "TCP", ATCOMMAND_ARGUMENT_DELIM))
            {
                return false;
            }
        }
        break;
        case StephanoI_ATSocket_Type_TCPv6:
        {
            if (!ATCommand_AppendArgumentStringQuotationMarks(pRequestCommand, "TCPv6", ATCOMMAND_ARGUMENT_DELIM))
            {
                return false;
            }
        }
        break;
        case StephanoI_ATSocket_Type_UDP:
        {
            if (!ATCommand_AppendArgumentStringQuotationMarks(pRequestCommand, "UDP", ATCOMMAND_ARGUMENT_DELIM))
            {
                return false;
            }
        }
        break;
        case StephanoI_ATSocket_Type_UDPv6:
        {
            if (!ATCommand_AppendArgumentStringQuotationMarks(pRequestCommand, "UDPv6", ATCOMMAND_ARGUMENT_DELIM))
            {
                return false;
            }
        }
        break;
        default:
        {
            return false;
        }
    }
    if (!ATCommand_AppendArgumentStringQuotationMarks(pRequestCommand, remote_IP, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentInt(pRequestCommand, remote_port, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentString(pRequestCommand, ATCOMMAND_CRLF, ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }

    if (!StephanoI_SendRequest(pRequestCommand))
    {
        return false;
    }
    return StephanoI_WaitForConfirm(StephanoI_GetTimeout(StephanoI_Timeout_SocketOpen), StephanoI_CNFStatus_Success);
}

bool StephanoI_ATSocket_CloseSocket(bool multiple_connections, uint8_t link_ID)
{
    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+CIPCLOSE=");

    if (multiple_connections)
    {
        /* AT+CIPMUX=1 */
        strcpy(pRequestCommand, "AT+CIPCLOSE=");
        if (!ATCommand_AppendArgumentInt(pRequestCommand, link_ID, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_STRING_TERMINATE))
        {
            return false;
        }
        if (!ATCommand_AppendArgumentString(pRequestCommand, ATCOMMAND_CRLF, ATCOMMAND_STRING_TERMINATE))
        {
            return false;
        }
    }
    else
    {
        strcpy(pRequestCommand, "AT+CIPCLOSE\r\n");
    }

    if (!StephanoI_SendRequest(pRequestCommand))
    {
        return false;
    }
    return StephanoI_WaitForConfirm(StephanoI_GetTimeout(StephanoI_Timeout_General), StephanoI_CNFStatus_Success);
}

bool StephanoI_ATSocket_GetState(StephanoI_ATSocket_State_t* t)
{
    char responsebuffer[128];
    if (!StephanoI_SendRequest("AT+CIPSTATE?\r\n"))
    {
        return false;
    }
    if (!StephanoI_WaitForConfirm_ex(StephanoI_GetTimeout(StephanoI_Timeout_General), StephanoI_CNFStatus_Success, responsebuffer, sizeof(responsebuffer)))
    {
        return false;
    }

    return StephanoI_ATSocket_ParseState(responsebuffer, t);
}

bool StephanoI_ATSocket_Ping(char* host, StephanoI_ATSocket_Ping_t* t)
{
    char responsebuffer[16];
    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+PING=");

    if (!ATCommand_AppendArgumentStringQuotationMarks(pRequestCommand, host, ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentString(pRequestCommand, ATCOMMAND_CRLF, ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }

    if (!StephanoI_SendRequest(pRequestCommand))
    {
        return false;
    }

    if (!StephanoI_WaitForConfirm_ex(StephanoI_GetTimeout(StephanoI_Timeout_SocketPing), StephanoI_CNFStatus_Success, responsebuffer, sizeof(responsebuffer)))
    {
        return false;
    }

    return StephanoI_ATSocket_ParsePing(responsebuffer, t);
}

bool StephanoI_ATSocket_ResolveDomain(char* domain_name, StephanoI_ATSocket_DomainResolved_t* domainP)
{
    char responsebuffer[64];
    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+CIPDOMAIN=");

    if (!ATCommand_AppendArgumentStringQuotationMarks(pRequestCommand, domain_name, ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentString(pRequestCommand, ATCOMMAND_CRLF, ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }

    if (!StephanoI_SendRequest(pRequestCommand))
    {
        return false;
    }
    if (!StephanoI_WaitForConfirm_ex(StephanoI_GetTimeout(StephanoI_Timeout_SocketPing), StephanoI_CNFStatus_Success, responsebuffer, sizeof(responsebuffer)))
    {
        return false;
    }

    return StephanoI_ATSocket_ParseDomainResolved(responsebuffer, domainP);
}

bool StephanoI_ATSocket_Send(bool multiple_connections, uint8_t link_id, char* remote_host, uint32_t remote_port, uint8_t* data, uint32_t length)
{
    char* pRequestCommand = AT_commandBuffer;

    if (length > 8192)
    {
        strcpy(pRequestCommand, "AT+CIPSENDL=");
    }
    else
    {
        strcpy(pRequestCommand, "AT+CIPSEND=");
    }

    if (multiple_connections)
    {
        /* AT+CIPMUX=1 */
        if (!ATCommand_AppendArgumentInt(pRequestCommand, link_id, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_ARGUMENT_DELIM))
        {
            return false;
        }
    }

    if (!ATCommand_AppendArgumentInt(pRequestCommand, length, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), (remote_host != NULL) ? ATCOMMAND_ARGUMENT_DELIM : ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }
    if (remote_host != NULL)
    {
        if (!ATCommand_AppendArgumentStringQuotationMarks(pRequestCommand, remote_host, ATCOMMAND_ARGUMENT_DELIM))
        {
            return false;
        }
        if (!ATCommand_AppendArgumentInt(pRequestCommand, remote_port, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_STRING_TERMINATE))
        {
            return false;
        }
    }
    if (!ATCommand_AppendArgumentString(pRequestCommand, ATCOMMAND_CRLF, ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }

    if (!StephanoI_SendRequest(pRequestCommand))
    {
        return false;
    }
    if (!StephanoI_WaitForConfirm(StephanoI_GetTimeout(StephanoI_Timeout_SocketPing), StephanoI_CNFStatus_Ready4Data))
    {
        return false;
    }

    if (!StephanoI_SendRequest_ex(data, length))
    {
        return false;
    }
    return StephanoI_WaitForConfirm(StephanoI_GetTimeout(StephanoI_Timeout_SocketPing), StephanoI_CNFStatus_SendOK);
}

bool StephanoI_ATSocket_SendConfiguration(uint32_t report_size, uint32_t transmit_size)
{
    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+CIPSENDLCFG=");

    if (!ATCommand_AppendArgumentInt(pRequestCommand, report_size, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentInt(pRequestCommand, transmit_size, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentString(pRequestCommand, ATCOMMAND_CRLF, ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }

    if (!StephanoI_SendRequest(pRequestCommand))
    {
        return false;
    }
    return StephanoI_WaitForConfirm(StephanoI_GetTimeout(StephanoI_Timeout_General), StephanoI_CNFStatus_Success);
}

bool StephanoI_ATSocket_EnableTCPSSLServer(bool multiple_connections, bool mode, uint16_t param2, StephanoI_ATSocket_Type_t type, bool CA_enable)
{
    if ((false == multiple_connections) && (mode == true))
    {
        /* Server can be only started if multiple connections are enabled */
        return false;
    }

    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+CIPSERVER=");

    if (!ATCommand_AppendArgumentInt(pRequestCommand, mode ? (uint32_t)1 : (uint32_t)0, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentInt(pRequestCommand, param2, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), mode ? ATCOMMAND_ARGUMENT_DELIM : ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }
    if (mode)
    {
        switch (type)
        {
            case StephanoI_ATSocket_Type_TCP:
            {
                if (!ATCommand_AppendArgumentStringQuotationMarks(pRequestCommand, "TCP", ATCOMMAND_ARGUMENT_DELIM))
                {
                    return false;
                }
            }
            break;
            case StephanoI_ATSocket_Type_TCPv6:
            {
                if (!ATCommand_AppendArgumentStringQuotationMarks(pRequestCommand, "TCPv6", ATCOMMAND_ARGUMENT_DELIM))
                {
                    return false;
                }
            }
            break;
            case StephanoI_ATSocket_Type_SSL:
            {
                if (!ATCommand_AppendArgumentStringQuotationMarks(pRequestCommand, "SSL", ATCOMMAND_ARGUMENT_DELIM))
                {
                    return false;
                }
            }
            break;
            case StephanoI_ATSocket_Type_SSLv6:
            {
                if (!ATCommand_AppendArgumentStringQuotationMarks(pRequestCommand, "SSLv6", ATCOMMAND_ARGUMENT_DELIM))
                {
                    return false;
                }
            }
            break;
            default:
            {
                return false;
            }
        }
        if (!ATCommand_AppendArgumentInt(pRequestCommand, CA_enable ? (uint32_t)1 : (uint32_t)0, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_STRING_TERMINATE))
        {
            return false;
        }
    }
    if (!ATCommand_AppendArgumentString(pRequestCommand, ATCOMMAND_CRLF, ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }

    if (!StephanoI_SendRequest(pRequestCommand))
    {
        return false;
    }
    return StephanoI_WaitForConfirm(StephanoI_GetTimeout(StephanoI_Timeout_General), StephanoI_CNFStatus_Success);
}

bool StephanoI_ATSocket_SetTCPSSLServerMaxConnections(uint8_t max_num)
{
    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+CIPSERVERMAXCONN=");

    if (!ATCommand_AppendArgumentInt(pRequestCommand, max_num, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentString(pRequestCommand, ATCOMMAND_CRLF, ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }

    if (!StephanoI_SendRequest(pRequestCommand))
    {
        return false;
    }
    return StephanoI_WaitForConfirm(StephanoI_GetTimeout(StephanoI_Timeout_General), StephanoI_CNFStatus_Success);
}

bool StephanoI_ATSocket_GetTCPSSLServerMaxConnections(uint8_t* max_num)
{
    char responsebuffer[6];

    if (!StephanoI_SendRequest("AT+CIPSERVERMAXCONN?\r\n"))
    {
        return false;
    }

    if (!StephanoI_WaitForConfirm_ex(StephanoI_GetTimeout(StephanoI_Timeout_General), StephanoI_CNFStatus_Success, responsebuffer, sizeof(responsebuffer)))
    {
        return false;
    }

    return StephanoI_ATSocket_ParseTCPSSLServerMaxConnections(responsebuffer, max_num);
}

bool StephanoI_ATSocket_SetTCPSSLServerTimeout(uint16_t time)
{
    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+CIPSTO=");

    if (!ATCommand_AppendArgumentInt(pRequestCommand, time, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentString(pRequestCommand, ATCOMMAND_CRLF, ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }

    if (!StephanoI_SendRequest(pRequestCommand))
    {
        return false;
    }
    return StephanoI_WaitForConfirm(StephanoI_GetTimeout(StephanoI_Timeout_General), StephanoI_CNFStatus_Success);
}

bool StephanoI_ATSocket_GetTCPSSLServerTimeout(uint16_t* time)
{
    char responsebuffer[6];

    if (!StephanoI_SendRequest("AT+CIPSTO?\r\n"))
    {
        return false;
    }

    if (!StephanoI_WaitForConfirm_ex(StephanoI_GetTimeout(StephanoI_Timeout_General), StephanoI_CNFStatus_Success, responsebuffer, sizeof(responsebuffer)))
    {
        return false;
    }

    return StephanoI_ATSocket_ParseTCPSSLServerTimeout(responsebuffer, time);
}

bool StephanoI_ATSocket_EnableSNTPServer(bool enable, StephanoI_ATSocket_TimeZone_t timezone, char* server1)
{
    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+CIPSNTPCFG=");

    if (!ATCommand_AppendArgumentInt(pRequestCommand, enable ? (uint32_t)1 : (uint32_t)0, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentInt(pRequestCommand, (int32_t)timezone, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_SIGNED), ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentStringQuotationMarks(pRequestCommand, server1, ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentString(pRequestCommand, ATCOMMAND_CRLF, ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }

    if (!StephanoI_SendRequest(pRequestCommand))
    {
        return false;
    }
    return StephanoI_WaitForConfirm(StephanoI_GetTimeout(StephanoI_Timeout_SocketPing), StephanoI_CNFStatus_Success);
}

bool StephanoI_ATSocket_ReadSNTPTime(StephanoI_ATSocket_SNTP_Time_t* t)
{
    char responsebuffer[32];

    if (!StephanoI_SendRequest("AT+CIPSNTPTIME?\r\n"))
    {
        return false;
    }

    if (!StephanoI_WaitForConfirm_ex(StephanoI_GetTimeout(StephanoI_Timeout_SocketPing), StephanoI_CNFStatus_Success, responsebuffer, sizeof(responsebuffer)))
    {
        return false;
    }

    return StephanoI_ATSocket_ParseSNTPTime(responsebuffer, t);
}

bool StephanoI_ATSocket_SetSNTPTimeInterval(uint32_t time)
{
    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+CIPSNTPINTV=");

    if (!ATCommand_AppendArgumentInt(pRequestCommand, time, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentString(pRequestCommand, ATCOMMAND_CRLF, ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }

    if (!StephanoI_SendRequest(pRequestCommand))
    {
        return false;
    }
    return StephanoI_WaitForConfirm(StephanoI_GetTimeout(StephanoI_Timeout_General), StephanoI_CNFStatus_Success);
}

bool StephanoI_ATSocket_GetSNTPTimeInterval(uint32_t* time)
{
    char responsebuffer[16];
    if (!StephanoI_SendRequest("AT+CIPSNTPINTV?\r\n"))
    {
        return false;
    }
    if (!StephanoI_WaitForConfirm_ex(StephanoI_GetTimeout(StephanoI_Timeout_General), StephanoI_CNFStatus_Success, responsebuffer, sizeof(responsebuffer)))
    {
        return false;
    }

    return StephanoI_ATSocket_ParseSNTPTimeInterval(responsebuffer, time);
}

bool StephanoI_ATSocket_SetSSLClient(bool multiple_connections, StephanoI_ATSocket_SSLClient_t client)
{
    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+CIPSSLCCONF=");

    if (multiple_connections)
    {
        /* AT+CIPMUX=1 */
        if (!ATCommand_AppendArgumentInt(pRequestCommand, client.link_ID, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_ARGUMENT_DELIM))
        {
            return false;
        }
    }
    if (client.auth_mode == StephanoI_ATSocket_SSLAuthMode_NoAuth)
    {
        if (!ATCommand_AppendArgumentInt(pRequestCommand, (uint32_t)client.auth_mode, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_STRING_TERMINATE))
        {
            return false;
        }
    }
    else
    {
        if (!ATCommand_AppendArgumentInt(pRequestCommand, (uint32_t)client.auth_mode, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_ARGUMENT_DELIM))
        {
            return false;
        }
        if (!ATCommand_AppendArgumentInt(pRequestCommand, client.pki_number, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_ARGUMENT_DELIM))
        {
            return false;
        }
        if (!ATCommand_AppendArgumentInt(pRequestCommand, client.ca_number, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_STRING_TERMINATE))
        {
            return false;
        }
    }

    if (!ATCommand_AppendArgumentString(pRequestCommand, ATCOMMAND_CRLF, ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }

    if (!StephanoI_SendRequest(pRequestCommand))
    {
        return false;
    }
    return StephanoI_WaitForConfirm(StephanoI_GetTimeout(StephanoI_Timeout_General), StephanoI_CNFStatus_Success);
}

bool StephanoI_ATSocket_GetSSLClient()
{
    if (!StephanoI_SendRequest("AT+CIPSSLCCONF?\r\n"))
    {
        return false;
    }
    return StephanoI_WaitForConfirm(StephanoI_GetTimeout(StephanoI_Timeout_General), StephanoI_CNFStatus_Success);
}

bool StephanoI_ATSocket_SetSSLClientName(bool multiple_connections, StephanoI_ATSocket_SSLClientName_t name)
{
    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+CIPSSLCCN=");

    if (multiple_connections)
    {
        /* AT+CIPMUX=1 */
        if (!ATCommand_AppendArgumentInt(pRequestCommand, name.link_ID, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_ARGUMENT_DELIM))
        {
            return false;
        }
    }
    if (!ATCommand_AppendArgumentStringQuotationMarks(pRequestCommand, name.common_name, ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentString(pRequestCommand, ATCOMMAND_CRLF, ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }

    if (!StephanoI_SendRequest(pRequestCommand))
    {
        return false;
    }
    return StephanoI_WaitForConfirm(StephanoI_GetTimeout(StephanoI_Timeout_General), StephanoI_CNFStatus_Success);
}

bool StephanoI_ATSocket_GetSSLClientName(StephanoI_ATSocket_SSLClientName_t* name)
{
    char responsebuffer[sizeof(StephanoI_ATSocket_SSLClientName_t) + 2];
    if (!StephanoI_SendRequest("AT+CIPSSLCCN?\r\n"))
    {
        return false;
    }
    if (!StephanoI_WaitForConfirm_ex(StephanoI_GetTimeout(StephanoI_Timeout_General), StephanoI_CNFStatus_Success, responsebuffer, sizeof(responsebuffer)))
    {
        return false;
    }

    return StephanoI_ATSocket_ParseSSLClientName(responsebuffer, name);
}

bool StephanoI_ATSocket_SetSSLClientServerNameIndication(bool multiple_connections, StephanoI_ATSocket_SSLClientServerNameIndication_t name)
{
    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+CIPSSLCSNI=");

    if (multiple_connections)
    {
        /* AT+CIPMUX=1 */
        if (!ATCommand_AppendArgumentInt(pRequestCommand, name.link_ID, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_ARGUMENT_DELIM))
        {
            return false;
        }
    }
    if (!ATCommand_AppendArgumentStringQuotationMarks(pRequestCommand, name.sni, ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentString(pRequestCommand, ATCOMMAND_CRLF, ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }

    if (!StephanoI_SendRequest(pRequestCommand))
    {
        return false;
    }
    return StephanoI_WaitForConfirm(StephanoI_GetTimeout(StephanoI_Timeout_General), StephanoI_CNFStatus_Success);
}

bool StephanoI_ATSocket_GetSSLClientServerNameIndication(StephanoI_ATSocket_SSLClientServerNameIndication_t* name)
{
    char responsebuffer[sizeof(StephanoI_ATSocket_SSLClientServerNameIndication_t) + 2];
    if (!StephanoI_SendRequest("AT+CIPSSLCSNI?\r\n"))
    {
        return false;
    }
    if (!StephanoI_WaitForConfirm_ex(StephanoI_GetTimeout(StephanoI_Timeout_General), StephanoI_CNFStatus_Success, responsebuffer, sizeof(responsebuffer)))
    {
        return false;
    }

    return StephanoI_ATSocket_ParseSSLClientServerNameIndication(responsebuffer, name);
}

bool StephanoI_ATSocket_SetSSLClientALPN(bool multiple_connections, StephanoI_ATSocket_SSLClientALPN_t alpn)
{
    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+CIPSSLCALPN=");

    if (multiple_connections)
    {
        /* AT+CIPMUX=1 */
        if (!ATCommand_AppendArgumentInt(pRequestCommand, alpn.link_ID, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_ARGUMENT_DELIM))
        {
            return false;
        }
    }
    if (!ATCommand_AppendArgumentInt(pRequestCommand, alpn.counts, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentStringQuotationMarks(pRequestCommand, alpn.alpn, ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentString(pRequestCommand, ATCOMMAND_CRLF, ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }

    if (!StephanoI_SendRequest(pRequestCommand))
    {
        return false;
    }
    return StephanoI_WaitForConfirm(StephanoI_GetTimeout(StephanoI_Timeout_General), StephanoI_CNFStatus_Success);
}

bool StephanoI_ATSocket_GetSSLClientALPN(StephanoI_ATSocket_SSLClientALPN_t* alpn)
{
    char responsebuffer[sizeof(StephanoI_ATSocket_SSLClientALPN_t) + 2];
    if (!StephanoI_SendRequest("AT+CIPSSLCALPN?\r\n"))
    {
        return false;
    }
    if (!StephanoI_WaitForConfirm_ex(StephanoI_GetTimeout(StephanoI_Timeout_General), StephanoI_CNFStatus_Success, responsebuffer, sizeof(responsebuffer)))
    {
        return false;
    }

    return StephanoI_ATSocket_ParseSSLALPN(responsebuffer, alpn);
}

bool StephanoI_ATSocket_SetSSLClientPSK(bool multiple_connections, StephanoI_ATSocket_SSLClientPSK_t psk)
{
    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+CIPSSLCPSK=");

    if (multiple_connections)
    {
        /* AT+CIPMUX=1 */
        if (!ATCommand_AppendArgumentInt(pRequestCommand, psk.link_ID, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_ARGUMENT_DELIM))
        {
            return false;
        }
    }
    if (!ATCommand_AppendArgumentStringQuotationMarks(pRequestCommand, psk.psk, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentStringQuotationMarks(pRequestCommand, psk.hint, ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentString(pRequestCommand, ATCOMMAND_CRLF, ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }

    if (!StephanoI_SendRequest(pRequestCommand))
    {
        return false;
    }
    return StephanoI_WaitForConfirm(StephanoI_GetTimeout(StephanoI_Timeout_General), StephanoI_CNFStatus_Success);
}

bool StephanoI_ATSocket_GetSSLClientPSK(StephanoI_ATSocket_SSLClientPSK_t* psk)
{
    char responsebuffer[sizeof(StephanoI_ATSocket_SSLClientPSK_t) + 2];
    if (!StephanoI_SendRequest("AT+CIPSSLCPSK?\r\n"))
    {
        return false;
    }
    if (!StephanoI_WaitForConfirm_ex(StephanoI_GetTimeout(StephanoI_Timeout_General), StephanoI_CNFStatus_Success, responsebuffer, sizeof(responsebuffer)))
    {
        return false;
    }

    return StephanoI_ATSocket_ParseSSLPSK(responsebuffer, psk);
}

bool StephanoI_ATSocket_SetDNSServerInformation(bool manual, char* DNS_server)
{
    char* pRequestCommand = AT_commandBuffer;

    if (manual)
    {
        strcpy(pRequestCommand, "AT+CIPDNS=");
        if (!ATCommand_AppendArgumentInt(pRequestCommand, manual ? (uint32_t)1 : (uint32_t)0, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_ARGUMENT_DELIM))
        {
            return false;
        }
        if (!ATCommand_AppendArgumentStringQuotationMarks(pRequestCommand, DNS_server, ATCOMMAND_STRING_TERMINATE))
        {
            return false;
        }
        if (!ATCommand_AppendArgumentString(pRequestCommand, ATCOMMAND_CRLF, ATCOMMAND_STRING_TERMINATE))
        {
            return false;
        }
    }
    else
    {
        strcpy(pRequestCommand, "AT+CIPDNS=0\r\n");
    }

    if (!StephanoI_SendRequest(pRequestCommand))
    {
        return false;
    }
    return StephanoI_WaitForConfirm(StephanoI_GetTimeout(StephanoI_Timeout_SocketPing), StephanoI_CNFStatus_Success);
}

bool StephanoI_ATSocket_SetReceiveMode(StephanoI_ATSocket_ReceiveMode_t mode)
{
    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+CIPRECVMODE=");

    if (!ATCommand_AppendArgumentInt(pRequestCommand, (uint32_t)mode, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentString(pRequestCommand, ATCOMMAND_CRLF, ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }

    if (!StephanoI_SendRequest(pRequestCommand))
    {
        return false;
    }
    return StephanoI_WaitForConfirm(StephanoI_GetTimeout(StephanoI_Timeout_General), StephanoI_CNFStatus_Success);
}

bool StephanoI_ATSocket_GetReceiveMode(StephanoI_ATSocket_ReceiveMode_t* modeP)
{
    char responsebuffer[32];
    if (!StephanoI_SendRequest("AT+CIPRECVMODE?\r\n"))
    {
        return false;
    }
    if (!StephanoI_WaitForConfirm_ex(StephanoI_GetTimeout(StephanoI_Timeout_General), StephanoI_CNFStatus_Success, responsebuffer, sizeof(responsebuffer)))
    {
        return false;
    }

    return StephanoI_ATSocket_ParseReceiveMode(responsebuffer, modeP);
}

bool StephanoI_ATSocket_GetReceiveLength(StephanoI_ATSocket_ReceiveLen_t* t)
{
    char responsebuffer[32];
    if (!StephanoI_SendRequest("AT+CIPRECVLEN?\r\n"))
    {
        return false;
    }

    if (!StephanoI_WaitForConfirm_ex(StephanoI_GetTimeout(StephanoI_Timeout_General), StephanoI_CNFStatus_Success, responsebuffer, sizeof(responsebuffer)))
    {
        return false;
    }

    return StephanoI_ATSocket_ParseReceiveLen(responsebuffer, t);
}

bool StephanoI_ATSocket_GetReceivedData(bool multiple_connections, uint8_t link_ID, uint16_t len, StephanoI_ATSocket_ReceiveData_t* dataP)
{
    char responsebuffer[len + 32];
    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+CIPRECVDATA=");

    if (multiple_connections)
    {
        /* AT+CIPMUX=1 */
        if (!ATCommand_AppendArgumentInt(pRequestCommand, link_ID, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_ARGUMENT_DELIM))
        {
            return false;
        }
    }

    if (!ATCommand_AppendArgumentInt(pRequestCommand, len, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentString(pRequestCommand, ATCOMMAND_CRLF, ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }

    if (!StephanoI_SendRequest(pRequestCommand))
    {
        return false;
    }

    if (!StephanoI_WaitForConfirm_ex(StephanoI_GetTimeout(StephanoI_Timeout_General), StephanoI_CNFStatus_Success, responsebuffer, sizeof(responsebuffer)))
    {
        return false;
    }

    return StephanoI_ATSocket_ParseReceiveData(responsebuffer, dataP);
}

bool StephanoI_ATSocket_ParseState(char* EventArgumentsP, StephanoI_ATSocket_State_t* t)
{
    char* argumentsP = EventArgumentsP;
    if (!ATCommand_GetNextArgumentInt(&argumentsP, &(t->link_ID), ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_GetNextArgumentString(&argumentsP, t->type, ATCOMMAND_ARGUMENT_DELIM, sizeof(t->type)))
    {
        return false;
    }
    if (!ATCommand_GetNextArgumentString(&argumentsP, t->remote_ip, ATCOMMAND_ARGUMENT_DELIM, sizeof(t->remote_ip)))
    {
        return false;
    }
    if (!ATCommand_GetNextArgumentInt(&argumentsP, &(t->remote_port), ATCOMMAND_INTFLAGS_SIZE32 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_GetNextArgumentInt(&argumentsP, &(t->local_port), ATCOMMAND_INTFLAGS_SIZE32 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_GetNextArgumentInt(&argumentsP, &(t->tetype), ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }

    return true;
}

bool StephanoI_ATSocket_ParsePing(char* EventArgumentsP, StephanoI_ATSocket_Ping_t* t)
{
    char* argumentsP = EventArgumentsP;
    if (0 == strcmp(argumentsP, "TIMEOUT"))
    {
        *t = 0xFFFFFFFF; /* in case of timeout set value to maximum */
        return true;
    }
    else
    {
        return ATCommand_GetNextArgumentInt(&argumentsP, t, ATCOMMAND_INTFLAGS_SIZE32 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_STRING_TERMINATE);
    }
}

bool StephanoI_ATSocket_ParseDomainResolved(char* EventArgumentsP, StephanoI_ATSocket_DomainResolved_t* t)
{
    char* argumentsP = EventArgumentsP;
    return ATCommand_GetNextArgumentString(&argumentsP, *t, ATCOMMAND_STRING_TERMINATE, sizeof(StephanoI_ATSocket_DomainResolved_t));
}

bool StephanoI_ATSocket_ParseSendState(char* EventArgumentsP, StephanoI_ATSocket_SendState_t* t)
{
    char* argumentsP = EventArgumentsP;
    if (!ATCommand_GetNextArgumentInt(&argumentsP, &(t->had_sent_len), ATCOMMAND_INTFLAGS_SIZE32 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_GetNextArgumentInt(&argumentsP, &(t->port_rcv_len), ATCOMMAND_INTFLAGS_SIZE32 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }

    return true;
}

bool StephanoI_ATSocket_ParseReceive(char* EventArgumentsP, bool multiple_connections, StephanoI_ATSocket_Receive_t* t) // +IPD,<link ID>,<len>:data or +IPD,<len>:data
{
    char* argumentsP = EventArgumentsP;
    if (multiple_connections)
    {
        if (!ATCommand_GetNextArgumentInt(&argumentsP, &(t->link_ID), ATCOMMAND_INTFLAGS_SIZE32 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_ARGUMENT_DELIM))
        {
            return false;
        }

        if (!ATCommand_GetNextArgumentInt(&argumentsP, &(t->length), ATCOMMAND_INTFLAGS_SIZE32 | ATCOMMAND_INTFLAGS_UNSIGNED, ':'))
        {
            return false;
        }

        if (!ATCommand_GetNextArgumentByteArray(&argumentsP, t->length, t->data, sizeof(t->data)))
        {
            return false;
        }
    }

    return true;
}

bool StephanoI_ATSocket_ParseReceiveData(char* EventArgumentsP, StephanoI_ATSocket_ReceiveData_t* t)
{
    char* argumentsP = EventArgumentsP;
    if (!ATCommand_GetNextArgumentInt(&argumentsP, &(t->actual_len), ATCOMMAND_INTFLAGS_SIZE32 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_GetNextArgumentByteArray(&argumentsP, t->actual_len, t->data, sizeof(t->data)))
    {
        return false;
    }

    return true;
}

bool StephanoI_ATSocket_ParseReceiveLen(char* EventArgumentsP, StephanoI_ATSocket_ReceiveLen_t* t)
{
    char* argumentsP = EventArgumentsP;
    if (!ATCommand_GetNextArgumentInt(&argumentsP, &(t->datalen_link0), ATCOMMAND_INTFLAGS_SIZE16 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_GetNextArgumentInt(&argumentsP, &(t->datalen_link1), ATCOMMAND_INTFLAGS_SIZE16 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_GetNextArgumentInt(&argumentsP, &(t->datalen_link2), ATCOMMAND_INTFLAGS_SIZE16 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_GetNextArgumentInt(&argumentsP, &(t->datalen_link3), ATCOMMAND_INTFLAGS_SIZE16 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_GetNextArgumentInt(&argumentsP, &(t->datalen_link4), ATCOMMAND_INTFLAGS_SIZE16 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }

    return true;
}

bool StephanoI_ATSocket_ParseGetMultiple(char* EventArgumentsP, bool* b)
{
    char* argumentsP = EventArgumentsP;
    uint8_t mode = 0;
    if (!ATCommand_GetNextArgumentInt(&argumentsP, &mode, ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }
    *b = (mode == 0x01);
    return true;
}

bool StephanoI_ATSocket_ParseSNTPTime(char* EventArgumentsP, StephanoI_ATSocket_SNTP_Time_t* t)
{
    char* argumentsP = EventArgumentsP;
    return ATCommand_GetNextArgumentString(&argumentsP, *t, ATCOMMAND_STRING_TERMINATE, sizeof(StephanoI_ATSocket_SNTP_Time_t));
}

bool StephanoI_ATSocket_ParseSNTPTimeInterval(char* EventArgumentsP, uint32_t* t)
{
    char* argumentsP = EventArgumentsP;
    return ATCommand_GetNextArgumentInt(&argumentsP, t, ATCOMMAND_INTFLAGS_SIZE32 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_STRING_TERMINATE);
}

bool StephanoI_ATSocket_ParseReceiveMode(char* EventArgumentsP, StephanoI_ATSocket_ReceiveMode_t* b)
{
    char* argumentsP = EventArgumentsP;
    uint8_t mode;
    if (!ATCommand_GetNextArgumentInt(&argumentsP, &mode, ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }
    *b = (StephanoI_ATSocket_ReceiveMode_t)mode;
    return true;
}

bool StephanoI_ATSocket_ParseEnableIPv6(char* EventArgumentsP, bool* t)
{
    char* argumentsP = EventArgumentsP;
    uint8_t mode = 0;
    if (!ATCommand_GetNextArgumentInt(&argumentsP, &mode, ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }
    *t = (mode == 0x01);
    return true;
}

bool StephanoI_ATSocket_ParseTCPSSLServerMaxConnections(char* EventArgumentsP, uint8_t* t)
{
    char* argumentsP = EventArgumentsP;
    return ATCommand_GetNextArgumentInt(&argumentsP, t, ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_STRING_TERMINATE);
}

bool StephanoI_ATSocket_ParseTCPSSLServerTimeout(char* EventArgumentsP, uint16_t* t)
{
    char* argumentsP = EventArgumentsP;
    return ATCommand_GetNextArgumentInt(&argumentsP, t, ATCOMMAND_INTFLAGS_SIZE16 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_STRING_TERMINATE);
}

bool StephanoI_ATSocket_ParseSSLClient(char* EventArgumentsP, StephanoI_ATSocket_SSLClient_t* t)
{
    char* argumentsP = EventArgumentsP;
    if (!ATCommand_GetNextArgumentInt(&argumentsP, &(t->link_ID), ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_GetNextArgumentInt(&argumentsP, &(t->auth_mode), ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_GetNextArgumentInt(&argumentsP, &(t->pki_number), ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_GetNextArgumentInt(&argumentsP, &(t->ca_number), ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }

    return true;
}

bool StephanoI_ATSocket_ParseSSLClientName(char* EventArgumentsP, StephanoI_ATSocket_SSLClientName_t* t)
{
    char* argumentsP = EventArgumentsP;
    if (!ATCommand_GetNextArgumentInt(&argumentsP, &(t->link_ID), ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_GetNextArgumentString(&argumentsP, t->common_name, ATCOMMAND_STRING_TERMINATE, 64))
    {
        return false;
    }

    return true;
}

bool StephanoI_ATSocket_ParseSSLClientServerNameIndication(char* EventArgumentsP, StephanoI_ATSocket_SSLClientServerNameIndication_t* t)
{
    char* argumentsP = EventArgumentsP;
    if (!ATCommand_GetNextArgumentInt(&argumentsP, &(t->link_ID), ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_GetNextArgumentString(&argumentsP, t->sni, ATCOMMAND_STRING_TERMINATE, 64))
    {
        return false;
    }

    return true;
}

bool StephanoI_ATSocket_ParseSSLALPN(char* EventArgumentsP, StephanoI_ATSocket_SSLClientALPN_t* t)
{
    char* argumentsP = EventArgumentsP;
    if (!ATCommand_GetNextArgumentInt(&argumentsP, &(t->link_ID), ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    t->counts = 1;

    if (!ATCommand_GetNextArgumentString(&argumentsP, t->alpn, ATCOMMAND_STRING_TERMINATE, 64))
    {
        return false;
    }

    return true;
}

bool StephanoI_ATSocket_ParseSSLPSK(char* EventArgumentsP, StephanoI_ATSocket_SSLClientPSK_t* t)
{
    char* argumentsP = EventArgumentsP;
    if (!ATCommand_GetNextArgumentInt(&argumentsP, &(t->link_ID), ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_GetNextArgumentString(&argumentsP, t->psk, ATCOMMAND_ARGUMENT_DELIM, 32))
    {
        return false;
    }
    if (!ATCommand_GetNextArgumentString(&argumentsP, t->hint, ATCOMMAND_STRING_TERMINATE, 32))
    {
        return false;
    }

    return true;
}
