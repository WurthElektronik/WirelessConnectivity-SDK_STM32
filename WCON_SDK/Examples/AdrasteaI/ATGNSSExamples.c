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
#include <AdrasteaI/ATCommands/ATDevice.h>
#include <AdrasteaI/ATCommands/ATEvent.h>
#include <AdrasteaI/ATCommands/ATGNSS.h>
#include <AdrasteaI/ATCommands/ATPacketDomain.h>
#include <AdrasteaI/AdrasteaI.h>
#include <AdrasteaI/AdrasteaI_Examples.h>
#include <stdio.h>

void AdrasteaI_ATGNSS_EventCallback(char* eventText);

static AdrasteaI_ATPacketDomain_Network_Registration_Status_t status = {.state = 0};

static AdrasteaI_ATGNSS_Satellite_Count_t satelliteQueryCount = 0, satelliteQueryEventCount = 0;

/**
 * @brief This example configures the GNSS part of the Adrastea-I and tries to get a fix (current positional data)
 *
 */
void ATGNSSExample()
{
    WE_APP_PRINT("*** Start of Adrastea-I ATGNSS example ***\r\n");

    if (!AdrasteaI_Init(&AdrasteaI_uart, &AdrasteaI_pins, &AdrasteaI_ATGNSS_EventCallback))
    {
        WE_APP_PRINT("Initialization error\r\n");
        return;
    }

    bool ret = AdrasteaI_ATPacketDomain_SetNetworkRegistrationResultCode(AdrasteaI_ATPacketDomain_Network_Registration_Result_Code_Enable_with_Location_Info);
    AdrasteaI_ExamplesPrint("Set Network Registration Result Code", ret);

    while (status.state != AdrasteaI_ATPacketDomain_Network_Registration_State_Registered_Roaming)
    {
        WE_Delay(10);
    }

    ret = AdrasteaI_ATGNSS_DownloadCEPFile(AdrasteaI_ATGNSS_CEP_Number_of_Days_1Day);
    AdrasteaI_ExamplesPrint("Download CEP File", ret);

    AdrasteaI_ATGNSS_CEP_Status_t cepStatus;
    ret = AdrasteaI_ATGNSS_QueryCEPFileStatus(&cepStatus);
    AdrasteaI_ExamplesPrint("Query CEP Status", ret);
    if (ret)
    {
        WE_APP_PRINT("Validity: %d, Remaining Days: %d, Hours: %d, Minutes:%d\r\n", cepStatus.validity, cepStatus.remDays, cepStatus.remHours, cepStatus.remMinutes);
    }

    ret = AdrasteaI_ATDevice_SetPhoneFunctionality(AdrasteaI_ATDevice_Phone_Functionality_Min, AdrasteaI_ATDevice_Phone_Functionality_Reset_Do_Not_Reset);
    AdrasteaI_ExamplesPrint("Set Phone Functionality", ret);

    ret = AdrasteaI_ATGNSS_StartGNSS(AdrasteaI_ATGNSS_Start_Mode_Cold);
    AdrasteaI_ExamplesPrint("Start GNSS", ret);

    AdrasteaI_ATGNSS_Satellite_Systems_t satSystems = {.systems = {.GPS = AdrasteaI_ATGNSS_Runtime_Mode_State_Set, .GLONASS = AdrasteaI_ATGNSS_Runtime_Mode_State_Set}};
    ret = AdrasteaI_ATGNSS_SetSatelliteSystems(satSystems);
    AdrasteaI_ExamplesPrint("Set Satellite Systems", ret);

    AdrasteaI_ATGNSS_Fix_t fix;

    while (1)
    {
        WE_Delay(30000);
        satelliteQueryCount = 0;
        satelliteQueryEventCount = 0;
        ret = AdrasteaI_ATGNSS_QueryGNSSSatellites(&satelliteQueryCount);
        AdrasteaI_ExamplesPrint("Query GNSS Satellites", ret);
        WE_APP_PRINT("Satellites Count: %d\r\n", satelliteQueryCount);

        while (satelliteQueryCount != satelliteQueryEventCount)
        {
            WE_Delay(1000);
        }

        ret = AdrasteaI_ATGNSS_QueryGNSSFix(AdrasteaI_ATGNSS_Fix_Relavancy_Current, &fix);
        AdrasteaI_ExamplesPrint("Query GNSS Fix", ret);

        if (ret && fix.fixType != AdrasteaI_ATGNSS_Fix_Type_No_Fix)
        {
            WE_APP_PRINT("Fix Latitude: %f, Longitude: %f, Altitude: %f\r\n", fix.latitude, fix.longitude, fix.altitude);
        }
    }
}

void AdrasteaI_ATGNSS_EventCallback(char* eventText)
{
    AdrasteaI_ATEvent_t event;
    if (false == AdrasteaI_ATEvent_ParseEventType(&eventText, &event))
    {
        return;
    }

    switch (event)
    {
        case AdrasteaI_ATEvent_PacketDomain_Network_Registration_Status:
        {
            AdrasteaI_ATPacketDomain_ParseNetworkRegistrationStatusEvent(eventText, &status);
            break;
        }
        case AdrasteaI_ATEvent_GNSS_Satellite_Query:
        {
            AdrasteaI_ATGNSS_Satellite_t satellite;
            if (!AdrasteaI_ATGNSS_ParseSatelliteQueryEvent(eventText, &satellite))
            {
                return;
            }
            WE_APP_PRINT("PRN: %d, Elevation: %d, Azimuth: %d, SNR: %d\r\n", satellite.prn, satellite.elevation, satellite.azimuth, satellite.snr);
            satelliteQueryEventCount++;
            break;
        }
        default:
            break;
    }
}
