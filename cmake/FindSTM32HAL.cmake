##### FROM https://raw.githubusercontent.com/ObKo/stm32-cmake

# Check for previous stm32 initialization
if(NOT DEFINED CPU_STM32_FAMILY)
    message(FATAL_ERROR "STM32 has been defined")
endif()

# Check if the STM32Cube directory exists
if(NOT EXISTS "${STM32CUBE_BASE_DIR}")
    message(FATAL_ERROR "The STM32Cube directory doesn't exist")
endif()

# Define the available and mandatory peripherals for each family and set each family file prefix
if(CPU_STM32_FAMILY STREQUAL "F0")
    set(STM32_HAL_COMPONENTS adc can cec comp cortex crc dac dma flash gpio i2c
                       i2s irda iwdg pcd pwr rcc rtc smartcard smbus
                       spi tim tsc uart usart wwdg)

    set(STM32_HAL_REQUIRED_COMPONENTS cortex pwr rcc)

    # Components that have _ex sources
    set(HAL_EX_COMPONENTS adc crc dac flash i2c pcd pwr rcc rtc smartcard spi tim uart)

    set(HAL_PREFIX stm32f0xx_)

elseif(CPU_STM32_FAMILY STREQUAL "F1")
    set(STM32_HAL_COMPONENTS adc can cec cortex crc dac dma eth flash gpio hcd i2c
                       i2s irda iwdg nand nor pccard pcd pwr rcc rtc sd smartcard
                       spi sram tim uart usart wwdg fsmc sdmmc usb)

    set(STM32_HAL_REQUIRED_COMPONENTS cortex pwr rcc)

    # Components that have _ex sources
    set(HAL_EX_COMPONENTS adc dac flash gpio pcd rcc rtc tim)

    set(HAL_PREFIX stm32f1xx_)

elseif(CPU_STM32_FAMILY STREQUAL "F2")
    set(STM32_HAL_COMPONENTS adc can cortex crc cryp dac dcmi dma eth flash
                       gpio hash hcd i2c i2s irda iwdg nand nor pccard
                       pcd pwr rcc rng rtc sd smartcard spi sram tim
                       uart usart wwdg fsmc sdmmc usb)

    set(STM32_HAL_REQUIRED_COMPONENTS cortex pwr rcc)

    # Components that have _ex sources
    set(HAL_EX_COMPONENTS adc dac dma flash pwr rcc rtc tim)

    set(HAL_PREFIX stm32f2xx_)

elseif(CPU_STM32_FAMILY STREQUAL "F3")
    set(STM32_HAL_COMPONENTS adc can cec comp cortex crc dac dma flash gpio i2c i2s
                       irda nand nor opamp pccard pcd pwr rcc rtc sdadc
                       smartcard smbus spi sram tim tsc uart usart wwdg)

    set(STM32_HAL_REQUIRED_COMPONENTS cortex pwr rcc)

    set(HAL_EX_COMPONENTS adc crc dac flash i2c i2s opamp pcd pwr
                          rcc rtc smartcard spi tim uart)

    set(HAL_PREFIX stm32f3xx_)

elseif(CPU_STM32_FAMILY STREQUAL "F4")
    set(STM32_HAL_COMPONENTS adc can cec cortex crc cryp dac dcmi dma dma2d eth flash
                       flash_ramfunc fmpi2c gpio hash hcd i2c i2s irda iwdg ltdc
                       nand nor pccard pcd pwr qspi rcc rng rtc sai sd sdram
                       smartcard spdifrx spi sram tim uart usart wwdg)

    set(STM32_HAL_REQUIRED_COMPONENTS cortex pwr rcc)

    # Components that have _ex sources
    set(HAL_EX_COMPONENTS adc cryp dac dcmi dma flash fmpi2c hash i2c i2s pcd
                          pwr rcc rtc sai tim)

    set(HAL_PREFIX stm32f4xx_)

elseif(CPU_STM32_FAMILY STREQUAL "F7")
    set(STM32_HAL_COMPONENTS adc can cec cortex crc cryp dac dcmi dma dma2d eth flash
                       gpio hash hcd i2c i2s irda iwdg lptim ltdc nand nor pcd
                       pwr qspi rcc rng rtc sai sd sdram smartcard spdifrx spi
                       sram tim uart usart wwdg fmc sdmmc usb)

    set(STM32_HAL_REQUIRED_COMPONENTS cortex pwr rcc)

    # Components that have _ex sources
    set(HAL_EX_COMPONENTS adc crc cryp dac dcmi dma flash hash i2c pcd
                          pwr rcc rtc sai tim)

    set(HAL_PREFIX stm32f7xx_)

elseif(CPU_STM32_FAMILY STREQUAL "H7")
    set(STM32_HAL_COMPONENTS adc can cec cortex crc cryp dac dcmi dma dma2d eth flash
                       gpio hash hcd i2c i2s irda iwdg lptim ltdc nand nor pcd
                       pwr qspi rcc rng rtc sai sd sdram smartcard spdifrx spi
                       sram tim uart usart wwdg fmc sdmmc usb)

    set(STM32_HAL_REQUIRED_COMPONENTS cortex pwr rcc)

    # Components that have _ex sources
    set(HAL_EX_COMPONENTS adc crc cryp dac dcmi dma flash hash i2c pcd
                          pwr rcc rtc sai tim)

    set(HAL_PREFIX stm32h7xx_)

