cmake_minimum_required(VERSION 3.20)

include(pico-sdk/pico_sdk_init.cmake)

set(CMAKE_EXPORT_COMPILE_COMMANDS on)
set(CMAKE_CXX_STANDARD 20)

set(PICO_BOARD pico_w)

project(n64-sniffer)

pico_sdk_init()

add_executable(${PROJECT_NAME} src/main.cpp)

target_link_libraries(
  ${PROJECT_NAME}
  pico_stdlib
  pico_cyw43_arch_none)
pico_add_extra_outputs(${PROJECT_NAME})
