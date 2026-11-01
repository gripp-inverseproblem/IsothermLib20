//==============================================================================
// Name        : Oswin.hpp
// Authors     : Joao Flavio Vasconcellos
// Version     : 2.0.0
// Description : Four-parameter temperature-dependent extension of Oswin.
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
 * @brief Four-coefficient Oswin extension:
 *        q_e = (A + B*T) * [a_w/(1-a_w)]^(1/(C + D*T)).
 *
 * The fourth fitted coefficient D allows the exponent to vary with T.
 * Setting D=0 reduces the model to the three-coefficient modified Oswin
 * equation. This extra temperature term is a library extension, not the
 * historical Oswin (1946) formulation.
 *
 * T is absolute temperature (K); a_w is dimensionless water activity.
 */
class Oswin final {
public:
    Oswin(Real a, Real b, Real c, Real d)
        : a_(a), b_(b), c_(c), d_(d) {
        require_positive(a_, "Oswin A");
        require_finite(b_, "Oswin B");
        require_positive(c_, "Oswin C");
        require_finite(d_, "Oswin D");
    }

    [[nodiscard]] Real evaluate(Real water_activity,
                                Real temperature_k) const {
        const Real amplitude = amplitude_at(temperature_k);
        const Real exponent_denom = denominator_at(temperature_k);
        require_nonnegative(water_activity, "water activity");
        if (water_activity >= Real{1}) {
            throw isotherm_error(error_code::outside_domain,
                                 "Water activity must be below one");
        }
        if (water_activity == Real{0}) return Real{0};
        const Real ratio = water_activity / (Real{1} - water_activity);
        return amplitude * std::pow(ratio, Real{1} / exponent_denom);
    }

    [[nodiscard]] Real derivative(Real water_activity,
                                  Real temperature_k) const {
        require_positive(water_activity, "water activity for derivative");
        const Real value = evaluate(water_activity, temperature_k);
        const Real exponent_denom = denominator_at(temperature_k);
        return value / (exponent_denom * water_activity
                        * (Real{1} - water_activity));
    }

    /** @brief Parameters in the order of model_traits::parameter_names. */
    [[nodiscard]] std::array<Real, 4> parameters() const noexcept {
        return {a_, b_, c_, d_};
    }

    /** @brief Copy with new parameters; the constructor validates them. */
    [[nodiscard]] Oswin with_parameters(const std::array<Real, 4>& p) const {
        return Oswin{p[0], p[1], p[2], p[3]};
    }

private:
    [[nodiscard]] Real amplitude_at(Real temperature_k) const {
        require_positive(temperature_k, "absolute temperature in kelvin");
        const Real amplitude = a_ + b_ * temperature_k;
        require_positive(amplitude, "Oswin A + B*T");
        return amplitude;
    }

    [[nodiscard]] Real denominator_at(Real temperature_k) const {
        require_positive(temperature_k, "absolute temperature in kelvin");
        const Real denom = c_ + d_ * temperature_k;
        require_positive(denom, "Oswin C + D*T");
        return denom;
    }

    Real a_;
    Real b_;
    Real c_;
    Real d_;
};
} // namespace isotherm::models

namespace isotherm {
template<> struct model_traits<models::Oswin> {
    static constexpr std::size_t parameter_count = 4;
    static constexpr std::array<std::string_view, 4> parameter_names{
        "A", "B", "C", "D"};
    static constexpr std::string_view identifier = "oswin";
    static constexpr independent_variable variable =
        independent_variable::relative_pressure;
    static constexpr equation_type equation = equation_type::explicit_equation;
    static constexpr bool temperature_dependent = true;
};
} // namespace isotherm
