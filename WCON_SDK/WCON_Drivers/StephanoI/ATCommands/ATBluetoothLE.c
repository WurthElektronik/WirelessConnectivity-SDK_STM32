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
 * @file ATBluetoothLE.c
 * @brief AT commands for Bluetooth LE functionality.
 */

#include <StephanoI/ATCommands/ATBluetoothLE.h>
#include <StephanoI/StephanoI.h>
#include <global/ATCommands.h>

bool StephanoI_ATBluetoothLE_Init(StephanoI_ATBluetoothLE_InitType_t type)
{
    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+BLEINIT=");

    if (!ATCommand_AppendArgumentInt(pRequestCommand, (uint32_t)type, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_STRING_TERMINATE))
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

bool StephanoI_ATBluetoothLE_SetDeviceName(StephanoI_ATBluetoothLE_DeviceName_t dev_name)
{
    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+BLENAME=");

    if (!ATCommand_AppendArgumentStringQuotationMarks(pRequestCommand, dev_name, ATCOMMAND_STRING_TERMINATE))
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

bool StephanoI_ATBluetoothLE_GetDeviceName(StephanoI_ATBluetoothLE_DeviceName_t* dev_name)
{
    char responsebuffer[sizeof(StephanoI_ATBluetoothLE_DeviceName_t) + 1];
    if (!StephanoI_SendRequest("AT+BLENAME?\r\n"))
    {
        return false;
    }
    if (!StephanoI_WaitForConfirm_ex(StephanoI_GetTimeout(StephanoI_Timeout_General), StephanoI_CNFStatus_Success, responsebuffer, sizeof(responsebuffer)))
    {
        return false;
    }

    return StephanoI_ATBluetoothLE_ParseDeviceName(responsebuffer, dev_name);
}

bool StephanoI_ATBluetoothLE_SetDeviceID(StephanoI_ATBluetoothLE_Address_t type, StephanoI_ATBluetoothLE_DeviceID_t ID)
{
    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+BLEADDR=");

    if ((ID == NULL) || (type == StephanoI_ATBluetoothLE_Address_Public))
    {
        if (!ATCommand_AppendArgumentInt(pRequestCommand, (uint32_t)type, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_STRING_TERMINATE))
        {
            return false;
        }
    }
    else
    {
        if (!ATCommand_AppendArgumentInt(pRequestCommand, (uint32_t)type, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_ARGUMENT_DELIM))
        {
            return false;
        }
        if (!ATCommand_AppendArgumentStringQuotationMarks(pRequestCommand, ID, ATCOMMAND_STRING_TERMINATE))
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

bool StephanoI_ATBluetoothLE_GetDeviceID(StephanoI_ATBluetoothLE_DeviceID_t* ID)
{
    char responsebuffer[sizeof(StephanoI_ATBluetoothLE_DeviceID_t) + 1];
    if (!StephanoI_SendRequest("AT+BLEADDR?\r\n"))
    {
        return false;
    }
    if (!StephanoI_WaitForConfirm_ex(StephanoI_GetTimeout(StephanoI_Timeout_General), StephanoI_CNFStatus_Success, responsebuffer, sizeof(responsebuffer)))
    {
        return false;
    }

    return StephanoI_ATBluetoothLE_ParseDeviceID(responsebuffer, ID);
}

bool StephanoI_ATBluetoothLE_SetAdvertisingData(StephanoI_ATBluetoothLE_AdvertisingData_t data)
{
    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+BLEADVDATAEX=");

    if (!ATCommand_AppendArgumentStringQuotationMarks(pRequestCommand, data.devname, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentStringQuotationMarks(pRequestCommand, data.uuid, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentStringQuotationMarks(pRequestCommand, data.manufacturerdata, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentInt(pRequestCommand, data.include_power ? (uint32_t)1 : (uint32_t)0, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_STRING_TERMINATE))
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

bool StephanoI_ATBluetoothLE_GetAdvertisingData(StephanoI_ATBluetoothLE_AdvertisingData_t* data)
{
    char responsebuffer[sizeof(StephanoI_ATBluetoothLE_AdvertisingData_t) + 1];
    if (!StephanoI_SendRequest("AT+BLEADVDATAEX?\r\n"))
    {
        return false;
    }
    if (!StephanoI_WaitForConfirm_ex(StephanoI_GetTimeout(StephanoI_Timeout_General), StephanoI_CNFStatus_Success, responsebuffer, sizeof(responsebuffer)))
    {
        return false;
    }

    return StephanoI_ATBluetoothLE_ParseAdvertisingData(responsebuffer, data);
}

bool StephanoI_ATBluetoothLE_SetAdvertisingParameters(StephanoI_ATBluetoothLE_AdvertisingParameters_t params)
{
    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+BLEADVPARAM=");

    if (!ATCommand_AppendArgumentInt(pRequestCommand, params.min_interval, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentInt(pRequestCommand, params.max_interval, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentInt(pRequestCommand, (uint32_t)params.type, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentInt(pRequestCommand, (uint32_t)params.addr_type, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentInt(pRequestCommand, (uint32_t)params.channel, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentInt(pRequestCommand, (uint32_t)params.filter, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_STRING_TERMINATE))
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

bool StephanoI_ATBluetoothLE_GetAdvertisingParameters(StephanoI_ATBluetoothLE_AdvertisingParameters_t* paramsP)
{
    char responsebuffer[64];
    if (!StephanoI_SendRequest("AT+BLEADVPARAM?\r\n"))
    {
        return false;
    }
    if (!StephanoI_WaitForConfirm_ex(StephanoI_GetTimeout(StephanoI_Timeout_General), StephanoI_CNFStatus_Success, responsebuffer, sizeof(responsebuffer)))
    {
        return false;
    }

    return StephanoI_ATBluetoothLE_ParseAdvertisingParameters(responsebuffer, paramsP);
}

bool StephanoI_ATBluetoothLE_SetScanResponseRawData(StephanoI_ATBluetoothLE_AdvertisingRawData_t data)
{
    char* pRequestCommand = AT_commandBuffer;
    strcpy(pRequestCommand, "AT+BLESCANRSPDATA=");

    if (!ATCommand_AppendArgumentStringQuotationMarks(pRequestCommand, data, ATCOMMAND_STRING_TERMINATE))
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

bool StephanoI_ATBluetoothLE_SetAdvertisingRawData(StephanoI_ATBluetoothLE_AdvertisingRawData_t data)
{
    char* pRequestCommand = AT_commandBuffer;
    strcpy(pRequestCommand, "AT+BLEADVDATA=");

    if (!ATCommand_AppendArgumentStringQuotationMarks(pRequestCommand, data, ATCOMMAND_STRING_TERMINATE))
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

bool StephanoI_ATBluetoothLE_StartAdvertising()
{
    if (!StephanoI_SendRequest("AT+BLEADVSTART\r\n"))
    {
        return false;
    }
    return StephanoI_WaitForConfirm(StephanoI_GetTimeout(StephanoI_Timeout_General), StephanoI_CNFStatus_Success);
}

bool StephanoI_ATBluetoothLE_StopAdvertising()
{
    if (!StephanoI_SendRequest("AT+BLEADVSTOP\r\n"))
    {
        return false;
    }
    return StephanoI_WaitForConfirm(StephanoI_GetTimeout(StephanoI_Timeout_General), StephanoI_CNFStatus_Success);
}

bool StephanoI_ATBluetoothLE_SetScanParameters(StephanoI_ATBluetoothLE_ScanParameters_t params)
{
    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+BLESCANPARAM=");

    if (!ATCommand_AppendArgumentInt(pRequestCommand, (uint32_t)params.type, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentInt(pRequestCommand, (uint32_t)params.addr_type, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentInt(pRequestCommand, (uint32_t)params.filter, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentInt(pRequestCommand, (uint32_t)params.scan_interval, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentInt(pRequestCommand, (uint32_t)params.scan_window, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_STRING_TERMINATE))
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

bool StephanoI_ATBluetoothLE_GetScanParameters(StephanoI_ATBluetoothLE_ScanParameters_t* paramsP)
{
    char responsebuffer[64];
    if (!StephanoI_SendRequest("AT+BLESCANPARAM?\r\n"))
    {
        return false;
    }
    if (!StephanoI_WaitForConfirm_ex(StephanoI_GetTimeout(StephanoI_Timeout_General), StephanoI_CNFStatus_Success, responsebuffer, sizeof(responsebuffer)))
    {
        return false;
    }

    return StephanoI_ATBluetoothLE_ParseScanParameters(responsebuffer, paramsP);
}

bool StephanoI_ATBluetoothLE_SetSecurityParameters(StephanoI_ATBluetoothLE_SecurityParameters_t params)
{
    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+BLESECPARAM=");

    if (!ATCommand_AppendArgumentInt(pRequestCommand, (uint32_t)params.auth, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentInt(pRequestCommand, (uint32_t)params.io, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentInt(pRequestCommand, (uint32_t)params.key_size, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentInt(pRequestCommand, (uint32_t)params.init_key, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentInt(pRequestCommand, (uint32_t)params.rsp_key, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentInt(pRequestCommand, (uint32_t)(params.auth_option ? 1 : 0), (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_STRING_TERMINATE))
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

bool StephanoI_ATBluetoothLE_GetSecurityParameters(StephanoI_ATBluetoothLE_SecurityParameters_t* paramsP)
{
    char responsebuffer[24];
    if (!StephanoI_SendRequest("AT+BLESECPARAM?\r\n"))
    {
        return false;
    }
    if (!StephanoI_WaitForConfirm_ex(StephanoI_GetTimeout(StephanoI_Timeout_General), StephanoI_CNFStatus_Success, responsebuffer, sizeof(responsebuffer)))
    {
        return false;
    }

    return StephanoI_ATBluetoothLE_ParseSecurityParameters(responsebuffer, paramsP);
}

bool StephanoI_ATBluetoothLE_SetStaticKey(StephanoI_ATBluetoothLE_Key_t key)
{
    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+BLESETKEY=");

    if (!ATCommand_AppendArgumentString(pRequestCommand, (char*)key, ATCOMMAND_STRING_TERMINATE))
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

bool StephanoI_ATBluetoothLE_GetStaticKey(StephanoI_ATBluetoothLE_Key_t* keyP)
{
    char responsebuffer[10];
    if (!StephanoI_SendRequest("AT+BLESETKEY?\r\n"))
    {
        return false;
    }
    if (!StephanoI_WaitForConfirm_ex(StephanoI_GetTimeout(StephanoI_Timeout_General), StephanoI_CNFStatus_Success, responsebuffer, sizeof(responsebuffer)))
    {
        return false;
    }

    return StephanoI_ATBluetoothLE_ParseStaticKey(responsebuffer, keyP);
}

bool StephanoI_ATBluetoothLE_RespondPairingRequest(int8_t conn_index, bool accept)
{
    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+BLEENCRSP=");

    if (!ATCommand_AppendArgumentInt(pRequestCommand, conn_index, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_SIGNED), ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentInt(pRequestCommand, (uint32_t)(accept ? 1 : 0), (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_STRING_TERMINATE))
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

bool StephanoI_ATBluetoothLE_InitiateEncryption(int8_t conn_index, StephanoI_ATBluetoothLE_InitiateEncryption_t sec_act)
{
    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+BLEENC=");

    if (!ATCommand_AppendArgumentInt(pRequestCommand, conn_index, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_SIGNED), ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentInt(pRequestCommand, sec_act, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_STRING_TERMINATE))
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

bool StephanoI_ATBluetoothLE_ReplyKey(int8_t conn_index, char* key)
{
    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+BLEKEYREPLY=");

    if (!ATCommand_AppendArgumentInt(pRequestCommand, conn_index, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_SIGNED), ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentString(pRequestCommand, key, ATCOMMAND_STRING_TERMINATE))
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

bool StephanoI_ATBluetoothLE_ConfirmValue(int8_t conn_index, bool confirm)
{
    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+BLECONFREPLY=");

    if (!ATCommand_AppendArgumentInt(pRequestCommand, conn_index, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_SIGNED), ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentInt(pRequestCommand, (uint32_t)(confirm ? 1 : 0), (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_STRING_TERMINATE))
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

bool StephanoI_ATBluetoothLE_BondsGet()
{
    if (!StephanoI_SendRequest("AT+BLEENCDEV?\r\n"))
    {
        return false;
    }
    return StephanoI_WaitForConfirm(StephanoI_GetTimeout(StephanoI_Timeout_General), StephanoI_CNFStatus_Success);
}

bool StephanoI_ATBluetoothLE_BondsDelete(int8_t bonding_index)
{
    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+BLEENCCLEAR");

    if (bonding_index >= 0)
    {
        if (!ATCommand_AppendArgumentString(pRequestCommand, "=", ATCOMMAND_STRING_TERMINATE))
        {
            return false;
        }
        if (!ATCommand_AppendArgumentInt(pRequestCommand, bonding_index, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_SIGNED), ATCOMMAND_STRING_TERMINATE))
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

bool StephanoI_ATBluetoothLE_SetDatalen(int8_t conn_index, uint16_t length)
{
    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+BLEDATALEN=");

    if (!ATCommand_AppendArgumentInt(pRequestCommand, conn_index, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_SIGNED), ATCOMMAND_ARGUMENT_DELIM))
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

    if (!StephanoI_SendRequest(pRequestCommand))
    {
        return false;
    }
    return StephanoI_WaitForConfirm(StephanoI_GetTimeout(StephanoI_Timeout_General), StephanoI_CNFStatus_Success);
}

bool StephanoI_ATBluetoothLE_CentralSetMTU(StephanoI_ATBluetoothLE_MTU_t data)
{
    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+BLECFGMTU=");

    if (!ATCommand_AppendArgumentInt(pRequestCommand, data.conn_index, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_SIGNED), ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentInt(pRequestCommand, data.MTU, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_STRING_TERMINATE))
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

bool StephanoI_ATBluetoothLE_CentralGetMTU(StephanoI_ATBluetoothLE_MTU_t* data)
{
    char responsebuffer[sizeof(StephanoI_ATBluetoothLE_MTU_t) + 1];
    if (!StephanoI_SendRequest("AT+BLECFGMTU?\r\n"))
    {
        return false;
    }
    if (!StephanoI_WaitForConfirm_ex(StephanoI_GetTimeout(StephanoI_Timeout_General), StephanoI_CNFStatus_Success, responsebuffer, sizeof(responsebuffer)))
    {
        return false;
    }

    return StephanoI_ATBluetoothLE_ParseMTU(responsebuffer, data);
}

bool StephanoI_ATBluetoothLE_CentralSetConnectionParameters(StephanoI_ATBluetoothLE_Central_ConnectionParameters_t param)
{
    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+BLECONNPARAM=");

    if (!ATCommand_AppendArgumentInt(pRequestCommand, param.conn_index, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_SIGNED), ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentInt(pRequestCommand, param.min_interval, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentInt(pRequestCommand, param.max_interval, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentInt(pRequestCommand, param.latency, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentInt(pRequestCommand, param.timeout, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_STRING_TERMINATE))
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

bool StephanoI_ATBluetoothLE_CentralGetConnectionParameters(StephanoI_ATBluetoothLE_Central_ConnectionParameters_t* paramP, uint16_t* current_intervalP)
{
    char responsebuffer[20];
    if (!StephanoI_SendRequest("AT+BLECONNPARAM?\r\n"))
    {
        return false;
    }
    if (!StephanoI_WaitForConfirm_ex(StephanoI_GetTimeout(StephanoI_Timeout_General), StephanoI_CNFStatus_Success, responsebuffer, sizeof(responsebuffer)))
    {
        return false;
    }

    return StephanoI_ATBluetoothLE_ParseConnectionParameters(responsebuffer, paramP, current_intervalP);
}

bool StephanoI_ATBluetoothLE_SetConnectionPhy(int8_t conn_index, StephanoI_ATBluetoothLE_Phy_t tx_rx_phy)
{
    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+BLESETPHY=");

    if (!ATCommand_AppendArgumentInt(pRequestCommand, conn_index, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_SIGNED), ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentInt(pRequestCommand, tx_rx_phy, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_STRING_TERMINATE))
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

bool StephanoI_ATBluetoothLE_GetConnectionPhy(int8_t conn_index, StephanoI_ATBluetoothLE_ReadPhy_t* tx_rx_phy)
{
    char responsebuffer[sizeof(StephanoI_ATBluetoothLE_ReadPhy_t) + 1];
    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+BLEREADPHY=");

    if (!ATCommand_AppendArgumentInt(pRequestCommand, conn_index, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_SIGNED), ATCOMMAND_STRING_TERMINATE))
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

    return StephanoI_ATBluetoothLE_ParseReadPhy(responsebuffer, tx_rx_phy);
}

bool StephanoI_ATBluetoothLE_Peripheral_ServiceCreate()
{
    if (!StephanoI_SendRequest("AT+BLEGATTSSRVCRE\r\n"))
    {
        return false;
    }
    return StephanoI_WaitForConfirm(StephanoI_GetTimeout(StephanoI_Timeout_General), StephanoI_CNFStatus_Success);
}

bool StephanoI_ATBluetoothLE_Peripheral_ServiceStart(int8_t srv_index)
{
    char* pRequestCommand = AT_commandBuffer;

    if (srv_index >= 0)
    {
        strcpy(pRequestCommand, "AT+BLEGATTSSRVSTART=");

        if (!ATCommand_AppendArgumentInt(pRequestCommand, srv_index, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_STRING_TERMINATE))
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
        /* start all */
        strcpy(pRequestCommand, "AT+BLEGATTSSRVSTART\r\n");
    }

    if (!StephanoI_SendRequest(pRequestCommand))
    {
        return false;
    }
    return StephanoI_WaitForConfirm(StephanoI_GetTimeout(StephanoI_Timeout_General), StephanoI_CNFStatus_Success);
}

bool StephanoI_ATBluetoothLE_Peripheral_ServiceStop(int8_t srv_index)
{
    char* pRequestCommand = AT_commandBuffer;

    if (srv_index >= 0)
    {
        strcpy(pRequestCommand, "AT+BLEGATTSSRVSTOP=");

        if (!ATCommand_AppendArgumentInt(pRequestCommand, srv_index, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_STRING_TERMINATE))
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
        /* stop all */
        strcpy(pRequestCommand, "AT+BLEGATTSSRVSTOP\r\n");
    }

    if (!StephanoI_SendRequest(pRequestCommand))
    {
        return false;
    }
    return StephanoI_WaitForConfirm(StephanoI_GetTimeout(StephanoI_Timeout_General), StephanoI_CNFStatus_Success);
}

bool StephanoI_ATBluetoothLE_Peripheral_DiscoverServices()
{
    if (!StephanoI_SendRequest("AT+BLEGATTSSRV?\r\n"))
    {
        return false;
    }
    return StephanoI_WaitForConfirm(StephanoI_GetTimeout(StephanoI_Timeout_General), StephanoI_CNFStatus_Success);
}

bool StephanoI_ATBluetoothLE_Peripheral_DiscoverCharacteristics()
{
    if (!StephanoI_SendRequest("AT+BLEGATTSCHAR?\r\n"))
    {
        return false;
    }
    return StephanoI_WaitForConfirm(StephanoI_GetTimeout(StephanoI_Timeout_General), StephanoI_CNFStatus_Success);
}

bool StephanoI_ATBluetoothLE_PeripheralNotifyClient(int8_t conn_index, uint8_t srv_index, uint8_t char_index, uint8_t* data, uint32_t length)
{
    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+BLEGATTSNTFY=");

    if (!ATCommand_AppendArgumentInt(pRequestCommand, conn_index, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_SIGNED), ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentInt(pRequestCommand, srv_index, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentInt(pRequestCommand, char_index, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_ARGUMENT_DELIM))
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

    if (!StephanoI_SendRequest(pRequestCommand))
    {
        return false;
    }

    if (!StephanoI_WaitForConfirm(StephanoI_GetTimeout(StephanoI_Timeout_General), StephanoI_CNFStatus_Ready4Data))
    {
        return false;
    }
    if (!StephanoI_SendRequest_ex(data, length))
    {
        return false;
    }
    return StephanoI_WaitForConfirm(StephanoI_GetTimeout(StephanoI_Timeout_General), StephanoI_CNFStatus_Success);
}

bool StephanoI_ATBluetoothLE_PeripheralIndicateClient(int8_t conn_index, uint8_t srv_index, uint8_t char_index, uint8_t* data, uint32_t length)
{
    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+BLEGATTSIND=");

    if (!ATCommand_AppendArgumentInt(pRequestCommand, conn_index, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_SIGNED), ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentInt(pRequestCommand, srv_index, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentInt(pRequestCommand, char_index, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_ARGUMENT_DELIM))
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

    if (!StephanoI_SendRequest(pRequestCommand))
    {
        return false;
    }

    if (!StephanoI_WaitForConfirm(StephanoI_GetTimeout(StephanoI_Timeout_General), StephanoI_CNFStatus_Ready4Data))
    {
        return false;
    }

    if (!StephanoI_SendRequest_ex(data, length))
    {
        return false;
    }
    return StephanoI_WaitForConfirm(StephanoI_GetTimeout(StephanoI_Timeout_General), StephanoI_CNFStatus_Success);
}

bool StephanoI_ATBluetoothLE_CentralScan(bool enable, int8_t interval, StephanoI_ATBluetoothLE_ScanFilter_t filter_type, char* filter_param)
{
    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+BLESCAN=");

    if (!ATCommand_AppendArgumentInt(pRequestCommand, enable ? (uint32_t)1 : (uint32_t)0, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), (true == enable) ? ATCOMMAND_ARGUMENT_DELIM : ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }
    if (true == enable)
    {
        if (!ATCommand_AppendArgumentInt(pRequestCommand, interval, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), (filter_type != StephanoI_ATBluetoothLE_ScanFilter_Invalid) ? ATCOMMAND_ARGUMENT_DELIM : ATCOMMAND_STRING_TERMINATE))
        {
            return false;
        }
        if (filter_type != StephanoI_ATBluetoothLE_ScanFilter_Invalid)
        {
            if (!ATCommand_AppendArgumentInt(pRequestCommand, filter_type, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_ARGUMENT_DELIM))
            {
                return false;
            }
            if (!ATCommand_AppendArgumentStringQuotationMarks(pRequestCommand, filter_param, ATCOMMAND_STRING_TERMINATE))
            {
                return false;
            }
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

bool StephanoI_ATBluetoothLE_CentralConnect(int8_t conn_index, char* remote_address, bool public_address, uint8_t timeout)
{
    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+BLECONN=");

    if (!ATCommand_AppendArgumentInt(pRequestCommand, conn_index, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_SIGNED), ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentStringQuotationMarks(pRequestCommand, remote_address, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentInt(pRequestCommand, public_address ? (uint32_t)0 : (uint32_t)1, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentInt(pRequestCommand, timeout, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_STRING_TERMINATE))
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
    return StephanoI_WaitForConfirm(timeout * 1000 + 500, StephanoI_CNFStatus_Success);
}

bool StephanoI_ATBluetoothLE_Disconnect(int8_t conn_index)
{
    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+BLEDISCONN=");

    if (!ATCommand_AppendArgumentInt(pRequestCommand, conn_index, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_SIGNED), ATCOMMAND_STRING_TERMINATE))
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

bool StephanoI_ATBluetoothLE_CentralDiscoverPrimaryService(int8_t conn_index)
{
    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+BLEGATTCPRIMSRV=");

    if (!ATCommand_AppendArgumentInt(pRequestCommand, conn_index, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_SIGNED), ATCOMMAND_STRING_TERMINATE))
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

bool StephanoI_ATBluetoothLE_CentralDiscoverIncludedServices(int8_t conn_index, uint8_t srv_index)
{
    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+BLEGATTCINCLSRV=");

    if (!ATCommand_AppendArgumentInt(pRequestCommand, conn_index, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_SIGNED), ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_AppendArgumentInt(pRequestCommand, srv_index, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_STRING_TERMINATE))
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

bool StephanoI_ATBluetoothLE_CentralDiscoverCharacteristics(int8_t conn_index, uint8_t srv_index)
{
    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+BLEGATTCCHAR=");

    if (!ATCommand_AppendArgumentInt(pRequestCommand, conn_index, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_SIGNED), ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_AppendArgumentInt(pRequestCommand, srv_index, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_STRING_TERMINATE))
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

bool StephanoI_ATBluetoothLE_CentralWrite(int8_t conn_index, uint8_t srv_index, uint8_t char_index, int8_t desc_index, uint8_t* data, uint32_t length)
{
    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+BLEGATTCWR=");

    if (!ATCommand_AppendArgumentInt(pRequestCommand, conn_index, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_SIGNED), ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentInt(pRequestCommand, srv_index, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentInt(pRequestCommand, char_index, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (desc_index >= 0)
    {
        if (!ATCommand_AppendArgumentInt(pRequestCommand, desc_index, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_SIGNED), ATCOMMAND_ARGUMENT_DELIM))
        {
            return false;
        }
    }
    if (!ATCommand_AppendArgumentInt(pRequestCommand, length, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_STRING_TERMINATE))
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
    if (!StephanoI_WaitForConfirm(StephanoI_GetTimeout(StephanoI_Timeout_General), StephanoI_CNFStatus_Ready4Data))
    {
        return false;
    }

    if (!StephanoI_SendRequest_ex(data, length))
    {
        return false;
    }
    return StephanoI_WaitForConfirm(StephanoI_GetTimeout(StephanoI_Timeout_General), StephanoI_CNFStatus_Success);
}

bool StephanoI_ATBluetoothLE_CentralRead(int8_t conn_index, uint8_t srv_index, uint8_t char_index, int8_t desc_index, StephanoI_ATBluetoothLE_Read_t* t)
{
    char responsebuffer[sizeof(StephanoI_ATBluetoothLE_Read_t) + 1];
    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+BLEGATTCRD=");

    if (!ATCommand_AppendArgumentInt(pRequestCommand, conn_index, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_SIGNED), ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentInt(pRequestCommand, srv_index, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentInt(pRequestCommand, char_index, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), (desc_index >= 0) ? ATCOMMAND_ARGUMENT_DELIM : ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }
    if (desc_index >= 0)
    {
        if (!ATCommand_AppendArgumentInt(pRequestCommand, desc_index, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_SIGNED), ATCOMMAND_STRING_TERMINATE))
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

    if (!StephanoI_WaitForConfirm_ex(StephanoI_GetTimeout(StephanoI_Timeout_General), StephanoI_CNFStatus_Success, responsebuffer, sizeof(responsebuffer)))
    {
        return false;
    }

    return StephanoI_ATBluetoothLE_ParseRead(responsebuffer, t);
}

bool StephanoI_ATBluetoothLE_ParseConnection(char* EventArgumentsP, StephanoI_ATBluetoothLE_Connection_t* t)
{
    char* argumentsP = EventArgumentsP;

    if (!ATCommand_GetNextArgumentInt(&argumentsP, &(t->conn_index), ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_SIGNED, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_GetNextArgumentString(&argumentsP, t->remote_address, ATCOMMAND_STRING_TERMINATE, sizeof(t->remote_address)))
    {
        return false;
    }

    return true;
}

bool StephanoI_ATBluetoothLE_ParseMTU(char* EventArgumentsP, StephanoI_ATBluetoothLE_MTU_t* t)
{
    char* argumentsP = EventArgumentsP;

    if (!ATCommand_GetNextArgumentInt(&argumentsP, &(t->conn_index), ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_SIGNED, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_GetNextArgumentInt(&argumentsP, &(t->MTU), ATCOMMAND_INTFLAGS_SIZE16 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }

    return true;
}

bool StephanoI_ATBluetoothLE_ParseWrite(char* EventArgumentsP, StephanoI_ATBluetoothLE_Write_t* t)
{
    char* argumentsP = EventArgumentsP;

    if (!ATCommand_GetNextArgumentInt(&argumentsP, &(t->conn_index), ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_SIGNED, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_GetNextArgumentInt(&argumentsP, &(t->srv_index), ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_GetNextArgumentInt(&argumentsP, &(t->char_index), ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (argumentsP[0] == ',')
    {
        /* entry is empty */
        t->desc = 0;
        argumentsP++;
    }
    else
    {
        if (!ATCommand_GetNextArgumentInt(&argumentsP, &(t->desc), ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_ARGUMENT_DELIM))
        {
            return false;
        }
    }

    if (!ATCommand_GetNextArgumentInt(&argumentsP, &(t->length), ATCOMMAND_INTFLAGS_SIZE16 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_GetNextArgumentByteArray(&argumentsP, t->length, t->data, sizeof(t->data)))
    {
        return false;
    }

    return true;
}

bool StephanoI_ATBluetoothLE_ParseRead(char* EventArgumentsP, StephanoI_ATBluetoothLE_Read_t* t)
{
    char* argumentsP = EventArgumentsP;

    if (!ATCommand_GetNextArgumentInt(&argumentsP, &(t->conn_index), ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_SIGNED, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_GetNextArgumentInt(&argumentsP, &(t->length), ATCOMMAND_INTFLAGS_SIZE16 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_GetNextArgumentByteArray(&argumentsP, t->length, t->data, sizeof(t->data)))
    {
        return false;
    }

    return true;
}

bool StephanoI_ATBluetoothLE_ParseScan(char* EventArgumentsP, StephanoI_ATBluetoothLE_Central_Scan_t* t)
{
    char* argumentsP = EventArgumentsP;

    if (!ATCommand_GetNextArgumentString(&argumentsP, t->remote_address, ATCOMMAND_ARGUMENT_DELIM, sizeof(t->remote_address)))
    {
        return false;
    }

    if (!ATCommand_GetNextArgumentInt(&argumentsP, &(t->rssi), ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_SIGNED, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_GetNextArgumentString(&argumentsP, t->adv_data, ATCOMMAND_ARGUMENT_DELIM, sizeof(t->adv_data)))
    {
        return false;
    }

    if (!ATCommand_GetNextArgumentString(&argumentsP, t->scanrsp_data, ATCOMMAND_ARGUMENT_DELIM, sizeof(t->scanrsp_data)))
    {
        return false;
    }

    if (!ATCommand_GetNextArgumentInt(&argumentsP, &(t->address_type), ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }

    return true;
}

bool StephanoI_ATBluetoothLE_ParseAuthenticationComplete(char* EventArgumentsP, StephanoI_ATBluetoothLE_AuthenticationComplete_t* t)
{
    char* argumentsP = EventArgumentsP;

    if (!ATCommand_GetNextArgumentInt(&argumentsP, &(t->conn_index), ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_SIGNED, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    uint8_t result;
    if (!ATCommand_GetNextArgumentInt(&argumentsP, &result, ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }
    t->success = (result == 0x00);

    return true;
}

bool StephanoI_ATBluetoothLE_ParseEncryptionRequest(char* EventArgumentsP, StephanoI_ATBluetoothLE_EncryptionRequest_t* t)
{
    char* argumentsP = EventArgumentsP;
    return ATCommand_GetNextArgumentInt(&argumentsP, &(t->conn_index), ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_SIGNED, ATCOMMAND_STRING_TERMINATE);
}

bool StephanoI_ATBluetoothLE_ParseSecurityKeyRequest(char* EventArgumentsP, StephanoI_ATBluetoothLE_SecurityKeyRequest_t* t)
{
    char* argumentsP = EventArgumentsP;
    return ATCommand_GetNextArgumentInt(&argumentsP, &(t->conn_index), ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_SIGNED, ATCOMMAND_STRING_TERMINATE);
}

bool StephanoI_ATBluetoothLE_ParsePeripheralDiscoverService(char* EventArgumentsP, StephanoI_ATBluetoothLE_Peripheral_DiscoverService_t* t)
{
    char* argumentsP = EventArgumentsP;

    if (!ATCommand_GetNextArgumentInt(&argumentsP, &(t->srv_index), ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_GetNextArgumentInt(&argumentsP, &(t->start), ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_GetNextArgumentString(&argumentsP, t->srv_uuid, ATCOMMAND_ARGUMENT_DELIM, sizeof(t->srv_uuid)))
    {
        return false;
    }

    if (!ATCommand_GetNextArgumentInt(&argumentsP, &(t->srv_type), ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }

    return true;
}

bool StephanoI_ATBluetoothLE_ParsePeripheralDiscoverCharacteristics(char* EventArgumentsP, StephanoI_ATBluetoothLE_Peripheral_DiscoverCharacteristics_t* t)
{
    char* argumentsP = EventArgumentsP;

    if (!ATCommand_GetNextArgumentStringWithoutQuotationMarks(&argumentsP, t->characteristics_type, ATCOMMAND_ARGUMENT_DELIM, sizeof(t->characteristics_type)))
    {
        return false;
    }

    if (!ATCommand_GetNextArgumentInt(&argumentsP, &(t->srv_index), ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_GetNextArgumentInt(&argumentsP, &(t->char_index), ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (0 == strcmp(t->characteristics_type, "char"))
    {
        if (!ATCommand_GetNextArgumentString(&argumentsP, t->char_uuid, ATCOMMAND_ARGUMENT_DELIM, sizeof(t->char_uuid)))
        {
            return false;
        }

        if (!ATCommand_GetNextArgumentInt(&argumentsP, &(t->char_prop), ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_HEX, ATCOMMAND_STRING_TERMINATE))
        {
            return false;
        }
    }
    else if (0 == strcmp(t->characteristics_type, "desc"))
    {
        if (!ATCommand_GetNextArgumentInt(&argumentsP, &(t->desc_index), ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_ARGUMENT_DELIM))
        {
            return false;
        }

        if (!ATCommand_GetNextArgumentString(&argumentsP, t->desc_uuid, ATCOMMAND_STRING_TERMINATE, sizeof(t->desc_uuid)))
        {
            return false;
        }
    }
    else
    {
        return false;
    }
    return true;
}

bool StephanoI_ATBluetoothLE_ParseCentralDiscoverPrimaryService(char* EventArgumentsP, StephanoI_ATBluetoothLE_Central_DiscoverPrimaryService_t* t)
{
    char* argumentsP = EventArgumentsP;

    if (!ATCommand_GetNextArgumentInt(&argumentsP, &(t->conn_index), ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_GetNextArgumentInt(&argumentsP, &(t->srv_index), ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_GetNextArgumentString(&argumentsP, t->srv_uuid, ATCOMMAND_ARGUMENT_DELIM, sizeof(t->srv_uuid)))
    {
        return false;
    }

    if (!ATCommand_GetNextArgumentInt(&argumentsP, &(t->srv_type), ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }

    return true;
}

bool StephanoI_ATBluetoothLE_ParseCentralDiscoverIncludedServices(char* EventArgumentsP, StephanoI_ATBluetoothLE_Central_DiscoverIncludedServices_t* t)
{
    char* argumentsP = EventArgumentsP;

    if (!ATCommand_GetNextArgumentInt(&argumentsP, &(t->conn_index), ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_GetNextArgumentInt(&argumentsP, &(t->srv_index), ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_GetNextArgumentString(&argumentsP, t->srv_uuid, ATCOMMAND_ARGUMENT_DELIM, sizeof(t->srv_uuid)))
    {
        return false;
    }

    if (!ATCommand_GetNextArgumentInt(&argumentsP, &(t->srv_type), ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_GetNextArgumentString(&argumentsP, t->included_srv_uuid, ATCOMMAND_ARGUMENT_DELIM, sizeof(t->included_srv_uuid)))
    {
        return false;
    }

    if (!ATCommand_GetNextArgumentInt(&argumentsP, &(t->included_srv_type), ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }

    return true;
}

bool StephanoI_ATBluetoothLE_ParseCentralDiscoverCharacteristics(char* EventArgumentsP, StephanoI_ATBluetoothLE_Central_DiscoverCharacteristics_t* t)
{
    char* argumentsP = EventArgumentsP;

    if (!ATCommand_GetNextArgumentString(&argumentsP, t->characteristics_type, ATCOMMAND_ARGUMENT_DELIM, sizeof(t->characteristics_type)))
    {
        return false;
    }
    if (!ATCommand_GetNextArgumentInt(&argumentsP, &(t->conn_index), ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_GetNextArgumentInt(&argumentsP, &(t->srv_index), ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_GetNextArgumentInt(&argumentsP, &(t->char_index), ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (0 == strcmp(t->characteristics_type, "char"))
    {
        if (!ATCommand_GetNextArgumentString(&argumentsP, t->char_uuid, ATCOMMAND_ARGUMENT_DELIM, sizeof(t->char_uuid)))
        {
            return false;
        }
        if (!ATCommand_GetNextArgumentInt(&argumentsP, &(t->char_prop), ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_STRING_TERMINATE))
        {
            return false;
        }
    }
    else if (0 == strcmp(t->characteristics_type, "desc"))
    {
        if (!ATCommand_GetNextArgumentInt(&argumentsP, &(t->desc_index), ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_ARGUMENT_DELIM))
        {
            return false;
        }
        if (!ATCommand_GetNextArgumentString(&argumentsP, t->desc_uuid, ATCOMMAND_STRING_TERMINATE, sizeof(t->char_uuid)))
        {
            return false;
        }
    }
    else
    {
        return false;
    }
    return true;
}

bool StephanoI_ATBluetoothLE_ParseDeviceID(char* EventArgumentsP, StephanoI_ATBluetoothLE_DeviceID_t* t)
{
    char* argumentsP = EventArgumentsP;
    return ATCommand_GetNextArgumentStringWithoutQuotationMarks(&argumentsP, *t, ATCOMMAND_STRING_TERMINATE, sizeof(StephanoI_ATBluetoothLE_DeviceID_t));
}

bool StephanoI_ATBluetoothLE_ParseDeviceName(char* EventArgumentsP, StephanoI_ATBluetoothLE_DeviceName_t* t)
{
    char* argumentsP = EventArgumentsP;
    return ATCommand_GetNextArgumentString(&argumentsP, *t, ATCOMMAND_STRING_TERMINATE, sizeof(StephanoI_ATBluetoothLE_DeviceName_t));
}

bool StephanoI_ATBluetoothLE_ParseAdvertisingData(char* EventArgumentsP, StephanoI_ATBluetoothLE_AdvertisingData_t* t)
{
    char* argumentsP = EventArgumentsP;

    if (!ATCommand_GetNextArgumentStringWithoutQuotationMarks(&argumentsP, t->devname, ATCOMMAND_ARGUMENT_DELIM, sizeof(t->devname)))
    {
        return false;
    }
    if (!ATCommand_GetNextArgumentStringWithoutQuotationMarks(&argumentsP, t->uuid, ATCOMMAND_ARGUMENT_DELIM, sizeof(t->uuid)))
    {
        return false;
    }

    if (!ATCommand_GetNextArgumentStringWithoutQuotationMarks(&argumentsP, t->manufacturerdata, ATCOMMAND_ARGUMENT_DELIM, sizeof(t->manufacturerdata)))
    {
        return false;
    }

    uint8_t val;
    if (!ATCommand_GetNextArgumentInt(&argumentsP, &val, ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }
    t->include_power = ((val == 0x01) ? true : false);

    return true;
}

bool StephanoI_ATBluetoothLE_ParseAdvertisingParameters(char* EventArgumentsP, StephanoI_ATBluetoothLE_AdvertisingParameters_t* t)
{
    char* argumentsP = EventArgumentsP;

    if (!ATCommand_GetNextArgumentInt(&argumentsP, &(t->min_interval), ATCOMMAND_INTFLAGS_SIZE16 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_GetNextArgumentInt(&argumentsP, &(t->max_interval), ATCOMMAND_INTFLAGS_SIZE16 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_GetNextArgumentInt(&argumentsP, &(t->type), ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_GetNextArgumentInt(&argumentsP, &(t->addr_type), ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_GetNextArgumentInt(&argumentsP, &(t->channel), ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_GetNextArgumentInt(&argumentsP, &(t->filter), ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    return true;
}

bool StephanoI_ATBluetoothLE_ParseScanParameters(char* EventArgumentsP, StephanoI_ATBluetoothLE_ScanParameters_t* t)
{
    char* argumentsP = EventArgumentsP;

    if (!ATCommand_GetNextArgumentInt(&argumentsP, &(t->type), ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_GetNextArgumentInt(&argumentsP, &(t->addr_type), ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_GetNextArgumentInt(&argumentsP, &(t->filter), ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_GetNextArgumentInt(&argumentsP, &(t->scan_interval), ATCOMMAND_INTFLAGS_SIZE16 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_GetNextArgumentInt(&argumentsP, &(t->scan_window), ATCOMMAND_INTFLAGS_SIZE16 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }

    return true;
}

bool StephanoI_ATBluetoothLE_ParseReadPhy(char* EventArgumentsP, StephanoI_ATBluetoothLE_ReadPhy_t* t)
{
    char* argumentsP = EventArgumentsP;

    if (!ATCommand_GetNextArgumentStringWithoutQuotationMarks(&argumentsP, t->address, ATCOMMAND_ARGUMENT_DELIM, sizeof(t->address)))
    {
        return false;
    }
    if (!ATCommand_GetNextArgumentInt(&argumentsP, &(t->tx), ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_GetNextArgumentInt(&argumentsP, &(t->rx), ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }

    return true;
}

bool StephanoI_ATBluetoothLE_ParseConnectionParameters(char* EventArgumentsP, StephanoI_ATBluetoothLE_Central_ConnectionParameters_t* t, uint16_t* current_intervalP)
{
    char* argumentsP = EventArgumentsP;

    if (!ATCommand_GetNextArgumentInt(&argumentsP, &(t->conn_index), ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_GetNextArgumentInt(&argumentsP, &(t->min_interval), ATCOMMAND_INTFLAGS_SIZE16 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_GetNextArgumentInt(&argumentsP, &(t->max_interval), ATCOMMAND_INTFLAGS_SIZE16 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_GetNextArgumentInt(&argumentsP, current_intervalP, ATCOMMAND_INTFLAGS_SIZE16 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_GetNextArgumentInt(&argumentsP, &(t->latency), ATCOMMAND_INTFLAGS_SIZE16 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_GetNextArgumentInt(&argumentsP, &(t->timeout), ATCOMMAND_INTFLAGS_SIZE16 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }

    return true;
}

bool StephanoI_ATBluetoothLE_ParseSecurityParameters(char* EventArgumentsP, StephanoI_ATBluetoothLE_SecurityParameters_t* t)
{
    char* argumentsP = EventArgumentsP;

    if (!ATCommand_GetNextArgumentInt(&argumentsP, &(t->auth), ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_GetNextArgumentInt(&argumentsP, &(t->io), ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_GetNextArgumentInt(&argumentsP, &(t->key_size), ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_GetNextArgumentInt(&argumentsP, &(t->init_key), ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_GetNextArgumentInt(&argumentsP, &(t->rsp_key), ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    uint8_t val;
    if (!ATCommand_GetNextArgumentInt(&argumentsP, &val, ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, '\n'))
    {
        return false;
    }
    t->auth_option = (val == 0x01);

    return true;
}

bool StephanoI_ATBluetoothLE_ParseNotifyKey(char* EventArgumentsP, StephanoI_ATBluetoothLE_NotifyKey_t* t)
{
    char* argumentsP = EventArgumentsP;

    if (!ATCommand_GetNextArgumentInt(&argumentsP, &(t->conn_index), ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_SIGNED, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_GetNextArgumentString(&argumentsP, t->key, ATCOMMAND_STRING_TERMINATE, sizeof(t->key)))
    {
        return false;
    }

    return true;
}

bool StephanoI_ATBluetoothLE_ParseBondingInformations(char* EventArgumentsP, StephanoI_ATBluetoothLE_Bonding_t* t)
{
    char* argumentsP = EventArgumentsP;

    if (!ATCommand_GetNextArgumentInt(&argumentsP, &(t->enc_dev_index), ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_SIGNED, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_GetNextArgumentStringWithoutQuotationMarks(&argumentsP, t->mac, ATCOMMAND_STRING_TERMINATE, sizeof(t->mac)))
    {
        return false;
    }

    return true;
}

bool StephanoI_ATBluetoothLE_ParseStaticKey(char* EventArgumentsP, StephanoI_ATBluetoothLE_Key_t* t)
{
    char* argumentsP = EventArgumentsP;

    if (*EventArgumentsP == '-')
    {
        /* no key set */
        return false;
    }

    if (!ATCommand_GetNextArgumentString(&argumentsP, *t, '\n', sizeof(StephanoI_ATBluetoothLE_Key_t)))
    {
        return false;
    }

    return true;
}
