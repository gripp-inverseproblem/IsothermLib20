//==============================================================================
// Name        : Smith.hpp
// Authors     : Joao Flavio Vasconcellos
// Version     : 2.0.0
// Description : Modified Smith moisture sorption versus temperature at fixed activity.
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
 * @brief Fixed-activity temperature section of the modified Smith equation.
 *
 * q_e(T; A, B, C, a_w) = A + B*T - C*log(1-a_w).
 * T is absolute temperature in kelvin; a_w is dimensionless and stored.
 * A, B, and C are fitted coefficients; a_w is a stored physical condition.
 * This is not the two-coefficient Smith (1947) relation.
 */
class Smith final {
public:
    Smith(Real a, Real b, Real c, Real water_activity)
        : a_(a), b_(b), c_(c), water_activity_(water_activity) {
        require_positive(a_, "Smith A");
        require_finite(b_, "Smith B");
        require_positive(c_, "Smith C");
        require_nonnegative(water_activity_, "water activity");
        if (water_activity_ >= Real{1}) {
            throw isotherm_error(error_code::outside_domain,
                                 "Water activity must be below one");
        }
    }

    [[nodiscard]] Real evaluate(Real temperature_k) const {
        const Real intercept = intercept_at(temperature_k);
        return intercept - c_ * std::log1p(-water_activity_);
    }

    /** @brief Derivative with respect to T (K), at fixed A, B, C and a_w. */
    [[nodiscard]] Real derivative(Real temperature_k) const {
        (void)intercept_at(temperature_k);
        return b_;
    }

private:
    [[nodiscard]] Real intercept_at(Real temperature_k) const {
        require_positive(temperature_k, "absolute temperature in kelvin");
        const Real intercept = a_ + b_ * temperature_k;
        require_positive(intercept, "Smith A + B*T");
        return intercept;
    }

    const Real a_;
    const Real b_;
    const Real c_;
    const Real water_activity_;
};
} // namespace isotherm::models

namespace isotherm {
template<> struct model_traits<models::Smith> {
    static constexpr std::size_t parameter_count = 4;
    static constexpr std::array<std::string_view, 4> parameter_names{"A", "B", "C", "a_w"};
    static constexpr std::string_view identifier = "smith";
    static constexpr independent_variable variable = independent_variable::temperature;
    static constexpr equation_type equation = equation_type::explicit_equation;
    // The core's 'temperature_dependent' flag selects the two-argument
    // (x, T) API. Here T is x itself, as for the Henderson temperature section.
    static constexpr bool temperature_dependent = false;
};
} // namespace isotherm
