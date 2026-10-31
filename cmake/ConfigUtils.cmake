#==============================================================================
# Name        : ConfigUtils.cmake
# Authors     : Joao Flavio Vasconcellos
# Version     : 2.0.0
# Description : Isotherm++ 2.0 project configuration.
#
# Copyright   : Copyright (C) 2026 Joao Flavio Vasconcellos
# Licence     : MIT License (see LICENSE)
# SPDX-License-Identifier: MIT
#==============================================================================

include_guard(GLOBAL)
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
    message(FATAL_ERROR "In-source builds are prohibited. Use cmake -S . -B build")
endif()
