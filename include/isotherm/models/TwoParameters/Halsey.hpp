//==============================================================================
// Name        : Halsey.hpp
// Authors     : Joao Flavio Vasconcellos
// Version     : 2.0.0
// Description : Two-parameter Halsey formulation with strict domain validation.
//
// Copyright   : Copyright (C) 2026 Joao Flavio Vasconcellos
// Licence     : MIT License (see LICENSE)
// SPDX-License-Identifier: MIT
//==============================================================================

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

/** @brief Historical library reciprocal-concentration Halsey branch. It decreases with C and is singular as C approaches zero; do not replace it with an alternative activity-based water-sorption equation. */
class Halsey final {
public:
    explicit Halsey(Real coefficient, Real exponent) : coefficient_(coefficient), exponent_(exponent) {
        require_positive(coefficient_, "Halsey coefficient");
        require_positive(exponent_, "Halsey exponent");
    }

    /** @brief Evaluate the model on its documented mathematical domain. */
    [[nodiscard]] Real evaluate(Real concentration) const {
        require_positive(concentration, "dimensionless equilibrium concentration");
        return std::exp((std::log(coefficient_) - std::log(concentration)) / exponent_);
    }

    /** @brief Analytical derivative with respect to concentration. */
    [[nodiscard]] Real derivative(Real concentration) const {
        require_positive(concentration, "dimensionless equilibrium concentration");
        return -evaluate(concentration) / (exponent_ * concentration);
    }

    /** @brief Parameters in the order of model_traits::parameter_names. */
    [[nodiscard]] std::array<Real, 2> parameters() const noexcept {
        return {coefficient_, exponent_};
    }

    /** @brief Copy with new parameters; the constructor validates them. */
    [[nodiscard]] Halsey with_parameters(const std::array<Real, 2>& p) const {
        return Halsey{p[0], p[1]};
    }

private:
    Real coefficient_;
    Real exponent_;
};

} // namespace isotherm::models

namespace isotherm {

/** @brief Compile-time traits for the Halsey formulation. */
template<> struct model_traits<models::Halsey> {
    static constexpr std::size_t parameter_count = 2;
    static constexpr std::array<std::string_view, 2> parameter_names{"K_H", "n"};
    static constexpr std::string_view identifier = "halsey";
    static constexpr independent_variable variable = independent_variable::concentration;
    static constexpr equation_type equation = equation_type::explicit_equation;
    static constexpr bool temperature_dependent = false;
};

} // namespace isotherm
