add_compile_options(-Wno-pointer-to-int-cast)

set(COMMON_INCLUDES 
		${CMAKE_CURRENT_SOURCE_DIR}/..
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
		os/common/ext/ST/STM32F1xx
		os/common/ext/ARM/CMSIS/Core/Include

		os/hal/osal/rt-nil
		os/hal/boards/STM32F103C8_MINIMAL
	)


file(GLOB FILES_RT_SRC os/rt/src/*.c os/common/ports/ARM/*)
file(GLOB FILES_COMMON os/common/ports/ARMv7-M/*)
file(GLOB FILES_HAL_SRC os/hal/src/*.c)
file(GLOB FILES_HAL_LLD os/hal/ports/STM32/STM32F1xx/*)
file(GLOB FILES_OS_SRC os/oslib/src/*.c)
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

add_library(
		chibios STATIC ${FILES_RT_SRC} ${FILES_COMMON} os/sb/various/syscalls.c 

		os/common/ports/ARMv7-M/compilers/GCC/chcoreasm.S
		os/common/startup/ARMCMx/compilers/GCC/crt0_v7m.S
		os/common/startup/ARMCMx/compilers/GCC/crt1.c
		os/common/startup/ARMCMx/compilers/GCC/vectors.S

		${FILES_HAL_SRC} ${FILES_HAL_LLD} ${HAL_LLD_ALL_FILES}
		os/hal/boards/STM32F103C8_MINIMAL/board.c os/hal/ports/common/ARMCMx/nvic.c
		${FILES_OS_SRC})
target_include_directories(chibios PUBLIC os/rt/include ${COMMON_INCLUDES}
	os/hal/include ${COMMON_INCLUDES} ${HAL_LLD_ALL_INCLUDES}
	os/oslib/include ${COMMON_INCLUDES})
