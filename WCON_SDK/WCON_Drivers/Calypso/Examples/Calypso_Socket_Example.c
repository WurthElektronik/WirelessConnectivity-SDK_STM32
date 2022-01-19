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
 * @brief Calypso socket example.
 */

#include "Calypso_Socket_Example.h"

#include <stdio.h>

#include <Calypso/ATCommands/ATDevice.h>
#include <Calypso/ATCommands/ATEvent.h>
#include <Calypso/ATCommands/ATNetApp.h>
#include <Calypso/ATCommands/ATNetCfg.h>
#include <Calypso/ATCommands/ATSocket.h>
#include <Calypso/ATCommands/ATWLAN.h>
#include <Calypso/Calypso.h>

#include "Calypso_Examples.h"

/**
 * @brief Static IPv4 address used for server in TCP and UDP examples
 */
static char* socketExampleServerAddress = "192.168.178.42";

/**
 * @brief Static IPv4 address used for client in TCP and UDP examples
 */
static char* socketExampleClientAddress = "192.168.178.43";

/**
 * @brief Subnet mask used for TCP and UDP examples
 */
static char* socketExampleSubnetMask = "255.255.255.0";

/**
 * @brief Gateway address used for TCP and UDP examples
 */
static char* socketExampleGateway = "192.168.178.1";

/**
 * @brief DNS server address used for TCP and UDP examples
 */
static char* socketExampleDns = "192.168.178.1";

/**
 * @brief Port of server in TCP and UDP examples
 */
static uint16_t socketExampleServerPort = 8888;

/**
 * @brief Enables encryption in socket example
 */
static bool socketExampleSecure = false;

/**
 * @brief Is set to true when the TCP server has accepted a connection. Used in TCP server example.
 */
static bool tcpServerConnectionAccepted = false;

/**
 * @brief Last TCP server accept event (filled in when a client connects to the server). Used in TCP server example.
 */
static ATEvent_SocketTCPAccept_t tcpServerAcceptEvent = {0};

/**
 * @brief Is set to true when the TCP client has established a connection to the server. Used in TCP client example.
 */
static bool tcpClientConnectionEstablished = false;

/**
 * @brief Last TCP client connect event (filled in when the client has established a connection to the server). Used in TCP client example.
 */
static ATEvent_SocketTCPConnect_t tcpClientConnectEvent = {0};

/**
 * @brief Is set to true when a TCP connection has been established. Used in TCP server and client examples.
 */
static bool tcpConnected = false;

/**
 * @brief Is set to true when waiting for incoming data via TCP or UDP
 */
static bool socketExampleWaitingForData = false;

/**
 * @brief Rx buffer for received TCP or UDP data. Note: Max recommended payload size is 1460 bytes.
 */
static char socketExampleReceiveBuffer[CALYPSO_MAX_PAYLOAD_SIZE + 1];

void Calypso_Socket_Example_EventCallback(char* eventText);
void Calypso_Socket_Example_OnDataReceived(ATEvent_SocketRcvd_t *rcvdEvent);

/**
 * @brief TCP server example. Sets IPv4 address to socketExampleServerAddress,
 * creates a TCP socket, waits for incoming connections on port socketExampleServerPort
 * and then receives/prints any incoming data.
 */
