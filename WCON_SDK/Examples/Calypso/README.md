# <p align="center"> How to setup the Calypso Wi-Fi module </p>

<font size = "10">**Quickstart Guide**</font>


The Calypso WiFi module comes pre-flashed and ready-to-use out-of-the box. The following items are necessary to go through this process.

• A Wi-Fi enabled PC with browser (Chrome browser recommended) to program the STM Nucleo board

• Calypso Evaluation Board

• STM Nucleo Board

• Jumpers for connection between STM Nucleo Board and Calypso Evaluation Board

•  A USB cable to power up the STM32 Nucleo board

• A Microsoft Azure account. This can be created under the following link,
https://azure.microsoft.com/en-us/free/

• The WE certificate generator tool. This can be downloaded from,
https://www.we-online.com/certificategenerator



<font size = "10">**Calypso Wi-Fi module setup**</font>

The brief description of the steps to be followed to setup the Calypso Wi-Fi module are summarized below: 

![Untitled Workspace_module](https://user-images.githubusercontent.com/105643484/210844658-b13a2dbf-2229-49e7-9041-aabd79cf5611.jpg)



<font size = "10">**1. Create an IoT central application**</font>

a.	Sign in to the Azure portal, https://portal.azure.com/

b.	 From the Azure homepage, select the "+ Create a resource button" and then enter "IoT Central application" in the Search the Marketplace field. 

c.	Select "IoT Central application" from the search results and the select "Create".

![createiotcertral](https://user-images.githubusercontent.com/105643484/209803323-591e9900-5177-4d69-9125-d2382bcdf53a.PNG)

<p align="center"><sub>Figure 1: IoT central application</sub><p>


On the Basics tab, complete the fields as follows: 

a.	Subscription: Select the subscription to use for the application.

b.	Resource group: Select a resource group or create a new one. To create a new one, select Create new and fill in the name you want to use. To use an existing re source group, select that resource group. For more information, see Manage Azure Resource Manager resource groups.

c.	Resource name: Type in a name for the IoT central application.

d.	Application URL: This will be automatically set to .azureiotcentral.com 

e.	Template: From the drop down, select "Custom application".

f.	Region: Select the region in which the application will be located. Select a location that is geographically the closest.

g.	Pricing Plan: Choose the appropriate pricing tier. The standard tier 0 is good to start prototyping. More details on pricing can be found under, https://azure.microsoft.com/en-us/pricing/details/iot-central
  
h.	Click on "Review + Create". 
  
i.	In the following page, review the terms and click on "Create". 
  
j.	Wait for the deploymet to complete. After the process is complete, click on "Go to re source" button to open the application. 
  
k.	Click on the IoT central application URL to open the newly created IoT central platform. 
  
l.	In the IoT central app open, "Permissions > Device connection groups" and note down the ID scope parameter for use in futher steps.

![createbasic](https://user-images.githubusercontent.com/105643484/209795404-b5cd1e9b-6309-4c69-8687-ed4e05c00ebe.PNG)

<p align="center"><sub>Figure 2: IoT central application parameters</sub><p>
  
  
![image](https://user-images.githubusercontent.com/105643484/207837683-add0c6da-7a6d-4c4a-af44-b60414e7edf4.png)

<p align="center"><sub>Figure 3: IoT central application deployment complete</sub><p>

 ![image](https://user-images.githubusercontent.com/105643484/207837718-22ec3e04-b952-42aa-b445-f3cba0815955.png)

<p align="center"><sub>Figure 4: IoT central application URL</sub><p>

![home](https://user-images.githubusercontent.com/105643484/209803683-6e7cd1cc-f162-47d0-b1b6-bc6994e7f9f1.png)

<p align="center"><sub>Figure 5: IoT central application homepage</sub><p>

 ![image](https://user-images.githubusercontent.com/105643484/207837777-1becb6a0-9a6c-4385-a6ec-397d0ee9b1e3.png)

<p align="center"><sub>Figure 6: ID scope for the created IoT central application</sub><p>

<font size = "10">**2.	Create certificates**</size>

In order to securely connect the device to IoT central application, the device needs to implement certain method for authentication. In this case, the X.509 certificate based authentication is implemented. This method requires creation of certificates for every device. In order to enable easy prototyping, Würth Elektronik eiSos’s Certificate creation tool can be used. This tool creates all the certificates necessary to get started.

•	Download the WE certificate generator tool from https://www.we-online.com/certificategenerator 

•	Unzip to a suitable location on the computer and open the executable "WECertificateUploader.exe"

Inside the WE Certificate Generator tool, fill in the Device ID to generate the required certificates for the cloud service. The Device ID is the name of the device as it appears in the IoT central APP. It needs to be unique per device. 


  •	<font size= “4”>**_Device root certificate:_**</font> This is the self-signed certificate that acts as the root of trust for all devices. The device root certificate is used to generate leaf certificates. Each device has a unique leaf certificate that identifies the device. The root certificate can be generated once and used for generating leaf certificates for several device. This tool allows creation on a new root certificate, saving the same and loading it back for subsequent usage. 
1. On first time use, set the validity time in months 

2. Click on "Create root certificate" to create a new root certificate. 

3. If a root certificate already exists, click on "Load root certificate". This opens a file browser. Browse to the correct location to choose the previously used root certificate. Use .pfx format certificate for loading an existing root certificate file.
  
4. Click on "Save root certificate" to save the generated root certificate for future use. This can be used generate the certificate in .pfx format which is the format which is used during loading a certificate in Azure.

5. Click on "Export root PEM" to export the certificate in PEM format. This file needs to be uploaded to the IoT central application

6. Click on "Display root certificate" to view the certificate in the standard Windows format.

 ![image](https://user-images.githubusercontent.com/105643484/207837851-1ff70a96-48c0-4400-9f57-5bb33a011939.png)

<p align="center"><sub>Figure 8: Device root certificate options</sub><p>
  
•	**_Device certificate:_** Every device requires a unique device certificate to securely connect to the IoT central application. Each device certificate generated is exclusively linked to the device through the device ID and cannot be used on any other device. 
1. For every device ID, set the validity time in months 
2. Click on "Create device certificate" to create a new device certificate
3. Click on "Export device PEM" to export the certificate in PEM format. This file needs to be uploaded to the device  
4. Click on "Display device certificate" to view the certificate in the standard Windows format.

 ![image](https://user-images.githubusercontent.com/105643484/207837880-2ee80271-2206-4bba-8590-ecc44fceed02.png)

<p align="center"><sub>Figure 9: Device certificate options</sub><p>

•	**_Device private key:_** This is the private key corresponding to the public key in the device certificate and is also uniquely linked to a device ID. Click on "Export device key" to export the key in PEM format. This file needs to be uploaded to the device as will be explained in the subsequent sections.
![image](https://user-images.githubusercontent.com/105643484/207837918-fede0f21-6b11-4493-8b5f-da0265531b22.png) 

<p align="center"><sub>Figure 10: Device certificate options</sub><p>
   
After completing the processes of generating the root and device certificates, export all the files by clicking "Export All Files". The SSID, SSID Password, Security, NTP Server and Timezone parameters are not required, which result in no configuration file being generated.
       
![image](https://user-images.githubusercontent.com/105643484/210812222-3c485b4a-2cd5-44ad-a243-f013d781f137.png)

<p align="center"><sub>Figure 11 : Display of WE Certificate Generator Uploader application</sub><p>


**_Certificates Generated in the output directory:_** After exporting all the necessary files, a new directory will be created in the same folder as the executable file with all the necessary certificate files.
 ![image](https://user-images.githubusercontent.com/105643484/210812591-6415806a-9b0c-4833-a381-2455aea273e2.png)
  
  <p align="center"><sub>Figure 12 : Directory of certificates generated by WE Certificate Generator tool</sub><p>
  
  
<font size = "10">**3. Upload the Root certificate to IoT central**</font>
    
    
In this step, the device root certificate is uploaded and a policy is set to allow all devices with leaf certificate that are generated from this root to be allowed to connect to the platform.
    
  • In the IoT central app open, "Permissions > Device connection groups" and click on "+New"
  
  ![createNewProfile](https://user-images.githubusercontent.com/105643484/209794764-7fbb3678-a669-4373-b057-883c5d5cd7e1.png)
  <p align="center"><sub>Figure 13 : Create a new device enrolment group</sub><p>
  
  • In the subsequent window,
    
a. Enter a name for the enrolment group.
    
b. Set the "Automatically connect devices in this group" to true.
    
c. Set the group type to "IoT devices"
    
d. Set the attestation type to "Certificates (X.509)"
    
e. Click on "Save".

    
  ![enrolmentgroup](https://user-images.githubusercontent.com/105643484/209794967-32981dc8-a453-4165-9f93-c829a9bbf0c9.png)
    <p align="center"><sub>Figure 14 : Enrolment group options</sub><p>
 
   • Once the enrolment group is created, the root certificate needs to be added to this group.
In order to do this,
      
a. Click on "Manage primary" in the "Certificates (X.509)" section of the enrolment
group.
  
b. In the pop-up window, click on "Add certificate" and select the device root file generated using the WE certificate generator.
  
c. Set the "Set certificate status to verified on upload" option to true.
  
d. Click on "Upload"
  
e. On completion of the upload process, close the pop-up window.
  

  ![managecert](https://user-images.githubusercontent.com/105643484/209795196-990106c7-a635-4135-b6c2-627d44dbde57.png)
 <p align="center"><sub>Figure 15 : Upload device root certificate</sub><p>
   
   
   <font size = "10">**4. Import the Calypso WiFi module profile into your application**</font>
   

The device template for the Calypso Azure IoT WiFi module is published in the Microsoft’s list of Plug and Play devices. This template needs to be imported to this IoT central application to enable automatic device and data detection. This can be done using the following steps.
   
• Select the "Device templates" option in the main menu and click on "New".
   
   ![newdevtemplate](https://user-images.githubusercontent.com/105643484/209797262-d26e15e7-c5e9-4e7b-a2c3-42b4d70eb4c6.png)
    <p align="center"><sub>Figure 16 :  New device template</sub><p>

   
• In the subsequent window, scroll down the list of all available devices to find "Calypso IoT WiFi module". Select this and click on "Review".
      
![image](https://user-images.githubusercontent.com/105643484/210820328-c76880e7-8c22-47bd-a8c1-cbb6b8ff7b5b.png)

<p align="center"><sub>Figure 17 : Select Calypso IoT WiFi module</sub><p>

   
• Add the device by clicking on "Create" button.
  
![createdevtemplate](https://user-images.githubusercontent.com/105643484/209797994-93ff332b-d625-4393-830e-bd4b2e851d87.png)
<p align="center"><sub>Figure 18 : Create device template</sub><p>
   
At this stage, the IoT platform is ready to authenticate and automatically detect the devices of type "Calypso WiFi module"
          

<font size = "10">**5. Hardware Setup**</font>

There are 2 main steps while connecting the STM32 Nucleo board to the Calypso Evaluation Board. We need to setup the power connection to the Calypso Evaluation board and the UART connections through jumper cables from the STM32 Nucleo board.

![stm32conn](https://user-images.githubusercontent.com/105643484/210845223-6fedcb15-9872-4ae4-9472-0a9528062df9.png)
   <p align="center"><sub>Figure 19 : Connection between Calypso Wi-Fi Evaluation Board and STM32 Nucleo Board</sub><p>


a. Powering Up the Calypso Evaluation Board :

Solder 2.54mm jumpers to either connector 9 or connector 4 (pins 1 & 4) from the STM32 Nucleo board power pins of 3V3 and GND as shown in the above diagram. More information on the connector pins of the Calypso Evaluation Board can be found at : 
 
https://www.we-online.com/components/products/manual/2610019225001_Calypso%20EV%202601129200001%20Manual_rev1.4.pdf
    
b. UART connection :

Connect the pin D10 from STM32 Nucleo board to pin 2 on JP1 of the Calypso Evaluation board, and pin 21 on CN7 of STM32 Nucleo board to pin 4 on JP1 of the Calypso Evaluation board as shown in the above diagram

```
 _____________________           ______________________
|        STM32        |         | Calypso Wi-Fi Module |
|  ________________   |         | ___________________  |
|         Reset (PA10)|-------->|Reset  (Optional)     |
|                     |         |                      |
|        Wake_up (PA9)|-------->|Wake_up (Optional)    |
|                     |         |                      |
|           Boot (PA7)|-------->|Boot    (Optional)    |
|                     |         |                      |
| D10-> UART1 TX (PB6)|-------->|UART0 RX -> pin2, JP1 |  <- Compulsory Connection
|                     |         |                      |
|       UART1 RX (PB7)|<--------|UART0 TX -> pin4, JP1 |  <- Compulsory Connection
|                     |         |                      |
|         Power ->3V3 |-------->|VCC -> pin 1, CON4    |  <- Compulsory Connection
|                     |         |                      |
|                 GND |<--------|GND -> pin 4, CON4    |  <- Compulsory Connection
|_____________________|         |______________________|
```

<font size = "10">**6. Software Setup**</font>

          
The Wi-Fi module needs to be configured through the software platform with certain parameters to connect to the desired Wi-Fi network and previously created IoT central application. Download the required files from github and open them on STM32 Cube Mx
   
The following changes need to be done on the Calypso_Azure_PnP_Example.h file

a. CALYPSO_AZURE_PNP_WLAN_SSID -> Define this variable with the desired Wi-Fi SSID to connect
   
b. CALYPSO_AZUER_PNP_WLAN_PW -> Define this variable with the password of the Wi-Fi to connect
   
c. CALYPSO_AZURE_PNP_DEVICE_ID -> Define this variable with the name of the device as it appears in the IoT central Application. It needs to be unique per device.
   
d. SCOPE_ID -> Define this variable with the scope noted from the previous section. scope ID is unique per application but common across devices.
   
The following certificates need to be updated in the same header file by copying the respective certificates generated from the WE Certificate Generator tool. Note that "\r\n\" needs to be added after every 64 characters and after " "-----BEGIN CERTIFICATE-----" and before "-----END CERTIFICATE-----" of the certificate files.
   
e. DIGICERT_GLOBAL_G2_ROOT_CERT -> Define this variable with the Device root certificate generated from the WE Certificate Generator Tool
   
f. DEVICE_CERT -> Define this variable with the Device certificate generated from the WE Certificate Generator Tool
   
g. DEVICE_KEY -> Define this variable with the Device private key generated from the WE Certificate Generator Tool
   
![image](https://user-images.githubusercontent.com/105643484/211503582-03d99463-6ccb-408d-8061-7bab511e578e.png)
   
   <p align="center"><sub>Figure 20 : Configuration Changes to be done in the header file to connect Wi-Fi module to Azure</sub><p>

After committing the changes, build the project and run. The module should then be configured to connect to Azure.
     
If the certificate files needs to be overwritten, comment out the lines 175 - 180 in Calypso_Azure_PnP_Example.c
This is because if certain files are already present, the logic skips the process of creating or overwriting new files.
     
```
if((false == Calypso_Azure_PnP_File_Exists(ROOT_CA_PATH)) || (false == Calypso_Azure_PnP_File_Exists(DEVICE_CERT_PATH)) ||
			(false == Calypso_Azure_PnP_File_Exists(DEVICE_KEY_PATH)))
	{
		/*(One time only) Write the certificates to the file system*/
		Calypso_Azure_PnP_Write_Certs();
	}
```
     

<font size = "10">**7.  View the device default dashboard**</font>

To access the device on the IoT platform, navigate to "Devices -> All devices -> <Device ID>".
  
Click on the device name to open the device page.
  
![selectDevice](https://user-images.githubusercontent.com/105643484/209802027-9ab29aff-4e85-4b71-90be-153bc21ac77d.png)
  <p align="center"><sub>Figure 21 : Access Device</sub><p>


In the "about" tab, the properties are displayed.
  
![about](https://user-images.githubusercontent.com/105643484/209802225-07f545e6-71f1-4505-9eca-b24d1278363b.png)
<p align="center"><sub>Figure 22 : About the device</sub><p>
  
The telemetry data is displayed graphically in the "Overview" tab.
  
  ![overview](https://user-images.githubusercontent.com/105643484/209802349-e972273f-f375-4b77-9e7a-41d80d90bd4f.png)
 
<p align="center"><sub>Figure 23 : Overview of telemetry data</sub><p>
  
