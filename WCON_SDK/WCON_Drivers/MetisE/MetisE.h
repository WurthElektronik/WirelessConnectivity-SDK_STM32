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
 * @brief Metis-e driver header file.
 */

#include <global/global_types.h>
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

#ifndef METIS_E_H_INCLUDED
#define METIS_E_H_INCLUDED

#define METIS_E_MAX_USERSETTING_LENGTH 1

#define METIS_E_DEFAULT_BAUDRATE (uint32_t)115200

#define METIS_E_RSSIINVALID (int8_t)-128

/* Masks for FLAGS */
/* RSSI output is indicated by bit 0 of cfg-flags */
/* Timestamp output is indicated by bit 1 of cfg-flags */
#define METIS_E_CFG_RSSI_ENABLE (uint8_t)0x01      /* Bit 0 for rssi enable  */
#define METIS_E_CFG_TIMESTAMP_ENABLE (uint8_t)0x02 /* Bit 1 for timestamp enable */
#define METIS_E_CFG_MASK (uint8_t)(METIS_E_CFG_RSSI_ENABLE | METIS_E_CFG_TIMESTAMP_ENABLE)

#define METIS_E_CLOCK_KHZ 4000
#define METIS_E_TICKS_TO_MS(ticks) (ticks / METIS_E_CLOCK_KHZ)

typedef struct MetisE_Pins_t
{
    WE_Pin_t MetisE_Pin_Reset;
    WE_Pin_t MetisE_Pin_SleepWakeUp;
    WE_Pin_t MetisE_Pin_Boot;
} MetisE_Pins_t;

typedef enum MetisE_wmBusMode_t
{
    MetisE_wmBusMode_S = 0,
    MetisE_wmBusMode_T_Meter,
    MetisE_wmBusMode_T_Other,
    MetisE_wmBusMode_CT_Other,
    MetisE_wmBusMode_C_Meter,
    MetisE_wmBusMode_C_Other,
    MetisE_wmBusMode_MAX,
    MetisE_wmBusMode_Undefined = 0xFF,
} MetisE_wmBusMode_t;

typedef enum MetisE_wmBusFrameFormat_t
{
    MetisE_FrameFormt_A = 1,
    MetisE_FrameFormat_B,
    MetisE_FrameFormat_Undefined = 0xFF
} MetisE_wmBusFrameFormat_t;

typedef enum MetisE_wmBusRole_t
{
    MetisE_wmBusRole_Meter = 1,
    MetisE_wmBusRole_Gateway,
    MetisE_wmBusRole_MAX,
    MetisE_wmBusRole_Undefined,
} MetisE_wmBusRole_t;

/**
 * @brief Available user/runtime settings. A subset of the available options
 */
typedef enum MetisE_UserSettings_t
{
    MetisE_USERSETTING_WMBUS_RXMODE = 0x03,
    MetisE_USERSETTING_WMBUS_ROLE = 0x07,
    MetisE_USERSETTING_UART_BAUDRATE = 0x0A,
    MetisE_USERSETTING_TXPOWER = 0x0B,
    MetisE_USERSETTING_CFGFLAGS = 0x0C,
    MetisE_USERSETTING_FACTORYSETTINGS = 0x80,
    MetisE_USERSETTING_FWVERSION = 0x81,
} MetisE_UserSettings_t;

/**
 * @brief Struct representing a user setting.
 */
typedef struct MetisE_Configuration_t
{
    MetisE_UserSettings_t usersetting;             /**< user setting */
    uint8_t value[METIS_E_MAX_USERSETTING_LENGTH]; /**< value */
    uint8_t value_length;                          /**< length of the value */
} MetisE_Configuration_t;

typedef struct MetisE_ReceivedData_t
{
    int8_t rssi;
    uint8_t payloadLength;
    uint32_t timestamp;
    uint8_t* pPayload;
    MetisE_wmBusMode_t wmBusModeRx;
    MetisE_wmBusFrameFormat_t wmBusModeFrameFormat;
} MetisE_ReceivedData_t;

/**
 * @brief Initialize the MetisE for serial interface.
 *
 * Caution: The parameter baudrate  must match the configured UserSettings of the MetisE.
 *          The baudrate parameter must match to perform a successful UART communication.
 *          Updating this parameter during runtime may lead to communication errors.
 *
 * @param[in] uartP: Definition of the uart connected to the module
 * @param[in] pinoutP: Definition of the gpios connected to the module
 * @param[in] RXcb: RX callback function
 *
 * @return True if initialization succeeded,
 *         false otherwise
 */
