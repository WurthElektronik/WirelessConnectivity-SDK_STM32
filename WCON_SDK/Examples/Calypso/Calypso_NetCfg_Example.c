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
 * @brief Calypso network configuration example.
 */
#include <Calypso/ATCommands/ATDevice.h>
#include <Calypso/ATCommands/ATNetCfg.h>
#include <Calypso/ATCommands/ATWLAN.h>
#include <Calypso/Calypso_Examples.h>
#include <Calypso/Calypso_NetCfg_Example.h>
#include <stdio.h>

/**
 * @brief Network configuration example (e.g. setting/getting IP and MAC configuration).
 */
void Calypso_NetCfg_Example(void)
{
    WE_DEBUG_PRINT("*** Start of Calypso ATNetCfg example ***\r\n");

    bool ret = false;

    if (!Calypso_Init(&Calypso_uart, &Calypso_pins, &Calypso_Examples_EventCallback))
    {
        WE_DEBUG_PRINT("Initialization error\r\n");
        return;
    }

    Calypso_PinReset();

    Calypso_Examples_WaitForStartup(5000);

    /* Get version info. This retrieves Calypso's firmware version (amongst other version info) and
	 * stores the firmware version in Calypso_firmwareVersionMajor, Calypso_firmwareVersionMinor and
	 * Calypso_firmwareVersionPatch for later use. */
    Calypso_ATDevice_Value_t deviceValue;
    ret = Calypso_ATDevice_Get(Calypso_ATDevice_GetId_General, Calypso_ATDevice_GetGeneral_Version, &deviceValue);
    Calypso_Examples_Print("Get device version", ret);

    /* WLAN station mode */
    ret = Calypso_ATWLAN_SetMode(Calypso_ATWLAN_SetMode_Station);
    Calypso_Examples_Print("Set WLAN station mode", ret);
    ret = Calypso_ATDevice_Restart(0);
    Calypso_Examples_Print("Restart network processor", ret);

    WE_Delay(1000);

    /* Connect to WLAN */
    Calypso_ATWLAN_ConnectionArguments_t connectArgs;
    memset(&connectArgs, 0, sizeof(connectArgs));
    strcpy(connectArgs.SSID, Calypso_Examples_wlanSSID);
    connectArgs.securityParams.securityType = Calypso_ATWLAN_SecurityType_WPA_WPA2;
    strcpy(connectArgs.securityParams.securityKey, Calypso_Examples_wlanKey);

    ret = Calypso_ATWLAN_Connect(connectArgs);
    Calypso_Examples_Print("Connect to WLAN", ret);

    WE_Delay(2000);

    /* Set interface modes */
    ret = Calypso_ATNetCfg_SetInterfaceModes(Calypso_ATNetCfg_InterfaceMode_IPv6StationLocal | Calypso_ATNetCfg_InterfaceMode_DisableFastRenew);
    Calypso_Examples_Print("Set interface modes", ret);

    WE_Delay(2000);

    /* Set MAC address */
    uint8_t newMacAddress[6] = {0x68, 0x47, 0x49, 0xc, 0x22, 0x43};
    ret = Calypso_ATNetCfg_SetMacAddress(newMacAddress);
    Calypso_Examples_Print("Set MAC address", ret);

    /* Get MAC address */
    uint8_t macAddress[6];
    ret = Calypso_ATNetCfg_GetMacAddress(macAddress);
    Calypso_Examples_Print("Get MAC address", ret);
    if (ret)
    {
        WE_DEBUG_PRINT("MAC address: %02X-%02X-%02X-%02X-%02X-%02X\r\n", macAddress[0], macAddress[1], macAddress[2], macAddress[3], macAddress[4], macAddress[5]);
    }

    /* Set IPv4 configuration (station) */
    Calypso_ATNetCfg_IPv4Config_t newIpV4Config = {0};
    newIpV4Config.method = Calypso_ATNetCfg_IPv4Method_Static;
    strcpy(newIpV4Config.ipAddress, "192.168.0.42");
    strcpy(newIpV4Config.subnetMask, "255.255.255.0");
    strcpy(newIpV4Config.gatewayAddress, "192.168.0.1");
    strcpy(newIpV4Config.dnsAddress, "192.168.0.23");
    ret = Calypso_ATNetCfg_SetIPv4AddressStation(&newIpV4Config);
    Calypso_Examples_Print("Set IPv4 config (station)", ret);

    /* Set IPv4 configuration (access point) */
    newIpV4Config.method = Calypso_ATNetCfg_IPv4Method_Static;
    strcpy(newIpV4Config.ipAddress, "192.168.0.142");
    strcpy(newIpV4Config.subnetMask, "255.255.255.0");
    strcpy(newIpV4Config.gatewayAddress, "192.168.0.101");
    strcpy(newIpV4Config.dnsAddress, "192.168.0.123");
    ret = Calypso_ATNetCfg_SetIPv4AddressAP(&newIpV4Config);
    Calypso_Examples_Print("Set IPv4 config (AP)", ret);

    /* Get IPv4 configuration (station) */
    Calypso_ATNetCfg_IPv4Config_t ipV4Config;
    ret = Calypso_ATNetCfg_GetIPv4AddressStation(&ipV4Config);
    Calypso_Examples_Print("Get IPv4 config (station)", ret);
    if (ret)
    {
        WE_DEBUG_PRINT("*** Station IPv4 configuration ***\r\n");
        WE_DEBUG_PRINT("IPv4 address: %s\r\n", ipV4Config.ipAddress);
        WE_DEBUG_PRINT("Subnet mask: %s\r\n", ipV4Config.subnetMask);
        WE_DEBUG_PRINT("Gateway address: %s\r\n", ipV4Config.gatewayAddress);
        WE_DEBUG_PRINT("DNS address: %s\r\n", ipV4Config.dnsAddress);
    }

    /* Get IPv4 configuration (access point) */
    ret = Calypso_ATNetCfg_GetIPv4AddressAP(&ipV4Config);
    Calypso_Examples_Print("Get IPv4 config (AP)", ret);
    if (ret)
    {
        WE_DEBUG_PRINT("*** Access point IPv4 configuration ***\r\n");
        WE_DEBUG_PRINT("IPv4 address: %s\r\n", ipV4Config.ipAddress);
        WE_DEBUG_PRINT("Subnet mask: %s\r\n", ipV4Config.subnetMask);
        WE_DEBUG_PRINT("Gateway address: %s\r\n", ipV4Config.gatewayAddress);
        WE_DEBUG_PRINT("DNS address: %s\r\n", ipV4Config.dnsAddress);
    }

    /* Set IPv6 configuration (local) */
    Calypso_ATNetCfg_IPv6Config_t newIpV6Config = {0};
    newIpV6Config.method = Calypso_ATNetCfg_IPv6Method_Static;
    strcpy(newIpV6Config.ipAddress, "0xfe800000:0x0:0x0:0x42");
    ret = Calypso_ATNetCfg_SetIPv6AddressLocal(&newIpV6Config);
    Calypso_Examples_Print("Set IPv6 config (local)", ret);

    /* Set IPv6 configuration (global) */
    newIpV6Config.method = Calypso_ATNetCfg_IPv6Method_Static;
    strcpy(newIpV6Config.ipAddress, "0xfe800000:0x0:0x0:0x43");
    strcpy(newIpV6Config.dnsAddress, "0xfe800000:0x0:0x0:0x44");
    ret = Calypso_ATNetCfg_SetIPv6AddressGlobal(&newIpV6Config);
    Calypso_Examples_Print("Set IPv6 config (global)", ret);

    /* Get IPv6 configuration (local) */
    Calypso_ATNetCfg_IPv6Config_t ipV6Config;
    ret = Calypso_ATNetCfg_GetIPv6AddressLocal(&ipV6Config);
    Calypso_Examples_Print("Get IPv6 config (local)", ret);
    if (ret)
    {
        WE_DEBUG_PRINT("*** Local IPv6 configuration ***\r\n");
        WE_DEBUG_PRINT("IPv6 address: %s\r\n", ipV6Config.ipAddress);
    }

    /* Get IPv6 configuration (global) */
    ret = Calypso_ATNetCfg_GetIPv6AddressGlobal(&ipV6Config);
    Calypso_Examples_Print("Get IPv6 config (global)", ret);
    if (ret)
    {
        WE_DEBUG_PRINT("*** Global IPv6 configuration ***\r\n");
        WE_DEBUG_PRINT("IPv6 address: %s\r\n", ipV6Config.ipAddress);
    }

    /* Set second DNS server address (IPv4) */
    char newDns2ndServerAddress[16] = "192.168.178.42";
    ret = Calypso_ATNetCfg_SetIPv4DnsClient(newDns2ndServerAddress);
    Calypso_Examples_Print("Set 2nd DNS server (IPv4)", ret);

    /* Get second DNS server address (IPv4) */
    char dns2ndServerAddress[16];
    ret = Calypso_ATNetCfg_GetIPv4DnsClient(dns2ndServerAddress);
    Calypso_Examples_Print("Get 2nd DNS server (IPv4)", ret);
    if (ret)
    {
        WE_DEBUG_PRINT("DNS second server address: %s\r\n", dns2ndServerAddress);
    }

    /* Reset interface modes */
    ret = Calypso_ATNetCfg_SetInterfaceModes(Calypso_ATNetCfg_InterfaceMode_None);
    Calypso_Examples_Print("Reset interface modes", ret);

    Calypso_Deinit();
}
