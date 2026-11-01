//==============================================================================
// Name        : Henderson.hpp
// Authors     : Joao Flavio Vasconcellos
// Version     : 2.0.0
// Description : Henderson moisture sorption at fixed water activity versus absolute temperature.
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
/**
 * Henderson convention 1 - a_w = exp(-A T q_e^B), with T in kelvin.
 * A, B and a_w are constructor parameters; T is the independent variable.
 * This is a fixed-water-activity temperature section, not q_e versus a_w.
 */
class Henderson final {
public:
    Henderson(Real a, Real b, Real water_activity)
        : a_(a), b_(b), water_activity_(water_activity) {
        require_positive(a_, "A");
        require_positive(b_, "B");
        require_nonnegative(water_activity_, "water activity");
        if (water_activity_ >= Real{1}) {
            throw isotherm_error(error_code::outside_domain,
                                 "Water activity must be less than one");
        }
    }

    [[nodiscard]] Real evaluate(Real temperature_k) const {
        require_positive(temperature_k, "absolute temperature in kelvin");
        if (water_activity_ == Real{0}) return Real{0};
        const Real potential = -std::log1p(-water_activity_);
        return std::pow(potential / (a_ * temperature_k), Real{1} / b_);
    }

    [[nodiscard]] Real derivative(Real temperature_k) const {
        require_positive(temperature_k, "absolute temperature in kelvin");
        return -evaluate(temperature_k) / (b_ * temperature_k);
    }

    /** @brief Parameters in the order of model_traits::parameter_names. */
    [[nodiscard]] std::array<Real, 3> parameters() const noexcept {
        return {a_, b_, water_activity_};
    }

    /** @brief Copy with new parameters; the constructor validates them. */
    [[nodiscard]] Henderson with_parameters(const std::array<Real, 3>& p) const {
        return Henderson{p[0], p[1], p[2]};
    }

private:
    Real a_;
    Real b_;
    Real water_activity_;
};
} // namespace isotherm::models

namespace isotherm {
template<> struct model_traits<models::Henderson> {
    static constexpr std::size_t parameter_count = 3;
    static constexpr std::array<std::string_view, 3> parameter_names{"A", "B", "a_w"};
    static constexpr std::string_view identifier = "henderson";
    static constexpr independent_variable variable = independent_variable::temperature;
    static constexpr equation_type equation = equation_type::explicit_equation;
    // This is a scalar function of T; temperature is the abscissa, not
    // the additional temperature argument of the generic thermal-model API.
    static constexpr bool temperature_dependent = false;
};
} // namespace isotherm
