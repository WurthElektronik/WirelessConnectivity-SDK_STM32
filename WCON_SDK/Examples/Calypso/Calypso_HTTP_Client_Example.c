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
 * @brief Calypso HTTP client example.
 */
#include <Calypso/ATCommands/ATDevice.h>
#include <Calypso/ATCommands/ATFile.h>
#include <Calypso/ATCommands/ATHTTP.h>
#include <Calypso/ATCommands/ATNetCfg.h>
#include <Calypso/ATCommands/ATWLAN.h>
#include <Calypso/Calypso_Examples.h>
#include <Calypso/Calypso_HTTP_Client_Example.h>
#include <stdio.h>

void Calypso_HTTP_Client_Example(void)
{
    /* Host for testing HTTP clients (httpbin has various testing features - e.g. returns sent
     * payload data or header fields in JSON string) */
    const char host[] = "httpbin.org/";

    /* Payload data that will be sent to HTTP server */
    char payload[] = "Hello world!";

    /* Enables encrypted connection */
    bool secure = true;

    /* Name of file (on Calypso module) used for storing certificate of the root CA that
     * signed the HTTP server's certificate (required for encrypted connection). */
    const char* serverRootCertFile = "http_example_root_ca.pem";

    /* Size of HTTP response chunks fetched from the Calypso module */
    uint16_t rxChunkSize = 60;

    /* Enables encoding of data transferred to/from Calypso in Base64 format */
    bool base64 = false;

    WE_APP_PRINT("*** Start of Calypso HTTP client example ***\r\n");

    bool ret = false;

    if (!Calypso_Init(&Calypso_uart, &Calypso_pins, &Calypso_Examples_EventCallback))
    {
        WE_APP_PRINT("Initialization error\r\n");
        return;
    }

    Calypso_PinReset();

    Calypso_Examples_WaitForStartup(5000);

    WE_Delay(1000);

    /* Get version info. This retrieves Calypso's firmware version (amongst other version info) and
     * stores the firmware version in Calypso_firmwareVersionMajor, Calypso_firmwareVersionMinor and
     * Calypso_firmwareVersionPatch for later use. */
    Calypso_ATDevice_Value_t deviceValue;
    ret = Calypso_ATDevice_Get(Calypso_ATDevice_GetId_General, Calypso_ATDevice_GetGeneral_Version, &deviceValue);
    Calypso_Examples_Print("Get device version", ret);

    Calypso_ATWLAN_Disconnect();

    /* Set IPv4 address method DHCP */
    Calypso_ATNetCfg_IPv4Config_t newIpV4Config = {0};
    newIpV4Config.method = Calypso_ATNetCfg_IPv4Method_Dhcp;
    ret = Calypso_ATNetCfg_SetIPv4AddressStation(&newIpV4Config);
    Calypso_Examples_Print("Set IPv4 DHCP", ret);

    /* No automatic connection */
    ret = Calypso_ATWLAN_SetConnectionPolicy(Calypso_ATWLAN_PolicyConnection_None);
    Calypso_Examples_Print("Set no WLAN connection policy", ret);

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

    char hostWithPrefix[128] = "";
    if (secure)
    {
        /* Secure host URL - prefixed with https:// */
        strcat(hostWithPrefix, "https://");
        strcat(hostWithPrefix, host);

        /* Root CA certificate for HTTPS communication with the above server.
         * Note that the certificate may change in the future and might thus become invalid. */
        const char* serverRootCert = "-----BEGIN CERTIFICATE-----\r\n"
                                     "MIIEDzCCAvegAwIBAgIBADANBgkqhkiG9w0BAQUFADBoMQswCQYDVQQGEwJVUzEl\r\n"
                                     "MCMGA1UEChMcU3RhcmZpZWxkIFRlY2hub2xvZ2llcywgSW5jLjEyMDAGA1UECxMp\r\n"
                                     "U3RhcmZpZWxkIENsYXNzIDIgQ2VydGlmaWNhdGlvbiBBdXRob3JpdHkwHhcNMDQw\r\n"
                                     "NjI5MTczOTE2WhcNMzQwNjI5MTczOTE2WjBoMQswCQYDVQQGEwJVUzElMCMGA1UE\r\n"
                                     "ChMcU3RhcmZpZWxkIFRlY2hub2xvZ2llcywgSW5jLjEyMDAGA1UECxMpU3RhcmZp\r\n"
                                     "ZWxkIENsYXNzIDIgQ2VydGlmaWNhdGlvbiBBdXRob3JpdHkwggEgMA0GCSqGSIb3\r\n"
                                     "DQEBAQUAA4IBDQAwggEIAoIBAQC3Msj+6XGmBIWtDBFk385N78gDGIc/oav7PKaf\r\n"
                                     "8MOh2tTYbitTkPskpD6E8J7oX+zlJ0T1KKY/e97gKvDIr1MvnsoFAZMej2YcOadN\r\n"
                                     "+lq2cwQlZut3f+dZxkqZJRRU6ybH838Z1TBwj6+wRir/resp7defqgSHo9T5iaU0\r\n"
                                     "X9tDkYI22WY8sbi5gv2cOj4QyDvvBmVmepsZGD3/cVE8MC5fvj13c7JdBmzDI1aa\r\n"
                                     "K4UmkhynArPkPw2vCHmCuDY96pzTNbO8acr1zJ3o/WSNF4Azbl5KXZnJHoe0nRrA\r\n"
                                     "1W4TNSNe35tfPe/W93bC6j67eA0cQmdrBNj41tpvi/JEoAGrAgEDo4HFMIHCMB0G\r\n"
                                     "A1UdDgQWBBS/X7fRzt0fhvRbVazc1xDCDqmI5zCBkgYDVR0jBIGKMIGHgBS/X7fR\r\n"
                                     "zt0fhvRbVazc1xDCDqmI56FspGowaDELMAkGA1UEBhMCVVMxJTAjBgNVBAoTHFN0\r\n"
                                     "YXJmaWVsZCBUZWNobm9sb2dpZXMsIEluYy4xMjAwBgNVBAsTKVN0YXJmaWVsZCBD\r\n"
                                     "bGFzcyAyIENlcnRpZmljYXRpb24gQXV0aG9yaXR5ggEAMAwGA1UdEwQFMAMBAf8w\r\n"
                                     "DQYJKoZIhvcNAQEFBQADggEBAAWdP4id0ckaVaGsafPzWdqbAYcaT1epoXkJKtv3\r\n"
                                     "L7IezMdeatiDh6GX70k1PncGQVhiv45YuApnP+yz3SFmH8lU+nLMPUxA2IGvd56D\r\n"
                                     "eruix/U0F47ZEUD0/CwqTRV/p2JdLiXTAAsgGh1o+Re49L2L7ShZ3U0WixeDyLJl\r\n"
                                     "xy16paq8U4Zt3VekyvggQQto8PT7dL5WXXp59fkdheMtlb71cZBDzI0fmgAKhynp\r\n"
                                     "VSJYACPq4xJDKVtHCN2MQWplBqjlIapBtJUhlbl90TSrE9atvNziPTnNvT51cKEY\r\n"
                                     "WQPJIrSPnNVeKtelttQKbfi3QBFGmh95DmK/D5fs4C8fF5Q=\r\n"
                                     "-----END CERTIFICATE-----";

        /* Write the HTTP server's root CA certificate to file */
        uint32_t fileID, secureToken;
        uint16_t bytesWritten;
        ret = Calypso_ATFile_Open(serverRootCertFile, Calypso_ATFile_OpenFlags_Create | Calypso_ATFile_OpenFlags_Overwrite, 5000, &fileID, &secureToken);
        Calypso_Examples_Print("Open server CA certificate file", ret);
        ret = Calypso_ATFile_Write(fileID, 0, Calypso_DataFormat_Binary, false, strlen(serverRootCert), serverRootCert, &bytesWritten);
        Calypso_Examples_Print("Write server CA certificate file", ret);
        ret = Calypso_ATFile_Close(fileID, NULL, NULL);
        Calypso_Examples_Print("Close server CA certificate file", ret);
    }
    else
    {
        /* Insecure host URL - prefixed with http:// */
        strcat(hostWithPrefix, "http://");
        strcat(hostWithPrefix, host);
    }

    /* HTTP client handle */
    uint8_t clientHandle;

    /* Create HTTP client */
    ret = Calypso_ATHTTP_Create(&clientHandle);
    Calypso_Examples_Print("Create HTTP client", ret);

    /* Connect to HTTP server */
    ret = Calypso_ATHTTP_Connect(clientHandle, hostWithPrefix, Calypso_ATHTTP_ConnectFlags_IgnoreProxy, NULL, NULL, secure ? serverRootCertFile : "");
    Calypso_Examples_Print("HTTP connect", ret);

    /* Using URL http(s)://host/anything - if using the httpbin service,
     * this returns a JSON string containing, among other things, the payload
     * sent to the server. */
    char url[128] = "";
    strcat(url, hostWithPrefix);
    strcat(url, "anything");
    uint32_t status;
    Calypso_DataFormat_t dataFormat = base64 ? Calypso_DataFormat_Base64 : Calypso_DataFormat_Binary;
    WE_APP_PRINT("Sending \"%s\" to HTTP server...\r\n", payload);
    ret = Calypso_ATHTTP_SendRequest(clientHandle, Calypso_ATHTTP_Method_Get, url, Calypso_ATHTTP_RequestFlags_None, dataFormat, base64, strlen(payload), payload, &status);
    Calypso_Examples_Print("Send HTTP request", ret);
    WE_APP_PRINT("HTTP status code is %ld (%s)\r\n", status, status == 200 ? "OK" : "NOK");

    /* Read HTTP response body in chunks of length up to rxChunksize */
    Calypso_ATHTTP_ResponseBody_t body;
    uint16_t chunkIndex = 0;
    do
    {
        ret = Calypso_ATHTTP_ReadResponseBody(clientHandle, dataFormat, base64, rxChunkSize, &body);
        Calypso_Examples_Print("Read HTTP response", ret);
        if (ret)
        {
            WE_APP_PRINT("Received HTTP response body chunk %d: \"%s\"\r\n", chunkIndex++, body.body);
        }
    } while (ret && body.hasMoreData);

    /* Disconnect from HTTP server */
    ret = Calypso_ATHTTP_Disconnect(clientHandle);
    Calypso_Examples_Print("HTTP disconnect client", ret);

    /* Delete HTTP client */
    ret = Calypso_ATHTTP_Destroy(clientHandle);
    Calypso_Examples_Print("Destroy HTTP client", ret);

    Calypso_Deinit();
}
