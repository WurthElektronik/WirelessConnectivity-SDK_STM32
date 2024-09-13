/**
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
 **/

/**
 * @file
 * @brief AT event definitions.
 */

#include <DaphnisI/Commands/ATEvent.h>
#include <DaphnisI/Commands/LoRaWAN.h>
#include <global/ATCommands.h>
#include <stdlib.h>

const static ATCommand_Event_t ContextSubEvents[] = {
				EVENTENTRY("NVM_DATA_STORED", DaphnisI_ATEvent_ContextState_Stored)
				LASTEVENTENTRY("NVM_DATA_RESTORED", DaphnisI_ATEvent_ContextState_Restored)
		};

const static ATCommand_Event_t JoinSubEvents[] = {
				EVENTENTRY("JOINED", DaphnisI_ATEvent_JoinState_Success)
				LASTEVENTENTRY("JOIN_FAILED", DaphnisI_ATEvent_JoinState_Fail)
		};

static ATCommand_Event_t moduleMainEvents[] = {
				PARENTEVENTENTRY("+CS", ContextSubEvents, ATCOMMAND_STRING_TERMINATE)
				PARENTEVENTENTRY("+JOIN", JoinSubEvents, ATCOMMAND_STRING_TERMINATE)
				EVENTENTRY("+SYSNOTF", DaphnisI_ATEvent_Sysnotf)
				EVENTENTRY("+RXINFO", DaphnisI_ATEvent_RxInfo)
				EVENTENTRY("+RXDATA", DaphnisI_ATEvent_RxData)
				EVENTENTRY("+TXCONF", DaphnisI_ATEvent_TxConf)
				EVENTENTRY("+CLASSUPDATE", DaphnisI_ATEvent_Class)
				EVENTENTRY("+BEACONINFO", DaphnisI_ATEvent_BeaconInfo)
				EVENTENTRY("+BEACONLOST", DaphnisI_ATEvent_BeaconLost)
				LASTEVENTENTRY("+BEACONNOTRECEIVED", DaphnisI_ATEvent_BeaconNotReceived)
		};

/**
 * @brief Parses the received AT command and returns the corresponding DaphnisI_ATEvent_t.
 *
 * @param[in,out] pAtCommand AT command starting with '+'
 * @param[out] pEvent DaphnisI_ATEvent_t representing the event
 *
 * @return true if parsed successfully, false otherwise
 */
bool DaphnisI_ATEvent_ParseEventType(char **pAtCommand, DaphnisI_ATEvent_t *pEvent)
{
	char delimiters[] = {
			ATCOMMAND_EVENT_DELIM,
			ATCOMMAND_STRING_TERMINATE };

	if (!ATCommand_ParseEventType(pAtCommand, moduleMainEvents, delimiters, sizeof(delimiters), (uint16_t*) pEvent))
	{
		*pEvent = DaphnisI_ATEvent_Invalid;
		return false;
	}

	return true;
}

/**
 * @brief Parses the values of the join event arguments.
 *
 * @param[in,out] pEventArguments String containing arguments of the AT command
 * @param[out] pDevice_class The parsed class parameter
 *
 * @return true if parsed successfully, false otherwise
 */
bool DaphnisI_ATEvent_ParseClass(char **pEventArguments, DaphnisI_DeviceClass *pDevice_class)
{
	char temp[2];
	if (ATCommand_GetNextArgumentString(pEventArguments, temp, ATCOMMAND_STRING_TERMINATE, sizeof(temp)))
	{
		*pDevice_class = temp[0] - 'A';
		return true;
	}

	return false;
}

/**
 * @brief Parses the values of the join event arguments.
 *
 * @param[in,out] pEventArguments String containing arguments of the AT command
 * @param[out] pRxInfo The parsed rx info event parameters
 *
 * @return true if parsed successfully, false otherwise
 */
bool DaphnisI_ATEvent_ParseRxInfo(char **pEventArguments, DaphnisI_RxInfo_t *pRxInfo)
{
	char temp[5];
	int argumentsCount = ATCommand_CountArgs(*pEventArguments);

	if (ATCommand_GetNextArgumentString(pEventArguments, temp, ATCOMMAND_ARGUMENT_DELIM, sizeof(temp)))
	{
		switch (temp[3])
		{
		case '1':
		{
			pRxInfo->window = DaphnisI_SlotWindow_1;
			break;
		}
		case '2':
		{
			pRxInfo->window = DaphnisI_SlotWindow_2;
			break;
		}
		case 'B':
		{
			pRxInfo->window = DaphnisI_SlotWindow_B;
			break;
		}
		case 'C':
		{
			pRxInfo->window = DaphnisI_SlotWindow_C;
			break;
		}
		default:
			break;
		}
	}

	if (!ATCommand_GetNextArgumentInt(pEventArguments, &(pRxInfo->port), ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_SIZE8, ATCOMMAND_ARGUMENT_DELIM))
	{
		return false;
	}

	if (!ATCommand_GetNextArgumentInt(pEventArguments, &(pRxInfo->dataRate), ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_SIZE8, ATCOMMAND_ARGUMENT_DELIM))
	{
		return false;
	}

	if (!ATCommand_GetNextArgumentInt(pEventArguments, &(pRxInfo->RSSI), ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_SIGNED, ATCOMMAND_ARGUMENT_DELIM))
	{
		return false;
	}

	//depending on the argument count, this is the last or 2 more following
	if (!ATCommand_GetNextArgumentInt(pEventArguments, &(pRxInfo->SNR), ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_SIZE8, (argumentsCount == 5) ? ATCOMMAND_STRING_TERMINATE : ATCOMMAND_ARGUMENT_DELIM))
	{
		return false;
	}

	// optional parameters present
	if (argumentsCount == 7)
	{
		if (!ATCommand_GetNextArgumentInt(pEventArguments, &(pRxInfo->linkMargin), ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_SIZE8, ATCOMMAND_ARGUMENT_DELIM))
		{
			return false;
		}

		if (!ATCommand_GetNextArgumentInt(pEventArguments, &(pRxInfo->gatewaysReached), ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_SIZE8, ATCOMMAND_STRING_TERMINATE))
		{
			return false;
		}
	}

	return true;
}

