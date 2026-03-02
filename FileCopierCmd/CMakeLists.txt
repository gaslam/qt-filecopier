cmake_minimum_required(VERSION 3.19)
project(FileCopierCmd LANGUAGES CXX)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Test)
find_package(Qt6 REQUIRED COMPONENTS Concurrent)

qt_standard_project_setup()

qt_add_executable(FileCopierCmd
    main.cpp
    filecopier.h filecopier.cpp
    CopierBenchmarks.h
)

target_link_libraries(FileCopierCmd
    PRIVATE
        Qt::Core Qt::Test
)
target_link_libraries(FileCopierCmd PRIVATE Qt6::Concurrent)

set(TEST_FILE "resource.flac")

add_custom_command(
        TARGET FileCopierCmd POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
                "${CMAKE_SOURCE_DIR}/${TEST_FILE}"
                "${CMAKE_CURRENT_BINARY_DIR}/${TEST_FILE}")

target_compile_definitions(FileCopierCmd PRIVATE SOURCE_FILE="${TEST_FILE}")
target_compile_definitions(FileCopierCmd PRIVATE DESTINATION_DIR="./")

include(GNUInstallDirs)

install(TARGETS FileCopierCmd
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
