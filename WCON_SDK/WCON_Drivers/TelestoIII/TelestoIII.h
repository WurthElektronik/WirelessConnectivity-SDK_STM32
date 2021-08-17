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
 * COPYRIGHT (c) 2021 Würth Elektronik eiSos GmbH & Co. KG
 *
 ***************************************************************************************************
 **/


#include <stdbool.h>
#include <stdint.h>
#include "../global/global.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _TelestoIII_defined
#define _TelestoIII_defined

#define TelestoIII_BROADCASTADDRESS 0xFF
#define MAX_USERSETTING_LENGTH 4

typedef enum TelestoIII_AddressMode_t
{
    AddressMode_0 = 0x00,
    AddressMode_1,
    AddressMode_2,
    AddressMode_3,
}
TelestoIII_AddressMode_t;

typedef enum TelestoIII_UserSettings_t
{
    TelestoIII_CMD_SETGET_OPTION_UARTBAUDRATE = 0x00,
    TelestoIII_CMD_SETGET_OPTION_DEFAULTRFPROFILE = 0x01,
    TelestoIII_CMD_SETGET_OPTION_DEFAULTRFTXPOWER = 0x02,
    TelestoIII_CMD_SETGET_OPTION_DEFAULTRFCHANNEL = 0x03,
    TelestoIII_CMD_SETGET_OPTION_DEFAULTADDRESSMODE = 0x04,
    TelestoIII_CMD_SETGET_OPTION_NUMRETRYS = 0x06,
    TelestoIII_CMD_SETGET_OPTION_DEFAULTDESTNETID = 0x07,
    TelestoIII_CMD_SETGET_OPTION_DEFAULTDESTADDR = 0x08,
    TelestoIII_CMD_SETGET_OPTION_SOURCENETID = 0x0A,
    TelestoIII_CMD_SETGET_OPTION_SOURCEADDR = 0x0B,
    TelestoIII_CMD_SETGET_OPTION_CFG_FLAGS = 0x0F,
    TelestoIII_CMD_SETGET_OPTION_RP_FLAGS = 0x10,
    TelestoIII_CMD_SETGET_OPTION_RP_NUMSLOTS = 0x11,
    TelestoIII_CMD_SETGET_OPTION_FACTORYSETTINGS = 0x20,
    TelestoIII_CMD_SETGET_OPTION_FWVERSION = 0x21,
    TelestoIII_CMD_SETGET_OPTION_RUNTIMESETTINGS = 0x22,
} TelestoIII_UserSettings_t;

/*
 * Struct repesenting  a usersetting
 * members:
 * -usersetting:  usersetting
 * -value:        value
 * -value-length: length of the value
 */
typedef struct TelestoIII_Configuration_t
{
   TelestoIII_UserSettings_t usersetting;
   uint8_t value[MAX_USERSETTING_LENGTH];
   uint8_t value_length;
} TelestoIII_Configuration_t;


extern bool TelestoIII_Init(uint32_t baudrate, FlowControl_t flow_control, TelestoIII_AddressMode_t addrmode, void(*RXcb)(uint8_t*,uint8_t,uint8_t,uint8_t,uint8_t,int8_t));
extern bool TelestoIII_Deinit(void);

extern bool TelestoIII_PinReset(void);
extern bool TelestoIII_Reset(void);

extern bool TelestoIII_Transmit_Extended(uint8_t* payload, uint8_t length, uint8_t channel, uint8_t dest_network_id, uint8_t dest_address_lsb, uint8_t dest_address_msb);
extern bool TelestoIII_Transmit(uint8_t* payload, uint8_t length);

extern bool TelestoIII_Shutdown(void);
extern bool TelestoIII_Standby(void);
extern bool TelestoIII_PinWakeup();

/* read the non-volatile settings */
extern bool TelestoIII_Get(TelestoIII_UserSettings_t us, uint8_t* response, uint8_t* response_length);
extern bool TelestoIII_GetFirmwareVersion(uint8_t* fw);
extern bool TelestoIII_GetSerialNumber(uint8_t* sn);
extern bool TelestoIII_GetDefaultTXPower(uint8_t* txpower);
extern bool TelestoIII_GetSourceAddr(uint8_t* srcaddr_lsb, uint8_t* srcaddr_msb);
extern bool TelestoIII_GetSourceNetID(uint8_t* srcnetid);
extern bool TelestoIII_GetDefaultDestAddr(uint8_t* destaddr_lsb, uint8_t* srcaddr_msb);
extern bool TelestoIII_GetDefaultDestNetID(uint8_t* destnetid);
extern bool TelestoIII_GetDefaultRFChannel(uint8_t* channel);
extern bool TelestoIII_GetDefaultRFProfile(uint8_t* profile);

/* functions that write the non-volatile settings in the flash,
 * after modification of any non-volatile setting, the module must be reset such that the update takes effect
 * IMPORTANT: use them only in rare cases, since flash can be updated only a limited number times
 */
extern bool TelestoIII_FactoryReset(void);
extern bool TelestoIII_Set(TelestoIII_UserSettings_t us, uint8_t* value, uint8_t length);
extern bool TelestoIII_Configure(TelestoIII_Configuration_t* config, uint8_t config_length, bool factory_reset);
extern bool TelestoIII_SetDefaultTXPower(uint8_t txpower);
extern bool TelestoIII_SetSourceAddr(uint8_t srcaddr_lsb, uint8_t srcaddr_msb);
extern bool TelestoIII_SetSourceNetID(uint8_t srcnetid);
extern bool TelestoIII_SetDefaultDestAddr(uint8_t destaddr_lsb, uint8_t srcaddr_msb);
extern bool TelestoIII_SetDefaultDestNetID(uint8_t destnetid);
extern bool TelestoIII_SetDefaultRFChannel(uint8_t channel);
extern bool TelestoIII_SetDefaultRFProfile(uint8_t profile);
extern bool TelestoIII_EnableSnifferMode();

/* write volatile settings into RAM, these settings are lost after a reset */
extern bool TelestoIII_SetVolatile_DestAddr(uint8_t destaddr_lsb, uint8_t destaddr_msb);
extern bool TelestoIII_SetVolatile_DestNetID(uint8_t destnetid);
extern bool TelestoIII_SetVolatile_TXPower(uint8_t power);
extern bool TelestoIII_SetVolatile_Channel(uint8_t channel);

#endif // _TelestoIII_defined
#ifdef __cplusplus
}
#endif
