cmake_minimum_required(VERSION 3.21)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_OSX_ARCHITECTURES "x86_64")
set(CMAKE_CXX_VISIBILITY_PRESET hidden)

project(DirectionIndicator VERSION 1.0.0)

add_library(${PROJECT_NAME} SHARED
    src/main.cpp
)

if (NOT DEFINED ENV{GEODE_SDK})
    message(FATAL_ERROR
        "No se encontró el Geode SDK.\n"
        "Definí la variable de entorno GEODE_SDK apuntando a tu instalación del SDK.\n"
        "Ejemplo (Windows): set GEODE_SDK=C:/Geode/sdk"
    )
else()
    message(STATUS "Geode SDK encontrado en: $ENV{GEODE_SDK}")
endif()

set(GEODE_SDK $ENV{GEODE_SDK})
add_subdirectory(${GEODE_SDK} ${CMAKE_CURRENT_BINARY_DIR}/geode)

target_link_libraries(${PROJECT_NAME} geode-sdk)
create_geode_file(${PROJECT_NAME})