extern bool MetisE_Init(WE_UART_t* uartP, MetisE_Pins_t* pinoutP, void (*RXcb)(MetisE_ReceivedData_t));

/**
 * @brief Deinitialize the MetisE interface
 *
 * @return True if deinitialization succeeded,
 *         false otherwise
 */
extern bool MetisE_Deinit(void);

/**
 * @brief Reset the MetisE by pin
 *
 * @return True if reset succeeded,
 *         false otherwise
 */
extern bool MetisE_PinReset(void);

/**
 * @brief Reset the MetisE by command
 *
 * @return True if reset succeeded,
 *         false otherwise
 */
extern bool MetisE_Reset(void);

/**
 * @brief Transmit data using the configured settings
 *
 * @param[in] mode: Mode
 * @param[in] frameFormat: Frame format
 * @param[in] payloadP: Pointer to the data
 * @param[in] length: Length of the data
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool MetisE_Transmit(MetisE_wmBusMode_t mode, MetisE_wmBusFrameFormat_t frameFormat, uint8_t* payloadP, uint8_t length);

/**
 * @brief Start rx mode to receive data. Module will stay in rx mode until it is stopped with the ReceiveStop() Command.
 *
 * @return True if reset succeeded,
 *         false otherwise
 */
extern bool MetisE_ReceiveStart(void);

/**
 * @brief Stops rx mode so the module will stop receiving data
 *
 * @return True if reset succeeded,
 *         false otherwise
 */
extern bool MetisE_ReceiveStop(void);

/**
 * @brief Switch the module to shutdown mode
 *
 * @return True if switching succeeded,
 *         false otherwise
 */
extern bool MetisE_Shutdown(void);

/**
 * @brief Switch the module to standby mode
 *
 * @return True if switching succeeded,
 *         false otherwise
 */
extern bool MetisE_Standby(void);

/**
 * @brief Wake-up the MetisE from standby or shutdown by pin
 *
 * @return True if wakeup succeeded,
 *         false otherwise
 */
extern bool MetisE_PinWakeup(void);

/**
 * @brief Request the 3 byte firmware version
 *
 * @param[out] fwP: Pointer to the 3 byte firmware version
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool MetisE_GetFirmwareVersion(uint8_t* fwP);

/**
 * @brief Request the 4 byte serial number
 *
 * @param[out] snP: Pointer to the 4 byte serial number
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool MetisE_GetSerialNumber(uint8_t* snP);

/**
 * @brief Request the current MetisE settings
 *
 * @param[in] us: user setting to be requested
 * @param[out] responseP: Pointer of the memory to put the request content
 * @param[out] response_lengthP: Length of the request content
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool MetisE_GetDefault(MetisE_UserSettings_t us, uint8_t* responseP, uint8_t* response_lengthP);

/**
 * @brief Request the default TX power
 *
 * @param[out] txpowerP: Pointer to the TXpower
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool MetisE_GetDefault_TXPower(uint8_t* txpowerP);

/**
 * @brief Request the default wmBus RX Mode
 *
 * @param[out] wmbusRxModeP: Pointer to the default wmBus RxMode
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool MetisE_GetDefault_wmBusRxMode(MetisE_wmBusMode_t* wmbusRxModeP);

/**
 * @brief Request the default wmBus RX role
 *
 * @param[out] wmbusRoleP: Pointer to the default wmBus role
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool MetisE_GetDefault_wmBusRole(MetisE_wmBusRole_t* wmbusRoleP);

/**
 * @brief Request the default CFG flags
 *
 * @param[out] cfgFlagsP: Pointer to the default cfg Flags
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool MetisE_GetDefault_CFGFlags(uint8_t* cfgFlagsP);

/* read volatile settings out of RAM */

