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
 * @file CordeliaI_Quarklink_Example.c
 * @brief Example implementation for connecting the Cordelia-I module to a QuarkLink platform.
 *
 * This example initializes the Cordelia-I module, configures it for secure communication with
 * QuarkLink, and demonstrates its capabilities such as connecting to a WLAN, provisioning via
 * QuarkLink, and communicating with an MQTT broker over TLS. It includes:
 * - Module initialization and reset.
 * - WLAN configuration and connection.
 * - Writing a QuarkLink Root CA to the module's filesystem.
 * - Setting CSR parameters for secure device provisioning.
 * - Configuring QuarkLink parameters, including hostname and port.
 * - Enrolling the device using QuarkLink for secure asset generation.
 * - Setting MQTT connection flags and topics.
 * - Publishing and subscribing to MQTT messages.
 * - Disconnecting from the MQTT broker and cleaning up resources.
 *
 * Ensure the following prior to running:
 * - Update the `WLAN_SSID` and `WLAN_KEY` macros with your WLAN credentials.
 * - Replace the `QL_ROOT_CA` macro with your unique QuarkLink Root CA.
 * - Update the `QL_HOSTNAME` macro to match your QuarkLink instance hostname.
 * - Verify that the module is connected to the microcontroller and the UART interface is configured.
 *
 * Usage:
 * - Ensure the correct WLAN credentials, QuarkLink Root CA certificate, and the QuarkLink hostname are provided.
 * - Compile and load the program onto the host microcontroller.
 * - Connect the host microcontroller to the Cordelia-I module (see connection diagram below).
 * - Follow the serial output of your STM32 (using 115200 baud, 8N1 settings) for progress and debug information.
 *
 * Connection Diagram:
 *
 *     +-------------------+         +---------------------+
 *     |  HOST MCU (STM32) |         |  CORDELIA-I MODULE  |
 *     |-------------------|         |---------------------|
 *     | GPIO PB7 (UART RX)|<--------| UART0_TX (JP1 Pin 4)|
 *     | GPIO PB6 (UART TX)|-------->| UART0_RX (JP1 Pin 2)|
 *     | GPIO PA10 (NRESET)|-------->| NRESET (CON4 Pin 15)|
 *     |        3V3        |-------->| VCC (CON4 Pin 1)    |
 *     |        GND        |-------->| GND (CON4 Pin 4)    |
 *     +-------------------+         +---------------------+
 *
 */
#include <CordeliaI/CordeliaI_Quarklink_Example.h>
#include <stdio.h>
#include <CordeliaI/ATCommands/ATDevice.h>
#include <CordeliaI/ATCommands/ATWLAN.h>
#include <CordeliaI/ATCommands/ATFile.h>
#include <CordeliaI/CordeliaI_Examples.h>

/**
 * @brief WLAN credentials for connecting to the wireless network.
 */
#define WLAN_SSID "YOUR-WLAN-SSID"
#define WLAN_KEY "YOUR-WLAN-PASSWORD"

/**
 * @brief QuarkLink root certificate in PEM format.
 * This certificate is used for establishing a secure connection to the QuarkLink platform.
 * This Root CA is unique to your QuarkLink instance and can be downloaded from your QuarkLink portal.
 * Here, a dummy value is provided that needs to be replaced before you run this example.
 * You can find more information on how to do that in the Cordelia-I User Manual.
 */
