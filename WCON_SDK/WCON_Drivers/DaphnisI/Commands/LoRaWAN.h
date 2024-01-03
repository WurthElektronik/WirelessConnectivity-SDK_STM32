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
 * @brief LoRaWAN commands for DaphnisI header file.
 */

#ifndef DAPHNIS_COMMANDS_LORAWAN_H_
#define DAPHNIS_COMMANDS_LORAWAN_H_

#include <stdbool.h>
#include <stdint.h>

#define DAPHNISI_EUI_LENGTH 24
#define DAPHNISI_KEY_LENGTH 48
#define DAPHNISI_ADDRESS_LENGTH 12

typedef char pEui_t[DAPHNISI_EUI_LENGTH];
typedef char pKey_t[DAPHNISI_KEY_LENGTH];
typedef char pDeviceAddress_t[DAPHNISI_ADDRESS_LENGTH];

typedef enum
{
	DaphnisI_DeviceClass_A,
	DaphnisI_DeviceClass_B,
	DaphnisI_DeviceClass_C,
	DaphnisI_DeviceClass_Count,
} DaphnisI_DeviceClass;

typedef enum
{
	DaphnisI_JoinMode_ABP,
	DaphnisI_JoinMode_OTAA,
	DaphnisI_JoinMode_Count,

} DaphnisI_JoinMode;

typedef enum
{
	DaphnisI_KEY_APP,
	DaphnisI_KEY_NETWORK_SESSION,
	DaphnisI_KEY_APP_SESSION,
	DaphnisI_KEY_COUNT
} DaphnisI_KEYTYPE;

typedef enum
{
	DaphnisI_Delay_JoinWindow1,
	DaphnisI_Delay_JoinWindow2,
	DaphnisI_Delay_RX1,
	DaphnisI_Delay_RX2,
	DaphnisI_Delay_Count
} DaphnisI_Delay;

#define DAPHNISI_MAX_PAYLOAD_LENGTH 1460

/* Keys, IDs and EUIs */
extern bool DaphnisI_SetAPPEUI(pEui_t app_eui);
extern bool DaphnisI_GetAPPEUI(pEui_t *app_euiP);
extern bool DaphnisI_SetDeviceEUI(pEui_t device_eui);
extern bool DaphnisI_GetDeviceEUI(pEui_t *device_euiP);
extern bool DaphnisI_SetKey(DaphnisI_KEYTYPE keyType, pKey_t key);
extern bool DaphnisI_SetDeviceAddress(pDeviceAddress_t device_address);
extern bool DaphnisI_GetDeviceAddress(pDeviceAddress_t *device_addressP);
extern bool DaphnisI_SetNetworkID(uint8_t network_id);
extern bool DaphnisI_GetNetworkID(uint8_t *network_idP);

/* Stack Context */
extern bool DaphnisI_StoreStackContext();
extern bool DaphnisI_EraseStoredStackContext();

/* Network */
extern bool DaphnisI_SetDeviceClass(DaphnisI_DeviceClass device_class);
extern bool DaphnisI_GetDeviceClass(DaphnisI_DeviceClass *device_classP);
extern bool DaphnisI_Join(DaphnisI_JoinMode join_Mode);
extern bool DaphnisI_Send(uint8_t port, const uint8_t *payload, uint16_t length, bool ack_Requested);
extern bool DaphnisI_SetDelay(DaphnisI_Delay delayType, uint16_t DelayMS);
extern bool DaphnisI_GetDelay(DaphnisI_Delay delayType, uint16_t *DelayMSP);
extern bool DaphnisI_SetRX2Frequency(uint32_t frequency);
extern bool DaphnisI_GetRX2Frequency(uint32_t *frequencyP);
extern bool DaphnisI_SetRX2DataRate(uint8_t dataRate);
extern bool DaphnisI_GetRX2DataRate(uint8_t *dataRateP);
extern bool DaphnisI_SetPingSlotPeriodicityFactor(uint8_t factor);
extern bool DaphnisI_GetPingSlotPeriodicityFactor(uint8_t *factorP);
extern bool DaphnisI_SetTxPower(uint8_t power);
extern bool DaphnisI_GetTxPower(uint8_t *powerP);
extern bool DaphnisI_SetAdaptiveDataRate(bool adr);
extern bool DaphnisI_GetAdaptiveDataRate(bool *adrP);
extern bool DaphnisI_SetDataRate(uint8_t dataRate);
extern bool DaphnisI_GetDataRate(uint8_t *dataRateP);
extern bool DaphnisI_SetDutyCycleRestriction(bool dcRestriction);
extern bool DaphnisI_GetDutyCycleRestriction(bool *dcRestrictionP);

#endif /* DAPHNIS_COMMANDS_LORAWAN_H_ */
