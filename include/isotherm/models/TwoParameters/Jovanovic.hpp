//==============================================================================
// Name        : Jovanovic.hpp
// Authors     : Joao Flavio Vasconcellos
// Version     : 2.0.0
// Description : Two-parameter, saturating Jovanovic monolayer isotherm.
//
// Copyright   : Copyright (C) 2026 Joao Flavio Vasconcellos
// Licence     : MIT License (see LICENSE.txt)
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

/** @brief Jovanovic monolayer isotherm with a finite saturation uptake. */
class Jovanovic final {
public:
    explicit Jovanovic(Real saturation_uptake, Real affinity)
        : saturation_uptake_(saturation_uptake), affinity_(affinity) {
        require_positive(saturation_uptake_, "Jovanovic saturation uptake");
        require_positive(affinity_, "Jovanovic affinity");
    }

    /** @brief Evaluate q = q_sat * (1 - exp(-K * C)). */
    [[nodiscard]] Real evaluate(Real concentration) const {
        require_nonnegative(concentration, "equilibrium concentration");
        return -saturation_uptake_ * std::expm1(-affinity_ * concentration);
    }

    /** @brief Analytical derivative with respect to equilibrium concentration. */
    [[nodiscard]] Real derivative(Real concentration) const {
        require_nonnegative(concentration, "equilibrium concentration");
        return saturation_uptake_ * affinity_ *
               std::exp(-affinity_ * concentration);
    }

    /** @brief Parameters in the order of model_traits::parameter_names. */
    [[nodiscard]] std::array<Real, 2> parameters() const noexcept {
        return {saturation_uptake_, affinity_};
    }

    /** @brief Copy with new parameters; the constructor validates them. */
    [[nodiscard]] Jovanovic with_parameters(const std::array<Real, 2>& p) const {
        return Jovanovic{p[0], p[1]};
    }

private:
    Real saturation_uptake_;
    Real affinity_;
};

} // namespace isotherm::models

namespace isotherm {

/** @brief Compile-time traits for the Jovanovic monolayer formulation. */
template<> struct model_traits<models::Jovanovic> {
    static constexpr std::size_t parameter_count = 2;
    static constexpr std::array<std::string_view, 2> parameter_names{"q_sat", "K"};
    static constexpr std::string_view identifier = "jovanovic";
    static constexpr independent_variable variable = independent_variable::concentration;
    static constexpr equation_type equation = equation_type::explicit_equation;
    static constexpr bool temperature_dependent = false;
};

} // namespace isotherm
