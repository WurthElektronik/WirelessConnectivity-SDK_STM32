# Description

This project contains drivers of various wireless connectivity radio modules for the STM32L0 microcontroller.
It implements the UART communication (UART1, interrupt based) with the radio module without underlying RTOS and threads.

## The pinout of the STM32 controller is as follows:
```
 _____________________           __________________ 
|        STM32        |         |   Radio module   |
|                     |         |                  |
|         Reset (PA10)|-------->|Reset             |
|                     |         |                  |
|        Wake_up (PA9)|-------->|Wake_up           |
|                     |         |                  |
|           Boot (PA7)|-------->|Boot              |
|                     |         |                  |
|           Mode (PA8)|-------->|Mode              |
|                     |         |                  |
|       UART1 TX (PB6)|-------->|UART RX           |
|                     |         |                  |
|       UART1 RX (PB7)|<--------|UART TX           |
|                     |         |                  |
|       UART1 RTS(PB3)|-------->|UART CTS          |
|                     |         |                  |
|       UART1 CTS(PB4)|<--------|UART RTS          |
|_____________________|         |__________________|
```
For more details, please refere to the application note [ANR008_WirelessConnectivity_Software_Development_Kit_SDK](http://www.we-online.com/ANR008).