void Calypso_TCPServer_Example(void)
{
    printf("*** Start of Calypso ATSocket TCP server example ***\r\n");

    bool ret = false;

    /* ID of socket on which the server listens for incoming connections */
    uint8_t socketID;

    /* ID of socket connected to client (is set when a client connects to this server) */
    uint8_t tcpServerClientSocketID = 0;

    if (!Calypso_Init(Calypso_Examples_baudRate, Calypso_Examples_flowControl, Calypso_Examples_parity, &Calypso_Socket_Example_EventCallback, NULL))
    {
        return;
    }

    Calypso_PinReset();

    WE_Delay(2000);

    /* Get version info. This retrieves Calypso's firmware version (amongst other version info) and
     * stores the firmware version in Calypso_firmwareVersionMajor, Calypso_firmwareVersionMinor and
     * Calypso_firmwareVersionPatch for later use. */
    ATDevice_Value_t deviceValue;
    ret = ATDevice_Get(ATDevice_GetId_General, ATDevice_GetGeneral_Version, &deviceValue);
    Calypso_Examples_Print("Get device version", ret);

    /* WLAN station mode */
    ret = ATWLAN_SetMode(ATWLAN_SetMode_Station);
    Calypso_Examples_Print("Set WLAN station mode", ret);
    ret = ATDevice_Restart(0);
    Calypso_Examples_Print("Restart network processor", ret);

    WE_Delay(1000);

    /* Connect to WLAN */
    ATWLAN_ConnectionArguments_t connectArgs;
    memset(&connectArgs, 0, sizeof(connectArgs));
    strcpy(connectArgs.SSID, Calypso_Examples_wlanSSID);
    connectArgs.securityParams.securityType = ATWLAN_SecurityType_WPA_WPA2;
    strcpy(connectArgs.securityParams.securityKey, Calypso_Examples_wlanKey);

    ret = ATWLAN_Connect(connectArgs);
    Calypso_Examples_Print("Connect to WLAN", ret);

    WE_Delay(2000);

    /* Set static IPv4 configuration for server */
    ATNetCfg_IPv4Config_t newIpV4Config = {0};
    newIpV4Config.method = ATNetCfg_IPv4Method_Static;
    strcpy(newIpV4Config.ipAddress, socketExampleServerAddress);
    strcpy(newIpV4Config.subnetMask, socketExampleSubnetMask);
    strcpy(newIpV4Config.gatewayAddress, socketExampleGateway);
    strcpy(newIpV4Config.dnsAddress, socketExampleDns);
    ret = ATNetCfg_SetIPv4AddressStation(&newIpV4Config);
    Calypso_Examples_Print("Set IPv4 config", ret);

    /* Get IPv4 configuration (station) */
    ATNetCfg_IPv4Config_t ipV4Config;
    ret = ATNetCfg_GetIPv4AddressStation(&ipV4Config);
    Calypso_Examples_Print("Get IPv4 config", ret);
    if (ret)
    {
        printf("*** Station IPv4 configuration ***\r\n");
        printf("IPv4 address: %s\r\n", ipV4Config.ipAddress);
        printf("Subnet mask: %s\r\n", ipV4Config.subnetMask);
        printf("Gateway address: %s\r\n", ipV4Config.gatewayAddress);
        printf("DNS address: %s\r\n", ipV4Config.dnsAddress);
    }

    /* Create TCP socket, bind it to a port and start listening for incoming connections */
    ret = ATSocket_Create(ATSocket_Family_INET,
                          ATSocket_Type_Stream,
                          socketExampleSecure ? ATSocket_Protocol_SEC : ATSocket_Protocol_TCP,
                          &socketID);
    Calypso_Examples_Print("Create socket", ret);

    if (socketExampleSecure)
    {
        /* Using secure sockets - must configure certificates and other socket options */
        const char *keyFileName = "dummy-trusted-cert-key.pem";
        const char *certificateFileName = "dummy-trusted-cert.pem";

        uint32_t fileID;
        uint32_t secureToken;

        /* Write the server's private key */
        ret = ATFile_Open(keyFileName,
                          (ATFile_OpenFlags_Create | ATFile_OpenFlags_Overwrite),
                          5000,
                          &fileID,
                          &secureToken);
        Calypso_Examples_Print("Open key file", ret);

        uint16_t bytesWritten = 0;
        const char* key = "-----BEGIN RSA PRIVATE KEY-----\r\n"
                "MIIEowIBAAKCAQEAybKyk3wdqP40p/QvZo+vAMWLpV8RLTlv1Jdno5dPUz7WzQZg\r\n"
                "CwKqt1c4FWbq3XJszRaEtniFO6gNj1IIKalFrYjq7JPlhp5qir/b0LTuJwKvAIRn\r\n"
                "st2FP9YpvcBf3e/0UyMI4nbnOw3scrkR7VA8S0MkPraR88OSb2LitWbCxPqATlM8\r\n"
                "94odKO5RuJ+9j/2590yVAFM/Wfazl/OR04nYFTyLIMUXyMSrfEj/fnyvGO+4hJfg\r\n"
                "jh5p9YLu/bwYso5ALxHMoUbYXK89S6SdLz3shoIQpmdnVCoUNdmG4vk2V/vGEftS\r\n"
                "WTbn3sVKSlnkQ0tfFtr+7jhgIFbfrJncwLBujwIDAQABAoIBAEjkuR8OGxYJEPSm\r\n"
                "Dj9trByGSEqyyd1jRaQ6N61qsLNwIWclZ65y/0mVTuKFOFph0SppefsMIXKG8Kec\r\n"
                "SPDtiQ4HTofDh48YT37nicSqrusQUDjKoUOGRXLlrl4yWTM3VkB5h+t4d2bjSFHJ\r\n"
                "ys3OUowMrtuvVZ4YTyLMRQy0H63aCYfxLDNpxOR4HZsxxIC4M5cjUbsoHteFS68G\r\n"
                "zUIlnG0o/iW/MeoXnOMWISzQ4nVQKoE8+cKaBV2CEqh1DD84KGv2OfcayANuoNJV\r\n"
                "rChNEhhC3aVK4BnT8ohTJpd9Jp0s487ylyU2kS6uO6YYloyfBNdQEzuS2pNmKn18\r\n"
                "kLYX+5ECgYEA9+7fX1AepGvdp36viTi+liRnSKK+i4Az676HZFNNSZyM7ecl7Uto\r\n"
                "o5PCD7VbtE13sCOL5Ee3ue01/HAOCifWHH/qz+ylgX+0etYZfSHMcbwcAs1JNagv\r\n"
                "9mnlKq08kBCoW8v2GPhJqnytbPgoNIj0tK+F2aQ9nltZQSJGkXXE57kCgYEA0EK3\r\n"
                "Ru77wFpv63EOyPl0E6aMnaE2IMAcWUZKqEwTdNCAMZ5Bmab8/eGZQRlLh6Y/lO3H\r\n"
                "i729jl5zW/PVoUlQAMr/tbCiQbrvwPfNcOaNPhNVxEMOuQe653scM4qTafPjrT6y\r\n"
                "33mz9gtRX4chtxub8XzdNVUnlAkvO0zYih2SHIcCgYEAiwpSCHC1Dn5Tasg4Bttk\r\n"
                "vmyh43EpLLBz7aQG57Nzp3F88znq4h2lC/8kGI0bZY0ALy5wGFBD76fg27W3mehn\r\n"
                "QcpW084hrvMAMtb4ql0W+z2D8+pOQSIV67nNQ9atHRnKYNiMJcqv/UU4OOEeIwAf\r\n"
                "Q2kfxkM+Lx2Je2aObqq+BzkCgYAcrRMPcVi447sFcEvY4IBbNFOoYMpTOCCvKj6d\r\n"
                "bdw/+XWtX10pCxVK4AV6MUF1LC8VEqinVTkEoXzA7JI94xphAZK0Md4lrtfUekqP\r\n"
                "gS9xnLfyjkk4sRfLR+haMHqb3Dk9YaS/oMJm3qgVv0WlMOgIGGldLpyEhb87Kxml\r\n"
                "6uxzgQKBgHXcBJY1I4SwcHBXGOsLChk3RAHr1dGGizreB05fGfcuXWdTOSELlSAA\r\n"
                "Rsl5/raQqsktCaGpdUGIwHKioZ+ymruY2PxbOznywei6IXW6VwMHeMYiRvupqCtb\r\n"
                "xh/+YPmmSn97k3JNfGEU+cOpCr6CEhJrLpB9DutRBfNme/PI9hE/\r\n"
                "-----END RSA PRIVATE KEY-----";
        ret = ATFile_Write(fileID,
                           0,
                           Calypso_DataFormat_Binary,
                           false,
                           strlen(key),
                           key,
                           &bytesWritten);
        Calypso_Examples_Print("Write key file", ret);
        printf("Wrote key file \"%s\" (length %d)\r\n", keyFileName, strlen(key));

        ret = ATFile_Close(fileID, NULL, NULL);
        Calypso_Examples_Print("Close key file", ret);


        /* Write the server's certificate */
        ret = ATFile_Open(certificateFileName,
                          (ATFile_OpenFlags_Create | ATFile_OpenFlags_Overwrite),
                          ATFILE_FILE_MIN_SIZE,
                          &fileID,
                          &secureToken);
        Calypso_Examples_Print("Open certificate file", ret);

        bytesWritten = 0;
        const char * certificate = "-----BEGIN CERTIFICATE-----\r\n"
                "MIID8TCCAtmgAwIBAgIBATANBgkqhkiG9w0BAQsFADB/MQswCQYDVQQGEwJJTDEP\r\n"
                "MA0GA1UECAwGU2hhcm9uMRAwDgYDVQQHDAdSYWFuYW5hMR8wHQYDVQQKDBZUZXhh\r\n"
                "cyBJbnN0cnVtZW50cyBJbmMuMQwwCgYDVQQLDANSTkQxHjAcBgNVBAMMFWR1bW15\r\n"
                "LXRydXN0ZWQtY2EtY2VydDAeFw0xNjA5MTExNDExNTNaFw0yNjA5MDkxNDExNTNa\r\n"
                "MHwxCzAJBgNVBAYTAklMMQ8wDQYDVQQIDAZTaGFyb24xEDAOBgNVBAcMB1JhYW5h\r\n"
                "bmExHzAdBgNVBAoMFlRleGFzIEluc3RydW1lbnRzIEluYy4xDDAKBgNVBAsMA1JO\r\n"
                "RDEbMBkGA1UEAwwSZHVtbXktdHJ1c3RlZC1jZXJ0MIIBIjANBgkqhkiG9w0BAQEF\r\n"
                "AAOCAQ8AMIIBCgKCAQEAybKyk3wdqP40p/QvZo+vAMWLpV8RLTlv1Jdno5dPUz7W\r\n"
                "zQZgCwKqt1c4FWbq3XJszRaEtniFO6gNj1IIKalFrYjq7JPlhp5qir/b0LTuJwKv\r\n"
                "AIRnst2FP9YpvcBf3e/0UyMI4nbnOw3scrkR7VA8S0MkPraR88OSb2LitWbCxPqA\r\n"
                "TlM894odKO5RuJ+9j/2590yVAFM/Wfazl/OR04nYFTyLIMUXyMSrfEj/fnyvGO+4\r\n"
                "hJfgjh5p9YLu/bwYso5ALxHMoUbYXK89S6SdLz3shoIQpmdnVCoUNdmG4vk2V/vG\r\n"
                "EftSWTbn3sVKSlnkQ0tfFtr+7jhgIFbfrJncwLBujwIDAQABo3sweTAJBgNVHRME\r\n"
                "AjAAMCwGCWCGSAGG+EIBDQQfFh1PcGVuU1NMIEdlbmVyYXRlZCBDZXJ0aWZpY2F0\r\n"
                "ZTAdBgNVHQ4EFgQUfr0oSfwFBf0/3ZsLAf3gxUuCoy4wHwYDVR0jBBgwFoAUZBNZ\r\n"
                "rat6KZdGgsv3gSkontGSDgowDQYJKoZIhvcNAQELBQADggEBAGdBUq6NwyZ1cKM9\r\n"
                "73cUEoG80zH6AU2Xi792NiAmd/uLW4VN08s1aUE4gXj45ivGNwTOTwyiDZYwGCem\r\n"
                "xXd/BOQ0ecMgTweCmhrFdChQvHgkR/a6VDFTSpzwtBwRLmNZE1Ppdle+2VEIBpcb\r\n"
                "OZcpRUnkkQXs9tnmYFmXDmD5Ey5GY00+B7igVikkrI7baeHCLW4ol/6/oAzDF3qO\r\n"
                "gIimbefrgV0KEMWooiA9FEa72KwsyHDfT1QY4sJZZRkSzNqBrLNRDiZDWx5JT4u6\r\n"
                "MnFhPhzKmQEGX+cwkhf43IgXO20W46dp0Vx6UzMjgGTEL6W9uXMXDh45U1N5HOx2\r\n"
                "ZeojP/c=\r\n"
                "-----END CERTIFICATE-----\r\n"
                "-----BEGIN CERTIFICATE-----\r\n"
                "MIIDxjCCAq6gAwIBAgIBATANBgkqhkiG9w0BAQsFADB8MQswCQYDVQQGEwJJTDEP\r\n"
                "MA0GA1UECAwGU2hhcm9uMRAwDgYDVQQHDAdSYWFuYW5hMR8wHQYDVQQKDBZUZXhh\r\n"
                "cyBJbnN0cnVtZW50cyBJbmMuMQwwCgYDVQQLDANSTkQxGzAZBgNVBAMMEmR1bW15\r\n"
                "LXJvb3QtY2EtY2VydDAeFw0xNjA5MTExMzQwMDJaFw0yNjA5MDkxMzQwMDJaMH8x\r\n"
                "CzAJBgNVBAYTAklMMQ8wDQYDVQQIDAZTaGFyb24xEDAOBgNVBAcMB1JhYW5hbmEx\r\n"
                "HzAdBgNVBAoMFlRleGFzIEluc3RydW1lbnRzIEluYy4xDDAKBgNVBAsMA1JORDEe\r\n"
                "MBwGA1UEAwwVZHVtbXktdHJ1c3RlZC1jYS1jZXJ0MIIBIjANBgkqhkiG9w0BAQEF\r\n"
                "AAOCAQ8AMIIBCgKCAQEA9ovPCLWx50nQ0iQUsHbrE68bQm64SRHSAn8KT6vc/5dZ\r\n"
                "kYyqxGz446dmuC2TeYQQcVJjYrrC7fOgjzhHJIKm0mlxwB4WOFKp2hQa3yunahN6\r\n"
                "aowa31pXhOECaHp3zaPKAK1tQdhLZPMTldnvTY+ltBcoeLCOSuB6L+9PimMW7dzO\r\n"
                "UWZY5AihERlHCcFT1JjHhq5bY4DDrTa//r/nzgx8hrJIFSeZDdcBVqhHE7tr1zzy\r\n"
                "zRFWGZRQXirGn/pJEodPwoZ5vqi5JZo5LPVW8C7MK/l3uNLJ8U0Co+hqhgSxnolG\r\n"
                "EF0vWvZA11YTm5JmyEGFs8Mb0YOS4CEEcW5Y3k/PlwIDAQABo1AwTjAdBgNVHQ4E\r\n"
                "FgQUZBNZrat6KZdGgsv3gSkontGSDgowHwYDVR0jBBgwFoAU/ctvc/HkJk5Wpz4n\r\n"
                "oopsPF7GsTMwDAYDVR0TBAUwAwEB/zANBgkqhkiG9w0BAQsFAAOCAQEAb75KbUFc\r\n"
                "hDeowjdNjGDm07vhAUak0/rD4Uu66PqCBCor3cajViZD8S7SNrdJREPoGABHH9/z\r\n"
                "m9M/HsPrcaFkC5EvXIF1vpr21ESml94y+QJDsUv7xg+udlDC3lqpuKr9PWk7oXEY\r\n"
                "qX75bVAe/t04fP9vVjjVwp4/27YAW2PmAtR/af3lwlGf0vY4yk99b+y+vrgPGgAH\r\n"
                "cJXkpNJqTtij9M42tEIR8493h+vKF5dnz9CUvTyeqMFk+FG6QTj1xetj060LRSOn\r\n"
                "QtrJJ5TOWpjaafFhsj4t292SxcyZqW1nC/O5CaiKYXD0YIE/ukyeFywLyVYn9SQH\r\n"
                "NV2w3kDPYRkX9Q==\r\n"
                "-----END CERTIFICATE-----\r\n";
        ret = ATFile_Write(fileID,
                           0,
                           Calypso_DataFormat_Binary,
                           false,
                           strlen(certificate),
                           certificate,
                           &bytesWritten);
        Calypso_Examples_Print("Write certificate file", ret);
        printf("Wrote certificate file \"%s\" (length %d)\r\n", certificateFileName, strlen(certificate));

        ret = ATFile_Close(fileID, NULL, NULL);
        Calypso_Examples_Print("Close certificate file", ret);


        /* Set secure socket options: Security method, key file, certificate file */
        ATSocket_Options_t options;
        memset(&options, 0, sizeof(options));
        options.secMethod = ATSocket_SockOptSecMethod_SSLv3_TLSv1_2;
        ret = ATSocket_SetSocketOption(socketID,
                                       ATSocket_SockOptLevel_Socket,
                                       ATSocket_SockOptSocket_SecMethod,
                                       &options);
        Calypso_Examples_Print("Set security method", ret);
        memset(&options, 0, sizeof(options));
        strcpy(options.fileName, keyFileName);
        ret = ATSocket_SetSocketOption(socketID,
                                       ATSocket_SockOptLevel_Socket,
                                       ATSocket_SockOptSocket_SecureFilesPrivateKeyFileName,
                                       &options);
        Calypso_Examples_Print("Set private key file name", ret);
        memset(&options, 0, sizeof(options));
        strcpy(options.fileName, certificateFileName);
        ret = ATSocket_SetSocketOption(socketID,
                                       ATSocket_SockOptLevel_Socket,
                                       ATSocket_SockOptSocket_SecureFilesCertificateFileName,
                                       &options);
        Calypso_Examples_Print("Set certificate file name", ret);

        /* Must have the correct time on client and server for certificate
         * verification - use SNTP client to set the current time */
        ret = ATNetApp_StartApplications(ATNetApp_Application_SntpClient);
        Calypso_Examples_Print("Start SNTP client", ret);
        ATNetApp_OptionValue_t value;
        memset(&value, 0, sizeof(value));
        strcpy(value.sntp.servers[0], "pool.ntp.org");
        strcpy(value.sntp.servers[1], "");
        strcpy(value.sntp.servers[2], "");
        ret = ATNetApp_Set(ATNetApp_Application_SntpClient, ATNetApp_SntpOption_Servers, &value);
        Calypso_Examples_Print("Set SNTP server address", ret);
        WE_Delay(2000);
        ret = ATNetApp_UpdateTime();
        Calypso_Examples_Print("Update device time", ret);
        ATDevice_Value_t deviceValue;
        ret = ATDevice_Get(ATDevice_GetId_General, ATDevice_GetGeneral_Time, &deviceValue);
        Calypso_Examples_Print("Get device time", ret);
        if (ret)
        {
            printf("date(dd:mm:yy): %u.%u.%u time(hh:mm:ss): %u:%u:%u \r\n",
                   deviceValue.general.time.day,
                   deviceValue.general.time.month,
                   deviceValue.general.time.year,
                   deviceValue.general.time.hour,
                   deviceValue.general.time.minute,
                   deviceValue.general.time.second);
        }
    }

    ATSocket_Descriptor_t socketDescriptor;
    socketDescriptor.family = ATSocket_Family_INET;
    strcpy(socketDescriptor.address, ipV4Config.ipAddress);
    socketDescriptor.port = socketExampleServerPort;
    ret = ATSocket_Bind(socketID, socketDescriptor);
    Calypso_Examples_Print("Bind", ret);

    ret = ATSocket_Listen(socketID, 10);
    Calypso_Examples_Print("Listen", ret);

    ret = ATSocket_Accept(socketID, ATSocket_Family_INET);
    Calypso_Examples_Print("Accept", ret);

    while (true)
    {
        if (tcpServerConnectionAccepted)
        {
            /* A client has connected to the server */
            tcpServerConnectionAccepted = false;

            printf("Client %s:%d connected.\r\n", tcpServerAcceptEvent.clientAddress, tcpServerAcceptEvent.clientPort);

            /* This is the socket ID that can be used for communicating with the client */
            tcpServerClientSocketID = tcpServerAcceptEvent.socketID;

            tcpConnected = true;
        }

        if (tcpConnected)
        {
            if (socketExampleWaitingForData)
            {
                /* NOP */
            }
            else
            {
                /* Start waiting for data. An ATEvent_SocketRcvd event is generated when new
                 * data is available (see Calypso_eventCallback()). */
                socketExampleWaitingForData = true;
                ATSocket_Receive(tcpServerClientSocketID, Calypso_DataFormat_Binary, CALYPSO_MAX_PAYLOAD_SIZE);
            }
        }

        WE_Delay(250);
    }

    Calypso_Deinit();
}

