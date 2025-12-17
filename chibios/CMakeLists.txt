add_compile_options(-Wno-pointer-to-int-cast)

set(COMMON_INCLUDES 
		os/hal/include
		os/rt/include
		os/oslib/include
		os/license

		os/sb/common
		os/sb/user

		os/hal/ports/STM32/STM32F1xx
		os/hal/ports/common/ARMCMx
		os/common/ports/ARM-common 
		os/common/ports/ARMv7-M 
		os/common/portability/GCC 
		os/common/startup/ARMCMx/devices/STM32F1xx
		os/hal/boards/STM32F103C8_MINIMAL
		os/common/ext/ST/STM32F1xx
		os/common/ext/ARM/CMSIS/Core/Include
		os/hal/osal/rt-nil)


file(GLOB FILES_RT_SRC os/rt/src/*.c os/common/ports/ARM/*)
file(GLOB FILES_COMMON os/common/ports/ARMv7-M/*)
add_library(chibios_rt OBJECT ${FILES_RT_SRC} ${FILES_COMMON} os/sb/various/syscalls.c 
		os/common/startup/ARMCMx/compilers/GCC/crt0_v7m.S
		os/common/ports/ARMv7-M/compilers/GCC/chcoreasm.S)
target_include_directories(chibios_rt PUBLIC os/rt/include ${COMMON_INCLUDES})
target_include_directories(chibios_rt PUBLIC os/license)
target_include_directories(chibios_rt PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)

#-------------------------------------------------------------------------------
file(GLOB FILES_HAL_SRC os/hal/src/*.c)
file(GLOB FILES_HAL_LLD os/hal/ports/STM32/STM32F1xx/*)

file(GLOB LIBS_HAL_LLD os/hal/ports/STM32/LLD/*)
foreach(lib ${LIBS_HAL_LLD})
	file(GLOB files ${lib}/*.c)

	if(files)
		get_filename_component(libname ${lib} NAME)
		if(NOT libname MATCHES v2 AND NOT libname MATCHES v3)
			list(APPEND HAL_LLD_ALL_FILES ${files})
			list(APPEND HAL_LLD_ALL_INCLUDES ${lib})
		endif()
	endif()
endforeach()

add_library(chibios_hal OBJECT ${FILES_HAL_SRC} ${FILES_HAL_LLD} ${HAL_LLD_ALL_FILES}
	os/hal/boards/STM32F103C8_MINIMAL/board.c os/hal/ports/common/ARMCMx/nvic.c)
target_include_directories(chibios_hal PUBLIC os/hal/include ${COMMON_INCLUDES} ${HAL_LLD_ALL_INCLUDES})
target_include_directories(chibios_hal PUBLIC os/license)
target_include_directories(chibios_hal PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)

#-------------------------------------------------------------------------------

file(GLOB FILES_OS_SRC os/oslib/src/*.c)
add_library(chibios_oslib OBJECT ${FILES_OS_SRC})
target_include_directories(chibios_oslib PUBLIC os/oslib/include ${COMMON_INCLUDES})
target_include_directories(chibios_oslib PUBLIC os/license)
target_include_directories(chibios_oslib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)
