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
 * @brief DaphnisI driver header file.
 */

#ifndef DAPHNISI_H_INCLUDED
#define DAPHNISI_H_INCLUDED

#include <stdbool.h>
#include <global/global_types.h>

/**
 * @brief Max recommended payload size is 242 bytes.
 */
#define DAPHNISI_MAX_PAYLOAD_SIZE 244

#define DAPHNISI_DEFAULT_BAUDRATE (uint32_t)9600

/**
 * @brief Max. length of sent commands and responses from DaphnisI.
 */
#define DAPHNISI_LINE_MAX_SIZE 512

#define DAPHNISI_RESPONSE_OK     "OK"                       /**< String sent by module if AT command was successful */
#define DAPHNISI_RESPONSE_ERROR  "AT_"                      /**< String sent by module if AT command failed */

/**
 * @brief Max. length of response text (size of buffer storing responses received from DaphnisI).
 * @see DaphnisI_currentResponseText
 */
#define DAPHNISI_MAX_RESPONSE_TEXT_LENGTH DAPHNISI_LINE_MAX_SIZE

#ifdef __cplusplus
extern "C" {
#endif

typedef struct DaphnisI_Pins_t
{
	WE_Pin_t DaphnisI_Pin_Reset;
	WE_Pin_t DaphnisI_Pin_WakeUp;
	WE_Pin_t DaphnisI_Pin_Boot;
	WE_Pin_t DaphnisI_Pin_StatusInd0;
	WE_Pin_t DaphnisI_Pin_StatusInd1;
} DaphnisI_Pins_t;

/**
 * @brief AT command confirmation status.
 */
typedef enum DaphnisI_CNFStatus_t
{
	DaphnisI_CNFStatus_Success,
	DaphnisI_CNFStatus_Failed,
	DaphnisI_CNFStatus_Invalid,
	DaphnisI_CNFStatus_NumberOfValues
} DaphnisI_CNFStatus_t;

/**
 * @brief AT command error messages.
 */
typedef enum DaphnisI_ErrorMessage_t
{
	DaphnisI_ErrorMessage_ERROR,
	DaphnisI_ErrorMessage_PARAM_ERROR,
	DaphnisI_ErrorMessage_BUSY_ERROR,
	DaphnisI_ErrorMessage_TEST_PARAM_OVERFLOW,
	DaphnisI_ErrorMessage_NO_NET_JOINED,
	DaphnisI_ErrorMessage_RX_ERROR,
	DaphnisI_ErrorMessage_DUTYCYCLE_RESTRICTED,
	DaphnisI_ErrorMessage_CRYPTO_ERROR,
	DaphnisI_ErrorMessage_Count,
	DaphnisI_ErrorMessage_Invalid
} DaphnisI_ErrorMessage_t;

/**
 * @brief Timeout categories (for responses to AT commands).
 * @see DaphnisI_SetTimeout(), DaphnisI_GetTimeout()
 */
typedef enum DaphnisI_Timeout_t
{
	DaphnisI_Timeout_General,
	DaphnisI_Timeout_NumberOfValues
} DaphnisI_Timeout_t;

/**
 * @brief DaphnisI event callback.
 * Arguments: Event text
 */
typedef void (*DaphnisI_EventCallback_t)(char*);

extern bool DaphnisI_Init(WE_UART_t *uartP, DaphnisI_Pins_t *pinoutP, DaphnisI_EventCallback_t eventCallback);

extern bool DaphnisI_Deinit(void);

extern bool DaphnisI_PinReset(void);
extern bool DaphnisI_PinWakeUp(void);
extern bool DaphnisI_SetPin(WE_Pin_t pin, WE_Pin_Level_t level);
extern WE_Pin_Level_t DaphnisI_GetPinLevel(WE_Pin_t pin);

extern bool DaphnisI_Transparent_Transmit(const char *data, uint16_t dataLength);
extern bool DaphnisI_SendRequest(char *data);
extern bool DaphnisI_WaitForConfirm(uint32_t maxTimeMs, DaphnisI_CNFStatus_t expectedStatus, char *pOutResponse);

extern bool DaphnisI_GetLastError(DaphnisI_ErrorMessage_t *lastError);

extern bool DaphnisI_SetTimingParameters(uint32_t waitTimeStepMicroseconds, uint32_t minCommandIntervalMicroseconds);
extern void DaphnisI_SetTimeout(DaphnisI_Timeout_t type, uint32_t timeout);
extern uint32_t DaphnisI_GetTimeout(DaphnisI_Timeout_t type);

extern void DaphnisI_SetEolCharacters(uint8_t eol1, uint8_t eol2, bool twoEolCharacters);

#ifdef __cplusplus
}
#endif

#endif // DAPHNISI_H_INCLUDED
