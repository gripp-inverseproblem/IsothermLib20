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

cmake_minimum_required(VERSION 3.28)
project(IsothermPP VERSION 2.0.0 DESCRIPTION "Adsorption isotherm models and scientific catalogue" LANGUAGES CXX)

list(PREPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(ConfigUtils)
include(ConfigOptions)
include(TargetSettings)
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

# Header-only C++20 core with statically dispatched scientific model classes.
add_library(isotherm_core INTERFACE)
add_library(Isotherm::Isotherm ALIAS isotherm_core)
target_compile_features(isotherm_core INTERFACE cxx_std_20)
target_include_directories(isotherm_core INTERFACE
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
    "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")
set_target_properties(isotherm_core PROPERTIES EXPORT_NAME Isotherm)

if(ISOTHERM_BUILD_EXAMPLES)
    add_subdirectory(Examples)
endif()
if(ISOTHERM_BUILD_TESTS)
    include(CTest)
    find_package(Python3 COMPONENTS Interpreter QUIET)
    if(Python3_Interpreter_FOUND)
        add_test(NAME source_headers
            COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/tools/check_source_headers.py")
        add_test(NAME catalogue_order
            COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/tools/check_catalogue_order.py")
        add_test(NAME model_coverage
            COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/tools/check_model_coverage.py")
        add_test(NAME two_parameter_real
            COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/tools/check_two_parameter_real.py")
        add_test(NAME external_html_links
            COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/tools/check_external_links.py")
        add_test(NAME three_parameter_real
            COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/tools/check_three_parameter_real.py")
        add_test(NAME remaining_parameter_models
            COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/tools/check_remaining_models.py")
    endif()
    add_subdirectory(IsothermTest)

    # A single build target builds all discovered test executables and then
    # runs the complete CTest suite, including the source-style checks.
    add_custom_target(run_all_tests
        COMMAND "${CMAKE_CTEST_COMMAND}" --test-dir "${CMAKE_BINARY_DIR}" --output-on-failure
        WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
        COMMENT "Building and running all Isotherm++ tests"
        USES_TERMINAL
        VERBATIM)
    get_property(_isotherm_test_targets GLOBAL PROPERTY ISOTHERM_TEST_EXECUTABLE_TARGETS)
    if(_isotherm_test_targets)
        add_dependencies(run_all_tests ${_isotherm_test_targets})
    endif()
endif()
if(ISOTHERM_BUILD_DOCS)
    add_subdirectory(docs)
endif()

install(DIRECTORY include/isotherm DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
    PATTERN ".gitkeep" EXCLUDE)
install(TARGETS isotherm_core EXPORT IsothermPPTargets)
install(EXPORT IsothermPPTargets FILE IsothermPPTargets.cmake
    NAMESPACE Isotherm:: DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/IsothermPP")
configure_package_config_file(cmake/IsothermPPConfig.cmake.in
    "${CMAKE_CURRENT_BINARY_DIR}/IsothermPPConfig.cmake"
    INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/IsothermPP")
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/IsothermPPConfigVersion.cmake"
    VERSION "${PROJECT_VERSION}" COMPATIBILITY SameMajorVersion)
install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/IsothermPPConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/IsothermPPConfigVersion.cmake"
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/IsothermPP")

message(STATUS "Isotherm++ C++ minimum: 20")
message(STATUS "Isotherm++ examples/tests/docs: ${ISOTHERM_BUILD_EXAMPLES}/${ISOTHERM_BUILD_TESTS}/${ISOTHERM_BUILD_DOCS}")
message(STATUS "Isotherm++ scientific models: 56 identifiers across one, two, three, four, five and six parameters")
