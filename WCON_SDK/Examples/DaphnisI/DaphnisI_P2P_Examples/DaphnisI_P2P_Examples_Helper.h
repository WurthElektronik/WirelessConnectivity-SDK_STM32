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
 * @brief DaphnisI LoRaWAN example.
 *
 */

#ifndef DAPHNISI_P2P_EXAMPLES_HELPER_H_INCLUDED
#define DAPHNISI_P2P_EXAMPLES_HELPER_H_INCLUDED

#include <DaphnisI/ATCommands/ATUserSettings.h>
#include <DaphnisI/DaphnisI_Examples.h>

#if DAPHNISI_MIN_FW_VER >= FW(1, 4, 0)

#ifdef __cplusplus
extern "C"
{
#endif

extern DaphnisI_Setting_Status_t DaphnisI_P2P_Role_Check_and_Set(DaphnisI_P2P_Role_t new_p2p_role);

extern DaphnisI_Setting_Status_t DaphnisI_P2P_RF_Profile_Check_and_Set(uint8_t new_rf_profile);

extern DaphnisI_Setting_Status_t DaphnisI_P2P_DC_Enforce_Check_and_Set(bool new_dc_enforce);

extern DaphnisI_Setting_Status_t DaphnisI_P2P_GPIO_Remote_Config_Check_and_Set(bool new_gpio_remote_cfg);

#ifdef __cplusplus
}
#endif

#endif

#endif /* DAPHNISI_P2P_EXAMPLES_HELPER_H_INCLUDED */
