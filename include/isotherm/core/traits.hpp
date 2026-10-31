//==============================================================================
// Name        : traits.hpp
// Authors     : Joao Flavio Vasconcellos
// Version     : 2.0.0
// Description : Isotherm++ 2.0 project source and documentation.
//
// Copyright   : Copyright (C) 2026 Joao Flavio Vasconcellos
// Licence     : MIT License (see LICENSE)
// SPDX-License-Identifier: MIT
//==============================================================================

#pragma once

//==============================================================================
// C++ standard library headers
//==============================================================================

#include <cstddef>
#include <string_view>

namespace isotherm {
// Every scientific model provides a specialisation. No base class is required.
template<class Model> struct model_traits;
enum class independent_variable { concentration, pressure, relative_pressure, temperature };
enum class equation_type { explicit_equation, implicit_equation };
// Names identify parameters, not variables or concentrations.
// Required trait members: parameter_count, parameter_names, identifier,
// variable, equation, temperature_dependent.
} // namespace isotherm
