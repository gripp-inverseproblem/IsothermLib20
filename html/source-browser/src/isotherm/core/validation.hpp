//==============================================================================
// Name        : validation.hpp
// Authors     : Joao Flavio Vasconcellos
// Version     : 2.0.0
// Description : Isotherm++ 2.0 project source and documentation.
//
// Copyright   : Copyright (C) 2026 Joao Flavio Vasconcellos
// Licence     : MIT License (see LICENSE.txt)
// SPDX-License-Identifier: MIT
//==============================================================================

#pragma once

//==============================================================================
// C++ standard library headers
//==============================================================================

#include <cmath>
#include <string>
#include <string_view>


//==============================================================================
// Isotherm++ headers
//==============================================================================

#include "isotherm/core/error.hpp"
#include "isotherm/core/type.hpp"

namespace isotherm {
inline void require_finite(Real value, std::string_view name) {
    if (!std::isfinite(value)) {
        throw isotherm_error(error_code::invalid_input,
                             std::string(name) + " must be finite");
    }
}
inline void require_positive(Real value, std::string_view name) {
    require_finite(value, name);
    if (value <= 0.0) {
        throw isotherm_error(error_code::outside_domain,
                             std::string(name) + " must be strictly positive");
    }
}
inline void require_nonnegative(Real value, std::string_view name) {
    require_finite(value, name);
    if (value < 0.0) {
        throw isotherm_error(error_code::outside_domain,
                             std::string(name) + " must be non-negative");
    }
}
} // namespace isotherm
