cmake_minimum_required(VERSION 3.16)

project(nuklear LANGUAGES C)

add_library(nuklear STATIC
    src/nuklear_impl.c
)

target_include_directories(nuklear PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

if(MSVC)
    target_compile_options(nuklear PRIVATE /W0)
else()
    target_compile_options(nuklear PRIVATE -w)
endif()

target_include_directories(nuklear PRIVATE
    $<TARGET_PROPERTY:SDL3::SDL3,INTERFACE_INCLUDE_DIRECTORIES>
)
