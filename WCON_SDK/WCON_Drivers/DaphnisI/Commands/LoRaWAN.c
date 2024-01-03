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
 * @brief LoRaWAN commands for DaphnisI source file.
 */
#include <DaphnisI/Commands/LoRaWAN.h>
#include <DaphnisI/DaphnisI.h>
#include <global/ATCommands.h>
#include <stdint.h>
#include <stdio.h>

static char *DaphnisI_Delay_GetRequestStrings[DaphnisI_Delay_Count] = {
		"AT+JN1DL=?\r\n",
		"AT+JN2DL=?\r\n",
		"AT+RX1DL=?\r\n",
		"AT+RX2DL=?\r\n" };
static char *DaphnisI_Delay_SetRequestStrings[DaphnisI_Delay_Count] = {
		"AT+JN1DL=",
		"AT+JN2DL=",
		"AT+RX1DL=",
		"AT+RX2DL=" };
static char *DaphnisI_Delay_CmdStrings[DaphnisI_Delay_Count] = {
		"+JN1DL:",
		"+JN2DL:",
		"+RX1DL:",
		"+RX2DL:" };

static char *DaphnisI_Key_SetRequestStrings[DaphnisI_KEY_COUNT] = {
		"AT+APPKEY=",
		"AT+APPSKEY=",
		"AT+NWKSKEY=" };

/**
 * @brief Sets the App EUI
 *
 * @param[in] app_eui: the app eui (string in the form of XX:XX:XX:XX:XX:XX:XX:XX)
 *
 * @return true if successful, false otherwise
 */
bool DaphnisI_SetAPPEUI(pEui_t app_eui)
{
	if (NULL == app_eui)
	{
		return false;
	}

	char *pRequestCommand = AT_commandBuffer;
	strcpy(pRequestCommand, "AT+APPEUI=");

	if (!ATCommand_AppendArgumentString(pRequestCommand, app_eui, ATCOMMAND_STRING_TERMINATE))
	{
		return false;
	}

	if (!ATCommand_AppendArgumentString(pRequestCommand, ATCOMMAND_CRLF, ATCOMMAND_STRING_TERMINATE))
	{
		return false;
	}

	if (!DaphnisI_SendRequest(pRequestCommand))
	{
		return false;
	}

	return DaphnisI_WaitForConfirm(DaphnisI_GetTimeout(DaphnisI_Timeout_General), DaphnisI_CNFStatus_Success, NULL);
}

/**
 * @brief Gets the App EUI
 *
 * @param[out] app_euiP: pointer to the app eui (string in the form of XX:XX:XX:XX:XX:XX:XX:XX)
 *
 * @return true if successful, false otherwise
 */
bool DaphnisI_GetAPPEUI(pEui_t *app_euiP)
{
	if (NULL == app_euiP)
	{
		return false;
	}

	if (!DaphnisI_SendRequest("AT+APPEUI=?\r\n"))
	{
		return false;
	}

	char *pRespondCommand = AT_commandBuffer;
	if (!DaphnisI_WaitForConfirm(DaphnisI_GetTimeout(DaphnisI_Timeout_General), DaphnisI_CNFStatus_Success, pRespondCommand))
	{
		return false;
	}

	const char *cmd = "+APPEUI:";
	const size_t cmdLength = strlen(cmd);

	if (0 != strncmp(pRespondCommand, cmd, cmdLength))
	{
		return false;
	}

	pRespondCommand += cmdLength;

	return ATCommand_GetNextArgumentString(&pRespondCommand, *app_euiP, ATCOMMAND_STRING_TERMINATE, DAPHNISI_EUI_LENGTH);
}

/**
 * @brief Sets the Device EUI
 *
 * @param[in] device_eui: the device eui (string in the form of XX:XX:XX:XX:XX:XX:XX:XX)
 *
 * @return true if successful, false otherwise
 */
bool DaphnisI_SetDeviceEUI(pEui_t device_eui)
{
	if (NULL == device_eui)
	{
		return false;
	}

	char *pRequestCommand = AT_commandBuffer;
	strcpy(pRequestCommand, "AT+DEUI=");

	if (!ATCommand_AppendArgumentString(pRequestCommand, device_eui, ATCOMMAND_STRING_TERMINATE))
	{
		return false;
	}

	if (!ATCommand_AppendArgumentString(pRequestCommand, ATCOMMAND_CRLF, ATCOMMAND_STRING_TERMINATE))
	{
		return false;
	}

	if (!DaphnisI_SendRequest(pRequestCommand))
	{
		return false;
	}

	return DaphnisI_WaitForConfirm(DaphnisI_GetTimeout(DaphnisI_Timeout_General), DaphnisI_CNFStatus_Success, NULL);
}

