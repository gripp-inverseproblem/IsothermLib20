#==============================================================================
# Name        : CMakeLists.txt
# Authors     : Joao Flavio Vasconcellos
# Version     : 2.0.0
# Description : Build configuration and dependency discovery.
#
# Copyright   : Copyright (C) 2026 Joao Flavio Vasconcellos
# Licence     : MIT License (see LICENSE.txt)
# SPDX-License-Identifier: MIT
#==============================================================================

include(AutoProgrammes)
find_package(GTest CONFIG QUIET)
if(NOT TARGET GTest::gtest_main)
    # The CMake-provided FindGTest module also supports older distributions.
    find_package(GTest QUIET)
endif()
if(TARGET GTest::gtest_main)
    include(GoogleTest)
    message(STATUS "GoogleTest found: per-model suites enabled")
endif()
isotherm_add_test_programmes("${CMAKE_CURRENT_SOURCE_DIR}")
