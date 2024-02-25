cmake_minimum_required(VERSION 3.20)

include(pico-sdk/pico_sdk_init.cmake)

set(CMAKE_EXPORT_COMPILE_COMMANDS on)
set(CMAKE_CXX_STANDARD 20)

set(PICO_BOARD pico_w)

project(n64-sniffer)

pico_sdk_init()

add_executable(${PROJECT_NAME} src/main.cpp)

target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra)

pico_generate_pio_header(${PROJECT_NAME}
                         ${CMAKE_CURRENT_LIST_DIR}/src/squarewave.pio)

target_link_libraries(${PROJECT_NAME} pico_stdlib hardware_pio)
pico_add_extra_outputs(${PROJECT_NAME})