/**
 * @brief Gets the device EUI
 *
 * @param[out] device_euiP: pointer to the device eui (string in the form of XX:XX:XX:XX:XX:XX:XX:XX)
 *
 * @return true if successful, false otherwise
 */
bool DaphnisI_GetDeviceEUI(pEui_t *device_euiP)
{
	if (NULL == device_euiP)
	{
		return false;
	}

	if (!DaphnisI_SendRequest("AT+DEUI=?\r\n"))
	{
		return false;
	}

	char *pRespondCommand = AT_commandBuffer;
	if (!DaphnisI_WaitForConfirm(DaphnisI_GetTimeout(DaphnisI_Timeout_General), DaphnisI_CNFStatus_Success, pRespondCommand))
	{
		return false;
	}

	const char *cmd = "+DEUI:";
	const size_t cmdLength = strlen(cmd);

	if (0 != strncmp(pRespondCommand, cmd, cmdLength))
	{
		return false;
	}

	pRespondCommand += cmdLength;

	return ATCommand_GetNextArgumentString(&pRespondCommand, *device_euiP, ATCOMMAND_STRING_TERMINATE, DAPHNISI_EUI_LENGTH);
}

/**
 * @brief Sets the Network key
 *
 * @param[in] keyType: which type of key to set (see DaphnisI_KEYTYPE)
 * @param[in] key: the key to set (as string - bytes are seperated by ':')
 *
 * @return true if successful, false otherwise
 */
bool DaphnisI_SetKey(DaphnisI_KEYTYPE keyType, pKey_t key)
{
	if (NULL == key)
	{
		return false;
	}

	char *pRequestCommand = AT_commandBuffer;
	strcpy(pRequestCommand, DaphnisI_Key_SetRequestStrings[keyType]);

	if (!ATCommand_AppendArgumentString(pRequestCommand, key, ATCOMMAND_STRING_TERMINATE))
	{
		return false;
	}

	if (!ATCommand_AppendArgumentString(pRequestCommand, ATCOMMAND_CRLF, ATCOMMAND_STRING_TERMINATE))
	{
		return false;
	}

	if (!DaphnisI_SendRequest(pRequestCommand))
	{
		return false;
	}

	return DaphnisI_WaitForConfirm(DaphnisI_GetTimeout(DaphnisI_Timeout_General), DaphnisI_CNFStatus_Success, NULL);
}

/**
 * @brief Sets the device address
 *
 * @param[in] device_address: the device address (string in the form of XX:XX:XX:XX)
 *
 * @return true if successful, false otherwise
 */
bool DaphnisI_SetDeviceAddress(pDeviceAddress_t device_address)
{
	if (NULL == device_address)
	{
		return false;
	}

	char *pRequestCommand = AT_commandBuffer;
	strcpy(pRequestCommand, "AT+DADDR=");

	if (!ATCommand_AppendArgumentString(pRequestCommand, device_address, ATCOMMAND_STRING_TERMINATE))
	{
		return false;
	}

	if (!ATCommand_AppendArgumentString(pRequestCommand, ATCOMMAND_CRLF, ATCOMMAND_STRING_TERMINATE))
	{
		return false;
	}

	if (!DaphnisI_SendRequest(pRequestCommand))
	{
		return false;
	}

	return DaphnisI_WaitForConfirm(DaphnisI_GetTimeout(DaphnisI_Timeout_General), DaphnisI_CNFStatus_Success, NULL);
}

/**
 * @brief Gets the device address
 *
 * @param[out] device_addressP: pointer to the device address (string in the form of XX:XX:XX:XX)
 *
 * @return true if successful, false otherwise
 */