#define QL_ROOT_CA "-----BEGIN CERTIFICATE-----\r\n\
MIIBWjCCAQKgAwIBAgIIF7XblCUPhYIwCgYIKoZIzj0EAwIwEjEQMA4GA1UEAxMH\r\n\
RFVNTVJvb3QwIBYDVQQDExNEVU1NUm9vdDAgFw0yNDAxMDFMTU5aGA8yMDU0MDEw\r\n\
MTEwNTc0MlowEjEQMA4GA1UEAxMHRFVNTVJvb3QwWTATBgcqhkjOPQIBBggqhkjO\r\n\
PQMBBwNCAARpNL74QJkWX0ac7LYAJgP0jBziHXUxmCdAIyyIRpgie91MuilMiucr\r\n\
qywKYLvRZHc2qFwuRhIdFMU6LZDkoU3WjQjBAMA4GA1UdDwEB/wQEAwIChDAPBgN\r\n\
VHRMBAf8EBTADAQH/MB0GA1UdDgQWBBQJtup0YM3p7d6WfJlTxOYO2i/+wDAKBgg\r\n\
qhkjOPQQDAgNIADBFAiBfF4ZzuSWVlEwq4Qv4iafwCmvNI3tgZdjhb/CxyE/gQAI\r\n\
hALbuIhqNeFFJO9Upr/ZtsX+ejk+TA/cn9CSL12g9TndZ\r\n\
-----END CERTIFICATE-----"

/**
 * @brief Define constants for QuarkLink UserSettings.
 * These settings are used to configure the QuarkLink connection parameters.
 */
#define QL_FILE_NAME       "/ql/ql_ca.pem"   // Path to the Root CA file on the Cordelia-I file system. The hard-coded "QL_ROOT_CA" will be saved to this path.
#define QL_PORT            6000             // Port number for QuarkLink communication.
#define QL_HOSTNAME        "https://yourquarklinkinstance.quarklink.io" // QuarkLink server hostname of your personal QuarkLink instance.

/**
 * @brief Define constants for CSR UserSettings.
 * These settings are used for generating a Certificate Signing Request (CSR).
 */
#define CSR_COUNTRY        "DE"
#define CSR_STATE          "Bavaria"
#define CSR_LOCALITY       "Munich"
#define CSR_SURNAME        "Mustermann"
#define CSR_EMAIL          "info@example.com"
#define CSR_ORGANIZATION   "Generic GmbH"
#define CSR_UNIT           "Wireless Solutions"

/**
 * @brief Define constants for MQTT and Topic settings
 */
#define MQTT_FLAGS            "url|sec|whitelist_rootca"
#define SUBTOPIC0_NAME        "home/sensor1/data"
#define PUBTOPIC0_NAME        "home/sensor1/data"

/**
 * @brief Performs the setup and enrolment of the Cordelia-I module with QuarkLink.
 *
 * This function sets up the necessary parameters and initiates the enrolment process
 * with the QuarkLink portal. This step is required only once for a device. Once the
 * device has been enrolled with the desired parameters, subsequent runs of the example
 * should skip this function and directly call `CordeliaI_IoT_Connect` after startup.
 *
 * @note If the enrolment has been completed previously, the call for this function can be removed from CordeliaI_Quarklink_Example.
 */
