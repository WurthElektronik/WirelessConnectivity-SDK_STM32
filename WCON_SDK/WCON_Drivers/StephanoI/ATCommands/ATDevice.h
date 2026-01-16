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
 * @file ATDevice.h
 * @brief AT commands for basic device functionality.
 */

#ifndef STEPHANOI_DEVICE_H_INCLUDED
#define STEPHANOI_DEVICE_H_INCLUDED

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define ATDEVICE_DEEPSLEEP_TIME_MAX (uint32_t)2147483647 /**< Maximum timeout for sleep command */

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief UART parity
 */
typedef enum StephanoI_ATDevice_UartParity_t
{
    StephanoI_ATDevice_UartParity_None = 0,
    StephanoI_ATDevice_UartParity_Odd = 1,
    StephanoI_ATDevice_UartParity_Even = 2,
} StephanoI_ATDevice_UartParity_t;

/**
 * @brief UART flow control
 */
typedef enum StephanoI_ATDevice_UartFlowControl_t
{
    StephanoI_ATDevice_UartFlowControl_None = 0,
    StephanoI_ATDevice_UartFlowControl_RTS = 1,
    StephanoI_ATDevice_UartFlowControl_CTS = 2,
    StephanoI_ATDevice_UartFlowControl_RTSnCTS = 3,
} StephanoI_ATDevice_UartFlowControl_t;

/**
 * @brief UART configuration
 */
typedef struct StephanoI_ATDevice_UART_t
{
    uint32_t baudrate;
    StephanoI_ATDevice_UartParity_t parity;
    StephanoI_ATDevice_UartFlowControl_t flow_control;
} StephanoI_ATDevice_UART_t;

/**
 * @brief Sleep mode
 */
typedef enum StephanoI_ATDevice_SleepMode_t
{
    StephanoI_ATDevice_SleepMode_Disable = 0,
    StephanoI_ATDevice_SleepMode_Modem = 1,
    StephanoI_ATDevice_SleepMode_Light = 2,
    StephanoI_ATDevice_SleepMode_ModemInterval = 3,
} StephanoI_ATDevice_SleepMode_t;

/**
 * @brief Wake-up source
 */
typedef enum StephanoI_ATDevice_WakeupSource_t
{
    StephanoI_ATDevice_WakeupSource_Reserved0 = 0,
    StephanoI_ATDevice_WakeupSource_Reserved1 = 1,
    StephanoI_ATDevice_WakeupSource_GPIO = 2,
} StephanoI_ATDevice_WakeupSource_t;

/**
 * @brief Wake-up pin number
 */
typedef enum StephanoI_ATDevice_WakeupPin_t
{
    StephanoI_ATDevice_WakeupPin_GPIO3 = 3,
    StephanoI_ATDevice_WakeupPin_GPIO10 = 10
} StephanoI_ATDevice_WakeupPin_t;

/**
 * @brief Bluetooth LE power value
 */
typedef enum StephanoI_ATDevice_BluetoothLEPower_t
{
    StephanoI_ATDevice_BluetoothLEPower_None = -1,
    StephanoI_ATDevice_BluetoothLEPower_Minus27 = 0,
    StephanoI_ATDevice_BluetoothLEPower_Minus24 = 1,
    StephanoI_ATDevice_BluetoothLEPower_Minus21 = 2,
    StephanoI_ATDevice_BluetoothLEPower_Minus18 = 3,
    StephanoI_ATDevice_BluetoothLEPower_Minus15 = 4,
    StephanoI_ATDevice_BluetoothLEPower_Minus12 = 5,
    StephanoI_ATDevice_BluetoothLEPower_Minus9 = 6,
    StephanoI_ATDevice_BluetoothLEPower_Minus6 = 7,
    StephanoI_ATDevice_BluetoothLEPower_Minus3 = 8,
    StephanoI_ATDevice_BluetoothLEPower_0 = 9,
    StephanoI_ATDevice_BluetoothLEPower_Plus3 = 10,
    StephanoI_ATDevice_BluetoothLEPower_Plus6 = 11,
    StephanoI_ATDevice_BluetoothLEPower_Plus9 = 12,
    StephanoI_ATDevice_BluetoothLEPower_Plus12 = 13,
    StephanoI_ATDevice_BluetoothLEPower_Plus15 = 14,
    StephanoI_ATDevice_BluetoothLEPower_Plus18 = 15,
} StephanoI_ATDevice_BluetoothLEPower_t;

