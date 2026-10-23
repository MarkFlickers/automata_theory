cmake_minimum_required(VERSION 3.20)

project(automata_lab3 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(LAB3_OBJECT_DIRECTORY "${CMAKE_BINARY_DIR}/object")
file(MAKE_DIRECTORY "${LAB3_OBJECT_DIRECTORY}")

add_library(lab3_core OBJECT
    src/CompTable.cpp
    src/FCompTable.cpp
    src/FileIO.cpp
    src/ItemOutput.cpp
    src/ItemState.cpp
    src/MinAutomaton.cpp
    src/PreCompTable.cpp
    src/Printer.cpp
    src/SCouple.cpp
    src/SMaxComp.cpp
    src/Table.cpp
)

target_include_directories(lab3_core PUBLIC include)

add_executable(lab3 src/main.cpp)
target_link_libraries(lab3 PRIVATE lab3_core)

set_target_properties(lab3_core PROPERTIES
    COMPILE_PDB_OUTPUT_DIRECTORY "${LAB3_OBJECT_DIRECTORY}"
    PDB_OUTPUT_DIRECTORY "${LAB3_OBJECT_DIRECTORY}"
)

set_target_properties(lab3 PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
    COMPILE_PDB_OUTPUT_DIRECTORY "${LAB3_OBJECT_DIRECTORY}"
    PDB_OUTPUT_DIRECTORY "${LAB3_OBJECT_DIRECTORY}"
)

foreach(configuration DEBUG RELEASE RELWITHDEBINFO MINSIZEREL)
    set_target_properties(lab3_core PROPERTIES
        "COMPILE_PDB_OUTPUT_DIRECTORY_${configuration}" "${LAB3_OBJECT_DIRECTORY}"
        "PDB_OUTPUT_DIRECTORY_${configuration}" "${LAB3_OBJECT_DIRECTORY}"
    )
    set_target_properties(lab3 PROPERTIES
        "RUNTIME_OUTPUT_DIRECTORY_${configuration}" "${CMAKE_BINARY_DIR}"
        "COMPILE_PDB_OUTPUT_DIRECTORY_${configuration}" "${LAB3_OBJECT_DIRECTORY}"
        "PDB_OUTPUT_DIRECTORY_${configuration}" "${LAB3_OBJECT_DIRECTORY}"
    )
endforeach()

add_custom_command(TARGET lab3 POST_BUILD
    COMMAND "${CMAKE_COMMAND}" -E copy_if_different
        $<TARGET_OBJECTS:lab3_core>
        "${LAB3_OBJECT_DIRECTORY}"
    COMMAND_EXPAND_LISTS
)

if(MSVC)
    target_compile_options(lab3_core PRIVATE /W4 /permissive- /utf-8)
    target_compile_options(lab3 PRIVATE /W4 /permissive- /utf-8)
else()
    target_compile_options(lab3_core PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(lab3 PRIVATE -Wall -Wextra -Wpedantic)
endif()

configure_file(f_table.txt f_table.txt COPYONLY)
configure_file(g_table.txt g_table.txt COPYONLY)