void CordeliaI_SetupAndEnrol(void)
{
	bool ret = false;

	// Retrieve and print the unique device ID of the Cordelia-I module. You need this device ID to register your Cordelia-I module on your QuarkLink portal. You can find more information on how to do that in the Cordelia-I User Manual.
	{
		char deviceID[65] = {0};
		ret = CordeliaI_ATDevice_Get(CordeliaI_ATDevice_GetId_IOT, CordeliaI_ATDevice_GetIot_DeviceID, deviceID);
		CordeliaI_Examples_Print("Get deviceID", ret);
		if(ret) printf("Register the following device ID using your QuarkLink instance: %s\r\n",  deviceID);
		WE_Delay(1000);
	}

    // Write the QuarkLink Root CA (that is hard-coded as a define in this example code) to the module's file system. This Root CA is unique to your QuarkLink instance and can be downloaded from your QuarkLink portal. You can find more information on how to do that in the Cordelia-I User Manual.
	{
		uint32_t fileID;
		uint32_t secureToken;
		char fileName[] = QL_FILE_NAME;
		char fileContent[] = QL_ROOT_CA;
		bool encodeAsBase64 = false;

		ret = CordeliaI_ATFile_Open(fileName, (CordeliaI_ATFile_OpenFlags_Create | CordeliaI_ATFile_OpenFlags_Overwrite), ATFILE_FILE_MIN_SIZE, &fileID, &secureToken);
		CordeliaI_Examples_Print("Open file for writing", ret);

		if (ret)
		{
			uint16_t bytesWritten = 0;
			ret = CordeliaI_ATFile_Write(fileID, 0, encodeAsBase64 ? CordeliaI_DataFormat_Base64 : CordeliaI_DataFormat_Binary, encodeAsBase64, strlen(fileContent), fileContent, &bytesWritten);
			CordeliaI_Examples_Print("Write file", ret);
			CordeliaI_ATFile_Close( fileID, NULL, NULL);
			printf("Wrote \"%s\" (length %d) in %s format\r\n", fileContent, strlen(fileContent), encodeAsBase64 ? "Base64" : "binary");
		}

	}

    // Configure the CSR parameters for secure device provisioning.
	{
        // Repeat similar blocks for other CSR parameters (state, locality, etc.)
	    char country[] = CSR_COUNTRY;
	    ret = CordeliaI_ATDevice_Set(CordeliaI_ATDevice_GetId_CSR, CordeliaI_ATDevice_GetCSR_Country, country);
	    CordeliaI_Examples_Print("Set Country", ret);

	    char state[] = CSR_STATE;
	    ret = CordeliaI_ATDevice_Set(CordeliaI_ATDevice_GetId_CSR, CordeliaI_ATDevice_GetCSR_State, state);
	    CordeliaI_Examples_Print("Set State", ret);

	    char locality[] = CSR_LOCALITY;
	    ret = CordeliaI_ATDevice_Set(CordeliaI_ATDevice_GetId_CSR, CordeliaI_ATDevice_GetCSR_Locality, locality);
	    CordeliaI_Examples_Print("Set Locality", ret);

	    char surname[] = CSR_SURNAME;
	    ret = CordeliaI_ATDevice_Set(CordeliaI_ATDevice_GetId_CSR, CordeliaI_ATDevice_GetCSR_Surname, surname);
	    CordeliaI_Examples_Print("Set Surname", ret);

	    char email[] = CSR_EMAIL;
	    ret = CordeliaI_ATDevice_Set(CordeliaI_ATDevice_GetId_CSR, CordeliaI_ATDevice_GetCSR_Email, email);
	    CordeliaI_Examples_Print("Set Email", ret);

	    char organization[] = CSR_ORGANIZATION;
	    ret = CordeliaI_ATDevice_Set(CordeliaI_ATDevice_GetId_CSR, CordeliaI_ATDevice_GetCSR_Organization, organization);
	    CordeliaI_Examples_Print("Set Organization", ret);

	    char unit[] = CSR_UNIT;
	    ret = CordeliaI_ATDevice_Set(CordeliaI_ATDevice_GetId_CSR, CordeliaI_ATDevice_GetCSR_Unit, unit);
	    CordeliaI_Examples_Print("Set Unit", ret);

	    WE_Delay(2000);
	}

    // Configure QuarkLink-specific parameters such as port, Root CA path, and hostname.
	{
		// Repeat similar blocks for other QuarkLink settings.
		uint32_t port = QL_PORT;
		ret = CordeliaI_ATDevice_Set(CordeliaI_ATDevice_GetId_Quarklink, CordeliaI_ATDevice_GetQuarklink_Port, &port);
		CordeliaI_Examples_Print("Set QuarkLink Port", ret);

		char rootCAPath[] = QL_FILE_NAME;
		ret = CordeliaI_ATDevice_Set(CordeliaI_ATDevice_GetId_Quarklink, CordeliaI_ATDevice_GetQuarklink_RootCAPath, rootCAPath);
		CordeliaI_Examples_Print("Set QuarkLink RootCAPath", ret);

		char hostname[] = QL_HOSTNAME;
		ret = CordeliaI_ATDevice_Set(CordeliaI_ATDevice_GetId_Quarklink, CordeliaI_ATDevice_GetQuarklink_Hostname, hostname);
		CordeliaI_Examples_Print("Set QuarkLink Hostname", ret);

		WE_Delay(2000);
	}

	// Now we can initiate the enrolment process via the QuarkLink portal
	{
		ret =  CordeliaI_IoT_Enrol();
		CordeliaI_Examples_Print("IoT enrol", ret);
		CordeliaI_Examples_WaitForIoTEnrolCompleteEvent(50000);
	}
}

