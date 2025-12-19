cmake_minimum_required(VERSION 3.12)
project(katetris LANGUAGES C)

enable_language(ASM)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_C_COMPILER arm-none-eabi-gcc)
set(CMAKE_ASM_COMPILER arm-none-eabi-gcc)

set(LD_DIR ${CMAKE_CURRENT_SOURCE_DIR}/chibios/os/common/startup/ARMCMx/compilers/GCC/ld)

add_compile_options(
	-Wall
	-Og
	-mcpu=cortex-m3
	-mthumb
	-mno-thumb-interwork
	-g
)
add_link_options(
	-mcpu=cortex-m3
	-mthumb
	--specs=nosys.specs 
	-fno-exceptions 
	-nostartfiles
	-flto

	-Wl,--defsym=__main_stack_size__=0x800,--defsym=__process_stack_size__=0x800
	-Wl,-L${LD_DIR},-T${LD_DIR}/STM32F103x8.ld
)

add_subdirectory(chibios)

add_executable(${PROJECT_NAME}.elf main.c display.c)
target_link_libraries(${PROJECT_NAME}.elf chibios)

add_custom_target(bin ALL arm-none-eabi-objcopy -O binary ${PROJECT_NAME}.elf ${PROJECT_NAME}.bin
		COMMAND wc -c ${PROJECT_NAME}.bin)