bool DaphnisI_GetDeviceAddress(pDeviceAddress_t *device_addressP)
{
	if (NULL == device_addressP)
	{
		return false;
	}

	if (!DaphnisI_SendRequest("AT+DADDR=?\r\n"))
	{
		return false;
	}

	char *pRespondCommand = AT_commandBuffer;
	if (!DaphnisI_WaitForConfirm(DaphnisI_GetTimeout(DaphnisI_Timeout_General), DaphnisI_CNFStatus_Success, pRespondCommand))
	{
		return false;
	}

	const char *cmd = "+DADDR:";
	const size_t cmdLength = strlen(cmd);

	if (0 != strncmp(pRespondCommand, cmd, cmdLength))
	{
		return false;
	}

	pRespondCommand += cmdLength;

	return ATCommand_GetNextArgumentString(&pRespondCommand, *device_addressP, ATCOMMAND_STRING_TERMINATE, DAPHNISI_ADDRESS_LENGTH);
}

/**
 * @brief Sets the network id
 *
 * @param[in] network_id : the network id
 *
 * @return true if successful, false otherwise
 */
bool DaphnisI_SetNetworkID(uint8_t network_id)
{
	char *pRequestCommand = AT_commandBuffer;
	strcpy(pRequestCommand, "AT+NWKID=");

	if (!ATCommand_AppendArgumentInt(pRequestCommand, network_id, ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_SIZE8, ATCOMMAND_STRING_TERMINATE))
	{
		return false;
	}

	if (!ATCommand_AppendArgumentString(pRequestCommand, ATCOMMAND_CRLF, ATCOMMAND_STRING_TERMINATE))
	{
		return false;
	}

	if (!DaphnisI_SendRequest(pRequestCommand))
	{
		return false;
	}

	return DaphnisI_WaitForConfirm(DaphnisI_GetTimeout(DaphnisI_Timeout_General), DaphnisI_CNFStatus_Success, NULL);
}

/**
 * @brief Gets the network id
 *
 * @param[out] network_idP: pointer to the network id
 *
 * @return true if successful, false otherwise
 */
bool DaphnisI_GetNetworkID(uint8_t *network_idP)
{
	if (NULL == network_idP)
	{
		return false;
	}

	if (!DaphnisI_SendRequest("AT+NWKID=?\r\n"))
	{
		return false;
	}

	char *pRespondCommand = AT_commandBuffer;
	if (!DaphnisI_WaitForConfirm(DaphnisI_GetTimeout(DaphnisI_Timeout_General), DaphnisI_CNFStatus_Success, pRespondCommand))
	{
		return false;
	}

	const char *cmd = "+NWKID:";
	const size_t cmdLength = strlen(cmd);

	if (0 != strncmp(pRespondCommand, cmd, cmdLength))
	{
		return false;
	}

	pRespondCommand += cmdLength;

	return ATCommand_GetNextArgumentInt(&pRespondCommand, network_idP, ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_SIZE8, ATCOMMAND_STRING_TERMINATE);
}

/**
 * @brief Stores the LoRaWAN context (using the AT+CS command).
 *
 * @return true if successful, false otherwise
 */
bool DaphnisI_StoreStackContext()
{
	if (!DaphnisI_SendRequest("AT+CS\r\n"))
	{
		return false;
	}
	return DaphnisI_WaitForConfirm(DaphnisI_GetTimeout(DaphnisI_Timeout_General), DaphnisI_CNFStatus_Success, NULL);
}

/**
 * @brief Erases the stored LoRaWAN context (using the AT+RFS command).
 *
 * @return true if successful, false otherwise
 */
bool DaphnisI_EraseStoredStackContext()
{
	if (!DaphnisI_SendRequest("AT+RFS\r\n"))
	{
		return false;
	}
	return DaphnisI_WaitForConfirm(DaphnisI_GetTimeout(DaphnisI_Timeout_General), DaphnisI_CNFStatus_Success, NULL);
}

/**
 * @brief Sets the device class
 *
 * @param[in] device_class : the device class
 *
 * @return true if successful, false otherwise
 */
bool DaphnisI_SetDeviceClass(DaphnisI_DeviceClass device_class)
{
	char *pRequestCommand = AT_commandBuffer;
	strcpy(pRequestCommand, "AT+CLASS=");

	*pRequestCommand = "ABC"[device_class];
	pRequestCommand++;
	*pRequestCommand = '\0';
	pRequestCommand++;

	if (!ATCommand_AppendArgumentString(pRequestCommand, ATCOMMAND_CRLF, ATCOMMAND_STRING_TERMINATE))
	{
		return false;
	}

	if (!DaphnisI_SendRequest(pRequestCommand))
	{
		return false;
	}

	return DaphnisI_WaitForConfirm(DaphnisI_GetTimeout(DaphnisI_Timeout_General), DaphnisI_CNFStatus_Success, NULL);
}

