cmake_minimum_required(VERSION 3.25)

set(PROJECT_NAME emnanovg)
set(CMAKE_C_STANDARD 11)

project(emnanovg)
option(BUILD_EXAMPLE "Build Example" ON)

add_library(nanovg STATIC "${CMAKE_CURRENT_SOURCE_DIR}/src/nanovg.c")
target_include_directories(nanovg PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src")

if(BUILD_EXAMPLE)
    add_executable(emnanovg "${CMAKE_CURRENT_SOURCE_DIR}/example/main.cpp")
    if("${CMAKE_C_COMPILER}" MATCHES "emcc$")
        set_target_properties(
            emnanovg
            PROPERTIES
                LINK_FLAGS
                "-sMODULARIZE=1 -sNO_EXIT_RUNTIME=1 -sEXPORT_NAME='NanoVGModule' -sMIN_WEBGL_VERSION=2  -s EXIT_RUNTIME=1 --preload-file \"${CMAKE_CURRENT_SOURCE_DIR}/example/Roboto-Regular.ttf@/Roboto-Regular.ttf\""
        )
        add_definitions(-DNANOVG_GLES3_IMPLEMENTATION)
        target_link_libraries(emnanovg PRIVATE nanovg embind)

    else()
        message(FATAL_ERROR "emcc not found")
    endif()

    install(
        FILES "${CMAKE_CURRENT_BINARY_DIR}/emnanovg.js" "${CMAKE_CURRENT_BINARY_DIR}/emnanovg.wasm"
              "${CMAKE_CURRENT_BINARY_DIR}/emnanovg.data"
        DESTINATION "${CMAKE_CURRENT_SOURCE_DIR}/example")

endif()
