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
 * COPYRIGHT (c) 2022 Würth Elektronik eiSos GmbH & Co. KG
 *
 ***************************************************************************************************
 */

/**
 * @file
 * @brief Metis driver header file.
 */

#include <stdbool.h>
#include <stdint.h>
#include "../global/global.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * This driver supports the following AMBER modules:
 * AMB8425-M
 * TarvosI-M
 * TarvosIPlug-M
 * TarvosII-M
 * TarvosIIPlug-M
 * Titania-M
 * AMB3665-M
 */

#ifndef METIS_H_INCLUDED
#define METIS_H_INCLUDED

/**
 * Max. length of user settings (number of bytes).
 */
#define METIS_MAX_USERSETTING_LENGTH 4

/**
 * @brief Enumeration for wM-Bus mode.
 *
 * By setting the mode preselect all RF parameters are set accordingly
 * and no further adaption is necessary.
 * There are different sets of modes for 868 MHz and 169 MHz respectively.
 */
typedef enum Metis_Mode_Preselect_t
{
    MBus_Mode_868_S1          = 0x01,
    MBus_Mode_868_S1_m        = 0x02,
    MBus_Mode_868_S2          = 0x03,
    MBus_Mode_868_T1_meter    = 0x05,
    MBus_Mode_868_T1_other    = 0x06,
    MBus_Mode_868_T2_meter    = 0x07,
    MBus_Mode_868_T2_other    = 0x08,
    MBus_Mode_868_C2_T2_other = 0x09,
    MBus_Mode_868_C1_meter    = 0x0C,
    MBus_Mode_868_C2_meter    = 0x0D,
    MBus_Mode_868_C2_other    = 0x0E,

    MBus_Mode_169_N1a = 0x01,
    MBus_Mode_169_N2a = 0x02,
    MBus_Mode_169_N1b = 0x03,
    MBus_Mode_169_N2b = 0x04,
    MBus_Mode_169_N1c = 0x05,
    MBus_Mode_169_N2c = 0x06,
    MBus_Mode_169_N1d = 0x07,
    MBus_Mode_169_N2d = 0x08,
    MBus_Mode_169_N1e = 0x09,
    MBus_Mode_169_N2e = 0x0A,
    MBus_Mode_169_N1f = 0x0B,
    MBus_Mode_169_N2f = 0x0C,
} Metis_Mode_Preselect_t;

/**
 * @brief wM-Bus Frequency.
 */
typedef enum Metis_Frequency_t
{
    MBus_Frequency_169,
    MBus_Frequency_868,
} Metis_Frequency_t;

/**
 * @brief Enumeration for baud rates supported by the module.
 *
 * Used to change the baud rate of the module via CMD_SetUartspeed
 * which expects predefined values as set here.
 */
typedef enum Metis_UartBaudrate_t
{
    MBus_Baudrate_1200   = 0x00,
    MBus_Baudrate_2400   = 0x01,
    MBus_Baudrate_4800   = 0x02,
    MBus_Baudrate_9600   = 0x03,
    MBus_Baudrate_19200  = 0x04,
    MBus_Baudrate_38400  = 0x05,
    MBus_Baudrate_56000  = 0x06,
    MBus_Baudrate_115200 = 0x07,
} Metis_UartBaudrate_t;

/**
 * @brief User settings memory locations.
 */
typedef enum Metis_UserSettings_t
{
    Metis_USERSETTING_MEMPOSITION_UART_CMD_OUT_ENABLE  = 0x05,
    Metis_USERSETTING_MEMPOSITION_APP_AES_ENABLE       = 0X0B,
    Metis_USERSETTING_MEMPOSITION_DEFAULTRFTXPOWER     = 0x3D,
    Metis_USERSETTING_MEMPOSITION_RSSI_ENABLE          = 0x45,
    Metis_USERSETTING_MEMPOSITION_MODE_PRESELECT       = 0X46,
    Metis_USERSETTING_MEMPOSITION_CFG_FLAGS            = 0x50,
} Metis_UserSettings_t;

/**
 * @brief Struct representing a user setting with its value.
 */
typedef struct Metis_Configuration_t
{
    Metis_UserSettings_t usersetting;           /**< User setting */
    uint8_t value[METIS_MAX_USERSETTING_LENGTH];      /**< Value */
    uint8_t value_length;                       /**< Length of value */
} Metis_Configuration_t;

/* Functions to initialize/deinitialize the module. */
extern bool Metis_Init(uint32_t baudrate,
                       WE_FlowControl_t flow_control,
                       Metis_Frequency_t frequency,
                       Metis_Mode_Preselect_t mode,
                       bool enable_rssi,
                       void(*RXcb)(uint8_t*,uint8_t,int8_t));
extern bool Metis_Deinit(void);

extern bool Metis_PinReset(void);
extern bool Metis_Reset(void);

extern bool Metis_Transmit(uint8_t* payload);

/* Reading of non-volatile settings */
extern bool Metis_Get(Metis_UserSettings_t us, uint8_t* response, uint8_t* response_length);
extern bool Metis_GetMultiple(uint8_t startAddress, uint8_t lengthToRead, uint8_t *response, uint8_t *response_length);
extern bool Metis_GetFirmwareVersion(uint8_t* fw);
extern bool Metis_GetSerialNumber(uint8_t* sn);
extern bool Metis_GetDefaultTXPower(int8_t* txpower);
extern bool Metis_GetUartOutEnable(uint8_t* uartEnable);
extern bool Metis_GetRSSIEnable(uint8_t* rssiEnable);
extern bool Metis_GetAESEnable(uint8_t* aesEnable);
extern bool Metis_GetModePreselect(uint8_t* modePreselect);


/* Functions that write the non-volatile settings in the flash: After modification of any non-volatile setting,
 * the module must be reset such that the update takes effect.
 * IMPORTANT: Use only in rare cases, since flash can be written to only a limited number of times.
 */
extern bool Metis_FactoryReset(void);
extern bool Metis_SetUartSpeed(Metis_UartBaudrate_t baudrate);
extern bool Metis_Set(Metis_UserSettings_t us, uint8_t* value, uint8_t length);
extern bool Metis_Configure(Metis_Configuration_t* config, uint8_t config_length, bool factory_reset);
extern bool Metis_SetDefaultTXPower(int8_t txpower);
extern bool Metis_SetUartOutEnable(uint8_t uartEnable);
extern bool Metis_SetRSSIEnable(uint8_t rssiEnable);
extern bool Metis_SetAESEnable(uint8_t aesEnable);
extern bool Metis_SetModePreselect(Metis_Mode_Preselect_t modePreselect);

/* Write volatile settings into RAM, these settings are lost after a reset. */
extern bool Metis_SetVolatile_ModePreselect(Metis_Mode_Preselect_t modePreselect);

#endif // METIS_H_INCLUDED
#ifdef __cplusplus
}
#endif
