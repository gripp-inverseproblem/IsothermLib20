#==============================================================================
# Name        : TargetSettings.cmake
# Authors     : Joao Flavio Vasconcellos
# Version     : 2.0.0
# Description : Isotherm++ 2.0 project configuration.
#
# Copyright   : Copyright (C) 2026 Joao Flavio Vasconcellos
# Licence     : MIT License (see LICENSE)
# SPDX-License-Identifier: MIT
#==============================================================================

include_guard(GLOBAL)
include(CheckIPOSupported)
include(CheckCXXCompilerFlag)
set(ISOTHERM_IPO_ACTIVE FALSE)
if(ISOTHERM_ENABLE_LTO)
    check_ipo_supported(RESULT _ipo_supported OUTPUT _ipo_reason LANGUAGES CXX)
    if(NOT _ipo_supported)
        message(FATAL_ERROR "Requested LTO is unavailable: ${_ipo_reason}")
    endif()
    set(ISOTHERM_IPO_ACTIVE TRUE)
endif()
if(ISOTHERM_ENABLE_NATIVE_ARCH)
    if(CMAKE_CROSSCOMPILING OR MSVC OR NOT CMAKE_CXX_COMPILER_ID MATCHES "^(GNU|Clang|AppleClang)$")
        message(FATAL_ERROR "Native architecture tuning requires a supported native GCC/Clang build")
    endif()
    check_cxx_compiler_flag("-march=native" _has_native)
    if(NOT _has_native)
        message(FATAL_ERROR "The compiler does not support -march=native")
    endif()
endif()
function(isotherm_apply_settings target)
    if(NOT TARGET "${target}")
        message(FATAL_ERROR "Target missing: ${target}")
    endif()
    target_compile_features("${target}" PRIVATE cxx_std_20)
    if(MSVC)
        target_compile_options("${target}" PRIVATE /W4 /permissive-)
    else()
        target_compile_options("${target}" PRIVATE -Wall -Wextra -Wpedantic)
        if(ISOTHERM_ENABLE_NATIVE_ARCH)
            target_compile_options("${target}" PRIVATE "$<$<CONFIG:Release,RelWithDebInfo>:-march=native>")
        endif()
    endif()
    set_target_properties("${target}" PROPERTIES
        INTERPROCEDURAL_OPTIMIZATION_RELEASE "${ISOTHERM_IPO_ACTIVE}"
        INTERPROCEDURAL_OPTIMIZATION_RELWITHDEBINFO "${ISOTHERM_IPO_ACTIVE}")
endfunction()
# Numerically unsafe flags, including -ffast-math, are deliberately excluded.
