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
 * COPYRIGHT (c) 2025 Würth Elektronik eiSos GmbH & Co. KG
 *
 ***************************************************************************************************
 */

/**
 * @file
 * @brief Bluetooth LE advertising functionality example.
 */

#ifndef STEPHANOI_BLUETOOTHLE_EXAMPLE_H_INCLUDED
#define STEPHANOI_BLUETOOTHLE_EXAMPLE_H_INCLUDED

#include <StephanoI/StephanoI.h>
#include <StephanoI/ATCommands/ATBluetoothLE.h>

extern StephanoI_ATBluetoothLE_Peripheral_DiscoverService_t StephanoI_Examples_Peripheral_DiscoverService;
extern StephanoI_ATBluetoothLE_Connection_t StephanoI_Examples_Connection;
extern StephanoI_ATBluetoothLE_MTU_t StephanoI_Examples_MTU;
extern StephanoI_ATBluetoothLE_Write_t StephanoI_Examples_Write;
extern StephanoI_ATBluetoothLE_Central_Scan_t StephanoI_Examples_Central_Scan;
extern StephanoI_ATBluetoothLE_Central_DiscoverPrimaryService_t StephanoI_Examples_Central_DiscoverPrimaryService;
extern StephanoI_ATBluetoothLE_Central_DiscoverIncludedServices_t StephanoI_Examples_Central_DiscoverIncludedServices;
extern StephanoI_ATBluetoothLE_Central_DiscoverCharacteristics_t StephanoI_Examples_Central_DiscoverCharacteristics;

extern void StephanoI_BluetoothLE_Peripheral_Advertising_Example(void);
extern void StephanoI_BluetoothLE_Peripheral_Data_Example(void);
extern void StephanoI_BluetoothLE_Central_Data_Example(void);
extern void StephanoI_BluetoothLE_Central_ThroughputData_Example(void);

#endif /* STEPHANOI_BLUETOOTHLE_EXAMPLE_H_INCLUDED */
