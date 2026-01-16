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
 * @brief Calypso file example.
 */
#include <Calypso/ATCommands/ATDevice.h>
#include <Calypso/ATCommands/ATFile.h>
#include <Calypso/Calypso_Examples.h>
#include <Calypso/Calypso_File_Example.h>
#include <stdio.h>

void Calypso_File_Example_EventCallback(char* eventText);

/**
 * @brief File handling example.
 */
void Calypso_File_Example(void)
{
    WE_APP_PRINT("*** Start of Calypso ATFile example ***\r\n");

    bool ret = false;

    if (!Calypso_Init(&Calypso_uart, &Calypso_pins, &Calypso_File_Example_EventCallback))
    {
        WE_APP_PRINT("Initialization error\r\n");
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

    uint32_t fileID;
    uint32_t secureToken;
    const char* fileName = "testfile";
    char* fileContent = "Hello World";
    bool encodeAsBase64 = true;

    ret = Calypso_ATFile_Open(fileName, (Calypso_ATFile_OpenFlags_Create | Calypso_ATFile_OpenFlags_Write), ATFILE_FILE_MIN_SIZE, &fileID, &secureToken);
    Calypso_Examples_Print("Open file for writing", ret);

    if (ret)
    {
        uint16_t bytesWritten = 0;
        ret = Calypso_ATFile_Write(fileID, 0, encodeAsBase64 ? Calypso_DataFormat_Base64 : Calypso_DataFormat_Binary, encodeAsBase64, strlen(fileContent), fileContent, &bytesWritten);
        Calypso_Examples_Print("Write file", ret);
        WE_APP_PRINT("Wrote \"%s\" (length %d) in %s format\r\n", fileContent, strlen(fileContent), encodeAsBase64 ? "Base64" : "binary");
    }

    ret = Calypso_ATFile_Close(fileID, NULL, NULL);
    Calypso_Examples_Print("Close file", ret);

    ret = Calypso_ATFile_Open(fileName, Calypso_ATFile_OpenFlags_Read, ATFILE_FILE_MIN_SIZE, &fileID, &secureToken);
    Calypso_Examples_Print("Open file for reading", ret);

    char fileContentReadBack[32];
    uint16_t readBytes = 0;
    if (ret)
    {
        ret = Calypso_ATFile_Read(fileID, 0, encodeAsBase64 ? Calypso_DataFormat_Base64 : Calypso_DataFormat_Binary, encodeAsBase64, strlen(fileContent), fileContentReadBack, &readBytes);
        Calypso_Examples_Print("Read file", ret);
        if (ret)
        {
            WE_APP_PRINT("Read \"%s\" (length %d) in %s format\r\n", fileContentReadBack, readBytes, encodeAsBase64 ? "Base64" : "binary");
        }
    }

    ret = Calypso_ATFile_Close(fileID, NULL, NULL);
    Calypso_Examples_Print("Close file", ret);

    /* Get list of files stored on Calypso module. Note that the file list
     * entries are provided in the form of individual events of type Calypso_ATEvent_FileListEntry,
     * as the complete list of files might be too large to fit in the receive buffer.
     * In this example, the entries are processed in Calypso_File_Example_EventCallback(). */
    ret = Calypso_ATFile_GetFileList();
    Calypso_Examples_Print("Get file list", ret);

    ret = Calypso_ATFile_Delete(fileName, secureToken);
    Calypso_Examples_Print("Delete file", ret);

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
 * events (i.e. events from Calypso_ATEvent_t). Some events might in fact be responses
 * to AT commands that are not included in Calypso_ATEvent_t.
 */
void Calypso_File_Example_EventCallback(char* eventText)
{
    Calypso_ATEvent_t event;
    if (false == Calypso_ATEvent_ParseEventType(&eventText, &event))
    {
        return;
    }

    switch (event)
    {
        case Calypso_ATEvent_FileListEntry:
        {
            Calypso_ATFile_FileListEntry_t fileListEntry;
            if (Calypso_ATEvent_ParseFileListEntryEvent(&eventText, &fileListEntry))
            {
                char propertiesStr[256] = {0};
                Calypso_ATFile_PrintFileProperties(fileListEntry.properties, propertiesStr, sizeof(propertiesStr));
                WE_APP_PRINT("File list entry: "
                             "Name = \"%s\", "
                             "max. size = %lu, "
                             "properties = \"%s\", "
                             "blocks = %lu\r\n",
                             fileListEntry.fileName, fileListEntry.maxFileSize, propertiesStr, fileListEntry.allocatedBlocks);
            }
            break;
        }

        case Calypso_ATEvent_Startup:
        case Calypso_ATEvent_WakeUp:
        case Calypso_ATEvent_Ping:
        case Calypso_ATEvent_SocketTxFailed:
        case Calypso_ATEvent_SocketAsyncEvent:
        case Calypso_ATEvent_SocketTCPConnect:
        case Calypso_ATEvent_SocketTCPAccept:
        case Calypso_ATEvent_SocketRcvd:
        case Calypso_ATEvent_SocketRcvdFrom:
        case Calypso_ATEvent_WlanP2PConnect:
        case Calypso_ATEvent_WlanP2PDisconnect:
        case Calypso_ATEvent_WlanP2PClientAdded:
        case Calypso_ATEvent_WlanP2PClientRemoved:
        case Calypso_ATEvent_WlanP2PDevFound:
        case Calypso_ATEvent_WlanP2PRequest:
        case Calypso_ATEvent_WlanP2PConnectFail:
        case Calypso_ATEvent_GeneralResetRequest:
        case Calypso_ATEvent_GeneralError:
        case Calypso_ATEvent_WlanConnect:
        case Calypso_ATEvent_WlanDisconnect:
        case Calypso_ATEvent_WlanStaAdded:
        case Calypso_ATEvent_WlanStaRemoved:
        case Calypso_ATEvent_WlanProvisioningStatus:
        case Calypso_ATEvent_WlanProvisioningProfileAdded:
        case Calypso_ATEvent_NetappIP4Acquired:
        case Calypso_ATEvent_NetappIP6Acquired:
        case Calypso_ATEvent_NetappIPCollision:
        case Calypso_ATEvent_NetappDHCPv4_leased:
        case Calypso_ATEvent_NetappDHCPv4_released:
        case Calypso_ATEvent_NetappIPv4Lost:
        case Calypso_ATEvent_NetappDHCPIPv4AcquireTimeout:
        case Calypso_ATEvent_NetappIPv6Lost:
        case Calypso_ATEvent_MQTTOperation:
        case Calypso_ATEvent_MQTTRecv:
        case Calypso_ATEvent_MQTTDisconnect:
        case Calypso_ATEvent_HTTPGet:
        case Calypso_ATEvent_CustomGPIO:
        case Calypso_ATEvent_CustomHTTPPost:
        case Calypso_ATEvent_FatalErrorDeviceAbort:
        case Calypso_ATEvent_FatalErrorDriverAbort:
        case Calypso_ATEvent_FatalErrorSyncLost:
        case Calypso_ATEvent_FatalErrorNoCmdAck:
        case Calypso_ATEvent_FatalErrorCmdTimeout:
        default:
            break;
    }
}
