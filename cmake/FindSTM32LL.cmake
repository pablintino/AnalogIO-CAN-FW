## MIT License
## 
## Copyright (c) 2020 Pablo Rodriguez Nava, @pablintino
## Copyright (c) 2012 - 2017 Konstantin Oblaukhov
##
## Permission is hereby granted, free of charge, to any person obtaining a copy
## of this software and associated documentation files (the "Software"), to deal
## in the Software without restriction, including without limitation the rights
## to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
## copies of the Software, and to permit persons to whom the Software is
## furnished to do so, subject to the following conditions:
## 
## The above copyright notice and this permission notice shall be included in all
## copies or substantial portions of the Software.
## 
## THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
## IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
## FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
## AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
## LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
## OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
## SOFTWARE.


# Check for previous stm32 initialization
if(NOT DEFINED CPU_STM32_FAMILY)
    message(FATAL_ERROR "STM32 has been defined")
endif()

# Check if the STM32Cube directory exists
if(NOT EXISTS "${STM32CUBE_BASE_DIR}")
    message(FATAL_ERROR "The STM32Cube directory doesn't exist")
endif()

set(STM32LL_HEADER_ONLY_COMPONENTS	bus cortex iwdg system wwdg dmamux)

if(CPU_STM32_FAMILY STREQUAL "F0")
    set(STM32_LL_COMPONENTS	adc bus comp cortex crc crs dac dma exti gpio i2c
						i2s iwdg pwr rcc rtc spi system tim usart utils wwdg)

    set(STM32_LL_REQUIRED_COMPONENTS bus cortex pwr rcc system utils)

    set(STM32_LL_PREFIX stm32f0xx_)

elseif(CPU_STM32_FAMILY STREQUAL "F1")
    set(STM32_LL_COMPONENTS	adc bus cortex crc dac dma exti gpio i2c i2s
						iwdg pwr rcc rtc spi system tim usart usb utils wwdg)

    set(STM32_LL_REQUIRED_COMPONENTS bus cortex pwr rcc system utils)

    set(STM32_LL_PREFIX stm32f1xx_)

elseif(CPU_STM32_FAMILY STREQUAL "F2")
    set(STM32_LL_COMPONENTS	adc bus cortex crc dac dma exti gpio i2c i2s iwdg pwr 
						rcc rng rtc spi system tim usart usb utils wwdg)

    set(STM32_LL_REQUIRED_COMPONENTS bus cortex pwr rcc system utils)

    set(STM32_LL_PREFIX stm32f2xx_)

elseif(CPU_STM32_FAMILY STREQUAL "F3")
    set(STM32_LL_COMPONENTS	adc bus comp cortex crc dac dma exti gpio hrtim i2c i2s
						iwdg opamp pwr rcc rtc spi system tim usart utils wwdg)

    set(STM32_LL_REQUIRED_COMPONENTS bus cortex pwr rcc system utils)

    set(STM32_LL_PREFIX stm32f3xx_)

elseif(CPU_STM32_FAMILY STREQUAL "F4")
    set(STM32_LL_COMPONENTS	adc bus cortex crc dac dma2d dma exti fmc gpio i2c i2s iwdg
						lptim pwr rcc rng rtc spi system tim usart usb utils wwdg)

    set(STM32_LL_REQUIRED_COMPONENTS bus cortex pwr rcc system utils)

    set(STM32_LL_PREFIX stm32f4xx_)

elseif(CPU_STM32_FAMILY STREQUAL "F7")
    set(STM32_LL_COMPONENTS	adc bus cortex crc dac dma2d dma exti gpio i2c i2s iwdg
						lptim pwr rcc rng rtc spi system tim usart usb utils wwdg)

    set(STM32_LL_REQUIRED_COMPONENTS bus cortex pwr rcc system utils)

    set(STM32_LL_PREFIX stm32f7xx_)
	
elseif(CPU_STM32_FAMILY STREQUAL "H7")
    set(STM32_LL_COMPONENTS	adc bus cortex crc dac dma2d dma exti gpio i2c i2s iwdg
						lptim pwr rcc rng rtc spi system tim usart usb utils wwdg)

    set(STM32_LL_REQUIRED_COMPONENTS bus cortex pwr rcc system utils)

    set(STM32_LL_PREFIX stm32h7xx_)

elseif(CPU_STM32_FAMILY STREQUAL "L0")
    set(STM32_LL_COMPONENTS	adc bus comp cortex crc crs dac dma exti gpio i2c i2s
						iwdg lptim lpuart pwr rcc rng rtc spi system tim usart
						utils wwdg)

    set(STM32_LL_REQUIRED_COMPONENTS bus cortex pwr rcc system utils)

    set(STM32_LL_PREFIX stm32l0xx_)