/**
 * @brief Gets the device class
 *
 * @param[out] device_classP: pointer to the device class
 *
 * @return true if successful, false otherwise
 */
bool DaphnisI_GetDeviceClass(DaphnisI_DeviceClass *device_classP)
{
	if (NULL == device_classP)
	{
		return false;
	}

	if (!DaphnisI_SendRequest("AT+CLASS=?\r\n"))
	{
		return false;
	}

	char *pRespondCommand = AT_commandBuffer;
	if (!DaphnisI_WaitForConfirm(DaphnisI_GetTimeout(DaphnisI_Timeout_General), DaphnisI_CNFStatus_Success, pRespondCommand))
	{
		return false;
	}

	const char *cmd = "+CLASS:";
	const size_t cmdLength = strlen(cmd);

	if (0 != strncmp(pRespondCommand, cmd, cmdLength))
	{
		return false;
	}

	pRespondCommand += cmdLength;

	return ATCommand_GetNextArgumentInt(&pRespondCommand, device_classP, ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_SIZE8, ATCOMMAND_STRING_TERMINATE);
}

/**
 * @brief Join the network
 *
 * @param join_Mode : which mode to use for join (see DaphnisI_JoinMode)
 *
 * @return true if successful, false otherwise
 */
bool DaphnisI_Join(DaphnisI_JoinMode join_Mode)
{
	char *pRequestCommand = AT_commandBuffer;
	strcpy(pRequestCommand, "AT+JOIN=");

	ATCommand_AppendArgumentInt(pRequestCommand, join_Mode,
	ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_SIZE8, ATCOMMAND_STRING_TERMINATE);

	if (!ATCommand_AppendArgumentString(pRequestCommand, ATCOMMAND_CRLF, ATCOMMAND_STRING_TERMINATE))
	{
		return false;
	}

	if (!DaphnisI_SendRequest(pRequestCommand))
	{
		return false;
	}

	char *pRespondCommand = AT_commandBuffer;
	return DaphnisI_WaitForConfirm(DaphnisI_GetTimeout(DaphnisI_Timeout_General), DaphnisI_CNFStatus_Success, pRespondCommand);
}

/**
 * @brief Send message to connected network
 *
 * @param[in] port : the port number (range 0-199)
 * @param[in] payload: the payload to send
 * @param[in] length: the length of the payload in bytes
 * @param[in] ack_Requested: ack requested from network
 *
 * @return true if successful, false otherwise
 */
bool DaphnisI_Send(uint8_t port, const uint8_t *payload, uint16_t length, bool ack_Requested)
{
	if (NULL == payload)
	{
		return false;
	}

	if ((port == 0) || (199 < port))
	{
		return false;
	}

	if (DAPHNISI_MAX_PAYLOAD_LENGTH < length)
	{
		return false;
	}

	char *pRequestCommand = AT_commandBuffer;
	strcpy(pRequestCommand, "AT+SEND=");

	if (!ATCommand_AppendArgumentInt(pRequestCommand, port, ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_SIGNED | ATCOMMAND_INTFLAGS_SIZE8, ATCOMMAND_EVENT_DELIM))
	{
		return false;
	}

	if (!ATCommand_AppendArgumentInt(pRequestCommand, ack_Requested ? 1 : 0, ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_SIGNED | ATCOMMAND_INTFLAGS_SIZE8, ATCOMMAND_EVENT_DELIM))
	{
		return false;
	}

	size_t pRequestCommandLength = strlen(pRequestCommand);

	for (uint16_t i = 0; i < length; i++)
	{
		sprintf(&pRequestCommand[pRequestCommandLength], "%02X", payload[i]);
		pRequestCommandLength += 2;
	}

	pRequestCommand[pRequestCommandLength] = '\0';

	if (!ATCommand_AppendArgumentString(pRequestCommand, ATCOMMAND_CRLF, ATCOMMAND_STRING_TERMINATE))
	{
		return false;
	}

	if (!DaphnisI_SendRequest(pRequestCommand))
	{
		return false;
	}

	return DaphnisI_WaitForConfirm(DaphnisI_GetTimeout(DaphnisI_Timeout_General), DaphnisI_CNFStatus_Success, NULL);
}

