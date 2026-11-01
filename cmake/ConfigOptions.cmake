#==============================================================================
# Name        : ConfigOptions.cmake
# Authors     : Joao Flavio Vasconcellos
# Version     : 2.0.0
# Description : Isotherm++ 2.0 project configuration.
#
# Copyright   : Copyright (C) 2026 Joao Flavio Vasconcellos
# Licence     : MIT License (see LICENSE.txt)
# SPDX-License-Identifier: MIT
#==============================================================================

include_guard(GLOBAL)
option(ISOTHERM_BUILD_EXAMPLES "Build automatically discovered C++ examples" ON)
option(ISOTHERM_BUILD_TESTS "Build tests and CTest smoke tests" ON)
option(ISOTHERM_REQUIRE_GTEST "Fail configuration if GoogleTest is not installed" OFF)
option(ISOTHERM_BUILD_DOCS "Configure Sphinx HTML and PDF targets" ON)
option(ISOTHERM_ENABLE_LTO "Enable link-time optimisation on owned compiled targets" OFF)
option(ISOTHERM_ENABLE_NATIVE_ARCH "Enable build-machine-specific code generation" OFF)
if(NOT DEFINED CMAKE_CXX_STANDARD)
    set(CMAKE_CXX_STANDARD 20)
endif()
if(CMAKE_CXX_STANDARD LESS 20)
    message(FATAL_ERROR "Isotherm++ requires C++20 or later")
endif()
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# C++20 modules are not used; avoid requiring clang-scan-deps on toolchains.
set(CMAKE_CXX_SCAN_FOR_MODULES OFF)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
if(NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build configuration" FORCE)
endif()