elseif(CPU_STM32_FAMILY STREQUAL "L1")
    set(STM32_LL_COMPONENTS	adc comp crc dac dma exti fsmc gpio i2c opamp pwr rcc 
						rtc sdmmc spi tim usart utils)
    set(STM32_LL_REQUIRED_COMPONENTS pwr rcc utils)

    set(STM32_LL_PREFIX stm32l1xx_)

elseif(CPU_STM32_FAMILY STREQUAL "L4")
    set(STM32_LL_COMPONENTS	adc bus comp cortex crc crs dac dma2d dmamux dma exti 
						gpio i2c iwdg lptim lpuart opamp pwr rcc rng rtc spi 
						system tim usart usb utils wwdg)

    set(STM32_LL_REQUIRED_COMPONENTS bus cortex pwr rcc system utils)

	set(STM32_LL_PREFIX stm32l4xx_)
	
elseif(CPU_STM32_FAMILY STREQUAL "G4")
    set(STM32_LL_COMPONENTS	adc bus comp cordic cortex crc crs dac dma dmamux dma exti 
						fmac fmc gpio hrtim i2c iwdg lptim lpuart opamp pwr rcc rng rtc spi 
						system tim ucpd usart usb utils wwdg)

    set(STM32_LL_REQUIRED_COMPONENTS bus cortex pwr rcc system utils)

    set(STM32_LL_PREFIX stm32g4xx_)

else()
    # If the above cases don't match the chip the family is not currently supported 
    message(FATAL_ERROR "HAL not supported family")
endif()


ADD_DEFINITIONS(-DUSE_FULL_LL_DRIVER)

FOREACH(cmp ${STM32_LL_REQUIRED_COMPONENTS})
	LIST(FIND STM32LL_FIND_COMPONENTS ${cmp} STM32LL_FOUND_INDEX)
	if(${STM32LL_FOUND_INDEX} LESS 0)
		LIST(APPEND STM32LL_FIND_COMPONENTS ${cmp})
	ENDIF()
ENDFOREACH()

FOREACH(cmp ${STM32LL_FIND_COMPONENTS})
	LIST(FIND STM32_LL_COMPONENTS ${cmp} STM32LL_FOUND_INDEX)
	if(${STM32LL_FOUND_INDEX} LESS 0)
		MESSAGE(FATAL_ERROR "Unknown STM32LL component: ${cmp}. Available components: ${STM32_LL_COMPONENTS}")
	ELSE()
		LIST(FIND STM32LL_HEADER_ONLY_COMPONENTS ${cmp} HEADER_ONLY_FOUND_INDEX)
		if(${HEADER_ONLY_FOUND_INDEX} LESS 0)
			LIST(APPEND LL_SRCS ${STM32_LL_PREFIX}ll_${cmp}.c)
		ENDIF()
		LIST(APPEND LL_HEADERS ${STM32_LL_PREFIX}ll_${cmp}.h)
	ENDIF()
ENDFOREACH()

LIST(REMOVE_DUPLICATES LL_HEADERS)
LIST(REMOVE_DUPLICATES LL_SRCS)

STRING(TOLOWER ${CPU_STM32_FAMILY} STM32_FAMILY_LOWER)

FIND_PATH(STM32LL_INCLUDE_DIR ${LL_HEADERS}
	PATH_SUFFIXES include stm32${STM32_FAMILY_LOWER}
	HINTS ${STM32CUBE_BASE_DIR}/Drivers/STM32${CPU_STM32_FAMILY}xx_HAL_Driver/Inc
	CMAKE_FIND_ROOT_PATH_BOTH
)

FOREACH(LL_SRC ${LL_SRCS})
	STRING(MAKE_C_IDENTIFIER "${LL_SRC}" LL_SRC_CLEAN)
	set(LL_${LL_SRC_CLEAN}_FILE LL_SRC_FILE-NOTFOUND)
	FIND_FILE(LL_${LL_SRC_CLEAN}_FILE ${LL_SRC}
		PATH_SUFFIXES src stm32${STM32_FAMILY_LOWER}
		HINTS ${STM32CUBE_BASE_DIR}/Drivers/STM32${CPU_STM32_FAMILY}xx_HAL_Driver/Src
		CMAKE_FIND_ROOT_PATH_BOTH
	)
	LIST(APPEND STM32LL_SOURCES ${LL_${LL_SRC_CLEAN}_FILE})
ENDFOREACH()

INCLUDE(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(STM32LL DEFAULT_MSG STM32LL_INCLUDE_DIR STM32LL_SOURCES)