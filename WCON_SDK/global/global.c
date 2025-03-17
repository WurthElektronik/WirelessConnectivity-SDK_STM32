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
 * @brief Contains global function definitions for the Wireless Connectivity SDK for STM32.
 */

#include "global_platform_types.h"
#include <global/global.h>

#include <string.h>

#ifdef __cplusplus
extern "C"
{
#endif

    static GPIO_TypeDef* gpioPorts[] = {
#ifdef GPIOA
        GPIOA,
#endif
#ifdef GPIOB
        GPIOB,
#endif
#ifdef GPIOC
        GPIOC,
#endif
#ifdef GPIOD
        GPIOD,
#endif
#ifdef GPIOE
        GPIOE,
#endif
#ifdef GPIOF
        GPIOF,
#endif
#ifdef GPIOG
        GPIOG,
#endif
#ifdef GPIOH
        GPIOH,
#endif
#ifdef GPIOI
        GPIOI,
#endif
#ifdef GPIOJ
        GPIOJ,
#endif
#ifdef GPIOK
        GPIOK,
#endif
    };

#define NUM_GPIO_PORTS (sizeof(gpioPorts) / sizeof(GPIO_TypeDef*))

    /* Sort pins by port */
    uint32_t inputPinsPerPort[NUM_GPIO_PORTS] = {0};
    uint32_t outputPinsPerPort[NUM_GPIO_PORTS] = {0};

    /*              Functions              */

    /**
 * @brief Initialise the microcontroller and setup system clock 
 */

    void WE_Platform_Init(void)
    {
        /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
        HAL_Init();

        /* Configure the system clock */
        WE_SystemClock_Config();
    }

    /**
 * @brief Disables the interrupts
 */
    void WE_Error_Handler(void)
    {
        /* User can add his own implementation to report the HAL error return state */
        __disable_irq();
        while (1)
        {
        }
    }

    /**
 * @brief Initialises the pins
 *
 * @param[in] pins: pins to be initialised
 * @param[in] numPins: number of pins
 *
 * @return true if request succeeded,
 *         false otherwise
 */
    bool WE_InitPins(WE_Pin_t pins[], uint8_t numPins)
    {
        for (uint8_t i = 0; i < numPins; i++)
        {
            if (IS_WE_PIN_UNDEFINED(pins[i]))
            {
                /* Unused */
                continue;
            }

            WE_STM32_Pin_t stm32_pin = (*(WE_STM32_Pin_t*)pins[i].pin_def);

            if (((uint32_t)0 == stm32_pin.pin) || (NULL == stm32_pin.port) || (!IS_GPIO_ALL_INSTANCE(stm32_pin.port)))
            {
                /* Since the pin was checked to be defined it should not have invalid port/pin */
                return false;
            }

            if (WE_Pin_Type_Output == pins[i].type)
            {
                /* Configure GPIO pin output level */
                HAL_GPIO_WritePin(stm32_pin.port, stm32_pin.pin, GPIO_PIN_RESET);
            }

            for (uint8_t j = 0; j < NUM_GPIO_PORTS; j++)
            {
                if (gpioPorts[j] == stm32_pin.port)
                {
                    if (pins[i].type == WE_Pin_Type_Output)
                    {
                        outputPinsPerPort[j] |= stm32_pin.pin;
                    }
                    else
                    {
                        inputPinsPerPort[j] |= stm32_pin.pin;
                    }
                    break;
                }
            }
        }

        for (uint8_t i = 0; i < NUM_GPIO_PORTS; i++)
        {
            if (((uint32_t)0 == outputPinsPerPort[i]) && ((uint32_t)0 == inputPinsPerPort[i]))
            {
                /* Unused */
                continue;
            }

            /* GPIO Ports Clock Enable */
            switch ((int)gpioPorts[i])
            {
#ifdef GPIOA
                case (int)GPIOA:
                {
                    __HAL_RCC_GPIOA_CLK_ENABLE();
                }
                break;
#endif
#ifdef GPIOB
                case (int)GPIOB:
                {
                    __HAL_RCC_GPIOB_CLK_ENABLE();
                }
                break;
#endif
#ifdef GPIOC
                case (int)GPIOC:
                {
                    __HAL_RCC_GPIOC_CLK_ENABLE();
                }
                break;
#endif
#ifdef GPIOD
                case (int)GPIOD:
                {
                    __HAL_RCC_GPIOD_CLK_ENABLE();
                }
                break;
#endif
#ifdef GPIOE
                case (int)GPIOE:
                {
                    __HAL_RCC_GPIOE_CLK_ENABLE();
                }
                break;
#endif
#ifdef GPIOF
                case (int)GPIOF:
                {
                    __HAL_RCC_GPIOF_CLK_ENABLE();
                }
                break;
#endif
#ifdef GPIOH
                case (int)GPIOH:
                {
                    __HAL_RCC_GPIOH_CLK_ENABLE();
                }
                break;
#endif
#ifdef GPIOI
                case (int)GPIOI:
                {
                    __HAL_RCC_GPIOI_CLK_ENABLE();
                }
                break;
#endif
#ifdef GPIOJ
                case (int)GPIOJ:
                {
                    __HAL_RCC_GPIOJ_CLK_ENABLE();
                }
                break;
#endif
#ifdef GPIOK
                case (int)GPIOK:
                {
                    __HAL_RCC_GPIOA_CLK_ENABLE();
                }
                break;
#endif
                default:
                {
                    /* error: port not implemented */
                    return false;
                }
            }

            if (0 != outputPinsPerPort[i])
            {
                GPIO_InitTypeDef GPIO_InitStruct = {0};
                GPIO_InitStruct.Pin = outputPinsPerPort[i];
                GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
                GPIO_InitStruct.Pull = GPIO_NOPULL;
                GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
                HAL_GPIO_Init(gpioPorts[i], &GPIO_InitStruct);
            }
            if (0 != inputPinsPerPort[i])
            {
                GPIO_InitTypeDef GPIO_InitStruct = {0};
                GPIO_InitStruct.Pin = inputPinsPerPort[i];
                GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
                GPIO_InitStruct.Pull = GPIO_NOPULL;
                GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
                HAL_GPIO_Init(gpioPorts[i], &GPIO_InitStruct);
            }
        }

        return true;
    }

    bool WE_Reconfigure(WE_Pin_t pin)
    {
        if (IS_WE_PIN_UNDEFINED(pin))
        {
            return false;
        }

        WE_STM32_Pin_t stm32_pin = (*(WE_STM32_Pin_t*)pin.pin_def);

        if (((uint32_t)0 == stm32_pin.pin) || (NULL == stm32_pin.port) || (!IS_GPIO_ALL_INSTANCE(stm32_pin.port)))
        {
            /* Unused */
            return false;
        }

        if (WE_Pin_Type_Output == pin.type)
        {
            /* Configure GPIO pin output level */
            HAL_GPIO_WritePin(stm32_pin.port, stm32_pin.pin, GPIO_PIN_RESET);
        }

        for (uint8_t j = 0; j < NUM_GPIO_PORTS; j++)
        {
            if (gpioPorts[j] == stm32_pin.port)
            {
                if (pin.type == WE_Pin_Type_Output)
                {
                    outputPinsPerPort[j] |= stm32_pin.pin;
                    inputPinsPerPort[j] &= ~stm32_pin.pin;
                }
                else
                {
                    inputPinsPerPort[j] |= stm32_pin.pin;
                    outputPinsPerPort[j] &= ~stm32_pin.pin;
                }

                GPIO_InitTypeDef GPIO_InitStruct = {0};
                GPIO_InitStruct.Pin = outputPinsPerPort[j];
                GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
                GPIO_InitStruct.Pull = GPIO_NOPULL;
                GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
                HAL_GPIO_Init(gpioPorts[j], &GPIO_InitStruct);

                GPIO_InitStruct.Pin = inputPinsPerPort[j];
                GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
                GPIO_InitStruct.Pull = GPIO_NOPULL;
                GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
                HAL_GPIO_Init(gpioPorts[j], &GPIO_InitStruct);

                return true;
            }
        }

        return false;
    }

    /**
 * @brief Deinitialises the pin to their default reset values
 *
 * @param[in] pin: pin to be deinitialised
 *
 * @return true if request succeeded,
 *         false otherwise
 */
    bool WE_DeinitPin(WE_Pin_t pin)
    {
        if (IS_WE_PIN_UNDEFINED(pin))
        {
            return false;
        }

        WE_STM32_Pin_t stm32_pin = (*(WE_STM32_Pin_t*)pin.pin_def);

        HAL_GPIO_DeInit(stm32_pin.port, stm32_pin.pin);
        return true;
    }

    /**
 * @brief Switch pin to output high/low
 *
 * @param[in] pin Output pin to be set
 * @param[in] out Output level to be set
 * @return true if request succeeded, false otherwise
 */

    bool WE_SetPin(WE_Pin_t pin, WE_Pin_Level_t out)
    {
        if (IS_WE_PIN_UNDEFINED(pin))
        {
            return false;
        }

        WE_STM32_Pin_t stm32_pin = (*(WE_STM32_Pin_t*)pin.pin_def);

        if (0 == stm32_pin.pin || NULL == stm32_pin.port || pin.type != WE_Pin_Type_Output)
        {
            return false;
        }

        switch (out)
        {
            case WE_Pin_Level_High:
                HAL_GPIO_WritePin(stm32_pin.port, stm32_pin.pin, GPIO_PIN_SET);
                break;

            case WE_Pin_Level_Low:
                HAL_GPIO_WritePin(stm32_pin.port, stm32_pin.pin, GPIO_PIN_RESET);
                break;

            default:
                return false;
        }

        return true;
    }

    /**
 * @brief Gets the pin level
 *
 * @param[in] pin: the pin to be checked
 *
 * @param[out] pin_levelP: the pin level
 *
 * @return true if request succeeded,
 *         false otherwise
 *         
 */
    bool WE_GetPinLevel(WE_Pin_t pin, WE_Pin_Level_t* pin_levelP)
    {
        if (IS_WE_PIN_UNDEFINED(pin) || (pin_levelP == NULL))
        {
            return false;
        }

        WE_STM32_Pin_t stm32_pin = (*(WE_STM32_Pin_t*)pin.pin_def);

        switch (HAL_GPIO_ReadPin(stm32_pin.port, stm32_pin.pin))
        {
            case GPIO_PIN_RESET:
                *pin_levelP = WE_Pin_Level_Low;
                break;
            case GPIO_PIN_SET:
                *pin_levelP = WE_Pin_Level_High;
                break;
            default:
                return false;
        }

        return true;
    }

    /**
 * @brief Delays the microcontoller for the specified time.
 *
 * @param[in] sleepForMs: time in milliseconds.
 *         
 */
    void WE_Delay(uint16_t sleepForMs)
    {
        if (sleepForMs > 0)
        {
            HAL_Delay((uint32_t)sleepForMs);
        }
    }

    /**
 * @brief Gets the elapsed time since startup
 *
 * @return returns elapsed in ms
 *         
 */
    uint32_t WE_GetTick() { return HAL_GetTick(); }

    /**
 * @brief Delays the microcontoller for the specified time.
 *
 * @param[in] sleepForMs: time in microseconds.
 *         
 */
    __weak void WE_DelayMicroseconds(uint32_t sleepForUsec)
    {
        /* Microsecond tick is disabled: round to ms */
        WE_Delay(((sleepForUsec + 500) / 1000));
    }

    /**
 * @brief Gets the elapsed time since startup
 *
 * @return returns elapsed in microseconds
 *         
 */
    __weak uint32_t WE_GetTickMicroseconds()
    {
        /* Microsecond tick is disabled: return ms tick * 1000 */
        return WE_GetTick() * 1000;
    }

    /**
 * @brief Gets the Driver version
 *
 * @param[out] version: will contain the version value
 *
 * @return true if request succeeded,
 *         false otherwise        
 */
    bool WE_GetDriverVersion(uint8_t* version)
    {
        uint8_t help[3] = WE_WIRELESS_CONNECTIVITY_SDK_VERSION;
        memcpy(version, help, 3);
        return true;
    }

#ifdef __cplusplus
}
#endif