/**
 * @brief TCP client example. Sets IPv4 address to socketExampleClientAddress,
 * creates a TCP socket, connects to socketExampleServerAddress:socketExampleServerPort
 * and then sends a counter value to the server at regular intervals.
 */
void Calypso_TCPClient_Example(void)
{
    printf("*** Start of Calypso ATSocket TCP client example ***\r\n");

    bool ret = false;

    /* ID of socket which is connected to the TCP server. */
    uint8_t socketID;

    if (!Calypso_Init(Calypso_Examples_baudRate, Calypso_Examples_flowControl, Calypso_Examples_parity, &Calypso_Socket_Example_EventCallback, NULL))
    {
        return;
    }

    Calypso_PinReset();

    WE_Delay(2000);

    /* Get version info. This retrieves Calypso's firmware version (amongst other version info) and
     * stores the firmware version in Calypso_firmwareVersionMajor, Calypso_firmwareVersionMinor and
     * Calypso_firmwareVersionPatch for later use. */
    ATDevice_Value_t deviceValue;
    ret = ATDevice_Get(ATDevice_GetId_General, ATDevice_GetGeneral_Version, &deviceValue);
    Calypso_Examples_Print("Get device version", ret);

    /* WLAN station mode */
    ret = ATWLAN_SetMode(ATWLAN_SetMode_Station);
    Calypso_Examples_Print("Set WLAN station mode", ret);
    ret = ATDevice_Restart(0);
    Calypso_Examples_Print("Restart network processor", ret);

    WE_Delay(1000);

    /* Connect to WLAN */
    ATWLAN_ConnectionArguments_t connectArgs;
    memset(&connectArgs, 0, sizeof(connectArgs));
    strcpy(connectArgs.SSID, Calypso_Examples_wlanSSID);
    connectArgs.securityParams.securityType = ATWLAN_SecurityType_WPA_WPA2;
    strcpy(connectArgs.securityParams.securityKey, Calypso_Examples_wlanKey);

    ret = ATWLAN_Connect(connectArgs);
    Calypso_Examples_Print("Connect to WLAN", ret);

    WE_Delay(2000);

    /* Set static IPv4 configuration for client */
    ATNetCfg_IPv4Config_t newIpV4Config = {0};
    newIpV4Config.method = ATNetCfg_IPv4Method_Static;
    strcpy(newIpV4Config.ipAddress, socketExampleClientAddress);
    strcpy(newIpV4Config.subnetMask, socketExampleSubnetMask);
    strcpy(newIpV4Config.gatewayAddress, socketExampleGateway);
    strcpy(newIpV4Config.dnsAddress, socketExampleDns);
    ret = ATNetCfg_SetIPv4AddressStation(&newIpV4Config);
    Calypso_Examples_Print("Set IPv4 config", ret);

    /* Get IPv4 configuration (station) */
    ATNetCfg_IPv4Config_t ipV4Config;
    ret = ATNetCfg_GetIPv4AddressStation(&ipV4Config);
    Calypso_Examples_Print("Get IPv4 config", ret);
    if (ret)
    {
        printf("*** Station IPv4 configuration ***\r\n");
        printf("IPv4 address: %s\r\n", ipV4Config.ipAddress);
        printf("Subnet mask: %s\r\n", ipV4Config.subnetMask);
        printf("Gateway address: %s\r\n", ipV4Config.gatewayAddress);
        printf("DNS address: %s\r\n", ipV4Config.dnsAddress);
    }

    /* Create TCP socket and connect to server. */
    ret = ATSocket_Create(ATSocket_Family_INET,
                          ATSocket_Type_Stream,
                          socketExampleSecure ? ATSocket_Protocol_SEC : ATSocket_Protocol_TCP,
                          &socketID);
    Calypso_Examples_Print("Create socket", ret);

    if (socketExampleSecure)
    {
        /* Using secure sockets - must configure certificates and other socket options */
        const char *caCertificateFileName = "dummy-root-ca-cert.pem";

        uint32_t fileID;
        uint32_t secureToken;

        /* Write the certificate of the CA that issued the server's certificate */
        ret = ATFile_Open(caCertificateFileName,
                          (ATFile_OpenFlags_Create | ATFile_OpenFlags_Overwrite),
                          5000,
                          &fileID,
                          &secureToken);
        Calypso_Examples_Print("Open CA file", ret);

        uint16_t bytesWritten = 0;
        const char* caCert = "-----BEGIN CERTIFICATE-----\r\n"
                "MIIDyzCCArOgAwIBAgIJAJOqV2LiCvmIMA0GCSqGSIb3DQEBCwUAMHwxCzAJBgNV\r\n"
                "BAYTAklMMQ8wDQYDVQQIDAZTaGFyb24xEDAOBgNVBAcMB1JhYW5hbmExHzAdBgNV\r\n"
                "BAoMFlRleGFzIEluc3RydW1lbnRzIEluYy4xDDAKBgNVBAsMA1JORDEbMBkGA1UE\r\n"
                "AwwSZHVtbXktcm9vdC1jYS1jZXJ0MB4XDTE2MDkxMTEzMTgxMFoXDTI2MDkwOTEz\r\n"
                "MTgxMFowfDELMAkGA1UEBhMCSUwxDzANBgNVBAgMBlNoYXJvbjEQMA4GA1UEBwwH\r\n"
                "UmFhbmFuYTEfMB0GA1UECgwWVGV4YXMgSW5zdHJ1bWVudHMgSW5jLjEMMAoGA1UE\r\n"
                "CwwDUk5EMRswGQYDVQQDDBJkdW1teS1yb290LWNhLWNlcnQwggEiMA0GCSqGSIb3\r\n"
                "DQEBAQUAA4IBDwAwggEKAoIBAQC+F2fNjjVABanE6OWnG1wIlc4ODDVul/5W1YCC\r\n"
                "i76uEzUOOMBxYTOFkL6b0rJN7Y22orJC22WuGd3admsG5o4MpYXqJKPOnSa8gZyH\r\n"
                "1UGDC+Las5jCg1oHSCI/uZ6D767e+6XVO61JCd6xcAv9AWpfVqA1Rte1utRo6U3r\r\n"
                "Duo8/lnCn7DSVgTVVyW1+PAvgRQJDh8+iaTO3KcPpJF6ePZy1ko2LxJtkFhJYM5Y\r\n"
                "mEhnqYA7HaS7vCSLk037JUju8scyzJJB9TYoP+2Qz83QIxdNGdSkiTCZYjYCOjxu\r\n"
                "MZZEtMPiHEkOgBkCYZvvo9RvPCkCRBSTNRuq9PI68jJ7gBjdAgMBAAGjUDBOMB0G\r\n"
                "A1UdDgQWBBT9y29z8eQmTlanPieiimw8XsaxMzAfBgNVHSMEGDAWgBT9y29z8eQm\r\n"
                "TlanPieiimw8XsaxMzAMBgNVHRMEBTADAQH/MA0GCSqGSIb3DQEBCwUAA4IBAQA+\r\n"
                "xBZzTey9Z17lCaxbOt8DLdhGm37NIXxU8qjLSWsW6CfPQ47xju8aHuy0CFLZmVTl\r\n"
                "EsWhqIAZR/JBiofD0fNMGC6w1EsT0HgtPQsoBU40bquQ//oMNxy2gLKhGf631yDc\r\n"
                "Hb+w7MWd1pYXAUHZP1upy+9MB4fFQEZrD8fB1mPhZBnMPzgKL/Q2EU4aUWL53Gii\r\n"
                "3ZmxWzVcO2FNkdSoXunnzmmu94ZRliUevHRoy9iWmjJrYd7WfNdxyIBuYK8QFZ9l\r\n"
                "VV6SfQDzPHoC39ux/AvyJ2AiUnJmGIhb7V+wtzRB328Rn5/rA2u4OJ0ZucN864Ox\r\n"
                "zfDcM/moAoyTj0uE8EJe\r\n"
                "-----END CERTIFICATE-----";
        ret = ATFile_Write(fileID,
                           0,
                           Calypso_DataFormat_Binary,
                           false,
                           strlen(caCert),
                           caCert,
                           &bytesWritten);
        Calypso_Examples_Print("Write CA file", ret);
        printf("Wrote CA certificate file \"%s\" (length %d)\r\n", caCertificateFileName, strlen(caCert));

        ret = ATFile_Close(fileID, NULL, NULL);
        Calypso_Examples_Print("Close CA file", ret);

        /* Set secure socket options: Security method, CA certificate file, disable certificate store */
        ATSocket_Options_t options;
        memset(&options, 0, sizeof(options));
        options.secMethod = ATSocket_SockOptSecMethod_SSLv3_TLSv1_2;
        ret = ATSocket_SetSocketOption(socketID,
                                       ATSocket_SockOptLevel_Socket,
                                       ATSocket_SockOptSocket_SecMethod,
                                       &options);
        Calypso_Examples_Print("Set security method", ret);
        memset(&options, 0, sizeof(options));
        strcpy(options.fileName, caCertificateFileName);
        ret = ATSocket_SetSocketOption(socketID,
                                       ATSocket_SockOptLevel_Socket,
                                       ATSocket_SockOptSocket_SecureFilesCAFileName,
                                       &options);
        Calypso_Examples_Print("Set CA file name", ret);

        /* The following command disables the module's certificate store and thus allows
         * us to use self-signed certificates. Don't set this option if using an official
         * root CA that should be checked against the module's certificate catalog.
         *
         * Note that when the certificate store is enabled and the root CA can't be verified
         * (e.g. when using a self-signed certificate), the encrypted connection is established
         * and data can be sent via the socket, but the module does not generate a
         * ATEvent_SocketTCPConnect event - instead it returns an error code (e.g. error code
         * SL_ERROR_BSD_ESEC_ASN_NO_SIGNER_E). */
        memset(&options, 0, sizeof(options));
        options.disableCertificateStore = ATSocket_OptionState_Enabled;
        ret = ATSocket_SetSocketOption(socketID,
                                       ATSocket_SockOptLevel_Socket,
                                       ATSocket_SockOptSocket_DisableCertificateStore,
                                       &options);
        Calypso_Examples_Print("Disable certificate store", ret);

        /* Must have the correct time on client and server for certificate
         * verification - use SNTP client to set the current time. */
        ret = ATNetApp_StartApplications(ATNetApp_Application_SntpClient);
        Calypso_Examples_Print("Start SNTP client", ret);
        ATNetApp_OptionValue_t value;
        memset(&value, 0, sizeof(value));
        strcpy(value.sntp.servers[0], "pool.ntp.org");
        strcpy(value.sntp.servers[1], "");
        strcpy(value.sntp.servers[2], "");
        ret = ATNetApp_Set(ATNetApp_Application_SntpClient, ATNetApp_SntpOption_Servers, &value);
        Calypso_Examples_Print("Set SNTP server address", ret);
        WE_Delay(2000);
        ret = ATNetApp_UpdateTime();
        Calypso_Examples_Print("Update device time", ret);
        ATDevice_Value_t deviceValue;
        ret = ATDevice_Get(ATDevice_GetId_General, ATDevice_GetGeneral_Time, &deviceValue);
        Calypso_Examples_Print("Get device time", ret);
        if (ret)
        {
            printf("date(dd:mm:yy): %u.%u.%u time(hh:mm:ss): %u:%u:%u \r\n",
                   deviceValue.general.time.day,
                   deviceValue.general.time.month,
                   deviceValue.general.time.year,
                   deviceValue.general.time.hour,
                   deviceValue.general.time.minute,
                   deviceValue.general.time.second);
        }
    }

    ATSocket_Descriptor_t socketDescriptor;
    socketDescriptor.family = ATSocket_Family_INET;
    strcpy(socketDescriptor.address, socketExampleServerAddress);
    socketDescriptor.port = socketExampleServerPort;
    ret = ATSocket_Connect(socketID, socketDescriptor);
    Calypso_Examples_Print("Connect to server", ret);

    while (true)
    {
        if (tcpClientConnectionEstablished)
        {
            /* Connection to server has been established */
            tcpClientConnectionEstablished = false;

            printf("Connected to server %s:%d.\r\n", tcpClientConnectEvent.serverAddress, tcpClientConnectEvent.serverPort);

            tcpConnected = true;
        }

        if (tcpConnected)
        {
            /* When connected, a 16bit counter value (converted to ASCII) is sent to the server every 250ms */
            static uint16_t counter = 0;
            char data[8];
            uint16_t bytesSent = 0;
            sprintf(data, "%d\r\n", counter++);
            ATSocket_Send(socketID,
                          Calypso_DataFormat_Binary,
                          false,
                          strlen(data),
                          data,
                          &bytesSent);
        }

        WE_Delay(250);
    }

    Calypso_Deinit();
}

