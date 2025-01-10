# Description

This project contains drivers of various wireless connectivity radio modules for the STM32L0 and STM32F4 microcontrollers.
It implements the UART communication (UART1, DMA-based and/or interrupt-based) with the radio module without underlying RTOS and threads.

For more details, please refer to the application note [ANR008_WirelessConnectivity_Software_Development_Kit_SDK](http://www.we-online.com/ANR008).

# Module pinout

The following sections contain wiring instructions for each radio module. Note that some connections are optional and may not be required depending on the application.

## Adrastea-I pinout
```
 _____________________           ______________________
|        STM32        |         |       Adrastea-I     |
|                     |         |                      |
|         Reset (PA10)|-------->|Reset                 | 
|                     |         |                      |  
|        Wake_up (PA9)|-------->|Wake_up               |
|                     |         |                      |
|       UART1 TX (PB6)|-------->|UART RX               |
|                     |         |                      |
|       UART1 RX (PB7)|<--------|UART TX               |
|_____________________|         |______________________|
```
**Note** Please make sure to match the voltage level according to the module since the GPIO pins in Adrastea-I are 1.8V (In case of using the evaluation board there is already a level shifter to 1.8V on the UART pins only).

## Calypso-I pinout
```
 _____________________           ______________________
|        STM32        |         |       Calypso-I      |
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

## Cordelia-I pinout
```
 _____________________           ______________________
|        STM32        |         |      Cordelia-I      |
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

## Daphnis-I pinout
```
 _____________________           ______________________
|        STM32        |         |       Daphnis-I      |
|                     |         |                      |
|         Reset (PA10)|-------->|Reset                 |
|                     |         |                      |
|        Wake_up (PA5)|-------->|Wake_up               |
|                     |         |                      |
|           Boot (PA7)|-------->|Boot                  |
|                     |         |                      |
|       UART1 TX (PB6)|-------->|LPUART1 RX            |
|                     |         |                      |
|       UART1 RX (PB7)|<--------|LPUART1 TX            |
|                     |         |                      |
|   STATUS_IND_0 (PB8)|<--------|LED_0                 |
|                     |         |                      |
|   STATUS_IND_1 (PB9)|<--------|LED_1                 |
|_____________________|         |______________________|
```

## Metis (-I, -II) pinout
```
 _____________________           ______________________
|        STM32        |         |    Metis (-I, -II)   |
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

## Metis-e pinout
```
 _____________________           ______________________
|        STM32        |         |        Metis-e       |
|                     |         |                      |
|         Sleep  (PA9)|-------->|Wake-up               |
|                     |         |                      |
|         Boot   (PA7)|-------->|Boot                  |
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

## Proteus-II pinout
```
 _____________________           ______________________
|        STM32        |         |     Proteus-II       |
|                     |         |                      |
|         Reset (PA10)|-------->|Reset                 |
|                     |         |                      |
|        Wake_up (PA9)|-------->|Wake_up               |
|                     |         |                      |
|           Boot (PA7)|-------->|Boot                  |
|                     |         |                      |
|           Mode (PA8)|-------->|Mode                  |
|                     |         |                      |
|           Busy (PB8)|<--------|Busy                  |
|                     |         |                      |
| STATUS (LED 2) (PB9)|<--------|LED_2                 |
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
|           Busy (PB8)|<--------|Busy                  |
|                     |         |                      |
| STATUS (LED 2) (PB9)|<--------|LED_2                 |
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

## Proteus-e pinout
```
 _______________________           ______________________
|         STM32         |         |      Proteus-e       |
|                       |         |                      |
|           Reset (PA10)|-------->|Reset                 |
|                       |         |                      |
|             Mode (PA8)|-------->|Mode                  |
|                       |         |                      |
| Busy/UART_Enable (PB8)|<------->|Busy/UART_Enable      |
|                       |         |                      |
|   STATUS (LED 1) (PB9)|<--------|LED_1                 |
|                       |         |                      |
|         UART1 TX (PB6)|-------->|UART RX               |
|                       |         |                      |
|         UART1 RX (PB7)|<--------|UART TX               |
|                       |         |                      |
|       UART1 RTS (PA12)|-------->|UART CTS              |
|                       |         |                      |
|       UART1 CTS (PA11)|<--------|UART RTS              |
|_______________________|         |______________________|
```

## Stephano-I pinout
```
 _____________________           ______________________
|        STM32        |         |     Stephano-I       |
|                     |         |                      |
|         Reset (PA10)|-------->|Reset                 |
|                     |         |                      |
|        Wake_up (PA5)|-------->|GPIO10                |
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

## Tarvos-e pinout
```
 _____________________           ______________________
|        STM32        |         |       Tarvos-e       |
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

## Thebe-II (-IND) pinout
```
 _____________________           ______________________
|        STM32        |         |    Thebe-II (-IND)   |
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

## Thyone-e pinout
```
 _____________________           ______________________
|        STM32        |         |       Thyone-e       |
|                     |         |                      |
|         Reset (PA10)|-------->|Reset                 |
|                     |         |                      |
|           Mode (PA8)|-------->|Mode                  |
|                     |         |                      |
|           Busy (PA9)|<--------|Busy                  |
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
|           Busy (PA6)|<--------|Busy                  |
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

# STM32 Nucleo pinout
![alt text](Doc/Nucleo-L073RZ.png)
![alt text](Doc/Nucleo-F401RE.png)