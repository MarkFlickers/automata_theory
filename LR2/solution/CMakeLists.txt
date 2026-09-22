cmake_minimum_required(VERSION 3.20)

project(automata_lab2 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(LAB2_OBJECT_DIRECTORY "${CMAKE_BINARY_DIR}/object")
file(MAKE_DIRECTORY "${LAB2_OBJECT_DIRECTORY}")

# ЛР1 собирается как отдельный проект в build/lab1, lab2 запускает её исполняемый файл
include(ExternalProject)

cmake_path(SET LAB1_DEFAULT_SOURCE_DIRECTORY NORMALIZE
    "${CMAKE_CURRENT_SOURCE_DIR}/../../LR1/solution")
set(LAB1_SOURCE_DIRECTORY "${LAB1_DEFAULT_SOURCE_DIRECTORY}"
    CACHE PATH "LR1 solution directory (Quine-McCluskey minimizer)")
set(LAB1_BINARY_DIRECTORY "${CMAKE_BINARY_DIR}/lab1")
set(LAB1_EXECUTABLE "${LAB1_BINARY_DIRECTORY}/lab1${CMAKE_EXECUTABLE_SUFFIX}")

ExternalProject_Add(lab1_project
    SOURCE_DIR "${LAB1_SOURCE_DIRECTORY}"
    BINARY_DIR "${LAB1_BINARY_DIRECTORY}"
    CMAKE_ARGS
        "-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}"
        "-DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}"
        "-DCMAKE_MAKE_PROGRAM=${CMAKE_MAKE_PROGRAM}"
    BUILD_ALWAYS ON
    BUILD_BYPRODUCTS "${LAB1_EXECUTABLE}"
    INSTALL_COMMAND ""
)

add_library(lab2_core OBJECT
    src/FileIO.cpp
    src/ItemOutput.cpp
    src/ItemState.cpp
    src/Lab1Runner.cpp
    src/StructSynthesis.cpp
    src/Table.cpp
)

target_include_directories(lab2_core PUBLIC include)

add_executable(lab2 src/main.cpp)
target_link_libraries(lab2 PRIVATE lab2_core)
target_compile_definitions(lab2 PRIVATE LAB1_EXECUTABLE_PATH="${LAB1_EXECUTABLE}")
add_dependencies(lab2 lab1_project)

set_target_properties(lab2_core PROPERTIES
    COMPILE_PDB_OUTPUT_DIRECTORY "${LAB2_OBJECT_DIRECTORY}"
    PDB_OUTPUT_DIRECTORY "${LAB2_OBJECT_DIRECTORY}"
)

set_target_properties(lab2 PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
    COMPILE_PDB_OUTPUT_DIRECTORY "${LAB2_OBJECT_DIRECTORY}"
    PDB_OUTPUT_DIRECTORY "${LAB2_OBJECT_DIRECTORY}"
)

foreach(configuration DEBUG RELEASE RELWITHDEBINFO MINSIZEREL)
    set_target_properties(lab2_core PROPERTIES
        "COMPILE_PDB_OUTPUT_DIRECTORY_${configuration}" "${LAB2_OBJECT_DIRECTORY}"
        "PDB_OUTPUT_DIRECTORY_${configuration}" "${LAB2_OBJECT_DIRECTORY}"
    )
    set_target_properties(lab2 PROPERTIES
        "RUNTIME_OUTPUT_DIRECTORY_${configuration}" "${CMAKE_BINARY_DIR}"
        "COMPILE_PDB_OUTPUT_DIRECTORY_${configuration}" "${LAB2_OBJECT_DIRECTORY}"
        "PDB_OUTPUT_DIRECTORY_${configuration}" "${LAB2_OBJECT_DIRECTORY}"
    )
endforeach()

add_custom_command(TARGET lab2 POST_BUILD
    COMMAND "${CMAKE_COMMAND}" -E copy_if_different
        $<TARGET_OBJECTS:lab2_core>
        "${LAB2_OBJECT_DIRECTORY}"
    COMMAND_EXPAND_LISTS
)

if(MSVC)
    target_compile_options(lab2_core PRIVATE /W4 /permissive- /utf-8)
    target_compile_options(lab2 PRIVATE /W4 /permissive- /utf-8)
else()
    target_compile_options(lab2_core PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(lab2 PRIVATE -Wall -Wextra -Wpedantic)
endif()

configure_file(f_table.txt f_table.txt COPYONLY)
configure_file(g_table.txt g_table.txt COPYONLY)
configure_file(check_word.txt check_word.txt COPYONLY)
