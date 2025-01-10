
# Cordelia-I Secure IoT Module - QuarkLink Example

This repository contains an example application demonstrating how to use the Cordelia-I Secure IoT module with the Würth Elektronik QuarkLink platform. The example showcases the setup, provisioning, and secure communication capabilities of the module, leveraging features such as WLAN connectivity, secure device provisioning, and MQTT communication over TLS.

## Getting Started

### Prerequisites

Before running this example, ensure you have:

1. **Hardware Requirements:**
   - Host microcontroller (e.g., STM32F4xx or STM32L0xx Nucleo evaluation board)
   - Cordelia-I Secure IoT module evaluation board
   - UART and RESET pin connections between the MCU and the Cordelia-I module (see connection diagram below)
   - WLAN access point

2. **Software Requirements:**
   - STM32 development tools (e.g., STM32CubeIDE)
   - Würth Elektronik Wireless Connectivity SDK (this repository)
   - Your own personal QuarkLink instance and credentials, which you can get under [https://signup.quarklink.io/](https://signup.quarklink.io/). You can get more information on the process in the [Cordelia-I User Manual](https://www.we-online.com/de/components/products/CORDELIA-I). 

### Hardware Setup

Connect the Cordelia-I module to the host microcontroller as per the following diagram.

> **Note:** Not all available pins of the Cordelia-I module are connected in this setup, as we are assuming the default AT command mode configuration. Ensure that the unconnected pins are handled appropriately if your application requires them.

```
+-------------------+         +---------------------+
|  HOST MCU (STM32) |         |  CORDELIA-I MODULE  |
|-------------------|         |---------------------|
| GPIO PB7 (UART RX)|<--------| UART0_TX (JP1 Pin 4)|
| GPIO PB6 (UART TX)|-------->| UART0_RX (JP1 Pin 2)|
| GPIO PA10 (NRESET)|-------->| NRESET (CON4 Pin 15)|
|        3V3        |-------->| VCC (CON4 Pin 1)    |
|        GND        |-------->| GND (CON4 Pin 4)    |
+-------------------+         +---------------------+
```

### Software Setup

1. Clone this repository to your local machine.
2. Open the project project matching your host microcontroller (STM32F4xx or STM32L0xx) in your STM32 development environment (preferably STM32CubeIDE).
3. Connect the Cordelia-I module to the host microcontroller as per the connection diagram.
4. Update the WLAN credentials, QuarkLink Root CA, and hostname in the source code.
    - Update the `WLAN_SSID` and `WLAN_KEY` macros in the source code with your network credentials.
    - Replace the `QL_ROOT_CA` macro with the QuarkLink Root CA in PEM format. This Root CA is unique to your QuarkLink instance and can be downloaded from your QuarkLink portal. You can find more information on how to do that in the Cordelia-I User Manual.
    - Configure the `QL_HOSTNAME` macro with your QuarkLink server's hostname.
5. Compile and flash the example onto your host microcontroller.

### Running the Example

1. Monitor the serial output of the host microcontroller using a terminal emulator (e.g., HTerm) using 115200 baud, 8N1 settings.
2. Follow the logs to observe:
   - WLAN connection status
   - Root CA configuration
   - Device enrolment with QuarkLink
   - MQTT publish and subscribe activities

## License

The software is provided "as is" under the terms specified in the SDK license agreement. See the license file included in the SDK for more details.

## Support

For technical support, refer to the [Cordelia-I User Manual](https://www.we-online.com/de/components/products/CORDELIA-I) or contact Würth Elektronik WCS support at [wcs@we-online.com](mailto:wcs@we-online.com).