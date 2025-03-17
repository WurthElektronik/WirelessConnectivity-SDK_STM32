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
 * @brief Calypso Azure Plug and Play example.
 */

#ifndef CALYPSO_AZURE_PNP_EXAMPLE_H_INCLUDED
#define CALYPSO_AZURE_PNP_EXAMPLE_H_INCLUDED

/**
 * Parameters to be configured by the user
 */
/************************************************************************************************************/
/*WLAN credentials*/
#define CALYPSO_AZURE_PNP_WLAN_SSID "calypso_CAFFEE123456"
#define CALYPSO_AZURE_PNP_WLAN_PW "calypsowlan"

/*SNTP server addresses*/
#define SNTP_SERVER_ADDRESS_3 "pool.ntp.org"
#define SNTP_SERVER_ADDRESS_2 "europe.pool.ntp.org"
#define SNTP_SERVER_ADDRESS_1 "de.pool.ntp.org"
/*Provisioning  server address*/
#define DPS_SERVER_ADDRESS "global.azure-devices-provisioning.net"
#define MQTT_PORT (uint16_t)8883

/*Parameters from the IoT central application*/
#define CALYPSO_AZURE_PNP_DEVICE_ID "Calypso-129000000"
#define SCOPE_ID "0ms000E0000"
#define MODEL_ID "dtmi:wurthelektronik:module:calypso;1"

/*Root CA certificate to connect to Azure IoT central*/
#define DIGICERT_GLOBAL_G2_ROOT_CERT                                                                                                                                                                                                                                                                                                                                                                           \
    "-----BEGIN CERTIFICATE-----\r\n\
MIIDjjCCAnagAwIBAgIQAzrx5qcRqaC7KGSxHQn65TANBgkqhkiG9w0BAQsFADBh\r\n\
MQswCQYDVQQGEwJVUzEVMBMGA1UEChMMRGlnaUNlcnQgSW5jMRkwFwYDVQQLExB3\r\n\
d3cuZGlnaWNlcnQuY29tMSAwHgYDVQQDExdEaWdpQ2VydCBHbG9iYWwgUm9vdCBH\r\n\
MjAeFw0xMzA4MDExMjAwMDBaFw0zODAxMTUxMjAwMDBaMGExCzAJBgNVBAYTAlVT\r\n\
MRUwEwYDVQQKEwxEaWdpQ2VydCBJbmMxGTAXBgNVBAsTEHd3dy5kaWdpY2VydC5j\r\n\
b20xIDAeBgNVBAMTF0RpZ2lDZXJ0IEdsb2JhbCBSb290IEcyMIIBIjANBgkqhkiG\r\n\
9w0BAQEFAAOCAQ8AMIIBCgKCAQEAuzfNNNx7a8myaJCtSnX/RrohCgiN9RlUyfuI\r\n\
2/Ou8jqJkTx65qsGGmvPrC3oXgkkRLpimn7Wo6h+4FR1IAWsULecYxpsMNzaHxmx\r\n\
1x7e/dfgy5SDN67sH0NO3Xss0r0upS/kqbitOtSZpLYl6ZtrAGCSYP9PIUkY92eQ\r\n\
q2EGnI/yuum06ZIya7XzV+hdG82MHauVBJVJ8zUtluNJbd134/tJS7SsVQepj5Wz\r\n\
tCO7TG1F8PapspUwtP1MVYwnSlcUfIKdzXOS0xZKBgyMUNGPHgm+F6HmIcr9g+UQ\r\n\
vIOlCsRnKPZzFBQ9RnbDhxSJITRNrw9FDKZJobq7nMWxM4MphQIDAQABo0IwQDAP\r\n\
BgNVHRMBAf8EBTADAQH/MA4GA1UdDwEB/wQEAwIBhjAdBgNVHQ4EFgQUTiJUIBiV\r\n\
5uNu5g/6+rkS7QYXjzkwDQYJKoZIhvcNAQELBQADggEBAGBnKJRvDkhj6zHd6mcY\r\n\
1Yl9PMWLSn/pvtsrF9+wX3N3KjITOYFnQoQj8kVnNeyIv/iPsGEMNKSuIEyExtv4\r\n\
NeF22d+mQrvHRAiGfzZ0JFrabA0UWTW98kndth/Jsw1HKj2ZL7tcu7XUIOGZX1NG\r\n\
Fdtom/DzMNU+MeKNhJ7jitralj41E6Vf8PlwUHBHQRFXGU7Aj64GxJUTFy8bJZ91\r\n\
8rGOmaFvE7FBcf6IKshPECBV1/MUReXgRPTqh5Uykw7+U0b6LJ3/iyK5S9kJRaTe\r\n\
pLiaWN0bfVKfjllDiIGknibVb63dDcY3fe0Dkhvld1927jyNxF1WW6LZZm6zNTfl\r\n\
MrY=\r\n\
-----END CERTIFICATE-----"

/*Device certificate chain*/
#define DEVICE_CERT                                                                                                                                                                                                                                                                                                                                                                                            \
    "-----BEGIN CERTIFICATE-----\r\n\
-----END CERTIFICATE-----\r\n\
-----BEGIN CERTIFICATE-----\r\n\
-----END CERTIFICATE-----"

/*Device key*/
#define DEVICE_KEY                                                                                                                                                                                                                                                                                                                                                                                             \
    "-----BEGIN EC PRIVATE KEY-----\r\n\
-----END EC PRIVATE KEY-----"

/************************************************************************************************************/

/*File path to certificate files stored on Calypso internal storage*/
#define ROOT_CA_PATH "user/azrootca"
#define DEVICE_CERT_PATH "user/azdevcert"
#define DEVICE_KEY_PATH "user/azdevkey"
#define DEVICE_IOT_HUB_ADDRESS "user/azhubaddr"
#define CONFIG_FILE_PATH "user/azdevconf"

/*MQTT topics for provisioning*/
#define PROVISIONING_RESP_TOPIC "$dps/registrations/res/#"
#define PROVISIONING_REG_REQ_TOPIC "$dps/registrations/PUT/iotdps-register/?$rid="
#define PROVISIONING_STATUS_REQ_TOPIC "$dps/registrations/GET/iotdps-get-operationstatus/?$rid="

/*MQTT topics for telemetry, properties and direct methods*/
#define DEVICE_TWIN_DESIRED_PROP_RES_TOPIC "$iothub/twin/PATCH/properties/desired/#"
#define DEVICE_TWIN_RES_TOPIC "$iothub/twin/res/#"
#define DIRECT_METHOD_TOPIC "$iothub/methods/POST/#"
#define DEVICE_TWIN_MESSAGE_PATCH "$iothub/twin/PATCH/properties/reported/?$rid="
#define DEVICE_TWIN_GET_TOPIC "$iothub/twin/GET/?$rid="

extern void Calypso_Azure_PnP_Example(void);

#endif /* CALYPSO_AZURE_PNP_EXAMPLE_H_INCLUDED */
