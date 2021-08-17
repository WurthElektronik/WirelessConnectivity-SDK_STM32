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
#include "../../WCON_Drivers/ThyoneI/ThyoneI.h"
#include "../../WCON_Drivers/global/global.h"

/* callback for data reception */
static void RXcallback(uint8_t* payload, uint16_t payload_length, uint32_t sourceAddress, int8_t rssi)
{
    int i = 0;
    printf ("Received data from address 0x%02x with %d dBm:\n-> ", sourceAddress, rssi);
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

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  bool ret = false;

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* Configure the system clock */
  SystemClock_Config();

  ThyoneI_Init(ThyoneI_DEFAULT_BAUDRATE, No_flow_control, RXcallback);

  while (1)
  {
	uint8_t version[3];
	memset(version,0,sizeof(version));
	ret = ThyoneI_GetFWVersion(version);
	delay(500);

	uint8_t SN[4];
	memset(SN,0,sizeof(SN));
	ret = ThyoneI_GetSerialNumber(SN);
	delay(500);

	ret = ThyoneI_PinReset();
	delay(500);
  }
}