/**
 * @brief UDP receive example. Sets IPv4 address to socketExampleServerAddress,
 * creates a UDP socket (port socketExampleServerPort), waits for incoming data
 * from socketExampleClientAddress and prints all received data.
 */
void Calypso_UDPReceive_Example(void)
{
    printf("*** Start of Calypso ATSocket UDP receive example ***\r\n");

    bool ret = false;

    /* ID of socket used for communicating with peer */
    uint8_t socketID;

    if (!Calypso_Init(Calypso_Examples_baudRate, Calypso_Examples_flowControl, Calypso_Examples_parity, &Calypso_Socket_Example_EventCallback, NULL))
    {
        return;
    }

    Calypso_PinReset();

    WE_Delay(2000);

    /* Get version info. This retrieves Calypso's firmware version (amongst other version info) and
     * stores the firmware version in Calypso_firmwareVersionMajor, Calypso_firmwareVersionMinor and
     * Calypso_firmwareVersionPatch for later use. */
    ATDevice_Value_t deviceValue;
    ret = ATDevice_Get(ATDevice_GetId_General, ATDevice_GetGeneral_Version, &deviceValue);
    Calypso_Examples_Print("Get device version", ret);

    /* WLAN station mode */
    ret = ATWLAN_SetMode(ATWLAN_SetMode_Station);
    Calypso_Examples_Print("Set WLAN station mode", ret);
    ret = ATDevice_Restart(0);
    Calypso_Examples_Print("Restart network processor", ret);

    WE_Delay(1000);

    /* Connect to WLAN */
    ATWLAN_ConnectionArguments_t connectArgs;
    memset(&connectArgs, 0, sizeof(connectArgs));
    strcpy(connectArgs.SSID, Calypso_Examples_wlanSSID);
    connectArgs.securityParams.securityType = ATWLAN_SecurityType_WPA_WPA2;
    strcpy(connectArgs.securityParams.securityKey, Calypso_Examples_wlanKey);

    ret = ATWLAN_Connect(connectArgs);
    Calypso_Examples_Print("Connect to WLAN", ret);

    WE_Delay(2000);

    /* Set static IPv4 configuration (using server address) */
    ATNetCfg_IPv4Config_t newIpV4Config = {0};
    newIpV4Config.method = ATNetCfg_IPv4Method_Static;
    strcpy(newIpV4Config.ipAddress, socketExampleServerAddress);
    strcpy(newIpV4Config.subnetMask, socketExampleSubnetMask);
    strcpy(newIpV4Config.gatewayAddress, socketExampleGateway);
    strcpy(newIpV4Config.dnsAddress, socketExampleDns);
    ret = ATNetCfg_SetIPv4AddressStation(&newIpV4Config);
    Calypso_Examples_Print("Set IPv4 config", ret);

    /* Get IPv4 configuration (station) */
    ATNetCfg_IPv4Config_t ipV4Config;
    ret = ATNetCfg_GetIPv4AddressStation(&ipV4Config);
    Calypso_Examples_Print("Get IPv4 config", ret);
    if (ret)
    {
        printf("*** Station IPv4 configuration ***\r\n");
        printf("IPv4 address: %s\r\n", ipV4Config.ipAddress);
        printf("Subnet mask: %s\r\n", ipV4Config.subnetMask);
        printf("Gateway address: %s\r\n", ipV4Config.gatewayAddress);
        printf("DNS address: %s\r\n", ipV4Config.dnsAddress);
    }

    /* Create UDP socket and bind it to a port */
    ret = ATSocket_Create(ATSocket_Family_INET,
                          ATSocket_Type_Datagram,
                          ATSocket_Protocol_UDP,
                          &socketID);
    Calypso_Examples_Print("Create socket", ret);

    ATSocket_Descriptor_t socketDescriptor;
    socketDescriptor.family = ATSocket_Family_INET;
    strcpy(socketDescriptor.address, ipV4Config.ipAddress);
    socketDescriptor.port = socketExampleServerPort;
    ret = ATSocket_Bind(socketID, socketDescriptor);
    Calypso_Examples_Print("Bind", ret);

    ATSocket_Descriptor_t socketDescriptorClient;
    socketDescriptorClient.family = ATSocket_Family_INET;
    strcpy(socketDescriptorClient.address, socketExampleClientAddress);
    socketDescriptorClient.port = socketExampleServerPort;

    while (true)
    {
        if (socketExampleWaitingForData)
        {
            /* NOP */
        }
        else
        {
            /* Start waiting for data. An ATEvent_SocketRcvdFrom event is generated when new
             * data is available (see Calypso_eventCallback()). */
            socketExampleWaitingForData = true;
            ATSocket_ReceiveFrom(socketID, socketDescriptorClient, Calypso_DataFormat_Binary, CALYPSO_MAX_PAYLOAD_SIZE);
        }

        WE_Delay(250);
    }

    Calypso_Deinit();
}

