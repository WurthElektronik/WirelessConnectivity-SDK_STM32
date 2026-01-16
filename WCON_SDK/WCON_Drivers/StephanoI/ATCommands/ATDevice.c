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
 * @file ATDevice.c
 * @brief AT commands for basic device functionality.
 */

#include <StephanoI/ATCommands/ATDevice.h>
#include <StephanoI/StephanoI.h>
#include <global/ATCommands.h>

bool StephanoI_ATDevice_Test()
{
    if (!StephanoI_SendRequest("AT\r\n"))
    {
        return false;
    }
    return StephanoI_WaitForConfirm(StephanoI_GetTimeout(StephanoI_Timeout_General), StephanoI_CNFStatus_Success);
}

bool StephanoI_ATDevice_GetVersion()
{
    if (!StephanoI_SendRequest("AT+GMR\r\n"))
    {
        return false;
    }
    return StephanoI_WaitForConfirm(StephanoI_GetTimeout(StephanoI_Timeout_General), StephanoI_CNFStatus_Success);
}

bool StephanoI_ATDevice_Reset()
{
    if (!StephanoI_SendRequest("AT+RST\r\n"))
    {
        return false;
    }
    return StephanoI_WaitForConfirm(StephanoI_GetTimeout(StephanoI_Timeout_General), StephanoI_CNFStatus_Success);
}

bool StephanoI_ATDevice_Restore()
{
    if (!StephanoI_SendRequest("AT+RESTORE\r\n"))
    {
        return false;
    }
    return StephanoI_WaitForConfirm(StephanoI_GetTimeout(StephanoI_Timeout_FactoryReset), StephanoI_CNFStatus_Success);
}