elseif(CPU_STM32_FAMILY STREQUAL "L0")
    set(STM32_HAL_COMPONENTS adc comp cortex crc crs cryp dac dma exti firewall flash gpio i2c
                       i2s irda iwdg lcd lptim lpuart pcd pwr rcc rng rtc smartcard
                       smbus spi tim tsc uart usart utils wwdg)

    set(STM32_HAL_REQUIRED_COMPONENTS cortex pwr rcc)

    # Components that have _ex sources
    set(HAL_EX_COMPONENTS adc comp crc cryp dac flash i2c pcd pwr rcc rtc smartcard tim uart usart)

    set(HAL_PREFIX stm32l0xx_)
elseif(CPU_STM32_FAMILY STREQUAL "L1")
    set(STM32_HAL_COMPONENTS adc comp cortex crc cryp dac dma flash flash_ramfunc
					   gpio i2c i2s irda iwdg lcd nor opamp pcd pwr rcc rtc
					   sd smartcard spi sram tim uart usart wwdg)
    set(STM32_HAL_REQUIRED_COMPONENTS cortex pwr)
    
    # Components that have _ex sources
    set(HAL_EX_COMPONENTS adc cryp dac flash opamp pcd pwr rcc rtc spi tim)

    set(HAL_PREFIX stm32l1xx_)
elseif(CPU_STM32_FAMILY STREQUAL "L4")
    set(STM32_HAL_COMPONENTS adc can comp cortex crc cryp dac dcmi dfsdm dma dma2d dsi 
                       firewall flash flash_ramfunc gfxmmu gpio hash hcd i2c irda iwdg
                       lcd lptim ltdc nand nor opamp ospi pcd pwr qspi rcc rng rtc sai
                       sd smartcard smbus spi sram swpmi tim tsc uart usart wwdg)

    set(STM32_HAL_REQUIRED_COMPONENTS cortex pwr rcc)

    # Components that have _ex sources
    set(HAL_EX_COMPONENTS adc crc cryp dac dfsdm dma flash hash i2c ltdc 
                          opamp pcd pwr rcc rtc sai sd smartcard spi tim uart usart)
                          
    set(HAL_PREFIX stm32l4xx_)

else()
    # If the above cases don't match the chip the family is not currently supported 
    message(FATAL_ERROR "HAL not supported family")
endif()


# Set common headers for all families
set(STM32_HAL_HEADERS
	${HAL_PREFIX}hal.h
	${HAL_PREFIX}hal_def.h
)

# Set common sources for all families
set(STM32_HAL_SRCS
	${HAL_PREFIX}hal.c
)

if(NOT STM32HAL_FIND_COMPONENTS)
    set(STM32HAL_FIND_COMPONENTS ${STM32_HAL_COMPONENTS})
    message(STATUS "No STM32HAL components selected, using all: ${STM32HAL_FIND_COMPONENTS}")
endif()

foreach(cmp ${STM32_HAL_REQUIRED_COMPONENTS})
    list(FIND STM32HAL_FIND_COMPONENTS ${cmp} STM32HAL_FOUND_INDEX)
    if(${STM32HAL_FOUND_INDEX} LESS 0)
        list(APPEND STM32HAL_FIND_COMPONENTS ${cmp})
    endif()
endforeach()

foreach(cmp ${STM32HAL_FIND_COMPONENTS})
    list(FIND STM32_HAL_COMPONENTS ${cmp} STM32HAL_FOUND_INDEX)
    if(${STM32HAL_FOUND_INDEX} LESS 0)
        message(FATAL_ERROR "Unknown STM32HAL component: ${cmp}. Available components: ${STM32_HAL_COMPONENTS}")
	else()
        list(APPEND STM32_HAL_HEADERS ${HAL_PREFIX}hal_${cmp}.h)
        list(APPEND STM32_HAL_SRCS ${HAL_PREFIX}hal_${cmp}.c)
    endif()
    list(FIND HAL_EX_COMPONENTS ${cmp} STM32HAL_FOUND_INDEX)
    if(NOT (${STM32HAL_FOUND_INDEX} LESS 0))
        list(APPEND STM32_HAL_HEADERS ${HAL_PREFIX}hal_${cmp}_ex.h)
        list(APPEND STM32_HAL_SRCS ${HAL_PREFIX}hal_${cmp}_ex.c)
    endif()
endforeach()

list(REMOVE_DUPLICATES STM32_HAL_HEADERS)
list(REMOVE_DUPLICATES STM32_HAL_SRCS)

string(TOLOWER ${CPU_STM32_FAMILY} CPU_STM32_FAMILY_LOWER)

FIND_PATH(STM32HAL_INCLUDE_DIR ${STM32_HAL_HEADERS}
    PATH_SUFFIXES include stm32${CPU_STM32_FAMILY_LOWER}
    HINTS ${STM32CUBE_BASE_DIR}/Drivers/STM32${CPU_STM32_FAMILY}xx_HAL_Driver/Inc
    CMAKE_FIND_ROOT_PATH_BOTH
)

foreach(HAL_SRC ${STM32_HAL_SRCS})
    string(MAKE_C_IDENTIFIER "${HAL_SRC}" HAL_SRC_CLEAN)
    set(HAL_${HAL_SRC_CLEAN}_FILE HAL_SRC_FILE-NOTFOUND)
    FIND_FILE(HAL_${HAL_SRC_CLEAN}_FILE ${HAL_SRC}
        PATH_SUFFIXES src stm32${CPU_STM32_FAMILY_LOWER}
        HINTS ${STM32CUBE_BASE_DIR}/Drivers/STM32${CPU_STM32_FAMILY}xx_HAL_Driver/Src
        CMAKE_FIND_ROOT_PATH_BOTH
    )
    list(APPEND STM32HAL_SOURCES ${HAL_${HAL_SRC_CLEAN}_FILE})
endforeach()

INCLUDE(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(STM32HAL DEFAULT_MSG STM32HAL_INCLUDE_DIR STM32HAL_SOURCES)