/**
 * @brief Request the volatile TX power currently in use
 *
 * @param[out] txpowerP: pointer to the TXpower
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool MetisE_GetVolatile_TXPower(uint8_t* txpowerP);

/**
 * @brief Request the volatile wmBus RX Mode currently in use
 *
 * @param[out] wmbusRxModeP: Pointer to the wmBus RxMode
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool MetisE_GetVolatile_wmBusRxMode(MetisE_wmBusMode_t* wmbusRxModeP);

/**
 * @brief Request the volatile wmBus role currently in use
 *
 * @param[out] wmbusRxRoleP: Pointer to the wmBus role
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool MetisE_GetVolatile_wmBusRole(MetisE_wmBusRole_t* wmbusRxRoleP);

/**
 * @brief Request the volatile CFG flags currently in use
 *
 * @param[out] cfgFlagsP: Pointer to the cfg Flags
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool MetisE_GetVolatile_CFGFlags(uint8_t* cfgFlagsP);

/**
 * @brief Factory reset the MetisE
 *
 * Note: use only in rare cases, since flash can be updated only a limited number of times
 *
 * @return True if factory reset succeeded,
 *         false otherwise
 */
extern bool MetisE_FactoryReset(void);

/**
 * @brief Set a MetisE default setting
 *
 * Note: Reset the module after the adaption of the setting so that it can take effect.
 * Note: Use this function only in rare case, since flash can be updated only a limited number of times.
 *
 * @param[in] us: user setting to be updated
 * @param[in] valueP: Pointer to the new settings value
 * @param[in] length: Length of the value
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool MetisE_SetDefault(MetisE_UserSettings_t us, uint8_t* valueP, uint8_t length);

/**
 * @brief Set the default TX power
 *
 * Note: Reset the module after the adaption of the setting so that it can take effect.
 * Note: Use this function only in rare case, since flash can be updated only a limited number of times.
 * Note: Use MetisE_SetVolatile_TXPower for frequent adaption of the TX power.
 *
 * @param[in] txpower: TXpower
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool MetisE_SetDefault_TXPower(uint8_t txpower);

/**
 * @brief Set the default Rx mode
 *
 * Note: Reset the module after the adaption of the setting so that it can take effect.
 * Note: Use this function only in rare case, since flash can be updated only a limited number of times.
 * Note: Use MetisE_SetVolatile_wmBusRxMode for frequent adaption of the Rx mode.
 *
 * @param[in] wmbusRxMode: wmBus Rx Mode
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool MetisE_SetDefault_wmBusRxMode(MetisE_wmBusMode_t wmbusRxMode);

/**
 * @brief Set the default Rx Role
 *
 * Note: Reset the module after the adaption of the setting so that it can take effect.
 * Note: Use this function only in rare case, since flash can be updated only a limited number of times.
 * Note: Use MetisE_SetVolatile_wmBusRxRole for frequent adaption of the Rx role.
 *
 * @param[in] wmubsRxRole: wmBus Rx role
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool MetisE_SetDefault_wmBusRole(MetisE_wmBusRole_t wmubsRxRole);

/**
 * @brief Set the default CFG flags
 *
 * Note: Reset the module after the adaption of the setting so that it can take effect.
 * Note: Use this function only in rare case, since flash can be updated only a limited number of times.
 * Note: Use MetisE_SetVolatile_CFGFlags for frequent adaption of the CFG flags.
 *
 * @param[in] cfgFlags: configuration flags to set
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool MetisE_SetDefault_CFGFlags(uint8_t cfgFlags);

/* write volatile settings into RAM, these settings are lost after a reset */

/**
 * @brief Set the volatile TX power to use immediately
 *
 * @param[in] txpower: TXpower to set
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool MetisE_SetVolatile_TXPower(uint8_t txpower);

/**
 * @brief Set the volatile Rx mode to use immediately
 *
 * @param[in] wmbusRxMode: wmBus Rx Mode to set
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool MetisE_SetVolatile_wmBusRxMode(MetisE_wmBusMode_t wmbusRxMode);

/**
 * @brief Set the volatile Rx Role to use immediately
 *
 * @param[in] wmbusRxRole: wmBus Rx role to set
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool MetisE_SetVolatile_wmBusRole(MetisE_wmBusRole_t wmbusRxRole);

/**
 * @brief Set the volatile CFG flags to use immediately
 *
 * @param[in] cfgFlags: configuration flags to set
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool MetisE_SetVolatile_CFGFlags(uint8_t cfgFlags);

#endif // METIS_E_H_INCLUDED

#ifdef __cplusplus
}
#endif
