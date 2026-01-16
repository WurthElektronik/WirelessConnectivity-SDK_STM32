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
 * @brief Main file for DaphnisI driver examples.
 *
 */
#include <DaphnisI/ATCommands/ATUserSettings.h>
#include <DaphnisI/DaphnisI.h>
#include <DaphnisI/DaphnisI_Examples.h>
#include <DaphnisI/DaphnisI_P2P_Examples/DaphnisI_P2P_Examples_Helper.h>
#include <global/global.h>
#include <stdio.h>

#if DAPHNISI_MIN_FW_VER >= FW(1, 4, 0)

DaphnisI_Setting_Status_t DaphnisI_P2P_Role_Check_and_Set(DaphnisI_P2P_Role_t new_p2p_role)
{
    DaphnisI_P2P_Role_t current_p2p_role;

    if (!DaphnisI_GetP2PRoleRS(&current_p2p_role))
    {
        return DaphnisI_Setting_Status_Failure;
    }

    if (current_p2p_role == new_p2p_role)
    {
        return DaphnisI_Setting_Status_Unmodified;
    }

    if (!DaphnisI_SetP2PRoleUS(new_p2p_role))
    {
        return DaphnisI_Setting_Status_Failure;
    }

    return DaphnisI_Setting_Status_Modified;
}

DaphnisI_Setting_Status_t DaphnisI_P2P_RF_Profile_Check_and_Set(uint8_t new_rf_profile)
{
    uint8_t current_rf_profile;

    if (!DaphnisI_GetP2PRFProfileRS(&current_rf_profile))
    {
        return DaphnisI_Setting_Status_Failure;
    }

    if (current_rf_profile == new_rf_profile)
    {
        return DaphnisI_Setting_Status_Unmodified;
    }

    if (!DaphnisI_SetP2PRFProfileUS(new_rf_profile))
    {
        return DaphnisI_Setting_Status_Failure;
    }

    return DaphnisI_Setting_Status_Modified;
}

DaphnisI_Setting_Status_t DaphnisI_P2P_DC_Enforce_Check_and_Set(bool new_dc_enforce)
{
    bool current_dc_enforce;

    if (!DaphnisI_GetP2PDutyCycleEnforceRS(&current_dc_enforce))
    {
        return DaphnisI_Setting_Status_Failure;
    }

    if (current_dc_enforce == new_dc_enforce)
    {
        return DaphnisI_Setting_Status_Unmodified;
    }

    if (!DaphnisI_SetP2PDutyCycleEnforceUS(new_dc_enforce))
    {
        return DaphnisI_Setting_Status_Failure;
    }

    return DaphnisI_Setting_Status_Modified;
}

DaphnisI_Setting_Status_t DaphnisI_P2P_GPIO_Remote_Config_Check_and_Set(bool new_gpio_remote_cfg)
{
    bool current_gpio_remote_cfg;

    if (!DaphnisI_GetP2PGPIORemoteCfgBlockRS(&current_gpio_remote_cfg))
    {
        return DaphnisI_Setting_Status_Failure;
    }

    if (current_gpio_remote_cfg == new_gpio_remote_cfg)
    {
        return DaphnisI_Setting_Status_Unmodified;
    }

    if (!DaphnisI_SetP2PGPIORemoteCfgBlockUS(new_gpio_remote_cfg))
    {
        return DaphnisI_Setting_Status_Failure;
    }

    return DaphnisI_Setting_Status_Modified;
}

#endif /* DAPHNISI_MIN_FW_VER >= FW(1, 4, 0) */
