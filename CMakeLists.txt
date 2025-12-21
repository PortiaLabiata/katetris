cmake_minimum_required(VERSION 3.12)
project(katetris LANGUAGES C)

option(SITL "Build SITL" OFF)
if(SITL)
	set(CMAKE_C_COMPILER gcc)
else()
	set(CMAKE_C_COMPILER arm-none-eabi-gcc)
endif()
configure_file(env.h.in env.h)
include_directories(${PROJECT_BINARY_DIR})

if(NOT SITL)

enable_language(ASM)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_ASM_COMPILER ${CMAKE_C_COMPILER})

set(LD_DIR ${CMAKE_CURRENT_SOURCE_DIR}/chibios/os/common/startup/ARMCMx/compilers/GCC/ld)
set(UC_COMPILE_OPTIONS -Wall -Og -mcpu=cortex-m3 -mthumb -mno-thumb-interwork -g)
set(UC_LINK_OPTIONS -mcpu=cortex-m3 -mthumb --specs=nosys.specs -fno-exceptions 
	-nostartfiles -flto
	-Wl,--defsym=__main_stack_size__=0x400
	-Wl,--defsym=__process_stack_size__=0x400
	-Wl,-L${LD_DIR},-T${LD_DIR}/STM32F103x8.ld
)

add_subdirectory(chibios)

add_executable(${PROJECT_NAME}.elf main.c display.c tetris.c common.c)
target_compile_options(${PROJECT_NAME}.elf PUBLIC ${UC_COMPILE_OPTIONS})
target_link_options(${PROJECT_NAME}.elf PUBLIC ${UC_LINK_OPTIONS})
target_link_libraries(${PROJECT_NAME}.elf chibios)

add_custom_target(bin ALL arm-none-eabi-objcopy -O binary ${PROJECT_NAME}.elf ${PROJECT_NAME}.bin
		COMMAND arm-none-eabi-size ${PROJECT_NAME}.elf)

else()

find_package(SDL2 REQUIRED)
add_executable(sitl main.c display.c tetris.c common.c)
target_include_directories(sitl PUBLIC ${SDL_INCLUDE_DIRS})
target_link_libraries(sitl PUBLIC SDL2::SDL2 ncurses)
target_compile_options(sitl PUBLIC -g)

endif()