/**
 * @brief Wifi power value
 */
typedef enum StephanoI_ATDevice_WifiPower_t
{
    StephanoI_ATDevice_WifiPower_Plus10 = 40,
    StephanoI_ATDevice_WifiPower_Plus11 = 44,
    StephanoI_ATDevice_WifiPower_Plus12 = 48,
    StephanoI_ATDevice_WifiPower_Plus13 = 52,
    StephanoI_ATDevice_WifiPower_Plus14 = 56,
    StephanoI_ATDevice_WifiPower_Plus15 = 60,
    StephanoI_ATDevice_WifiPower_Plus16 = 64,
    StephanoI_ATDevice_WifiPower_Plus17 = 68,
    StephanoI_ATDevice_WifiPower_Plus18 = 72,
    StephanoI_ATDevice_WifiPower_Plus19 = 76,
    StephanoI_ATDevice_WifiPower_Plus20 = 80,
} StephanoI_ATDevice_WifiPower_t;

/**
 * @brief Manufacturer data type
 */
typedef enum StephanoI_ATDevice_SYSMFG_data_t
{
    StephanoI_ATDevice_SYSMFG_data_u8 = 1,
    StephanoI_ATDevice_SYSMFG_data_i8 = 2,
    StephanoI_ATDevice_SYSMFG_data_u16 = 3,
    StephanoI_ATDevice_SYSMFG_data_i16 = 4,
    StephanoI_ATDevice_SYSMFG_data_u32 = 5,
    StephanoI_ATDevice_SYSMFG_data_i32 = 6,
    StephanoI_ATDevice_SYSMFG_data_string = 7,
    StephanoI_ATDevice_SYSMFG_data_binary = 8
} StephanoI_ATDevice_SYSMFG_data_t;

/**
 * @brief Manufacturer data
 */
typedef struct StephanoI_ATDevice_SYSMFG_t
{
    char namespace[20];
    char key[20];
    StephanoI_ATDevice_SYSMFG_data_t type;
    uint16_t length;
    uint8_t* value;
} StephanoI_ATDevice_SYSMFG_t;

/**
 * @brief File system read
 */
typedef struct StephanoI_ATDevice_FileSystemRead_t
{
    uint16_t length;
    uint8_t* value;
} StephanoI_ATDevice_FileSystemRead_t;

/**
 * @brief Tests the connection to the wireless module (using the AT command)
 *
 * @return True if successful, false otherwise
 */
extern bool StephanoI_ATDevice_Test();

/**
 * @brief Get the version of the firmware
 *
 * @return True if successful, false otherwise
 */
extern bool StephanoI_ATDevice_GetVersion();

/**
 * @brief Resets the device by command
 *
 * @return True if successful, false otherwise
 */
extern bool StephanoI_ATDevice_Reset();

/**
 * @brief Performs a factory reset of the device (using the AT+RESTORE command)
 *
 * @return True if successful, false otherwise
 */
extern bool StephanoI_ATDevice_Restore();

/**
 * @brief Switch on or off the echo
 *
 * @param[in] on: Echo, true for enable, false for disable
 *
 * @return True if successful, false otherwise
 */
extern bool StephanoI_ATDevice_EchoOn(bool on);

/**
 * @brief Set the current UART configuration
 *
 * @param[in] t: UART configuration
 *
 * @return True if successful, false otherwise
 */
extern bool StephanoI_ATDevice_SetCurrentUART(StephanoI_ATDevice_UART_t t);

/**
 * @brief Set the default UART configuration
 *
 * @param[in] t: UART configuration
 *
 * @return True if successful, false otherwise
 */
extern bool StephanoI_ATDevice_SetDefaultUART(StephanoI_ATDevice_UART_t t);

/**
 * @brief Get the default UART configuration
 *
 * @param[out] t: Pointer to the configuration
 *
 * @return True if successful, false otherwise
 */
extern bool StephanoI_ATDevice_GetDefaultUART(StephanoI_ATDevice_UART_t* t);

/**
 * @brief Enable the sleep mode
 *
 * @param[in] mode: Sleep mode
 *
 * @return True if successful, false otherwise
 */
extern bool StephanoI_ATDevice_SetSleep(StephanoI_ATDevice_SleepMode_t mode);

/**
 * @brief Read the sleep mode
 *
 * @param[out] modeP: Pointer to the sleep mode
 *
 * @return True if successful, false otherwise
 */
