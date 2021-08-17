/**
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
 * COPYRIGHT (c) 2021 Würth Elektronik eiSos GmbH & Co. KG
 *
 ***************************************************************************************************
 **/

#include <string.h>
#include "../../WCON_Drivers/ProteusIII/ProteusIII.h"
#include "../../WCON_Drivers/global/global.h"

static void RXcallback(uint8_t* payload, uint16_t payload_length, uint8_t* BTMAC, int8_t rssi)
{
    int i = 0;
    printf (COLOR_RED "Received data from device with BTMAC (0x%02x%02x%02x%02x%02x%02x) with RSSI: %d dBm:\n-> " COLOR_CYAN, BTMAC[0],BTMAC[1],BTMAC[2],BTMAC[3],BTMAC[4],BTMAC[5], rssi);
    printf("0x ");
    for(i=0; i<payload_length; i++)
    {
        printf ("%02x ", *(payload+i)) ;
    }
    printf ("\n-> ") ;
    for(i=0; i<payload_length; i++)
    {
        printf ("%c", *(payload+i)) ;
    }
    printf ("\n") ;
    fflush (stdout) ;
}

static void Connectcallback(uint8_t* BTMAC)
{
    printf (COLOR_RED "Connected to device with BTMAC (0x%02x%02x%02x%02x%02x%02x) " COLOR_CYAN, BTMAC[0],BTMAC[1],BTMAC[2],BTMAC[3],BTMAC[4],BTMAC[5]);
    printf ("\n") ;
    fflush (stdout) ;
}

static void Securitycallback(uint8_t* BTMAC, ProteusIII_Security_t security_state)
{
    printf (COLOR_RED "Security to device with BTMAC (0x%02x%02x%02x%02x%02x%02x) established " COLOR_CYAN, BTMAC[0],BTMAC[1],BTMAC[2],BTMAC[3],BTMAC[4],BTMAC[5]);
    printf ("\n") ;
    fflush (stdout) ;
}

static void Passkeycallback(uint8_t* BTMAC)
{
    printf (COLOR_RED "Passkey request from device with BTMAC (0x%02x%02x%02x%02x%02x%02x) " COLOR_CYAN, BTMAC[0],BTMAC[1],BTMAC[2],BTMAC[3],BTMAC[4],BTMAC[5]);
    printf ("\n") ;
    fflush (stdout) ;

    uint8_t passkey[6] = {'1','2','3','1','2','3'};

    ProteusIII_Passkey(passkey);
}

static void DisplayPasskeycallback(ProteusIII_DisplayPasskeyAction_t action, uint8_t* BTMAC, uint8_t* passkey)
{
    printf (COLOR_RED "Passkey request from device with BTMAC (0x%02x%02x%02x%02x%02x%02x) " COLOR_CYAN, BTMAC[0],BTMAC[1],BTMAC[2],BTMAC[3],BTMAC[4],BTMAC[5]);
    printf (COLOR_RED "and pass key (%c%c%c%c%c%c) " COLOR_CYAN, passkey[0],passkey[1],passkey[2],passkey[3],passkey[4],passkey[5]);
    printf ("\n") ;
    fflush (stdout) ;

    if(ProteusIII_DisplayPasskeyAction_PleaseConfirm == action)
	{
		/* confirm key */
		ProteusIII_NumericCompareConfirm(true);
	}
}

static void Disconnectcallback()
{
    printf (COLOR_RED "Disconnected" COLOR_CYAN);
    printf ("\n") ;
    fflush (stdout) ;
}

static void Channelopencallback(uint8_t* BTMAC, uint16_t max_payload)
{
    printf (COLOR_RED "Channel opened to BTMAC (0x%02x%02x%02x%02x%02x%02x) with maximum payload: %d" COLOR_CYAN, BTMAC[0],BTMAC[1],BTMAC[2],BTMAC[3],BTMAC[4],BTMAC[5], max_payload);
    printf ("\n") ;
    fflush (stdout) ;
}

static void Phyupdatecallback(uint8_t* BTMAC, uint8_t phy_rx, uint8_t phy_tx)
{
    printf (COLOR_RED "Phy of connection to BTMAC (0x%02x%02x%02x%02x%02x%02x) updated (RX: %dMBit, TX: %dMBit)" COLOR_CYAN, BTMAC[0],BTMAC[1],BTMAC[2],BTMAC[3],BTMAC[4],BTMAC[5], phy_rx,phy_tx);
    printf ("\n") ;
    fflush (stdout) ;
}

int main(void)
{
  bool ret = false;

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* Configure the system clock */
  SystemClock_Config();

  ProteusIII_CallbackConfig_t callbackConfig;
  callbackConfig.rxCb = RXcallback;
  callbackConfig.connectCp = Connectcallback;
  callbackConfig.disconnectCb = Disconnectcallback;
  callbackConfig.channelOpenCb = Channelopencallback;
  callbackConfig.securityCb = Securitycallback;
  callbackConfig.passkeyCb = Passkeycallback;
  callbackConfig.displayPasskeyCb = DisplayPasskeycallback;
  callbackConfig.phyUpdateCb = Phyupdatecallback;

  ProteusIII_Init(ProteusIII_DEFAULT_BAUDRATE, No_flow_control, callbackConfig);

  while (1)
  {
	uint8_t version[3];
	memset(version,0,sizeof(version));
	ret = ProteusIII_GetFWVersion(version);
	delay(500);

	uint8_t BTMac[6];
	memset(BTMac,0,sizeof(BTMac));
	ret = ProteusIII_GetBTMAC(BTMac);
	delay(500);

	//ret = ProteusIII_PinReset();
	//delay(500);
  }
}
