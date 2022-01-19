# Description

This project contains drivers of various wireless connectivity radio modules for the STM32L0 and STM32F4 microcontrollers.
It implements the UART communication (UART1, DMA-based and/or interrupt-based) with the radio module without underlying RTOS and threads.

For more details, please refer to the application note [ANR008_WirelessConnectivity_Software_Development_Kit_SDK](http://www.we-online.com/ANR008).

# Pinout

The following sections contain wiring instructions for each radio module.

Note that some connections are optional and may not be required depending on the application.

## Calypso pinout
```
 _____________________           ______________________
|        STM32        |         |       Calypso        |
|                     |         |                      |
|         Reset (PA10)|-------->|Reset                 |
|                     |         |                      |
|        Wake_up (PA9)|-------->|Wake_up               |
|                     |         |                      |
|           Boot (PA7)|-------->|Boot                  |
|                     |         |                      |
|       UART1 TX (PB6)|-------->|UART RX               |
|                     |         |                      |
|       UART1 RX (PB7)|<--------|UART TX               |
|                     |         |                      |
|     UART1 RTS (PA12)|-------->|UART CTS              |
|                     |         |                      |
|     UART1 CTS (PA11)|<--------|UART RTS              |
|                     |         |                      |
|     APP_MODE_0 (PA0)|-------->|APP_MODE_0            |
|                     |         |                      |
|     APP_MODE_1 (PA1)|-------->|APP_MODE_1            |
|                     |         |                      |
|   STATUS_IND_0 (PB8)|<--------|STATUS_IND_0          |
|                     |         |                      |
|   STATUS_IND_1 (PB9)|<--------|STATUS_IND_1          |
|_____________________|         |______________________|
```

## Metis pinout
```
 _____________________           ______________________
|        STM32        |         |        Metis         |
|                     |         |                      |
|         Reset (PA10)|-------->|Reset                 |
|                     |         |                      |
|       UART1 TX (PB6)|-------->|UART RX               |
|                     |         |                      |
|       UART1 RX (PB7)|<--------|UART TX               |
|                     |         |                      |
|     UART1 RTS (PA12)|-------->|UART CTS              |
|                     |         |                      |
|     UART1 CTS (PA11)|<--------|UART RTS              |
|_____________________|         |______________________|
```

## Proteus-III pinout
```
 _____________________           ______________________
|        STM32        |         |     Proteus-III      |
|                     |         |                      |
|         Reset (PA10)|-------->|Reset                 |
|                     |         |                      |
|        Wake_up (PA9)|-------->|Wake_up               |
|                     |         |                      |
|           Boot (PA7)|-------->|Boot                  |
|                     |         |                      |
|           Mode (PA8)|-------->|Mode                  |
|                     |         |                      |
|       UART1 TX (PB6)|-------->|UART RX               |
|                     |         |                      |
|       UART1 RX (PB7)|<--------|UART TX               |
|                     |         |                      |
|     UART1 RTS (PA12)|-------->|UART CTS              |
|                     |         |                      |
|     UART1 CTS (PA11)|<--------|UART RTS              |
|_____________________|         |______________________|
```

## Tarvos-III pinout
```
 _____________________           ______________________
|        STM32        |         |      Tarvos-III      |
|                     |         |                      |
|         Reset (PA10)|-------->|Reset                 |
|                     |         |                      |
|        Wake_up (PA9)|-------->|Wake_up               |
|                     |         |                      |
|           Boot (PA7)|-------->|Boot                  |
|                     |         |                      |
|           Mode (PA8)|-------->|Mode                  |
|                     |         |                      |
|       UART1 TX (PB6)|-------->|UART RX               |
|                     |         |                      |
|       UART1 RX (PB7)|<--------|UART TX               |
|                     |         |                      |
|     UART1 RTS (PA12)|-------->|UART CTS              |
|                     |         |                      |
|     UART1 CTS (PA11)|<--------|UART RTS              |
|_____________________|         |______________________|
```

## Telesto-III pinout
```
 _____________________           ______________________
|        STM32        |         |     Telesto-III      |
|                     |         |                      |
|         Reset (PA10)|-------->|Reset                 |
|                     |         |                      |
|        Wake_up (PA9)|-------->|Wake_up               |
|                     |         |                      |
|           Boot (PA7)|-------->|Boot                  |
|                     |         |                      |
|           Mode (PA8)|-------->|Mode                  |
|                     |         |                      |
|       UART1 TX (PB6)|-------->|UART RX               |
|                     |         |                      |
|       UART1 RX (PB7)|<--------|UART TX               |
|                     |         |                      |
|     UART1 RTS (PA12)|-------->|UART CTS              |
|                     |         |                      |
|     UART1 CTS (PA11)|<--------|UART RTS              |
|_____________________|         |______________________|
```

## Thebe-II pinout
```
 _____________________           ______________________
|        STM32        |         |       Thebe-II       |
|                     |         |                      |
|         Reset (PA10)|-------->|Reset                 |
|                     |         |                      |
|        Wake_up (PA9)|-------->|Wake_up               |
|                     |         |                      |
|           Boot (PA7)|-------->|Boot                  |
|                     |         |                      |
|           Mode (PA8)|-------->|Mode                  |
|                     |         |                      |
|       UART1 TX (PB6)|-------->|UART RX               |
|                     |         |                      |
|       UART1 RX (PB7)|<--------|UART TX               |
|                     |         |                      |
|     UART1 RTS (PA12)|-------->|UART CTS              |
|                     |         |                      |
|     UART1 CTS (PA11)|<--------|UART RTS              |
|_____________________|         |______________________|
```

## Themisto-I pinout
```
 _____________________           ______________________
|        STM32        |         |      Themisto-I      |
|                     |         |                      |
|         Reset (PA10)|-------->|Reset                 |
|                     |         |                      |
|        Wake_up (PA9)|-------->|Wake_up               |
|                     |         |                      |
|           Boot (PA7)|-------->|Boot                  |
|                     |         |                      |
|           Mode (PA8)|-------->|Mode                  |
|                     |         |                      |
|       UART1 TX (PB6)|-------->|UART RX               |
|                     |         |                      |
|       UART1 RX (PB7)|<--------|UART TX               |
|                     |         |                      |
|     UART1 RTS (PA12)|-------->|UART CTS              |
|                     |         |                      |
|     UART1 CTS (PA11)|<--------|UART RTS              |
|_____________________|         |______________________|
```

## Thyone-I pinout
```
 _____________________           ______________________
|        STM32        |         |       Thyone-I       |
|                     |         |                      |
|         Reset (PA10)|-------->|Reset                 |
|                     |         |                      |
|        Wake_up (PA9)|-------->|Wake_up               |
|                     |         |                      |
|           Boot (PA7)|-------->|Boot                  |
|                     |         |                      |
|           Mode (PA8)|-------->|Mode                  |
|                     |         |                      |
|       UART1 TX (PB6)|-------->|UART RX               |
|                     |         |                      |
|       UART1 RX (PB7)|<--------|UART TX               |
|                     |         |                      |
|     UART1 RTS (PA12)|-------->|UART CTS              |
|                     |         |                      |
|     UART1 CTS (PA11)|<--------|UART RTS              |
|_____________________|         |______________________|
```