/**
 * @brief UDP transmit example. Sets IPv4 address to socketExampleClientAddress,
 * creates a UDP socket and sends a counter value to socketExampleServerAddress:socketExampleServerPort
 * at regular intervals.
 */
void Calypso_UDPTransmit_Example(void)
{
    printf("*** Start of Calypso ATSocket UDP transmit example ***\r\n");

    bool ret = false;

    /* ID of socket used for communicating with peer */
    uint8_t socketID;

    if (!Calypso_Init(Calypso_Examples_baudRate, Calypso_Examples_flowControl, Calypso_Examples_parity, &Calypso_Socket_Example_EventCallback, NULL))
    {
        return;
    }

    Calypso_PinReset();

    WE_Delay(2000);

    /* Get version info. This retrieves Calypso's firmware version (amongst other version info) and
     * stores the firmware version in Calypso_firmwareVersionMajor, Calypso_firmwareVersionMinor and
     * Calypso_firmwareVersionPatch for later use. */
    ATDevice_Value_t deviceValue;
    ret = ATDevice_Get(ATDevice_GetId_General, ATDevice_GetGeneral_Version, &deviceValue);
    Calypso_Examples_Print("Get device version", ret);

    /* WLAN station mode */
    ret = ATWLAN_SetMode(ATWLAN_SetMode_Station);
    Calypso_Examples_Print("Set WLAN station mode", ret);
    ret = ATDevice_Restart(0);
    Calypso_Examples_Print("Restart network processor", ret);

    WE_Delay(1000);

    /* Connect to WLAN */
    ATWLAN_ConnectionArguments_t connectArgs;
    memset(&connectArgs, 0, sizeof(connectArgs));
    strcpy(connectArgs.SSID, Calypso_Examples_wlanSSID);
    connectArgs.securityParams.securityType = ATWLAN_SecurityType_WPA_WPA2;
    strcpy(connectArgs.securityParams.securityKey, Calypso_Examples_wlanKey);

    ret = ATWLAN_Connect(connectArgs);
    Calypso_Examples_Print("Connect to WLAN", ret);

    WE_Delay(2000);

    /* Set static IPv4 configuration (using client address) */
    ATNetCfg_IPv4Config_t newIpV4Config = {0};
    newIpV4Config.method = ATNetCfg_IPv4Method_Static;
    strcpy(newIpV4Config.ipAddress, socketExampleClientAddress);
    strcpy(newIpV4Config.subnetMask, socketExampleSubnetMask);
    strcpy(newIpV4Config.gatewayAddress, socketExampleGateway);
    strcpy(newIpV4Config.dnsAddress, socketExampleDns);
    ret = ATNetCfg_SetIPv4AddressStation(&newIpV4Config);
    Calypso_Examples_Print("Set IPv4 config", ret);

    /* Get IPv4 configuration (station) */
    ATNetCfg_IPv4Config_t ipV4Config;
    ret = ATNetCfg_GetIPv4AddressStation(&ipV4Config);
    Calypso_Examples_Print("Get IPv4 config", ret);
    if (ret)
    {
        printf("*** Station IPv4 configuration ***\r\n");
        printf("IPv4 address: %s\r\n", ipV4Config.ipAddress);
        printf("Subnet mask: %s\r\n", ipV4Config.subnetMask);
        printf("Gateway address: %s\r\n", ipV4Config.gatewayAddress);
        printf("DNS address: %s\r\n", ipV4Config.dnsAddress);
    }

    /* Create UDP socket and bind it to a port */
    ret = ATSocket_Create(ATSocket_Family_INET,
                          ATSocket_Type_Datagram,
                          ATSocket_Protocol_UDP,
                          &socketID);
    Calypso_Examples_Print("Create socket", ret);

    ATSocket_Descriptor_t socketDescriptor;
    socketDescriptor.family = ATSocket_Family_INET;
    strcpy(socketDescriptor.address, ipV4Config.ipAddress);
    socketDescriptor.port = socketExampleServerPort;
    ret = ATSocket_Bind(socketID, socketDescriptor);
    Calypso_Examples_Print("Bind", ret);

    ATSocket_Descriptor_t socketDescriptorServer;
    socketDescriptorServer.family = ATSocket_Family_INET;
    strcpy(socketDescriptorServer.address, socketExampleServerAddress);
    socketDescriptorServer.port = socketExampleServerPort;

    while (true)
    {
        /* A 16bit counter value (converted to ASCII) is sent to the peer every 250ms */
        static uint16_t counter = 0;
        char data[8];
        uint16_t bytesSent = 0;
        sprintf(data, "%d\r\n", counter++);
        ATSocket_SendTo(socketID,
                        &socketDescriptorServer,
                        Calypso_DataFormat_Binary,
                        false,
                        strlen(data),
                        data,
                        &bytesSent);

        WE_Delay(250);
    }

    Calypso_Deinit();
}

