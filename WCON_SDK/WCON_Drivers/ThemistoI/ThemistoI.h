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
 * @file
 * @brief Themisto-I driver header file.
 */

#include <stdbool.h>
#include <stdint.h>
#include <global/global_types.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef THEMISTOI_H_INCLUDED
#define THEMISTOI_H_INCLUDED

#define THEMISTOI_BROADCASTADDRESS 0xFF
#define THEMISTOI_MAX_USERSETTING_LENGTH 4

#define THEMISTOI_MIN_RFCHANNEL 201
#define THEMISTOI_MAX_RFCHANNEL 251

#define THEMISTOI_DEFAULT_BAUDRATE (uint32_t)115200

typedef struct ThemistoI_Pins_t
{
	WE_Pin_t ThemistoI_Pin_Reset;
	WE_Pin_t ThemistoI_Pin_SleepWakeUp;
	WE_Pin_t ThemistoI_Pin_Boot;
	WE_Pin_t ThemistoI_Pin_Mode;

} ThemistoI_Pins_t;

typedef enum ThemistoI_AddressMode_t
{
	ThemistoI_AddressMode_0 = 0x00,
	ThemistoI_AddressMode_1,
	ThemistoI_AddressMode_2,
	ThemistoI_AddressMode_3,
} ThemistoI_AddressMode_t;

typedef enum ThemistoI_UserSettings_t
{
	ThemistoI_CMD_SETGET_OPTION_UARTBAUDRATE = 0x00,
	ThemistoI_CMD_SETGET_OPTION_DEFAULTRFPROFILE = 0x01,
	ThemistoI_CMD_SETGET_OPTION_DEFAULTRFTXPOWER = 0x02,
	ThemistoI_CMD_SETGET_OPTION_DEFAULTRFCHANNEL = 0x03,
	ThemistoI_CMD_SETGET_OPTION_DEFAULTADDRESSMODE = 0x04,
	ThemistoI_CMD_SETGET_OPTION_NUMRETRYS = 0x06,
	ThemistoI_CMD_SETGET_OPTION_DEFAULTDESTNETID = 0x07,
	ThemistoI_CMD_SETGET_OPTION_DEFAULTDESTADDR = 0x08,
	ThemistoI_CMD_SETGET_OPTION_SOURCENETID = 0x0A,
	ThemistoI_CMD_SETGET_OPTION_SOURCEADDR = 0x0B,
	ThemistoI_CMD_SETGET_OPTION_CFG_FLAGS = 0x0F,
	ThemistoI_CMD_SETGET_OPTION_RP_FLAGS = 0x10,
	ThemistoI_CMD_SETGET_OPTION_RP_NUMSLOTS = 0x11,
	ThemistoI_CMD_SETGET_OPTION_LBT_OBSERVATION_PERIOD = 0x1A,
	ThemistoI_CMD_SETGET_OPTION_LBT_THRESHOLD = 0x1B,
	ThemistoI_CMD_SETGET_OPTION_FACTORYSETTINGS = 0x20,
	ThemistoI_CMD_SETGET_OPTION_FWVERSION = 0x21,
} ThemistoI_UserSettings_t;

/**
 * @brief Struct representing a user setting.
 */
typedef struct ThemistoI_Configuration_t
{
	ThemistoI_UserSettings_t usersetting; /**< user setting */
	uint8_t value[THEMISTOI_MAX_USERSETTING_LENGTH]; /**< value */
	uint8_t value_length; /**< length of the value */
} ThemistoI_Configuration_t;

extern bool ThemistoI_Init(WE_UART_t *uartP, ThemistoI_Pins_t *pinoutP, ThemistoI_AddressMode_t addrmode, void (*RXcb)(uint8_t*, uint8_t, uint8_t, uint8_t, uint8_t, int8_t));
extern bool ThemistoI_Deinit(void);

