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
 * @brief AT event definitions.
 */

#ifndef DAPHNISI_EVENTS_H_INCLUDED
#define DAPHNISI_EVENTS_H_INCLUDED

#include <global/ATCommands.h>
#include <DaphnisI/DaphnisI.h>
#include <DaphnisI/Commands/LoRaWAN.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief AT event IDs.
 */
typedef enum DaphnisI_ATEvent_t
{
	DaphnisI_ATEvent_Invalid = (uint16_t) 0,
	DaphnisI_ATEvent_Sysnotf,
	DaphnisI_ATEvent_RxInfo,
	DaphnisI_ATEvent_RxData,
	DaphnisI_ATEvent_TxConf,
	DaphnisI_ATEvent_Class,
	DaphnisI_ATEvent_BeaconInfo,
	DaphnisI_ATEvent_BeaconLost,
	DaphnisI_ATEvent_BeaconNotReceived,
	DaphnisI_ATEvent_ContextState_Stored,
	DaphnisI_ATEvent_ContextState_Restored,
	DaphnisI_ATEvent_JoinState_Success,
	DaphnisI_ATEvent_JoinState_Fail,
	DaphnisI_ATEvent_NumberOfValues,
	DaphnisI_ATEvent_Max = UINT16_MAX
} DaphnisI_ATEvent_t;

typedef enum DaphnisI_SlotWindow_t
{
	DaphnisI_SlotWindow_1,
	DaphnisI_SlotWindow_2,
	DaphnisI_SlotWindow_B,
	DaphnisI_SlotWindow_C,
} DaphnisI_SlotWindow_t;

typedef struct DaphnisI_RxInfo_t
{
	uint8_t port;
	uint8_t dataRate;
	int8_t RSSI;
	uint8_t SNR;
	uint8_t linkMargin;
	uint8_t gatewaysReached;
	DaphnisI_SlotWindow_t window;
} DaphnisI_RxInfo_t;

typedef struct DaphnisI_RxData_t
{
	uint8_t port;
	uint8_t dataLength;
	uint8_t *data;
} DaphnisI_RxData_t;

typedef struct DaphnisI_BeaconInfo_t
{
	uint8_t dataRate;
	int8_t RSSI;
	uint8_t SNR;
	uint8_t infoDescriptor;
	char info[13];
	uint32_t frequency;
	uint32_t timestamp;
} DaphnisI_BeaconInfo_t;

extern bool DaphnisI_ATEvent_ParseEventType(char **pAtCommand, DaphnisI_ATEvent_t *pEvent);

extern bool DaphnisI_ATEvent_ParseClass(char **pEventArguments, DaphnisI_DeviceClass *pDevice_class);
extern bool DaphnisI_ATEvent_ParseRxInfo(char **pEventArguments, DaphnisI_RxInfo_t *pRxInfo);
extern bool DaphnisI_ATEvent_ParseRxData(char **pEventArguments, DaphnisI_RxData_t *pRxData);
extern bool DaphnisI_ATEvent_ParseBeaconInfo(char **pEventArguments, DaphnisI_BeaconInfo_t *pBeaconInfo);

#ifdef __cplusplus
}
#endif

#endif // DAPHNISI_EVENTS_H_INCLUDED