/**
 * @brief Is called when an event notification has been received.
 *
 * Note that this function is called from an interrupt - code in this function
 * should thus be kept simple.
 *
 * Note in particular that it is not possible to send AT commands to Calypso
 * from within this event handler.
 *
 * Also note that not all calls of this handler necessarily correspond to valid
 * events (i.e. events from ATEvent_t). Some events might in fact be responses
 * to AT commands that are not included in ATEvent_t.
 */
void Calypso_Socket_Example_EventCallback(char* eventText)
{
    Calypso_Examples_EventCallback(eventText);

    ATEvent_t event;
    ATEvent_ParseEventType(&eventText, &event);

    switch (event)
    {
    case ATEvent_SocketTxFailed:
        break;

    case ATEvent_SocketAsyncEvent:
        break;

    case ATEvent_SocketTCPConnect:
        if (ATEvent_ParseSocketTCPConnectEvent(&eventText, &tcpClientConnectEvent))
        {
            tcpClientConnectionEstablished = true;
        }
        break;

    case ATEvent_SocketTCPAccept:
        if (ATEvent_ParseSocketTCPAcceptEvent(&eventText, &tcpServerAcceptEvent))
        {
            tcpServerConnectionAccepted = true;
        }
        break;

    case ATEvent_SocketRcvd:
    case ATEvent_SocketRcvdFrom:
    {
        ATEvent_SocketRcvd_t rcvdEvent;
        if (ATEvent_ParseSocketRcvdEvent(&eventText, false, &rcvdEvent))
        {
            Calypso_Socket_Example_OnDataReceived(&rcvdEvent);
        }
        break;
    }

    case ATEvent_Startup:
    case ATEvent_WakeUp:
    case ATEvent_Ping:
    case ATEvent_WlanP2PConnect:
    case ATEvent_WlanP2PDisconnect:
    case ATEvent_WlanP2PClientAdded:
    case ATEvent_WlanP2PClientRemoved:
    case ATEvent_WlanP2PDevFound:
    case ATEvent_WlanP2PRequest:
    case ATEvent_WlanP2PConnectFail:
    case ATEvent_Invalid:
    case ATEvent_GeneralResetRequest:
    case ATEvent_GeneralError:
    case ATEvent_WlanConnect:
    case ATEvent_WlanDisconnect:
    case ATEvent_WlanStaAdded:
    case ATEvent_WlanStaRemoved:
    case ATEvent_WlanProvisioningStatus:
    case ATEvent_WlanProvisioningProfileAdded:
    case ATEvent_NetappIP4Acquired:
    case ATEvent_NetappIP6Acquired:
    case ATEvent_NetappIPCollision:
    case ATEvent_NetappDHCPv4_leased:
    case ATEvent_NetappDHCPv4_released:
    case ATEvent_NetappIPv4Lost:
    case ATEvent_NetappDHCPIPv4AcquireTimeout:
    case ATEvent_NetappIPv6Lost:
    case ATEvent_MQTTOperation:
    case ATEvent_MQTTRecv:
    case ATEvent_MQTTDisconnect:
    case ATEvent_FileListEntry:
    case ATEvent_HTTPGet:
    case ATEvent_CustomGPIO:
    case ATEvent_CustomHTTPPost:
    case ATEvent_FatalErrorDeviceAbort:
    case ATEvent_FatalErrorDriverAbort:
    case ATEvent_FatalErrorSyncLost:
    case ATEvent_FatalErrorNoCmdAck:
    case ATEvent_FatalErrorCmdTimeout:
    default:
        break;
    }
}

/**
 * @brief TCP / UDP data received callback.
 */
void Calypso_Socket_Example_OnDataReceived(ATEvent_SocketRcvd_t *rcvdEvent)
{
    memcpy(socketExampleReceiveBuffer, rcvdEvent->data, rcvdEvent->length);
    socketExampleReceiveBuffer[rcvdEvent->length] = '\0';
    printf("RECEIVED %s\r\n", socketExampleReceiveBuffer);

    socketExampleWaitingForData = false;
}
