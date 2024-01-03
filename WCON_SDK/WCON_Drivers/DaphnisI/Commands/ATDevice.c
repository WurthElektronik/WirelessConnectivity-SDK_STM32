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
 * COPYRIGHT (c) 2023 Würth Elektronik eiSos GmbH & Co. KG
 *
 ***************************************************************************************************
 */

/**
 * @file
 * @brief Device commands for DaphnisI source file.
 */
#include <DaphnisI/Commands/ATDevice.h>
#include <DaphnisI/DaphnisI.h>
#include <global/ATCommands.h>
#include <stdint.h>
#include <stdio.h>

/**
 * @brief Reboots the device (using the ATZ command).
 *
 * @return true if successful, false otherwise
 */
bool DaphnisI_Reset()
{
	if (!DaphnisI_SendRequest("ATZ\r\n"))
	{
		return false;
	}
	return DaphnisI_WaitForConfirm(DaphnisI_GetTimeout(DaphnisI_Timeout_General), DaphnisI_CNFStatus_Success, NULL);
}

/**
 * @brief Sleep device (using the AT+SLEEP command).
 *
 * @return true if successful, false otherwise
 */
bool DaphnisI_Sleep()
{
	if (!DaphnisI_SendRequest("AT+SLEEP\r\n"))
	{
		return false;
	}
	return DaphnisI_WaitForConfirm(DaphnisI_GetTimeout(DaphnisI_Timeout_General), DaphnisI_CNFStatus_Success, NULL);
}

/**
 * @brief gets the battery level (using the AT++BAT command)
 *
 * @param[out] batteryLevelP: Pointer to the battery level in mV.
 *
 * @return true if successful, false otherwise
 */
bool DaphnisI_GetBatteryLevel(uint16_t *batteryLevelP)
{
	if (NULL == batteryLevelP)
	{
		return false;
	}

	if (!DaphnisI_SendRequest("AT+BAT=?\r\n"))
	{
		return false;
	}

	char *pRespondCommand = AT_commandBuffer;
	if (!DaphnisI_WaitForConfirm(DaphnisI_GetTimeout(DaphnisI_Timeout_General), DaphnisI_CNFStatus_Success, pRespondCommand))
	{
		return false;
	}

	const char *cmd = "+BAT:";
	const size_t cmdLength = strlen(cmd);

	if (0 != strncmp(pRespondCommand, cmd, cmdLength))
	{
		return false;
	}

	pRespondCommand += cmdLength;

	return ATCommand_GetNextArgumentInt(&pRespondCommand, batteryLevelP,
	ATCOMMAND_INTFLAGS_SIZE16 | ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC,
	ATCOMMAND_STRING_TERMINATE);
}