/**
 * @brief Sets delay
 *
 * @param[in] delayType: which delay to set (see DaphnisI_Delay)
 * @param[in] delayMS: delay in milliseconds
 *
 * @return true if successful, false otherwise
 */
bool DaphnisI_SetDelay(DaphnisI_Delay delayType, uint16_t delayMS)
{
	char *pRequestCommand = AT_commandBuffer;
	strcpy(pRequestCommand, DaphnisI_Delay_SetRequestStrings[delayType]);

	if (!ATCommand_AppendArgumentInt(pRequestCommand, delayMS,
	ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_SIZE16 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_STRING_TERMINATE))
	{
		return false;
	}

	if (!ATCommand_AppendArgumentString(pRequestCommand, ATCOMMAND_CRLF, ATCOMMAND_STRING_TERMINATE))
	{
		return false;
	}

	if (!DaphnisI_SendRequest(pRequestCommand))
	{
		return false;
	}

	return DaphnisI_WaitForConfirm(DaphnisI_GetTimeout(DaphnisI_Timeout_General), DaphnisI_CNFStatus_Success, NULL);
}

/**
 * @brief Gets delay
 *
 * @param[in] delayType: which delay to set (see DaphnisI_Delay)
 * @param[out] delayMSP: pointer to the delay in milliseconds
 *
 * @return true if successful, false otherwise
 */
bool DaphnisI_GetDelay(DaphnisI_Delay delayType, uint16_t *delayMSP)
{
	if (NULL == delayMSP)
	{
		return false;
	}

	if (!DaphnisI_SendRequest(DaphnisI_Delay_GetRequestStrings[delayType]))
	{
		return false;
	}

	char *pRespondCommand = AT_commandBuffer;
	if (!DaphnisI_WaitForConfirm(DaphnisI_GetTimeout(DaphnisI_Timeout_General), DaphnisI_CNFStatus_Success, pRespondCommand))
	{
		return false;
	}

	const char *cmd = DaphnisI_Delay_CmdStrings[delayType];
	const size_t cmdLength = strlen(cmd);

	if (0 != strncmp(pRespondCommand, cmd, cmdLength))
	{
		return false;
	}

	pRespondCommand += cmdLength;

	return ATCommand_GetNextArgumentInt(&pRespondCommand, delayMSP,
	ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_SIZE16, ATCOMMAND_STRING_TERMINATE);
}

/**
 * @brief Set the Rx2 window frequency in Hz
 *
 * @param[in] frequency: The frequency to set
 *
 * @return true if successful, false otherwise
 */
bool DaphnisI_SetRX2Frequency(uint32_t frequency)
{
	char *pRequestCommand = AT_commandBuffer;
	strcpy(pRequestCommand, "AT+RX2FQ=");

	if (!ATCommand_AppendArgumentInt(pRequestCommand, frequency,
	ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_SIZE32, ATCOMMAND_STRING_TERMINATE))
	{
		return false;
	}

	if (!ATCommand_AppendArgumentString(pRequestCommand, ATCOMMAND_CRLF, ATCOMMAND_STRING_TERMINATE))
	{
		return false;
	}

	if (!DaphnisI_SendRequest(pRequestCommand))
	{
		return false;
	}

	return DaphnisI_WaitForConfirm(DaphnisI_GetTimeout(DaphnisI_Timeout_General), DaphnisI_CNFStatus_Success, NULL);
}

/**
 * @brief Get the Rx2 window frequency in Hz
 *
 * @param[out] frequencyP: The current Rx2 windows frequency
 *
 * @return true if successful, false otherwise
 */
bool DaphnisI_GetRX2Frequency(uint32_t *frequencyP)
{
	if (NULL == frequencyP)
	{
		return false;
	}

	if (!DaphnisI_SendRequest("AT+RX2FQ=?\r\n"))
	{
		return false;
	}

	char *pRespondCommand = AT_commandBuffer;
	if (!DaphnisI_WaitForConfirm(DaphnisI_GetTimeout(DaphnisI_Timeout_General), DaphnisI_CNFStatus_Success, pRespondCommand))
	{
		return false;
	}

	const char *cmd = "+RX2FQ:";
	const size_t cmdLength = strlen(cmd);

	if (0 != strncmp(pRespondCommand, cmd, cmdLength))
	{
		return false;
	}

	pRespondCommand += cmdLength;

	return ATCommand_GetNextArgumentInt(&pRespondCommand, frequencyP,
	ATCOMMAND_INTFLAGS_SIZE32 | ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC,
	ATCOMMAND_STRING_TERMINATE);
}

