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

#include <stdio.h>
#include <string.h>

#include "../../WCON_Drivers/ProteusIII/ProteusIII.h"
#include "../../WCON_Drivers/global/global.h"

static void RxCallback(uint8_t* payload, uint16_t payload_length, uint8_t* BTMAC, int8_t rssi)
{
    int i = 0;
    printf("Received data from device with BTMAC (0x%02x%02x%02x%02x%02x%02x) with RSSI: %d dBm:\n-> ", BTMAC[0],BTMAC[1],BTMAC[2],BTMAC[3],BTMAC[4],BTMAC[5], rssi);
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

static void ConnectCallback(uint8_t* BTMAC)
{
    printf("Connected to device with BTMAC (0x%02x%02x%02x%02x%02x%02x) ", BTMAC[0],BTMAC[1],BTMAC[2],BTMAC[3],BTMAC[4],BTMAC[5]);
    printf("\n");
    fflush(stdout);
}

static void SecurityCallback(uint8_t* BTMAC, ProteusIII_Security_t security_state)
{
    printf("Security to device with BTMAC (0x%02x%02x%02x%02x%02x%02x) established ", BTMAC[0],BTMAC[1],BTMAC[2],BTMAC[3],BTMAC[4],BTMAC[5]);
    printf("\n");
    fflush(stdout);
}

static void PasskeyCallback(uint8_t* BTMAC)
{
    printf("Passkey request from device with BTMAC (0x%02x%02x%02x%02x%02x%02x) ", BTMAC[0],BTMAC[1],BTMAC[2],BTMAC[3],BTMAC[4],BTMAC[5]);
    printf("\n");
    fflush(stdout);

    uint8_t passkey[6] = {'1','2','3','1','2','3'};

    ProteusIII_Passkey(passkey);
}

static void DisplayPasskeyCallback(ProteusIII_DisplayPasskeyAction_t action, uint8_t* BTMAC, uint8_t* passkey)
{
    printf("Passkey request from device with BTMAC (0x%02x%02x%02x%02x%02x%02x) ", BTMAC[0],BTMAC[1],BTMAC[2],BTMAC[3],BTMAC[4],BTMAC[5]);
    printf("and pass key (%c%c%c%c%c%c) ", passkey[0],passkey[1],passkey[2],passkey[3],passkey[4],passkey[5]);
    printf("\n");
    fflush(stdout);

    if(ProteusIII_DisplayPasskeyAction_PleaseConfirm == action)
	{
		/* confirm key */
		ProteusIII_NumericCompareConfirm(true);
	}
}

static void DisconnectCallback()
{
    printf("Disconnected");
    printf("\n");
    fflush(stdout);
}

static void ChannelOpenCallback(uint8_t* BTMAC, uint16_t max_payload)
{
    printf("Channel opened to BTMAC (0x%02x%02x%02x%02x%02x%02x) with maximum payload: %d", BTMAC[0],BTMAC[1],BTMAC[2],BTMAC[3],BTMAC[4],BTMAC[5], max_payload);
    printf("\n");
    fflush(stdout);
}

static void PhyUpdateCallback(uint8_t* BTMAC, uint8_t phy_rx, uint8_t phy_tx)
{
    printf("Phy of connection to BTMAC (0x%02x%02x%02x%02x%02x%02x) updated (RX: %dMBit, TX: %dMBit)", BTMAC[0],BTMAC[1],BTMAC[2],BTMAC[3],BTMAC[4],BTMAC[5], phy_rx,phy_tx);
    printf("\n");
    fflush(stdout);
}

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

  ProteusIII_CallbackConfig_t callbackConfig;
  callbackConfig.rxCb = RxCallback;
  callbackConfig.connectCb = ConnectCallback;
  callbackConfig.disconnectCb = DisconnectCallback;
  callbackConfig.channelOpenCb = ChannelOpenCallback;
  callbackConfig.securityCb = SecurityCallback;
  callbackConfig.passkeyCb = PasskeyCallback;
  callbackConfig.displayPasskeyCb = DisplayPasskeyCallback;
  callbackConfig.phyUpdateCb = PhyUpdateCallback;

  ProteusIII_Init(ProteusIII_DEFAULT_BAUDRATE, WE_FlowControl_NoFlowControl, callbackConfig);

  while (1)
  {
	uint8_t version[3];
	memset(version,0,sizeof(version));
	ret = ProteusIII_GetFWVersion(version);
	WE_Delay(500);

	uint8_t BTMac[6];
	memset(BTMac,0,sizeof(BTMac));
	ret = ProteusIII_GetBTMAC(BTMac);
	WE_Delay(500);

	//ret = ProteusIII_PinReset();
	//WE_Delay(500);
  }
}
