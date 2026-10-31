//==============================================================================
// Name        : Henry.hpp
// Authors     : Joao Flavio Vasconcellos
// Version     : 2.0.0
// Description : Henry adsorption isotherm with a validated one-parameter model.
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
 * @brief Henry linear adsorption isotherm at a fixed temperature.
 *
 * q(C) = K_H C, for C >= 0 and K_H > 0. The parameter has units of
 * (adsorbed amount)/(equilibrium concentration). Absolute units are chosen
 * by the application and must be internally consistent.
 *
 * This is a low-coverage limiting relation, not a finite-capacity model.
 * It is distinct from a thermodynamic gas-dissolution model, although both
 * use a proportionality historically associated with Henry's law.
 */
class Henry final {
public:
    /** @throws isotherm_error for a non-positive or non-finite coefficient. */
    explicit Henry(Real coefficient) : coefficient_(coefficient) {
        require_positive(coefficient_, "Henry coefficient");
    }

    /** @brief Uptake at a finite, non-negative equilibrium concentration. */
    [[nodiscard]] Real evaluate(Real concentration) const {
        require_nonnegative(concentration, "equilibrium concentration");
        return coefficient_ * concentration;
    }

    /** @brief Analytical derivative dq/dC = K_H. */
    [[nodiscard]] Real derivative(Real concentration) const {
        require_nonnegative(concentration, "equilibrium concentration");
        return coefficient_;
    }

    [[nodiscard]] Real coefficient() const noexcept { return coefficient_; }

private:
    Real coefficient_;
};

} // namespace isotherm::models

namespace isotherm {

/** @brief Compile-time description of the one-parameter Henry model. */
template<> struct model_traits<models::Henry> {
    static constexpr std::size_t parameter_count = 1;
    static constexpr std::array<std::string_view, 1> parameter_names{"K_H"};
    static constexpr std::string_view identifier = "henry";
    static constexpr independent_variable variable = independent_variable::concentration;
    static constexpr equation_type equation = equation_type::explicit_equation;
    static constexpr bool temperature_dependent = false;
};

} // namespace isotherm
