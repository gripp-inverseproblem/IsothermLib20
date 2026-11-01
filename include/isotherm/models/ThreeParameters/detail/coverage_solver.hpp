//==============================================================================
// Name        : coverage_solver.hpp
// Authors     : Joao Flavio Vasconcellos
// Version     : 2.0.0
// Description : Bracketed log-activity solver for implicit occupancy equations.
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
#include <limits>
//==============================================================================
// Isotherm++ headers
//==============================================================================
#include "isotherm/core/error.hpp"
#include "isotherm/core/type.hpp"

namespace isotherm::models::detail {
/** @brief Solve monotonically increasing log(f(theta)) = log(activity). */
template<class LogResponse>
[[nodiscard]] inline Real solve_coverage(Real log_activity, const LogResponse& log_response) {
    if (!std::isfinite(log_activity)) {
        throw isotherm_error(error_code::invalid_input, "Non-finite log activity");
    }
    Real lower = Real{0.0};
    Real upper = std::nextafter(Real{1.0}, Real{0.0});
    if (log_response(upper) <= log_activity) return upper;
    for (int i = 0; i < 1200; ++i) {
        const Real middle = lower + Real{0.5} * (upper - lower);
        if (middle == lower || middle == upper) break;
        if (log_response(middle) < log_activity) lower = middle;
        else upper = middle;
    }
    return lower + Real{0.5} * (upper - lower);
}
} // namespace isotherm::models::detail