/**
 * @brief Demonstrates the setup and usage of Cordelia-I with QuarkLink for secure IoT connectivity.
 *
 * Usage:
 * Call this function to run through the complete example of configuring the Cordelia-I module
 * for secure IoT communication with QuarkLink.
 *
 * Ensure the following prior to running:
 * - Update the `WLAN_SSID` and `WLAN_KEY` macros with your WLAN credentials.
 * - Replace the `QL_ROOT_CA` macro with your unique QuarkLink Root CA.
 * - Update the `QL_HOSTNAME` macro to match your QuarkLink instance hostname.
 * - Verify that the module is connected to the microcontroller and the UART interface is configured.
 *
 * @note This example uses hard-coded values for demonstration purposes.
 * Update these values to match your specific environment and use case.
 */
void CordeliaI_Quarklink_Example(void)
{
	printf("*** Start of Cordelia-I QuarkLink example ***\r\n");

	bool ret = false;

    // Initialize the Cordelia-I module and the UART interface of the host microcontroller.
	if (!CordeliaI_Init(&CordeliaI_uart, &CordeliaI_pins, &CordeliaI_Examples_EventCallback))
	{
		return;
	}

    // Reset the Cordelia-I module via its reset pin.
	CordeliaI_PinReset();

    // Wait for the module to complete its startup process.
	CordeliaI_Examples_WaitForStartup(5000);

    // Disconnect from any existing WLAN connection. If we have already been connected before, we automatically reconnect right after startup.
	ret = CordeliaI_ATWLAN_Disconnect();
	CordeliaI_Examples_Print("Disconnect from any existing WLAN connection", ret);

    // Restart the network processor to ensure a clean state.
	ret = CordeliaI_ATDevice_Restart(0);
	CordeliaI_Examples_Print("Restart network processor", ret);

    // Connect to the specified WLAN network using the provided credentials.
	{
		CordeliaI_ATWLAN_ConnectionArguments_t connectArgs;
		memset(&connectArgs, 0, sizeof(connectArgs));
		strcpy(connectArgs.SSID, WLAN_SSID);
		connectArgs.securityParams.securityType = CordeliaI_ATWLAN_SecurityType_WPA_WPA2;
		strcpy(connectArgs.securityParams.securityKey, WLAN_KEY);
		ret = CordeliaI_ATWLAN_Connect(connectArgs);
		CordeliaI_Examples_Print("Connect to WLAN", ret);
		WE_Delay(10000); // Allow time for the connection process.
	}

    // Retrieve the OTA firmware version to check for available updates.
	{
		char firmWareVersion[64] = {0};
		ret = CordeliaI_ATDevice_Get(CordeliaI_ATDevice_GetId_OTA, CordeliaI_ATDevice_GetOTA_FirmwareVersion, firmWareVersion);
		CordeliaI_Examples_Print("Retrieve the OTA firmware version to check for available updates", ret);
		if(ret) printf("OTA Firmware version available is: %s\r\n", firmWareVersion);
	}

	// If the CordeliaI_SetupAndEnrol function is called, it configures the device for secure communication
	// with the QuarkLink platform and initiates the device enrolment process. This involves setting up
	// CSR parameters, writing the QuarkLink Root CA, and configuring QuarkLink-specific settings like
	// the hostname and port. The function also registers the device ID with QuarkLink and retrieves
	// secure assets such as keys and certificates required for subsequent MQTT communication.
	//
	// It is important to note that if the CSR and QuarkLink parameters remain unchanged,
	// the enrolment process only needs to be performed once. In such cases, this function call
	// can be removed in future runs, provided the device has already been successfully enrolled
	// with the desired parameters. Once the enrolment process has been successfully completed,
	// the CordeliaI_IoT_Connect() function can be called directly, as the device ID has been registered
	// on the QuarkLink portal, and all the necessary assets are already available on the Cordelia module
	// from the latest CordeliaI_SetupAndEnrol() call.
    CordeliaI_SetupAndEnrol();

	// After enrolment, we have the secure assets that are required to estabilish a secure MQTT connection. But before we do that, we have to set up the MQTT connection related user settings, like the connection flags, and we also have to set up at least one predefined MQTT topic to subscribe and to publish to. In this example, we are setting them to the same value, so that everything that is being published by us, will also be immediately received back by us. This way we can verify that the module is working as expected.
	{
		// Set MQTT flags
		char mqttFlags[] = MQTT_FLAGS;
		ret = CordeliaI_ATDevice_Set(CordeliaI_ATDevice_GetId_MQTT, CordeliaI_ATDevice_GetMQTT_Flags, mqttFlags);
		CordeliaI_Examples_Print("Set MQTT Flags", ret);

		// Set SUBTOPIC0 name to a value (we could also set SUBTOPIC1, SUBTOPIC2, and SUBTOPIC3, as we have four predefined topic that we can subscribe to, but in this example we only need one)
		char subtopic0Name[] = SUBTOPIC0_NAME;
		ret = CordeliaI_ATDevice_Set(CordeliaI_ATDevice_GetId_SubTopic0, CordeliaI_ATDevice_GetSubTopic_Name, subtopic0Name);
		CordeliaI_Examples_Print("Set SUBTOPIC0 Name", ret);

		// Set PUBTOPIC0 name to the same value as SUBTOPIC0, so that everything that is being published by us, will also be immediately received back by us. This way we can verify that the module is working as expected.
		char pubtopic0Name[] = PUBTOPIC0_NAME;
		ret = CordeliaI_ATDevice_Set(CordeliaI_ATDevice_GetId_PubTopic0, CordeliaI_ATDevice_GetPubTopic_Name, pubtopic0Name);
		CordeliaI_Examples_Print("Set PUBTOPIC0 Name", ret);

		WE_Delay(2000);

	}

    // After setting up the required parameters, and getting the secure assets, we can establish the secure MQTT connection.
	{

		ret = CordeliaI_IoT_Connect();
		CordeliaI_Examples_Print("IoT connect", ret);
		CordeliaI_Examples_WaitForIoTConnectEvent(12000);
	}

	// Here, we publish a sample MQTT message. We first set up the required arguments, like the topic number (between 0-3, choosing from the predefined four pubtopics), and the actual message payload that we want to publish.
	{
		uint8_t topicNo = 0;
		char mqttMessage[] = "Hello World";
		CordeliaI_MQTT_PublishArguments_t publishArgs;
		publishArgs.topicNo=topicNo;
		strcpy(publishArgs.Message, mqttMessage);
		bool ret = CordeliaI_MQTT_Publish(publishArgs);
		CordeliaI_Examples_Print("Publish MQTT Message", ret);
		WE_Delay(5000);
	}

	// Disconnect from the MQTT broker to clean up resources.
	{
		ret = CordeliaI_IoT_Disconnect();
		CordeliaI_Examples_Print("IoT disconnect", ret);
		WE_Delay(12000);
	}

	// Deinitialize the serial communication to the Cordelia-I module.
	CordeliaI_Deinit();
}