extern bool StephanoI_ATDevice_GetSleep(StephanoI_ATDevice_SleepMode_t* modeP);

/**
 * @brief Enable the deep sleep mode
 *
 * @param[in] time_ms: Sleep time in ms
 *
 * @return True if successful, false otherwise
 */
extern bool StephanoI_ATDevice_SetDeepSleep(uint32_t time_ms);

/**
 * @brief Define the wake-up source
 *
 * @param[in] pin_number: Pin number
 * @param[in] active_high: High if 'true', low otherwise
 *
 * @return True if successful, false otherwise
 */
extern bool StephanoI_ATDevice_SetWakeUpSource(StephanoI_ATDevice_WakeupPin_t pin_number, bool active_high);

/**
 * @brief Set the TX power
 *
 * @param[in] wifi_power: Wifi TX power
 * @param[in] ble_power: Bluetooth LE TX power
 *
 * @return True if successful, false otherwise
 */
extern bool StephanoI_ATDevice_SetTXPower(StephanoI_ATDevice_WifiPower_t wifi_power, StephanoI_ATDevice_BluetoothLEPower_t ble_power);

/**
 * @brief Get the TX power
 *
 * @param[out] wifi_powerP: Pointer to Wifi TX power
 * @param[out] ble_powerP: Pointer to Bluetooth LE TX power
 *
 * @return True if successful, false otherwise
 */
extern bool StephanoI_ATDevice_GetTXPower(StephanoI_ATDevice_WifiPower_t* wifi_powerP, StephanoI_ATDevice_BluetoothLEPower_t* ble_powerP);

/**
 * @brief Query all namespaces of the manufacturing user partitions
 *
 * @return True if successful, false otherwise
 */
extern bool StephanoI_ATDevice_GetManufacturingUserPartitions();

/**
 * @brief Erase the manufacturing user partitions
 *
 * @param[in] namespace: Namespace
 * @param[in] key: Key
 *
 * @return True if successful, false otherwise
 */
extern bool StephanoI_ATDevice_EraseManufacturingUserPartitions(char* namespace, char* key);

/**
 * @brief Read the manufacturing user partitions
 *
 * @param[in] namespace: Namespace
 * @param[in] key: Key
 * @param[in] offset: Offset of the requested data
 * @param[in] length: Length of the requested data
 * @param[out] t: Pointer to the data to be read
 *
 * @return True if successful, false otherwise
 */
extern bool StephanoI_ATDevice_ReadManufacturingUserPartitions(char* namespace, char* key, uint16_t offset, uint16_t length, StephanoI_ATDevice_SYSMFG_t* t);

/**
 * @brief Write the manufacturing user partitions
 *
 * @param[in] namespace: Namespace
 * @param[in] key: Key
 * @param[in] type: Type of data
 * @param[in] length: Length of the data
 * @param[in] dataP: Data
 *
 * @return True if successful, false otherwise
 */
extern bool StephanoI_ATDevice_WriteManufacturingUserPartitions(char* namespace, char* key, StephanoI_ATDevice_SYSMFG_data_t type, uint16_t length, uint8_t* dataP);

/**
 * @brief Set the system time stamp
 *
 * @param[in] time: Timestamp
 *
 * @return True if successful, false otherwise
 */
extern bool StephanoI_ATDevice_SetSystemTimestamp(uint32_t time);

/**
 * @brief Get the system time stamp
 *
 * @param[in] timeP: Timestamp
 *
 * @return True if successful, false otherwise
 */
extern bool StephanoI_ATDevice_GetSystemTimestamp(uint32_t* timeP);

/**
 * @brief Set the system store mode
 *
 * @param[in] enable: True is 'enable', false is 'disable" system store mode
 *
 * @return True if successful, false otherwise
 */
extern bool StephanoI_ATDevice_SetSystemStoremode(bool enable);

/**
 * @brief Get the system store mode
 *
 * @param[in] enableP: Pointer to the value
 *
 * @return True if successful, false otherwise
 */
extern bool StephanoI_ATDevice_GetSystemStoremode(bool* enableP);

/**
 * @brief Mount/Unmount the file system
 *
 * @param[in] enable: True is 'enable', false is 'disable" the file system
 *
 * @return True if successful, false otherwise
 */
extern bool StephanoI_ATDevice_FileSystemSetMount(bool enable);

/**
 * @brief Get the file system file list
 *
 * @return True if successful, false otherwise
 */
