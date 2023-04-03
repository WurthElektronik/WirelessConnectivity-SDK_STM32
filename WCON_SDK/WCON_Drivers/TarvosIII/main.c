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
 * COPYRIGHT (c) 2023 Würth Elektronik eiSos GmbH & Co. KG
 *
 ***************************************************************************************************
 */

/**
 * @file
 * @brief Tarvos-III example.
 */

#include <stdio.h>
#include <string.h>

#include "../../WCON_Drivers/TarvosIII/TarvosIII.h"
#include "../../WCON_Drivers/global/global.h"

/* callback for data reception */
static void RxCallback(uint8_t* payload, uint8_t payload_length, uint8_t dest_network_id, uint8_t dest_address_lsb, uint8_t dest_address_msb, int8_t rssi)
{
    int i = 0;
    printf("Received data from address (NetID:0x%02x,Addr:0x%02x%02x) with %d dBm:\n-> ", dest_network_id, dest_address_lsb, dest_address_msb, rssi);
    printf("0x ");
    for(i=0; i<payload_length; i++)
    {
        printf("%02x ", *(payload+i));
    }
    printf("\n-> ");
    for(i=0; i<payload_length; i++)
    {
        printf("%c", *(payload+i));
    }
    printf("\n");
    fflush(stdout);
}

/**
 * @brief The application's main function.
 */
int main(void)
{
    bool ret = false;

    /* Initialize platform (peripherals, flash interface, Systick, system clock) */
    WE_Platform_Init();

#ifdef WE_DEBUG
    WE_Debug_Init();
#endif

    uint8_t driverVersion[3];
    WE_GetDriverVersion(driverVersion);
    printf("Wuerth Elektronik eiSos Wireless Connectivity SDK version %d.%d.%d\r\n", driverVersion[0], driverVersion[1], driverVersion[2]);

    TarvosIII_Init(TARVOSIII_DEFAULT_BAUDRATE, WE_FlowControl_NoFlowControl, AddressMode_0, RxCallback);

    while (1)
    {
        uint8_t version[3];
        memset(version,0,sizeof(version));
        ret = TarvosIII_GetFirmwareVersion(version);
        WE_Delay(500);

        uint8_t SN[4];
        memset(SN,0,sizeof(SN));
        ret = TarvosIII_GetSerialNumber(SN);
        WE_Delay(500);

        uint8_t payload[4*16] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
        ret = TarvosIII_Transmit(payload, sizeof(payload));
        WE_Delay(500);

        ret = TarvosIII_PinReset();
        WE_Delay(500);
    }
}
