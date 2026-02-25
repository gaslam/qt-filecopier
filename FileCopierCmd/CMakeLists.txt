cmake_minimum_required(VERSION 3.19)
project(FileCopierCmd LANGUAGES CXX)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Test)

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

target_compile_definitions(FileCopierCmd PRIVATE SOURCE_FILE="C:/Users/gaspa/Music/Find Your Dreams.flac")
target_compile_definitions(FileCopierCmd PRIVATE DESTINATION_DIR="C:/Users/gaspa/Music/test")

include(GNUInstallDirs)

install(TARGETS FileCopierCmd
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