extern bool StephanoI_ATDevice_FileSystemGetFiles();

/**
 * @brief Delete a file system file
 *
 * @param[in] filename: File name
 *
 * @return True if successful, false otherwise
 */
extern bool StephanoI_ATDevice_FileSystemDelete(char* filename);

/**
 * @brief Read a file from the file system
 *
 * @param[in] filename: File name
 * @param[in] offset: Offset of the requested data
 * @param[in] length: Length of the requested data
 * @param[out] t: Pointer to the data to be read
 *
 * @return True if successful, false otherwise
 */
extern bool StephanoI_ATDevice_FileSystemRead(char* filename, uint16_t offset, uint16_t length, StephanoI_ATDevice_FileSystemRead_t* t);

/**
 * @brief Get size of file system file
 *
 * @param[in] filename: File name
 * @param[out] sizeP: Pointer to the size
 *
 * @return True if successful, false otherwise
 */
extern bool StephanoI_ATDevice_FileSystemGetFileSize(char* filename, uint16_t* sizeP);

/**
 * @brief Write a file to the file system
 *
 * @param[in] filename: File name
 * @param[in] offset: Offset of the requested data
 * @param[in] length: Length of the requested data
 * @param[in] dataP: Pointer to the data to be written
 *
 * @return True if successful, false otherwise
 */
extern bool StephanoI_ATDevice_FileSystemWrite(char* filename, uint16_t offset, uint16_t length, uint8_t* dataP);

/**
 * @brief Get the system temperature
 *
 * @param[in] tempP: Pointer to the timestamp
 *
 * @return True if successful, false otherwise
 */
extern bool StephanoI_ATDevice_GetSystemTemp(float* tempP);

/**
 * @brief Parses the values of the SYSMFG event arguments
 *
 * @param[in] EventArgumentsP: String containing arguments of the AT command
 * @param[out] t: The parsed event data
 *
 * @return True if parsed successfully, false otherwise
 */
extern bool StephanoI_ATDevice_ParseManufacturingUserPartitions(char* EventArgumentsP, StephanoI_ATDevice_SYSMFG_t* t);

/**
 * @brief Parses the values of the SYSTIMESTAMP event arguments
 *
 * @param[in] EventArgumentsP: String containing arguments of the AT command
 * @param[out] t: The parsed event data
 *
 * @return True if parsed successfully, false otherwise
 */
extern bool StephanoI_ATDevice_ParseSystemTimestamp(char* EventArgumentsP, uint32_t* t);

/**
 * @brief Parses the values of the SYSSTORE event arguments
 *
 * @param[in] EventArgumentsP: String containing arguments of the AT command
 * @param[out] t: The parsed event data
 *
 * @return True if parsed successfully, false otherwise
 */
extern bool StephanoI_ATDevice_ParseSystemStoremode(char* EventArgumentsP, bool* t);

/**
 * @brief Parses the values of the system temp event arguments
 *
 * @param[in] EventArgumentsP: String containing arguments of the AT command
 * @param[out] t: The parsed event data
 *
 * @return True if parsed successfully, false otherwise
 */
extern bool StephanoI_ATDevice_ParseSystemTemp(char* EventArgumentsP, float* t);

/**
 * @brief Parses the values of the get file system file size event arguments
 *
 * @param[in] EventArgumentsP: String containing arguments of the AT command
 * @param[out] t: The parsed event data
 *
 * @return True if parsed successfully, false otherwise
 */
extern bool StephanoI_ATDevice_ParseFileSystemGetFilesize(char* EventArgumentsP, uint16_t* t);

/**
 * @brief Parses the values of the file system read event arguments
 *
 * @param[in] EventArgumentsP: String containing arguments of the AT command
 * @param[out] t: The parsed event data
 *
 * @return True if parsed successfully, false otherwise
 */
extern bool StephanoI_ATDevice_ParseFileSystemRead(char* EventArgumentsP, StephanoI_ATDevice_FileSystemRead_t* t);

/**
 * @brief Parses the values of the get sleep event arguments
 *
 * @param[in] EventArgumentsP: String containing arguments of the AT command
 * @param[out] t: The parsed event data
 *
 * @return True if parsed successfully, false otherwise
 */
extern bool StephanoI_ATDevice_ParseSleep(char* EventArgumentsP, StephanoI_ATDevice_SleepMode_t* t);

#ifdef __cplusplus
}
#endif

#endif /* STEPHANOI_DEVICE_H_INCLUDED */