/**
 * @brief Set the Rx2 data rate
 *
 * @param[in] dataRate: The data rate of RX2
 *
 * @return true if successful, false otherwise
 */
bool DaphnisI_SetRX2DataRate(uint8_t dataRate)
{
	// range of data rate: 0-7
	if (7 < dataRate)
	{
		return false;
	}

	char *pRequestCommand = AT_commandBuffer;
	strcpy(pRequestCommand, "AT+RX2DR=");

	if (!ATCommand_AppendArgumentInt(pRequestCommand, dataRate,
	ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_SIZE8, ATCOMMAND_STRING_TERMINATE))
	{
		return false;
	}

	if (!ATCommand_AppendArgumentString(pRequestCommand, ATCOMMAND_CRLF, ATCOMMAND_STRING_TERMINATE))
	{
		return false;
	}

	if (!DaphnisI_SendRequest(pRequestCommand))
	{
		return false;
	}

	return DaphnisI_WaitForConfirm(DaphnisI_GetTimeout(DaphnisI_Timeout_General), DaphnisI_CNFStatus_Success, NULL);
}

/**
 * @brief Get the Rx2 window data rate
 *
 * @param[out] dataRateP: The current Rx2 data rate
 *
 * @return true if successful, false otherwise
 */
bool DaphnisI_GetRX2DataRate(uint8_t *dataRateP)
{
	if (NULL == dataRateP)
	{
		return false;
	}

	if (!DaphnisI_SendRequest("AT+RX2DR=?\r\n"))
	{
		return false;
	}

	char *pRespondCommand = AT_commandBuffer;
	if (!DaphnisI_WaitForConfirm(DaphnisI_GetTimeout(DaphnisI_Timeout_General), DaphnisI_CNFStatus_Success, pRespondCommand))
	{
		return false;
	}

	const char *cmd = "+RX2DR:";
	const size_t cmdLength = strlen(cmd);

	if (0 != strncmp(pRespondCommand, cmd, cmdLength))
	{
		return false;
	}

	pRespondCommand += cmdLength;

	return ATCommand_GetNextArgumentInt(&pRespondCommand, dataRateP,
	ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC,
	ATCOMMAND_STRING_TERMINATE);
}

/**
 * @brief Set the ping slot periodicity factor
 *
 * @param[in] factor: The periodicity factor of the ping slot
 *
 * @return true if successful, false otherwise
 */
bool DaphnisI_SetPingSlotPeriodicityFactor(uint8_t factor)
{
	// range of factor: 0-7
	if (7 < factor)
	{
		return false;
	}

	char *pRequestCommand = AT_commandBuffer;
	strcpy(pRequestCommand, "AT+PGSLOT=");

	if (!ATCommand_AppendArgumentInt(pRequestCommand, factor,
	ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_SIZE8, ATCOMMAND_STRING_TERMINATE))
	{
		return false;
	}

	if (!ATCommand_AppendArgumentString(pRequestCommand, ATCOMMAND_CRLF, ATCOMMAND_STRING_TERMINATE))
	{
		return false;
	}

	if (!DaphnisI_SendRequest(pRequestCommand))
	{
		return false;
	}

	return DaphnisI_WaitForConfirm(DaphnisI_GetTimeout(DaphnisI_Timeout_General), DaphnisI_CNFStatus_Success, NULL);
}

/**
 * @brief Get the ping slot periodicity factor
 *
 * @param[out] factorP: The periodicity factor of the ping slot
 *
 * @return true if successful, false otherwise
 */
bool DaphnisI_GetPingSlotPeriodicityFactor(uint8_t *factorP)
{
	if (NULL == factorP)
	{
		return false;
	}

	if (!DaphnisI_SendRequest("AT+PGSLOT=?\r\n"))
	{
		return false;
	}

	char *pRespondCommand = AT_commandBuffer;
	if (!DaphnisI_WaitForConfirm(DaphnisI_GetTimeout(DaphnisI_Timeout_General), DaphnisI_CNFStatus_Success, pRespondCommand))
	{
		return false;
	}

	const char *cmd = "+PGSLOT:";
	const size_t cmdLength = strlen(cmd);

	if (0 != strncmp(pRespondCommand, cmd, cmdLength))
	{
		return false;
	}

	pRespondCommand += cmdLength;

	return ATCommand_GetNextArgumentInt(&pRespondCommand, factorP,
	ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC,
	ATCOMMAND_STRING_TERMINATE);
}

