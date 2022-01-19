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
 * @brief Calypso driver general AT command definitions.
 */

#ifndef AT_COMMMANDS_H_INCLUDED
#define AT_COMMMANDS_H_INCLUDED

/**
 * @brief Size of buffer used for commands sent to the wireless module and the responses
 * received from the module.
 *
 * May be adopted to required file/data sizes.
 */
#define AT_MAX_COMMAND_BUFFER_SIZE 2048

/**
 * @brief Max. length of host name strings (e.g. URLs or IP addresses).
 */
#define AT_MAX_HOST_NAME_LENGTH 128

/**
 * @brief Max. length of IP address strings.
 */
#define AT_MAX_IP_ADDRESS_LENGTH 44

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Buffer used for commands sent to the wireless module and the responses
 * received from the module.
 */
extern char AT_commandBuffer[AT_MAX_COMMAND_BUFFER_SIZE];

#ifdef __cplusplus
}
#endif

#endif /* AT_COMMMANDS_H_INCLUDED */
