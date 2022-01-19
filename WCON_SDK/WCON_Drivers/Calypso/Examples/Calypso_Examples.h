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
 * @brief Calypso examples.
 *
 * Comment/uncomment lines in Calypso_Examples() to start the desired example.
 */

#ifndef CALYPSO_EXAMPLES_H_INCLUDED
#define CALYPSO_EXAMPLES_H_INCLUDED

#include <Calypso/ATCommands/ATEvent.h>

#ifdef __cplusplus
extern "C" {
#endif

extern const uint32_t Calypso_Examples_baudRate;
extern const WE_FlowControl_t Calypso_Examples_flowControl;
extern const WE_Parity_t Calypso_Examples_parity;

extern const char *Calypso_Examples_wlanSSID;
extern const char *Calypso_Examples_wlanKey;

extern ATEvent_Startup_t Calypso_Examples_startupEvent;
extern bool Calypso_Examples_startupEventReceived;
extern bool Calypso_Examples_ip4Acquired;

extern void Calypso_Examples(void);
extern void Calypso_Examples_Print(char* str, bool success);
extern bool Calypso_Examples_WaitForStartup(uint32_t timeoutMs);
extern bool Calypso_Examples_WaitForIPv4Acquired(uint32_t timeoutMs);
extern void Calypso_Examples_EventCallback(char* eventText);

#ifdef __cplusplus
}
#endif

#endif /* CALYPSO_EXAMPLES_H_INCLUDED */
