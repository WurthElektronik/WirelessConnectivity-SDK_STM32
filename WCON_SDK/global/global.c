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

#include "global_platform.h"
#include <global/global.h>

#ifdef __cplusplus
extern "C"
{
#endif

/*              Functions              */

/**
* @brief Enable the clock of the GPIO port
*
* @param[in] GPIOx: pointer to the GPIO port
*
* @return true if request succeeded,
*         false otherwise
*/
static bool WE_Enable_GPIO_Clock(GPIO_TypeDef* GPIOx);

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

static bool WE_Enable_GPIO_Clock(GPIO_TypeDef* GPIOx)
{
#ifdef GPIOA
    if (GPIOx == GPIOA)
    {
        __HAL_RCC_GPIOA_CLK_ENABLE();
        return true;
    }
#endif
#ifdef GPIOB
    if (GPIOx == GPIOB)
    {
        __HAL_RCC_GPIOB_CLK_ENABLE();
        return true;
    }
#endif
#ifdef GPIOC
    if (GPIOx == GPIOC)
    {
        __HAL_RCC_GPIOC_CLK_ENABLE();
        return true;
    }
#endif
#ifdef GPIOD
    if (GPIOx == GPIOD)
    {
        __HAL_RCC_GPIOD_CLK_ENABLE();
        return true;
    }
#endif
#ifdef GPIOE
    if (GPIOx == GPIOE)
    {
        __HAL_RCC_GPIOE_CLK_ENABLE();
        return true;
    }
#endif
#ifdef GPIOF
    if (GPIOx == GPIOF)
    {
        __HAL_RCC_GPIOF_CLK_ENABLE();
        return true;
    }
#endif
#ifdef GPIOG
    if (GPIOx == GPIOG)
    {
        __HAL_RCC_GPIOG_CLK_ENABLE();
        return true;
    }
#endif
#ifdef GPIOH
    if (GPIOx == GPIOH)
    {
        __HAL_RCC_GPIOH_CLK_ENABLE();
        return true;
    }
#endif
#ifdef GPIOI
    if (GPIOx == GPIOI)
    {
        __HAL_RCC_GPIOI_CLK_ENABLE();
        return true;
    }
#endif
#ifdef GPIOJ
    if (GPIOx == GPIOJ)
    {
        __HAL_RCC_GPIOJ_CLK_ENABLE();
        return true;
    }
#endif
#ifdef GPIOK
    if (GPIOx == GPIOK)
    {
        __HAL_RCC_GPIOK_CLK_ENABLE();
        return true;
    }
#endif

    return false; // Unknown or unsupported port
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

        if (!WE_Enable_GPIO_Clock(stm32_pin.port))
        {
            return false;
        }

        GPIO_InitTypeDef GPIO_InitStruct = {0};
        GPIO_InitStruct.Pin = stm32_pin.pin;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;

        switch (pins[i].type)
        {
            case WE_Pin_Type_Output:
            {
                if (!WE_SetPin(pins[i], pins[i].initial_value.output))
                {
                    return false;
                }

                GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;

                break;
            }
            case WE_Pin_Type_Input:
            {

                GPIO_InitStruct.Mode = GPIO_MODE_INPUT;

                switch (pins[i].initial_value.input_pull)
                {
                    case WE_Pin_PullType_No:
                    {
                        GPIO_InitStruct.Pull = GPIO_NOPULL;
                        break;
                    }
                    case WE_Pin_PullType_Up:
                    {
                        GPIO_InitStruct.Pull = GPIO_PULLUP;
                        break;
                    }
                    case WE_Pin_PullType_Down:
                    {
                        GPIO_InitStruct.Pull = GPIO_PULLDOWN;
                        break;
                    }
                    default:
                        return false;
                }

                break;
            }
            default:
                return false;
        }

        HAL_GPIO_Init(stm32_pin.port, &GPIO_InitStruct);
    }

    return true;
}

bool WE_Reconfigure(WE_Pin_t pin)
{
    if (IS_WE_PIN_UNDEFINED(pin))
    {
        return false;
    }

    if (!WE_DeinitPins(&pin, 1))
    {
        return false;
    }

    if (!WE_InitPins(&pin, 1))
    {
        return false;
    }

    return true;
}

bool WE_DeinitPins(WE_Pin_t pins[], uint8_t numPins)
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

        HAL_GPIO_DeInit(stm32_pin.port, stm32_pin.pin);
    }

    return true;
}

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

void WE_Delay(uint32_t delay) { HAL_Delay(delay); }

__weak void WE_DelayMicroseconds(uint32_t delay)
{
    /* Microsecond tick is disabled: round to ms */
    WE_Delay(((delay + 500) / 1000));
}

uint32_t WE_GetTick() { return HAL_GetTick(); }

__weak uint32_t WE_GetTickMicroseconds()
{
    /* Microsecond tick is disabled: return ms tick * 1000 */
    return WE_GetTick() * 1000;
}

#ifdef __cplusplus
}
#endif
