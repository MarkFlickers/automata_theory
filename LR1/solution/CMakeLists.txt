cmake_minimum_required(VERSION 3.20)

project(automata_lab1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(LAB1_OBJECT_DIRECTORY "${CMAKE_BINARY_DIR}/object")
file(MAKE_DIRECTORY "${LAB1_OBJECT_DIRECTORY}")

add_library(lab1_core OBJECT
    src/DNF.cpp
    src/FileIO.cpp
    src/Impl.cpp
)

target_include_directories(lab1_core PUBLIC include)

add_executable(lab1 src/main.cpp)
target_link_libraries(lab1 PRIVATE lab1_core)

set_target_properties(lab1_core PROPERTIES
    COMPILE_PDB_OUTPUT_DIRECTORY "${LAB1_OBJECT_DIRECTORY}"
    PDB_OUTPUT_DIRECTORY "${LAB1_OBJECT_DIRECTORY}"
)

set_target_properties(lab1 PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
    COMPILE_PDB_OUTPUT_DIRECTORY "${LAB1_OBJECT_DIRECTORY}"
    PDB_OUTPUT_DIRECTORY "${LAB1_OBJECT_DIRECTORY}"
)

foreach(configuration DEBUG RELEASE RELWITHDEBINFO MINSIZEREL)
    set_target_properties(lab1_core PROPERTIES
        "COMPILE_PDB_OUTPUT_DIRECTORY_${configuration}" "${LAB1_OBJECT_DIRECTORY}"
        "PDB_OUTPUT_DIRECTORY_${configuration}" "${LAB1_OBJECT_DIRECTORY}"
    )
    set_target_properties(lab1 PROPERTIES
        "RUNTIME_OUTPUT_DIRECTORY_${configuration}" "${CMAKE_BINARY_DIR}"
        "COMPILE_PDB_OUTPUT_DIRECTORY_${configuration}" "${LAB1_OBJECT_DIRECTORY}"
        "PDB_OUTPUT_DIRECTORY_${configuration}" "${LAB1_OBJECT_DIRECTORY}"
    )
endforeach()

add_custom_command(TARGET lab1 POST_BUILD
    COMMAND "${CMAKE_COMMAND}" -E copy_if_different
        $<TARGET_OBJECTS:lab1_core>
        "${LAB1_OBJECT_DIRECTORY}"
    COMMAND_EXPAND_LISTS
)

if(MSVC)
    target_compile_options(lab1_core PRIVATE /W4 /permissive-)
    target_compile_options(lab1 PRIVATE /W4 /permissive-)
else()
    target_compile_options(lab1_core PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(lab1 PRIVATE -Wall -Wextra -Wpedantic)
endif()

configure_file(scale.txt scale.txt COPYONLY)
