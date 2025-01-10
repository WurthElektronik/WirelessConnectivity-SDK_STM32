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

#ifndef SKOLL_I_EXAMPLES_HANDLER_H_INCLUDED
#define SKOLL_I_EXAMPLES_HANDLER_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#include <Skoll_I/Skoll_I.h>

typedef struct Skoll_I_Common_States_t
{
	bool pairing_requested;
} Skoll_I_Common_States_t;

typedef struct Skoll_I_BluetoothClassic_States_t
{
	uint16_t conn_handle;
	bool inquiry_running;
	bool connected;
	bool pinentry_requested;
} Skoll_I_BluetoothClassic_States_t;

typedef struct Skoll_I_BluetoothLE_States_t
{
	uint16_t conn_handle;
	bool scan_running;
	bool connected;
	bool passkeyentry_requested;
} Skoll_I_BluetoothLE_States_t;

typedef struct Skoll_I_BluetoothLE_SPPLike_States_t
{
	bool peer_is_subscribed;
} Skoll_I_BluetoothLE_SPPLike_States_t;

typedef struct Skoll_I_BluetoothLE_CYSPP_States_t
{
	bool peer_is_subscribed;
} Skoll_I_BluetoothLE_CYSPP_States_t;

/* States of the Skoll-I, separated in Bluetooth LE, Bluetooth Classic, CYSPP profile and Bluetooth LE SPPlike profile */
typedef struct Skoll_I_States_t
{
	Skoll_I_Common_States_t Common;
	Skoll_I_BluetoothClassic_States_t Classic;
	Skoll_I_BluetoothLE_States_t LE;
	Skoll_I_BluetoothLE_SPPLike_States_t LE_SPPLike;
	Skoll_I_BluetoothLE_CYSPP_States_t LE_CYSPP;
} Skoll_I_States_t;

extern Skoll_I_States_t states;

/**
 * @brief Handler dealing with events and response messages received via UART
 *
 * @param packet UART message that has been received
 *
 * @return None
 */
extern void eventHandler(ezs_packet_t *packet);

/**
 * @brief Handler that is used, when we entered the transparent data mode
 *
 * @param payload Received data
 * @param payloadLength Length of the received data
 *
 * @return None
 */
extern void transparentDataHandler(uint8_t *payload, uint16_t payloadLength);

/**
 * @brief Prints the supplied string, prefixed with OK or NOK (depending on the success parameter).
 *
 * @param str String to print
 * @param success Variable indicating if action was ok
 *
 * @return None
 */
extern void Examples_Print(char *str, bool success);

/**
 * @brief Prints the supplied string as ascii characters
 *
 * @param text Array to print
 * @param width Length of the array
 *
 * @return None
 */
extern void print_hexstring(const char *text, size_t width);

/**
 * @brief Prints the supplied string as hex values prepended with a "0x"
 *
 * @param text Array to print
 * @param width Length of the array
 *
 * @return None
 */
extern void print_ascii(const char *text, size_t width);

#ifdef __cplusplus
}
#endif

#endif /* SKOLL_I_EXAMPLES_HANDLER_H_INCLUDED */