bool StephanoI_ATDevice_SetSleep(StephanoI_ATDevice_SleepMode_t mode)
{
    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+SLEEP=");

    if (!ATCommand_AppendArgumentInt(pRequestCommand, mode, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_STRING_TERMINATE))
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

bool StephanoI_ATDevice_GetSleep(StephanoI_ATDevice_SleepMode_t* modeP)
{
    char responsebuffer[sizeof(StephanoI_ATDevice_SleepMode_t) + 1];
    if (!StephanoI_SendRequest("AT+SLEEP?\r\n"))
    {
        return false;
    }
    if (!StephanoI_WaitForConfirm_ex(StephanoI_GetTimeout(StephanoI_Timeout_General), StephanoI_CNFStatus_Success, responsebuffer, sizeof(responsebuffer)))
    {
        return false;
    }

    return StephanoI_ATDevice_ParseSleep(responsebuffer, modeP);
}

bool StephanoI_ATDevice_SetDeepSleep(uint32_t time_ms)
{
    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+GSLP=");

    if (!ATCommand_AppendArgumentInt(pRequestCommand, time_ms, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_STRING_TERMINATE))
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

bool StephanoI_ATDevice_SetWakeUpSource(StephanoI_ATDevice_WakeupPin_t pin_number, bool active_high)
{
    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+SLEEPWKCFG=");
    if (!ATCommand_AppendArgumentInt(pRequestCommand, (uint32_t)StephanoI_ATDevice_WakeupSource_GPIO, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_AppendArgumentInt(pRequestCommand, (uint32_t)pin_number, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentInt(pRequestCommand, (uint32_t)active_high ? 1 : 0, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_STRING_TERMINATE))
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

bool StephanoI_ATDevice_SetTXPower(StephanoI_ATDevice_WifiPower_t wifi_power, StephanoI_ATDevice_BluetoothLEPower_t ble_power)
{
    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+RFPOWER=");

    if (ble_power != StephanoI_ATDevice_BluetoothLEPower_None)
    {
        /* set wifi and Bluetooth LE tx power */
        if (!ATCommand_AppendArgumentInt(pRequestCommand, (uint32_t)wifi_power, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_ARGUMENT_DELIM))
        {
            return false;
        }
        if (!ATCommand_AppendArgumentInt(pRequestCommand, (int32_t)ble_power, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_SIGNED), ATCOMMAND_ARGUMENT_DELIM))
        {
            return false;
        }
        if (!ATCommand_AppendArgumentInt(pRequestCommand, (int32_t)ble_power, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_SIGNED), ATCOMMAND_ARGUMENT_DELIM))
        {
            return false;
        }
        if (!ATCommand_AppendArgumentInt(pRequestCommand, (int32_t)ble_power, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_SIGNED), ATCOMMAND_STRING_TERMINATE))
        {
            return false;
        }
    }
    else
    {
        /* set wifi tx power only */
        if (!ATCommand_AppendArgumentInt(pRequestCommand, (uint32_t)wifi_power, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_STRING_TERMINATE))
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

bool StephanoI_ATDevice_GetTXPower(StephanoI_ATDevice_WifiPower_t* wifi_powerP, StephanoI_ATDevice_BluetoothLEPower_t* ble_powerP)
{
    char responsebuffer[16];
    char* pEventArguments = &responsebuffer[0];
    if (!StephanoI_SendRequest("AT+RFPOWER?\r\n"))
    {
        return false;
    }

    if (!StephanoI_WaitForConfirm_ex(StephanoI_GetTimeout(StephanoI_Timeout_General), StephanoI_CNFStatus_Success, responsebuffer, sizeof(responsebuffer)))
    {
        return false;
    }

    /* Wifi power */
    if (!ATCommand_GetNextArgumentInt(&pEventArguments, wifi_powerP, ATCOMMAND_INTFLAGS_SIZE32 | ATCOMMAND_INTFLAGS_SIGNED, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    /* Advertising power */
    if (!ATCommand_GetNextArgumentInt(&pEventArguments, ble_powerP, ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_SIGNED, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    /* Scan power */
    if (!ATCommand_GetNextArgumentInt(&pEventArguments, ble_powerP, ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_SIGNED, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    /* Connection power */
    if (!ATCommand_GetNextArgumentInt(&pEventArguments, ble_powerP, ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_SIGNED, ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }

    return true;
}

bool StephanoI_ATDevice_EchoOn(bool on)
{
    if (on)
    {
        if (!StephanoI_SendRequest("ATE1\r\n"))
        {
            return false;
        }
    }
    else
    {
        if (!StephanoI_SendRequest("ATE0\r\n"))
        {
            return false;
        }
    }
    return StephanoI_WaitForConfirm(StephanoI_GetTimeout(StephanoI_Timeout_General), StephanoI_CNFStatus_Success);
}

bool StephanoI_ATDevice_SetCurrentUART(StephanoI_ATDevice_UART_t t)
{
    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+UART_CUR=");

    if (!ATCommand_AppendArgumentInt(pRequestCommand, t.baudrate, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentInt(pRequestCommand, 8, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentInt(pRequestCommand, 1, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentInt(pRequestCommand, t.parity, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_AppendArgumentInt(pRequestCommand, t.flow_control, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_STRING_TERMINATE))
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

bool StephanoI_ATDevice_SetDefaultUART(StephanoI_ATDevice_UART_t t)
{
    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+UART_DEF=");

    if (!ATCommand_AppendArgumentInt(pRequestCommand, t.baudrate, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentInt(pRequestCommand, 8, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentInt(pRequestCommand, 1, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentInt(pRequestCommand, t.parity, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_AppendArgumentInt(pRequestCommand, t.flow_control, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_STRING_TERMINATE))
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

bool StephanoI_ATDevice_GetDefaultUART(StephanoI_ATDevice_UART_t* t)
{
    char responsebuffer[16];
    char* pEventArguments = &responsebuffer[0];
    if (!StephanoI_SendRequest("AT+UART_DEF?\r\n"))
    {
        return false;
    }

    if (!StephanoI_WaitForConfirm_ex(StephanoI_GetTimeout(StephanoI_Timeout_General), StephanoI_CNFStatus_Success, responsebuffer, sizeof(responsebuffer)))
    {
        return false;
    }

    /* Baud rate */
    if (!ATCommand_GetNextArgumentInt(&pEventArguments, &(t->baudrate), ATCOMMAND_INTFLAGS_SIZE32 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    /* Data bits */

    uint8_t data;
    if (!ATCommand_GetNextArgumentInt(&pEventArguments, &data, ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    /* Stop bits */
    uint8_t stop;
    if (!ATCommand_GetNextArgumentInt(&pEventArguments, &stop, ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    /* Parity */
    if (!ATCommand_GetNextArgumentInt(&pEventArguments, &(t->parity), ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    /* Flow control */
    if (!ATCommand_GetNextArgumentInt(&pEventArguments, &(t->flow_control), ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }

    return true;
}

bool StephanoI_ATDevice_GetManufacturingUserPartitions()
{
    if (!StephanoI_SendRequest("AT+SYSMFG?\r\n"))
    {
        return false;
    }

    return StephanoI_WaitForConfirm(StephanoI_GetTimeout(StephanoI_Timeout_General), StephanoI_CNFStatus_Success);
}

bool StephanoI_ATDevice_EraseManufacturingUserPartitions(char* namespace, char* key)
{
    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+SYSMFG=0,");

    if (!ATCommand_AppendArgumentStringQuotationMarks(pRequestCommand, namespace, (key != NULL) ? ATCOMMAND_ARGUMENT_DELIM : ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }

    if (key != NULL)
    {
        if (!ATCommand_AppendArgumentStringQuotationMarks(pRequestCommand, key, ATCOMMAND_STRING_TERMINATE))
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

bool StephanoI_ATDevice_ReadManufacturingUserPartitions(char* namespace, char* key, uint16_t offset, uint16_t length, StephanoI_ATDevice_SYSMFG_t* t)
{
    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+SYSMFG=1,");

    if (!ATCommand_AppendArgumentStringQuotationMarks(pRequestCommand, namespace, (key != NULL) ? ATCOMMAND_ARGUMENT_DELIM : ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }

    if (key != NULL)
    {
        if (!ATCommand_AppendArgumentStringQuotationMarks(pRequestCommand, key, (length != 0) ? ATCOMMAND_ARGUMENT_DELIM : ATCOMMAND_STRING_TERMINATE))
        {
            return false;
        }

        if (length != 0)
        {
            if (!ATCommand_AppendArgumentInt(pRequestCommand, (int32_t)offset, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_ARGUMENT_DELIM))
            {
                return false;
            }
            if (!ATCommand_AppendArgumentInt(pRequestCommand, (int32_t)length, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_STRING_TERMINATE))
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

    if (t != NULL)
    {
        char* responsebuffer = malloc(length + 50);
        if (!StephanoI_WaitForConfirm_ex(StephanoI_GetTimeout(StephanoI_Timeout_General), StephanoI_CNFStatus_Success, responsebuffer, length + 50))
        {
            free(responsebuffer);
            return false;
        }

        bool ret = StephanoI_ATDevice_ParseManufacturingUserPartitions(responsebuffer, t);
        free(responsebuffer);
        return ret;
    }
    else
    {
        return StephanoI_WaitForConfirm(StephanoI_GetTimeout(StephanoI_Timeout_General), StephanoI_CNFStatus_Success);
    }
}

bool StephanoI_ATDevice_WriteManufacturingUserPartitions(char* namespace, char* key, StephanoI_ATDevice_SYSMFG_data_t type, uint16_t length, uint8_t* dataP)
{
    char* pRequestCommand = AT_commandBuffer;
    strcpy(pRequestCommand, "AT+SYSMFG=2,");

    switch (type)
    {
        case StephanoI_ATDevice_SYSMFG_data_string:
        case StephanoI_ATDevice_SYSMFG_data_binary:
        {
            /* go on */
        }
        break;
        case StephanoI_ATDevice_SYSMFG_data_u8:
        case StephanoI_ATDevice_SYSMFG_data_i8:
        case StephanoI_ATDevice_SYSMFG_data_u16:
        case StephanoI_ATDevice_SYSMFG_data_i16:
        case StephanoI_ATDevice_SYSMFG_data_u32:
        case StephanoI_ATDevice_SYSMFG_data_i32:
        default:
        {
            /* not implemented */
            return false;
        }
    }

    if (!ATCommand_AppendArgumentStringQuotationMarks(pRequestCommand, namespace, (key != NULL) ? ATCOMMAND_ARGUMENT_DELIM : ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }

    if (!ATCommand_AppendArgumentStringQuotationMarks(pRequestCommand, key, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_AppendArgumentInt(pRequestCommand, (int32_t)type, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_AppendArgumentInt(pRequestCommand, (int32_t)length, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_STRING_TERMINATE))
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

    if (!StephanoI_SendRequest_ex(dataP, length))
    {
        return false;
    }

    return StephanoI_WaitForConfirm(StephanoI_GetTimeout(StephanoI_Timeout_General), StephanoI_CNFStatus_Success);
}

bool StephanoI_ATDevice_ParseManufacturingUserPartitions(char* EventArgumentsP, StephanoI_ATDevice_SYSMFG_t* t)
{
    char* argumentsP = EventArgumentsP;
    int number_of_arguments = ATCommand_CountArgs(EventArgumentsP);
    t->length = 0;

    if (number_of_arguments == 1)
    {
        return ATCommand_GetNextArgumentStringWithoutQuotationMarks(&argumentsP, t->namespace, ATCOMMAND_STRING_TERMINATE, sizeof(t->namespace));
    }
    else
    {
        if (!ATCommand_GetNextArgumentStringWithoutQuotationMarks(&argumentsP, t->namespace, ATCOMMAND_ARGUMENT_DELIM, sizeof(t->namespace)))
        {
            return false;
        }
    }

    if (number_of_arguments == 2)
    {
        return ATCommand_GetNextArgumentStringWithoutQuotationMarks(&argumentsP, t->key, ATCOMMAND_STRING_TERMINATE, sizeof(t->key));
    }
    else
    {
        if (!ATCommand_GetNextArgumentStringWithoutQuotationMarks(&argumentsP, t->key, ATCOMMAND_ARGUMENT_DELIM, sizeof(t->key)))
        {
            return false;
        }
    }

    if (number_of_arguments == 3)
    {
        return ATCommand_GetNextArgumentInt(&argumentsP, &(t->type), ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_STRING_TERMINATE);
    }
    else
    {
        if (!ATCommand_GetNextArgumentInt(&argumentsP, &(t->type), ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_ARGUMENT_DELIM))
        {
            return false;
        }
    }

    if (number_of_arguments == 4)
    {
        return ATCommand_GetNextArgumentInt(&argumentsP, &(t->length), ATCOMMAND_INTFLAGS_SIZE16 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_STRING_TERMINATE);
    }
    else
    {
        if (!ATCommand_GetNextArgumentInt(&argumentsP, &(t->length), ATCOMMAND_INTFLAGS_SIZE16 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_ARGUMENT_DELIM))
        {
            return false;
        }
    }

    t->value = malloc(t->length);
    if (!ATCommand_GetNextArgumentByteArray(&argumentsP, t->length, t->value, t->length))
    {
        return false;
    }

    return true;
}

bool StephanoI_ATDevice_ParseSystemTimestamp(char* EventArgumentsP, uint32_t* t)
{
    char* argumentsP = EventArgumentsP;
    return ATCommand_GetNextArgumentInt(&argumentsP, t, ATCOMMAND_INTFLAGS_SIZE32 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_STRING_TERMINATE);
}

bool StephanoI_ATDevice_SetSystemTimestamp(uint32_t time)
{
    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+SYSTIMESTAMP=");

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

bool StephanoI_ATDevice_GetSystemTimestamp(uint32_t* timeP)
{
    char responsebuffer[16];
    if (!StephanoI_SendRequest("AT+SYSTIMESTAMP?\r\n"))
    {
        return false;
    }

    if (!StephanoI_WaitForConfirm_ex(StephanoI_GetTimeout(StephanoI_Timeout_General), StephanoI_CNFStatus_Success, responsebuffer, sizeof(responsebuffer)))
    {
        return false;
    }

    return StephanoI_ATDevice_ParseSystemTimestamp(responsebuffer, timeP);
}

bool StephanoI_ATDevice_SetSystemStoremode(bool enable)
{
    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+SYSSTORE=");

    if (!ATCommand_AppendArgumentInt(pRequestCommand, enable ? 1 : 0, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_STRING_TERMINATE))
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

bool StephanoI_ATDevice_GetSystemStoremode(bool* enableP)
{
    char responsebuffer[16];
    if (!StephanoI_SendRequest("AT+SYSSTORE?\r\n"))
    {
        return false;
    }

    if (!StephanoI_WaitForConfirm_ex(StephanoI_GetTimeout(StephanoI_Timeout_General), StephanoI_CNFStatus_Success, responsebuffer, sizeof(responsebuffer)))
    {
        return false;
    }

    return StephanoI_ATDevice_ParseSystemStoremode(responsebuffer, enableP);
}

bool StephanoI_ATDevice_GetSystemTemp(float* tempP)
{
    char responsebuffer[16];
    if (!StephanoI_SendRequest("AT+SYSTEMP?\r\n"))
    {
        return false;
    }

    if (!StephanoI_WaitForConfirm_ex(StephanoI_GetTimeout(StephanoI_Timeout_General), StephanoI_CNFStatus_Success, responsebuffer, sizeof(responsebuffer)))
    {
        return false;
    }

    return StephanoI_ATDevice_ParseSystemTemp(responsebuffer, tempP);
}

bool StephanoI_ATDevice_FileSystemSetMount(bool enable)
{
    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+FSMOUNT=");

    if (!ATCommand_AppendArgumentInt(pRequestCommand, enable ? 1 : 0, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_STRING_TERMINATE))
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

bool StephanoI_ATDevice_FileSystemGetFiles()
{
    if (!StephanoI_SendRequest("AT+FS=0,4,\".\"\r\n"))
    {
        return false;
    }
    return StephanoI_WaitForConfirm(StephanoI_GetTimeout(StephanoI_Timeout_General), StephanoI_CNFStatus_Success);
}

bool StephanoI_ATDevice_FileSystemDelete(char* filename)
{
    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+FS=0,0,");

    if (!ATCommand_AppendArgumentStringQuotationMarks(pRequestCommand, filename, ATCOMMAND_STRING_TERMINATE))
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

bool StephanoI_ATDevice_FileSystemGetFileSize(char* filename, uint16_t* sizeP)
{
    char* pRequestCommand = AT_commandBuffer;
    char responsebuffer[32];

    strcpy(pRequestCommand, "AT+FS=0,3,");

    if (!ATCommand_AppendArgumentStringQuotationMarks(pRequestCommand, filename, ATCOMMAND_STRING_TERMINATE))
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

    return StephanoI_ATDevice_ParseFileSystemGetFilesize(responsebuffer, sizeP);
}

bool StephanoI_ATDevice_FileSystemRead(char* filename, uint16_t offset, uint16_t length, StephanoI_ATDevice_FileSystemRead_t* t)
{
    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+FS=0,2,");

    if (!ATCommand_AppendArgumentStringQuotationMarks(pRequestCommand, filename, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_AppendArgumentInt(pRequestCommand, (int32_t)offset, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentInt(pRequestCommand, (int32_t)length, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_STRING_TERMINATE))
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

    if (t != NULL)
    {
        char* responsebuffer = malloc(length + 50);
        if (!StephanoI_WaitForConfirm_ex(StephanoI_GetTimeout(StephanoI_Timeout_General), StephanoI_CNFStatus_Success, responsebuffer, length + 50))
        {
            free(responsebuffer);
            return false;
        }

        bool ret = StephanoI_ATDevice_ParseFileSystemRead(responsebuffer, t);
        free(responsebuffer);
        return ret;
    }
    else
    {
        return StephanoI_WaitForConfirm(StephanoI_GetTimeout(StephanoI_Timeout_General), StephanoI_CNFStatus_Success);
    }
}

bool StephanoI_ATDevice_FileSystemWrite(char* filename, uint16_t offset, uint16_t length, uint8_t* dataP)
{
    char* pRequestCommand = AT_commandBuffer;
    strcpy(pRequestCommand, "AT+FS=0,1,");

    if (!ATCommand_AppendArgumentStringQuotationMarks(pRequestCommand, filename, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_AppendArgumentInt(pRequestCommand, (int32_t)offset, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_AppendArgumentInt(pRequestCommand, (int32_t)length, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_STRING_TERMINATE))
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

    if (!StephanoI_SendRequest_ex(dataP, length))
    {
        return false;
    }

    return StephanoI_WaitForConfirm(StephanoI_GetTimeout(StephanoI_Timeout_General), StephanoI_CNFStatus_Success);
}

bool StephanoI_ATDevice_ParseSystemTemp(char* EventArgumentsP, float* t)
{
    char* argumentsP = EventArgumentsP;
    return ATCommand_GetNextArgumentFloat(&argumentsP, t, ATCOMMAND_STRING_TERMINATE);
}

bool StephanoI_ATDevice_ParseSystemStoremode(char* EventArgumentsP, bool* t)
{
    char* argumentsP = EventArgumentsP;
    uint8_t val;
    if (!ATCommand_GetNextArgumentInt(&argumentsP, &val, ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }
    *t = (val == (uint8_t)0x01);
    return true;
}

bool StephanoI_ATDevice_ParseFileSystemGetFilesize(char* EventArgumentsP, uint16_t* t)
{
    char* argumentsP = EventArgumentsP;

    return ATCommand_GetNextArgumentInt(&argumentsP, t, ATCOMMAND_INTFLAGS_SIZE16 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_STRING_TERMINATE);
}

bool StephanoI_ATDevice_ParseFileSystemRead(char* EventArgumentsP, StephanoI_ATDevice_FileSystemRead_t* t)
{
    char* argumentsP = EventArgumentsP;

    if (!ATCommand_GetNextArgumentInt(&argumentsP, &(t->length), ATCOMMAND_INTFLAGS_SIZE16 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    t->value = malloc(t->length);
    if (!ATCommand_GetNextArgumentByteArray(&argumentsP, t->length, t->value, t->length))
    {
        return false;
    }

    return true;
}

bool StephanoI_ATDevice_ParseSleep(char* EventArgumentsP, StephanoI_ATDevice_SleepMode_t* t)
{
    char* argumentsP = EventArgumentsP;

    if (!ATCommand_GetNextArgumentInt(&argumentsP, t, ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    return true;
}
