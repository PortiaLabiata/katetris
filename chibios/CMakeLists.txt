add_compile_options(-Wno-pointer-to-int-cast)

option(BUILD_SHELL "Build shell" OFF)
option(BUILD_STREAMS "Build chprintf" ON)

option(BUILD_GPIO "Build GPIO" ON)
option(BUILD_SERIAL "Build serial" OFF)
option(BUILD_SPI "Build SPI" OFF)
option(BUILD_I2C "Build I2C" OFF)

set(COMMON_INCLUDES 
		${CMAKE_CURRENT_SOURCE_DIR}/..
		os/hal/include
		os/rt/include
		os/oslib/include
		os/license

		os/sb/common
		os/sb/user

		os/test/include

		os/hal/ports/STM32/STM32F1xx
		os/hal/ports/common/ARMCMx

		os/common/ports/ARM-common 
		os/common/ports/ARMv7-M 
		os/common/startup/ARMCMx/devices/STM32F1xx
		os/common/ext/ST/STM32F1xx

		os/common/portability/GCC 
		os/common/ext/ARM/CMSIS/Core/Include

		os/hal/osal/rt-nil
		os/hal/boards/STM32F103C8_MINIMAL

		test/rt/source/test
		test/oslib/source/test
	)

file(GLOB FILES_RT_SRC os/rt/src/*.c os/common/ports/ARM/*)
file(GLOB FILES_COMMON_SRC os/common/ports/ARMv7-M/*)
#file(GLOB FILES_HAL_SRC os/hal/src/*.c)
file(GLOB FILES_LLD_SRC os/hal/ports/STM32/STM32F1xx/*)
file(GLOB FILES_OS_SRC os/oslib/src/*.c)
file(GLOB LIBS_HAL_LLD os/hal/ports/STM32/LLD/*)
file(GLOB FILES_TEST_SRC os/test/src/*.c)

#--------------------------------------------------

if(BUILD_STREAMS)
	file(GLOB FILES_STREAMS_SRC os/hal/lib/streams/*.c)
	list(APPEND COMMON_INCLUDES os/hal/lib/streams)
endif()

set(LLD_LIBS_PATH os/hal/ports/STM32/LLD)
set(LIBS_LLD_ALL SYSTICKv1 TIMv1 DMAv1)
set(FILES_HAL_SRC hal_st.c hal.c hal_queues.c)

if(BUILD_SHELL)
	file(GLOB FILES_SHELL_SRC os/various/shell/*.c)
	file(GLOB RT_TEST_SRC test/rt/source/test/*.c)

	list(APPEND FILES_SHELL_SRC ${RT_TEST_SRC})
	list(APPEND COMMON_INCLUDES os/various/shell)
endif()

if(BUILD_GPIO)
	list(APPEND LIBS_LLD_ALL GPIOv1)
	list(APPEND FILES_HAL_SRC hal_pal.c)
endif()

if(BUILD_SERIAL)
	list(APPEND LIBS_LLD_ALL USARTv1)
	list(APPEND FILES_HAL_SRC hal_serial.c)
endif()

if(BUILD_I2C)
	list(APPEND LIBS_LLD_ALL I2Cv1)
	list(APPEND FILES_HAL_SRC hal_i2c.c)
endif()

#-------------------------------------------

foreach(LLD_LIB ${LIBS_LLD_ALL})
	file(GLOB ALL_SRCS ${LLD_LIBS_PATH}/${LLD_LIB}/*.c)

	list(APPEND FILES_LLD_SRC_ALL ${ALL_SRCS})
	list(APPEND COMMON_INCLUDES ${LLD_LIBS_PATH}/${LLD_LIB})
endforeach()

foreach(HAL_FILE ${FILES_HAL_SRC})
	list(APPEND FILES_HAL_ALL_SRC os/hal/src/${HAL_FILE})
endforeach()

if(BUILD_SPI)
	list(APPEND FILES_LLD_SRC_ALL ${LLD_LIBS_PATH}/SPIv1/hal_spi_v2_lld.c)
	list(APPEND COMMON_INCLUDES ${LLD_LIBS_PATH}/SPIv1)
	list(APPEND FILES_HAL_ALL_SRC os/hal/src/hal_spi.c)
endif()

add_library(
		chibios STATIC 
		# Globs and configurable
		${FILES_RT_SRC} 
		${FILES_HAL_ALL_SRC} 
		${FILES_LLD_SRC_ALL}
		${FILES_LLD_SRC} 

		${FILES_OS_SRC}
		${FILES_TEST_SRC}
		${FILES_COMMON_SRC} 

		${FILES_SHELL_SRC}
		${FILES_STREAMS_SRC}

		# Syscalls
		os/sb/various/syscalls.c 

		# Startups
		os/common/ports/ARMv7-M/compilers/GCC/chcoreasm.S
		os/common/startup/ARMCMx/compilers/GCC/crt0_v7m.S
		os/common/startup/ARMCMx/compilers/GCC/crt1.c
		os/common/startup/ARMCMx/compilers/GCC/vectors.S

		# Board
		os/hal/boards/STM32F103C8_MINIMAL/board.c 
		os/hal/ports/common/ARMCMx/nvic.c
	)
target_include_directories(chibios PUBLIC ${COMMON_INCLUDES})
target_compile_options(chibios PUBLIC ${UC_COMPILE_OPTIONS})
target_link_options(chibios PUBLIC ${UC_LINK_OPTIONS})
