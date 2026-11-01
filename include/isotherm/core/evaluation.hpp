//==============================================================================
// Name        : evaluation.hpp
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
#include <span>


//==============================================================================
// Isotherm++ headers
//==============================================================================

#include "isotherm/core/concepts.hpp"
#include "isotherm/core/error.hpp"
#include "isotherm/core/type.hpp"
#include "isotherm/core/validation.hpp"

namespace isotherm {
// The core validates finite arguments and results; individual models enforce
// physical constraints, including positivity and any parameter restrictions.
template<scalar_isotherm Model>
[[nodiscard]] inline Real evaluate(const Model& model, Real x) {
    require_finite(x, "independent variable");
    const Real result = model.evaluate(x);
    if (!std::isfinite(result)) {
        throw isotherm_error(error_code::non_finite_result,
                             "Model evaluation returned a non-finite value");
    }
    return result;
}
template<thermal_isotherm Model>
[[nodiscard]] inline Real evaluate(const Model& model, Real x, Real temperature) {
    require_finite(x, "independent variable");
    require_positive(temperature, "absolute temperature");
    const Real result = model.evaluate(x, temperature);
    if (!std::isfinite(result)) {
        throw isotherm_error(error_code::non_finite_result,
                             "Model evaluation returned a non-finite value");
    }
    return result;
}
/** @brief Dispatch the analytical derivative and reject non-finite results. */
template<differentiable_scalar_isotherm Model>
[[nodiscard]] inline Real derivative(const Model& model, Real x) {
    require_finite(x, "independent variable");
    const Real result = model.derivative(x);
    if (!std::isfinite(result)) {
        throw isotherm_error(error_code::non_finite_result,
                             "Model derivative returned a non-finite value");
    }
    return result;
}

/** @brief Dispatch a thermal analytical derivative with input validation. */
template<differentiable_thermal_isotherm Model>
[[nodiscard]] inline Real derivative(const Model& model, Real x, Real temperature) {
    require_finite(x, "independent variable");
    require_positive(temperature, "absolute temperature");
    const Real result = model.derivative(x, temperature);
    if (!std::isfinite(result)) {
        throw isotherm_error(error_code::non_finite_result,
                             "Model derivative returned a non-finite value");
    }
    return result;
}

template<scalar_isotherm Model>
inline void evaluate_into(const Model& model, std::span<const Real> xs,
                          std::span<Real> output) {
    if (xs.size() != output.size()) {
        throw isotherm_error(error_code::invalid_input,
                             "Input and output spans must have equal lengths");
    }
    // Per-element execution: no rollback if model evaluation fails mid-batch.
    for (std::size_t i = 0; i < xs.size(); ++i) {
        output[i] = evaluate(model, xs[i]);
    }
}
template<thermal_isotherm Model>
inline void evaluate_into(const Model& model, std::span<const Real> xs,
                          Real temperature, std::span<Real> output) {
    if (xs.size() != output.size()) {
        throw isotherm_error(error_code::invalid_input,
                             "Input and output spans must have equal lengths");
    }
    require_positive(temperature, "absolute temperature");
    for (std::size_t i = 0; i < xs.size(); ++i) {
        output[i] = evaluate(model, xs[i], temperature);
    }
}
} // namespace isotherm
