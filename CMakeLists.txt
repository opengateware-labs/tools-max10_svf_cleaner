cmake_minimum_required(VERSION 3.0)

if (${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
    message(FATAL_ERROR [[In-source builds are not allowed]])
endif ()

# Project Information
project(MAX10-SVF-Cleaner
        VERSION "0.1.0"
        DESCRIPTION "MAX10 SVF Cleaner is a tool that solves a problem related to Intel® MAX® 10 devices."
        HOMEPAGE_URL "https://github.com/opengateware-labs/tools-max10_svf_cleaner"
        LANGUAGES CXX)

set(BIN_TARGET "max10_svf_cleaner")

set(APP_AUTHOR "OpenGateware")
set(APP_TITLE "MAX10 SVF Cleaner")
set(APP_COPYRIGHT "(c) 2023, OpenGateware authors and contributors")
set(APP_DESCRIPTION ${PROJECT_DESCRIPTION})
set(APP_VERSION ${PROJECT_VERSION})
set(DOMAIN ${PROJECT_NAME})

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # for clang-tidy

configure_file(src/include/version.h.in version.h)

# Compiler info
set(USE_GCC TRUE)
set(OPT_DEF_GCC_ATOMICS ON)

if (RELEASE AND CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    target_link_libraries(${BIN_TARGET} PUBLIC -static-libgcc -static-libstdc++)
endif ()

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -s")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lstdc++ -O3 -Ofast")

add_compile_definitions(_FILE_OFFSET_BITS=64)
add_compile_definitions(_LARGEFILE64_SOURCE)

if (APPLE)
    set(CMAKE_INSTALL_RPATH "@executable_path")
else ()
    set(CMAKE_INSTALL_RPATH "$ORIGIN")
endif ()
set(CMAKE_BUILD_WITH_INSTALL_RPATH ON)

if (WIN32)
    add_compile_definitions(__WIN32__)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
    set(CMAKE_EXE_LINKER_FLAGS "-static")
    if (RELEASE)
        set(CMAKE_CXX_STANDARD_LIBRARIES "-static-libgcc -static-libstdc++ ${CMAKE_CXX_STANDARD_LIBRARIES}")
        target_link_libraries(${BIN_TARGET} PUBLIC -static-libgcc -static-libstdc++)
    endif ()
else ()
    if (UNIX AND NOT APPLE)
        set(CMAKE_EXE_LINKER_FLAGS "-static")
    endif ()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -Wno-format-truncation -Wno-psabi -Wno-unused-parameter -Wno-unused-function -Wno-implicit-fallthrough -Wno-unused-variable")
endif ()

# Main Application
add_executable(${BIN_TARGET} src/main.cpp)

# generate version.h
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/src/include/version.h.in" "${CMAKE_CURRENT_BINARY_DIR}/version.h" @ONLY)

# Files to be installed
install(
        TARGETS ${BIN_TARGET}
        COMPONENT binaries
        RUNTIME DESTINATION "bin/"
)

# Create Package Files
set(CPACK_PACKAGE_VENDOR "OpenGateware")
set(CPACK_PACKAGE_NAME ${PROJECT_NAME})
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_STRIP_FILES YES)
SET(CPACK_OUTPUT_FILE_PREFIX releases)

if (WIN32 OR MSVC OR MSYS OR MINGW)
    set(CPACK_GENERATOR "ZIP")
elseif (APPLE)
    set(CPACK_GENERATOR "ZIP" "TGZ")
else ()
    set(CPACK_DEBIAN_PACKAGE_MAINTAINER "packages@raetro.org")
    set(CPACK_GENERATOR "ZIP" "TGZ" "DEB")
    set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS YES)
endif ()

include(CPack)