/**
 * @brief Parses the values of the join event arguments.
 *
 * @param[in,out] pEventArguments String containing arguments of the AT command
 * @param[out] pRxData The parsed rx data parameters
 *
 * @return true if parsed successfully, false otherwise
 */
bool DaphnisI_ATEvent_ParseRxData(char **pEventArguments, DaphnisI_RxData_t *pRxData)
{
	if (!ATCommand_GetNextArgumentInt(pEventArguments, &(pRxData->port), ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_SIZE8, ATCOMMAND_ARGUMENT_DELIM))
	{
		return false;
	}

	if (!ATCommand_GetNextArgumentInt(pEventArguments, &(pRxData->dataLength), ATCOMMAND_INTFLAGS_NOTATION_HEX | ATCOMMAND_INTFLAGS_SIZE8, ATCOMMAND_ARGUMENT_DELIM))
	{
		return false;
	}

	uint8_t outLength = (strlen(*pEventArguments) >> 1);
	if (outLength != pRxData->dataLength)
	{
		// not the expected length of data
		return false;
	}

	char byteString[3];
	byteString[2] = '\0';
	// Read to characters of hex representation and convert to bytes
	for (uint8_t i = 0; i < outLength; i++)
	{
		byteString[0] = (*pEventArguments)[2 * i];
		byteString[1] = (*pEventArguments)[2 * i + 1];

		pRxData->data[i] = (uint8_t) strtol(byteString, NULL, 16);
	}

	return true;
}

/**
 * @brief Parses the values of the ping event arguments.
 *
 * @param[in,out] pEventArguments String containing arguments of the AT command
 * @param[out] pBeaconInfo The parsed beacon info event parameters
 *
 * @return true if parsed successfully, false otherwise
 */
bool DaphnisI_ATEvent_ParseBeaconInfo(char **pEventArguments, DaphnisI_BeaconInfo_t *pBeaconInfo)
{
	char temp[6];

	if (!ATCommand_GetNextArgumentString(pEventArguments, temp, ATCOMMAND_ARGUMENT_DELIM, 6))
	{
		/* ignore first parameter. Is always the same */
		return false;
	}

	if (!ATCommand_GetNextArgumentInt(pEventArguments, &(pBeaconInfo->dataRate), ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_SIZE8, ATCOMMAND_ARGUMENT_DELIM))
	{
		return false;
	}

	if (!ATCommand_GetNextArgumentInt(pEventArguments, &(pBeaconInfo->RSSI), ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_SIGNED, ATCOMMAND_ARGUMENT_DELIM))
	{
		return false;
	}

	if (!ATCommand_GetNextArgumentInt(pEventArguments, &(pBeaconInfo->SNR), ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_SIZE8, ATCOMMAND_ARGUMENT_DELIM))
	{
		return false;
	}

	if (!ATCommand_GetNextArgumentInt(pEventArguments, &(pBeaconInfo->frequency), ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_SIZE32, ATCOMMAND_ARGUMENT_DELIM))
	{
		return false;
	}

	if (!ATCommand_GetNextArgumentInt(pEventArguments, &(pBeaconInfo->timestamp), ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_SIZE32, ATCOMMAND_ARGUMENT_DELIM))
	{
		return false;
	}

	if (!ATCommand_GetNextArgumentInt(pEventArguments, &(pBeaconInfo->infoDescriptor), ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_SIZE8, ATCOMMAND_ARGUMENT_DELIM))
	{
		return false;
	}

	if (!ATCommand_GetNextArgumentString(pEventArguments, pBeaconInfo->info, ATCOMMAND_ARGUMENT_DELIM, 7))
	{
		return false;
	}

	if (!ATCommand_GetNextArgumentString(pEventArguments, &(pBeaconInfo->info[7]), ATCOMMAND_STRING_TERMINATE, 7))
	{
		return false;
	}

	return true;
}