/**
 * @brief Sets the Tx power
 *
 * @param[in] power: Tx power.
 *
 * @return true if successful, false otherwise
 */
bool DaphnisI_SetTxPower(uint8_t power)
{
	// range of power: 0-7
	if (7 < power)
	{
		return false;
	}

	char *pRequestCommand = AT_commandBuffer;
	strcpy(pRequestCommand, "AT+TXP=");

	if (!ATCommand_AppendArgumentInt(pRequestCommand, power,
	ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_SIZE8, ATCOMMAND_STRING_TERMINATE))
	{
		return false;
	}

	if (!ATCommand_AppendArgumentString(pRequestCommand, ATCOMMAND_CRLF, ATCOMMAND_STRING_TERMINATE))
	{
		return false;
	}

	if (!DaphnisI_SendRequest(pRequestCommand))
	{
		return false;
	}

	return DaphnisI_WaitForConfirm(DaphnisI_GetTimeout(DaphnisI_Timeout_General), DaphnisI_CNFStatus_Success, NULL);
}

/**
 * @brief Gets the Tx power
 *
 * @param[out] powerP: Tx power.
 *
 * @return true if successful, false otherwise
 */
bool DaphnisI_GetTxPower(uint8_t *powerP)
{
	if (NULL == powerP)
	{
		return false;
	}

	if (!DaphnisI_SendRequest("AT+TXP=?\r\n"))
	{
		return false;
	}

	char *pRespondCommand = AT_commandBuffer;
	if (!DaphnisI_WaitForConfirm(DaphnisI_GetTimeout(DaphnisI_Timeout_General), DaphnisI_CNFStatus_Success, pRespondCommand))
	{
		return false;
	}

	const char *cmd = "+TXP:";
	const size_t cmdLength = strlen(cmd);

	if (0 != strncmp(pRespondCommand, cmd, cmdLength))
	{
		return false;
	}

	pRespondCommand += cmdLength;

	return ATCommand_GetNextArgumentInt(&pRespondCommand, powerP,
	ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC,
	ATCOMMAND_STRING_TERMINATE);
}

/**
 * @brief Sets the adaptive data rate state
 *
 * @param[in] adr: adaptive rate state.
 *
 * @return true if successful, false otherwise
 */
bool DaphnisI_SetAdaptiveDataRate(bool adr)
{
	char *pRequestCommand = AT_commandBuffer;
	strcpy(pRequestCommand, "AT+ADR=");

	if (!ATCommand_AppendArgumentInt(pRequestCommand, adr,
	ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_SIZE8, ATCOMMAND_STRING_TERMINATE))
	{
		return false;
	}

	if (!ATCommand_AppendArgumentString(pRequestCommand, ATCOMMAND_CRLF, ATCOMMAND_STRING_TERMINATE))
	{
		return false;
	}

	if (!DaphnisI_SendRequest(pRequestCommand))
	{
		return false;
	}

	return DaphnisI_WaitForConfirm(DaphnisI_GetTimeout(DaphnisI_Timeout_General), DaphnisI_CNFStatus_Success, NULL);
}

/**
 * @brief Gets the adaptive data rate state
 *
 * @param[out] adrP: Adaptive data rate state.
 *
 * @return true if successful, false otherwise
 */
bool DaphnisI_GetAdaptiveDataRate(bool *adrP)
{
	if (NULL == adrP)
	{
		return false;
	}

	if (!DaphnisI_SendRequest("AT+ADR=?\r\n"))
	{
		return false;
	}

	char *pRespondCommand = AT_commandBuffer;
	if (!DaphnisI_WaitForConfirm(DaphnisI_GetTimeout(DaphnisI_Timeout_General), DaphnisI_CNFStatus_Success, pRespondCommand))
	{
		return false;
	}

	const char *cmd = "+ADR:";
	const size_t cmdLength = strlen(cmd);

	if (0 != strncmp(pRespondCommand, cmd, cmdLength))
	{
		return false;
	}

	pRespondCommand += cmdLength;

	return ATCommand_GetNextArgumentInt(&pRespondCommand, adrP,
	ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC,
	ATCOMMAND_STRING_TERMINATE);
}

