/*
 ***************************************************************************************************
 * This file is part of WIRELESS CONNECTIVITY SDK:
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
 * @file debug.h
 * @brief Contains code related to redirecting WE_DEBUG_PRINT_X() output for debug reasons.
 */

#ifndef GLOBAL_DEBUG_H_INCLUDED
#define GLOBAL_DEBUG_H_INCLUDED

/**
 * @brief Debug print configuration.
 */
#define WE_DEBUG_PRINT_LEVEL_OFF 0U   /**< No debug messages */
#define WE_DEBUG_PRINT_LEVEL_ERROR 1U /**< Debug messages level error and lower are printed */
#define WE_DEBUG_PRINT_LEVEL_INFO 2U  /**< Debug messages level info and lower are printed */
#define WE_DEBUG_PRINT_LEVEL_DEBUG 3U /**< Debug messages level debug and lower are printed */

/**
 * @brief Default debug print level configuration.
 * @attention This can be overriden to a different level
 */
#ifndef WE_DEBUG_PRINT_LEVEL
#define WE_DEBUG_PRINT_LEVEL WE_DEBUG_PRINT_LEVEL_OFF // Default level
#endif

/** @cond DOXYGEN_IGNORE */
#if (WE_DEBUG_PRINT_LEVEL > WE_DEBUG_PRINT_LEVEL_OFF)
/** @endcond*/

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief Prints the debug message.
 * 
 * @param[in] level: The level of the message to be printed
 * @param[in] file: The file from which the message came
 * @param[in] func: The function from which the message came
 * @param[in] format: Format string (printf-style) for the message
 * @param[in] ...  Additional arguments for the format string
 */
void WE_Debug_Print(unsigned int level, const char* file, const char* func, const char* format, ...);

#ifdef __cplusplus
}
#endif

#define WE_DEBUG_PRINT(level, ...)                                                                                                                                                                                                                                                                                                                                                                             \
    do                                                                                                                                                                                                                                                                                                                                                                                                         \
    {                                                                                                                                                                                                                                                                                                                                                                                                          \
        if ((level) <= WE_DEBUG_PRINT_LEVEL)                                                                                                                                                                                                                                                                                                                                                                   \
        {                                                                                                                                                                                                                                                                                                                                                                                                      \
            WE_Debug_Print(level, __FILE__, __func__, __VA_ARGS__);                                                                                                                                                                                                                                                                                                                                            \
        }                                                                                                                                                                                                                                                                                                                                                                                                      \
    } while (0)

/** @cond DOXYGEN_IGNORE */
#else
#define WE_DEBUG_PRINT(level, ...)
#endif /* WE_DEBUG_PRINT_LEVEL */
/** @endcond*/

/**
 * @brief Print error messages
 * 
 * @param[in] ...  Format string and additional arguments.
 */
#define WE_DEBUG_PRINT_ERROR(...) WE_DEBUG_PRINT(WE_DEBUG_PRINT_LEVEL_ERROR, __VA_ARGS__)

/**
 * @brief Print info messages
 * 
 * @param[in] ...  Format string and additional arguments.
 */
#define WE_DEBUG_PRINT_INFO(...) WE_DEBUG_PRINT(WE_DEBUG_PRINT_LEVEL_INFO, __VA_ARGS__)

/**
 * @brief Print debug messages
 * 
 * @param[in] ...  Format string and additional arguments.
 */
#define WE_DEBUG_PRINT_DEBUG(...) WE_DEBUG_PRINT(WE_DEBUG_PRINT_LEVEL_DEBUG, __VA_ARGS__)

#endif /* GLOBAL_DEBUG_H_INCLUDED */
