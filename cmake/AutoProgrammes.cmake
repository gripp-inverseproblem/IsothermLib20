#==============================================================================
# Name        : AutoProgrammes.cmake
# Authors     : Joao Flavio Vasconcellos
# Version     : 2.0.0
# Description : Discover model examples and tests without manual CMake edits.
#
# Copyright   : Copyright (C) 2026 Joao Flavio Vasconcellos
# Licence     : MIT License (see LICENSE)
# SPDX-License-Identifier: MIT
#==============================================================================

include_guard(GLOBAL)

# New model programmes require only a new folder, NOT CMakeLists changes.
# Layout: Examples/<ParameterGroup>/<Model>/main.cpp + optional additional .cpp.
# Model tests: IsothermTest/<ParameterGroup>/<Model>/test_*.cpp.
# Core tests: IsothermTest/Core/<Area>/test_*.cpp.
# Independent smoke tests: IsothermTest/Core/<Area>/smoke_main.cpp.
# CONFIGURE_DEPENDS causes CMake to reconfigure after files are added/removed.
function(isotherm_add_example_programmes base)
    file(GLOB_RECURSE _mains CONFIGURE_DEPENDS "${base}/*/main.cpp")
    list(SORT _mains)
    foreach(_main IN LISTS _mains)
        file(RELATIVE_PATH _rel "${base}" "${_main}")
        if(NOT _rel MATCHES "^([A-Za-z]+Parameters?)/([^/]+)/main[.]cpp$")
            message(FATAL_ERROR "Example must be <ParameterGroup>/<Model>/main.cpp: ${_rel}")
        endif()
        set(_group "${CMAKE_MATCH_1}")
        set(_model "${CMAKE_MATCH_2}")
        set(_target "example_${_group}_${_model}")
        string(MAKE_C_IDENTIFIER "${_target}" _target)
        get_filename_component(_folder "${_main}" DIRECTORY)
        file(GLOB _sources CONFIGURE_DEPENDS "${_folder}/*.cpp")
        add_executable("${_target}" ${_sources})
        target_link_libraries("${_target}" PRIVATE Isotherm::Isotherm)
        isotherm_apply_settings("${_target}")
        set_target_properties("${_target}" PROPERTIES FOLDER "Examples/${_group}")
        message(STATUS "Auto-example: ${_group}/${_model} -> ${_target}")
    endforeach()
endfunction()

function(isotherm_add_test_programmes base)
    file(GLOB_RECURSE _tests CONFIGURE_DEPENDS "${base}/test_*.cpp")
    file(GLOB_RECURSE _smokes CONFIGURE_DEPENDS "${base}/smoke_main.cpp")
    list(SORT _tests)
    list(SORT _smokes)
    # A model's tests form one executable, regardless of the number of test_*.cpp files.
    set(_seen_dirs "")
    foreach(_test IN LISTS _tests)
        get_filename_component(_folder "${_test}" DIRECTORY)
        if(_folder IN_LIST _seen_dirs)
            continue()
        endif()
        list(APPEND _seen_dirs "${_folder}")
        file(RELATIVE_PATH _rel "${base}" "${_folder}")
        if(NOT _rel MATCHES "^([A-Za-z]+Parameters?|Core)/([^/]+)$")
            message(FATAL_ERROR "Test must be <ParameterGroup>/<Model>/test_*.cpp or Core/<Area>: ${_rel}")
        endif()
        if(NOT TARGET GTest::gtest_main)
            continue()
        endif()
        set(_target "gtest_${CMAKE_MATCH_1}_${CMAKE_MATCH_2}")
        string(MAKE_C_IDENTIFIER "${_target}" _target)
        file(GLOB _sources CONFIGURE_DEPENDS "${_folder}/test_*.cpp")
        add_executable("${_target}" ${_sources})
        target_link_libraries("${_target}" PRIVATE Isotherm::Isotherm GTest::gtest_main)
        isotherm_apply_settings("${_target}")
        gtest_discover_tests("${_target}" DISCOVERY_TIMEOUT 30)
        set_property(GLOBAL APPEND PROPERTY ISOTHERM_TEST_EXECUTABLE_TARGETS "${_target}")
        message(STATUS "Auto-GoogleTest: ${_rel} -> ${_target}")
    endforeach()
    foreach(_smoke IN LISTS _smokes)
        get_filename_component(_folder "${_smoke}" DIRECTORY)
        file(RELATIVE_PATH _rel "${base}" "${_folder}")
        if(NOT _rel MATCHES "^Core/([^/]+)$")
            message(FATAL_ERROR "Smoke test must be Core/<Area>/smoke_main.cpp: ${_rel}")
        endif()
        set(_target "smoke_${CMAKE_MATCH_1}")
        string(MAKE_C_IDENTIFIER "${_target}" _target)
        add_executable("${_target}" "${_smoke}")
        target_link_libraries("${_target}" PRIVATE Isotherm::Isotherm)
        isotherm_apply_settings("${_target}")
        add_test(NAME "${_target}" COMMAND "${_target}")
        set_property(GLOBAL APPEND PROPERTY ISOTHERM_TEST_EXECUTABLE_TARGETS "${_target}")
        message(STATUS "Auto-CTest: ${_rel} -> ${_target}")
    endforeach()
    list(LENGTH _tests _test_files)
    if(_test_files AND NOT TARGET GTest::gtest_main)
        if(ISOTHERM_REQUIRE_GTEST)
            message(FATAL_ERROR "GoogleTest is required but unavailable. Install GTest or provide GTest_DIR")
        endif()
        message(WARNING "GoogleTest unavailable: ${_test_files} test source file(s) NOT executed. Set ISOTHERM_REQUIRE_GTEST=ON in CI/release builds. CTest smoke tests remain active.")
    endif()
endfunction()