/**
 * @brief Sets the data rate
 *
 * @param[in] dataRate: Data rate.
 *
 * @return true if successful, false otherwise
 */
bool DaphnisI_SetDataRate(uint8_t dataRate)
{
	// range of data rate: 0-7
	if (7 < dataRate)
	{
		return false;
	}

	char *pRequestCommand = AT_commandBuffer;
	strcpy(pRequestCommand, "AT+DR=");

	if (!ATCommand_AppendArgumentInt(pRequestCommand, dataRate,
	ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_SIZE8, ATCOMMAND_STRING_TERMINATE))
	{
		return false;
	}

	if (!ATCommand_AppendArgumentString(pRequestCommand, ATCOMMAND_CRLF, ATCOMMAND_STRING_TERMINATE))
	{
		return false;
	}

	if (!DaphnisI_SendRequest(pRequestCommand))
	{
		return false;
	}

	return DaphnisI_WaitForConfirm(DaphnisI_GetTimeout(DaphnisI_Timeout_General), DaphnisI_CNFStatus_Success, NULL);
}

/**
 * @brief Gets the data rate power
 *
 * @param[out] dataRateP: Data rate.
 *
 * @return true if successful, false otherwise
 */
bool DaphnisI_GetDataRate(uint8_t *dataRateP)
{
	if (NULL == dataRateP)
	{
		return false;
	}

	if (!DaphnisI_SendRequest("AT+DR=?\r\n"))
	{
		return false;
	}

	char *pRespondCommand = AT_commandBuffer;
	if (!DaphnisI_WaitForConfirm(DaphnisI_GetTimeout(DaphnisI_Timeout_General), DaphnisI_CNFStatus_Success, pRespondCommand))
	{
		return false;
	}

	const char *cmd = "+DR:";
	const size_t cmdLength = strlen(cmd);

	if (0 != strncmp(pRespondCommand, cmd, cmdLength))
	{
		return false;
	}

	pRespondCommand += cmdLength;

	return ATCommand_GetNextArgumentInt(&pRespondCommand, dataRateP,
	ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC,
	ATCOMMAND_STRING_TERMINATE);
}

/**
 * @brief Sets the duty cycle restriction state
 *
 * @param[in] dcRestriction: Duty cycle restriction state.
 *
 * @return true if successful, false otherwise
 */
bool DaphnisI_SetDutyCycleRestriction(bool dcRestriction)
{
	char *pRequestCommand = AT_commandBuffer;
	strcpy(pRequestCommand, "AT+DCS=");

	if (!ATCommand_AppendArgumentInt(pRequestCommand, dcRestriction,
	ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_SIZE8, ATCOMMAND_STRING_TERMINATE))
	{
		return false;
	}

	if (!ATCommand_AppendArgumentString(pRequestCommand, ATCOMMAND_CRLF, ATCOMMAND_STRING_TERMINATE))
	{
		return false;
	}

	if (!DaphnisI_SendRequest(pRequestCommand))
	{
		return false;
	}

	return DaphnisI_WaitForConfirm(DaphnisI_GetTimeout(DaphnisI_Timeout_General), DaphnisI_CNFStatus_Success, NULL);
}

/**
 * @brief Gets the duty cycle restriction state
 *
 * @param[out] dcRestrictionP: Duty cycle restriction state.
 *
 * @return true if successful, false otherwise
 */
bool DaphnisI_GetDutyCycleRestriction(bool *dcRestrictionP)
{
	if (NULL == dcRestrictionP)
	{
		return false;
	}

	if (!DaphnisI_SendRequest("AT+DCS=?\r\n"))
	{
		return false;
	}

	char *pRespondCommand = AT_commandBuffer;
	if (!DaphnisI_WaitForConfirm(DaphnisI_GetTimeout(DaphnisI_Timeout_General), DaphnisI_CNFStatus_Success, pRespondCommand))
	{
		return false;
	}

	const char *cmd = "+DCS:";
	const size_t cmdLength = strlen(cmd);

	if (0 != strncmp(pRespondCommand, cmd, cmdLength))
	{
		return false;
	}

	pRespondCommand += cmdLength;

	return ATCommand_GetNextArgumentInt(&pRespondCommand, dcRestrictionP,
	ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC,
	ATCOMMAND_STRING_TERMINATE);
}