extern bool ThemistoI_PinReset(void);
extern bool ThemistoI_Reset(void);

extern bool ThemistoI_Transmit_Extended(uint8_t *payload, uint8_t length, uint8_t channel, uint8_t dest_network_id, uint8_t dest_address_lsb, uint8_t dest_address_msb);
extern bool ThemistoI_Transmit(uint8_t *payload, uint8_t length);
extern bool ThemistoI_Transparent_Transmit(const uint8_t *data, uint16_t dataLength);

extern bool ThemistoI_Shutdown(void);
extern bool ThemistoI_Standby(void);
extern bool ThemistoI_PinWakeup(bool standby);

/* Read the non-volatile settings */
extern bool ThemistoI_Get(ThemistoI_UserSettings_t us, uint8_t *response, uint8_t *response_length);
extern bool ThemistoI_GetFirmwareVersion(uint8_t *fw);
extern bool ThemistoI_GetSerialNumber(uint8_t *sn);
extern bool ThemistoI_GetDefaultTXPower(uint8_t *txpower);
extern bool ThemistoI_GetSourceAddr(uint8_t *srcaddr_lsb, uint8_t *srcaddr_msb);
extern bool ThemistoI_GetSourceNetID(uint8_t *srcnetid);
extern bool ThemistoI_GetDefaultDestAddr(uint8_t *destaddr_lsb, uint8_t *srcaddr_msb);
extern bool ThemistoI_GetDefaultDestNetID(uint8_t *destnetid);
extern bool ThemistoI_GetDefaultRFChannel(uint8_t *channel);
extern bool ThemistoI_GetDefaultRFProfile(uint8_t *profile);
extern bool ThemistoI_GetLBTObservationPeriod(uint8_t *period);
extern bool ThemistoI_GetLBTThreshold(int8_t *threshold);

/* Functions that write the non-volatile settings in the flash: After modification of any non-volatile setting,
 * the module must be reset such that the update takes effect.
 * IMPORTANT: Use only in rare cases, since flash can be written to only a limited number of times.
 */
extern bool ThemistoI_FactoryReset(void);
extern bool ThemistoI_Set(ThemistoI_UserSettings_t us, uint8_t *value, uint8_t length);
extern bool ThemistoI_CheckNSet(ThemistoI_UserSettings_t us, uint8_t *value, uint8_t length);
extern bool ThemistoI_Configure(ThemistoI_Configuration_t *config, uint8_t config_length, bool factory_reset);
extern bool ThemistoI_SetDefaultTXPower(uint8_t txpower);
extern bool ThemistoI_SetSourceAddr(uint8_t srcaddr_lsb, uint8_t srcaddr_msb);
extern bool ThemistoI_SetSourceNetID(uint8_t srcnetid);
extern bool ThemistoI_SetDefaultDestAddr(uint8_t destaddr_lsb, uint8_t srcaddr_msb);
extern bool ThemistoI_SetDefaultDestNetID(uint8_t destnetid);
extern bool ThemistoI_SetDefaultRFChannel(uint8_t channel);
extern bool ThemistoI_SetDefaultRFProfile(uint8_t profile);
extern bool ThemistoI_SetLBTObservationPeriod(uint8_t period);
extern bool ThemistoI_SetLBTThreshold(int8_t threshold);
extern bool ThemistoI_EnableSnifferMode();

/* Write volatile settings into RAM, these settings are lost after a reset */
extern bool ThemistoI_SetVolatile_DestAddr(uint8_t destaddr_lsb, uint8_t destaddr_msb);
extern bool ThemistoI_SetVolatile_DestNetID(uint8_t destnetid);
extern bool ThemistoI_SetVolatile_TXPower(uint8_t power);
extern bool ThemistoI_SetVolatile_Channel(uint8_t channel);

extern bool ThemistoI_Ping();

#endif // THEMISTOI_H_INCLUDED

#ifdef __cplusplus
}
#endif
