//==============================================================================
// Name        : Langmuir.hpp
// Authors     : Joao Flavio Vasconcellos
// Version     : 2.0.0
// Description : Numerically stable, two-parameter Langmuir adsorption model.
//
// Copyright   : Copyright (C) 2026 Joao Flavio Vasconcellos
// Licence     : MIT License (see LICENSE)
// SPDX-License-Identifier: MIT
//==============================================================================

#pragma once

//==============================================================================
// C++ standard library headers
//==============================================================================

#include <array>
#include <cmath>
#include <cstddef>
#include <string_view>

//==============================================================================
// Isotherm++ headers
//==============================================================================

#include "isotherm/core/traits.hpp"
#include "isotherm/core/type.hpp"
#include "isotherm/core/validation.hpp"

namespace isotherm::models {

/**
 * @brief Classical two-parameter, finite-capacity Langmuir adsorption model.
 *
 * q(C) = q_max (K C)/(1 + K C), with q_max > 0, K > 0 and C >= 0.
 * q_max has uptake units and K has inverse-concentration units.
 *
 * Infinity in the *intermediate* dimensionless product K*C is handled
 * analytically as saturation (q_max); an infinite input or parameter is
 * never accepted. This prevents NaN from inf/inf for large finite inputs.
 */
class Langmuir final {
public:
    /** @throws isotherm_error for non-positive or non-finite parameters. */
    Langmuir(Real capacity, Real affinity)
        : capacity_(capacity), affinity_(affinity) {
        require_positive(capacity_, "Langmuir maximum capacity");
        require_positive(affinity_, "Langmuir affinity");
    }

    /** @brief Evaluate Langmuir uptake at a non-negative concentration. */
    [[nodiscard]] Real evaluate(Real concentration) const {
        require_nonnegative(concentration, "equilibrium concentration");
        const Real scaled = affinity_ * concentration;
        if (std::isinf(scaled)) {
            return capacity_;
        }
        return capacity_ * (scaled / (1.0 + scaled));
    }

    /** @brief Analytical concentration derivative dq/dC. */
    [[nodiscard]] Real derivative(Real concentration) const {
        require_nonnegative(concentration, "equilibrium concentration");
        const Real scaled = affinity_ * concentration;
        if (std::isinf(scaled)) {
            return 0.0;
        }
        const Real inv = 1.0 / (1.0 + scaled);
        return (capacity_ * inv) * (affinity_ * inv);
    }

    [[nodiscard]] Real capacity() const noexcept { return capacity_; }
    [[nodiscard]] Real affinity() const noexcept { return affinity_; }

private:
    Real capacity_;
    Real affinity_;
};

} // namespace isotherm::models

namespace isotherm {

/** @brief Compile-time description of the two-parameter Langmuir model. */
template<> struct model_traits<models::Langmuir> {
    static constexpr std::size_t parameter_count = 2;
    static constexpr std::array<std::string_view, 2> parameter_names{"q_max", "K"};
    static constexpr std::string_view identifier = "langmuir";
    static constexpr independent_variable variable = independent_variable::concentration;
    static constexpr equation_type equation = equation_type::explicit_equation;
    static constexpr bool temperature_dependent = false;
};

} // namespace isotherm
