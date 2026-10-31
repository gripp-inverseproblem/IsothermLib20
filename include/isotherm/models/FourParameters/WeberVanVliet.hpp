//==============================================================================
// Name        : WeberVanVliet.hpp
// Authors     : Joao Flavio Vasconcellos
// Version     : 2.0.0
// Description : Implicit Weber–Van Vliet concentration-response law.
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
#include <limits>
#include <string_view>
//==============================================================================
// Isotherm++ headers
//==============================================================================
#include "isotherm/core/traits.hpp"
#include "isotherm/core/type.hpp"
#include "isotherm/core/validation.hpp"

namespace isotherm::models {
/** @brief Implicit Weber–Van Vliet concentration-response law. */
class WeberVanVliet final {
public:
    WeberVanVliet(Real p1, Real p2, Real p3, Real p4) : k1_(p1), k2_(p2), k3_(p3), k4_(p4) {
        require_positive(k1_, "WeberVanVliet K1");
        require_positive(k2_, "WeberVanVliet K2");
        require_positive(k3_, "WeberVanVliet K3");
        require_positive(k4_, "WeberVanVliet K4");
        // d/dt [(K2 exp(K3 t) + K4)t] >= K4 - K2 exp(-2).
        // A strictly positive lower bound guarantees a unique inverse.
        if (k4_ <= k2_ * std::exp(-2.0)) {
            throw isotherm_error(error_code::outside_domain,
                                 "WeberVanVliet requires K4 > K2 exp(-2) for a unique root");
        }
    }

    [[nodiscard]] Real evaluate(Real x) const {
        require_nonnegative(x, "equilibrium concentration");
        if (x == 0.0) return 0.0;
        // The inverse is solved in t = log(q_e), not in q_e directly.
        const Real log_activity = std::log(x) - std::log(k1_);
        return std::exp(solve_log_uptake(log_activity));
    }

    [[nodiscard]] Real derivative(Real x) const {
        require_positive(x, "concentration for the derivative");
        const Real log_activity = std::log(x) - std::log(k1_);
        const Real t = solve_log_uptake(log_activity);
        const Real power = k2_ * std::exp(k3_ * t);
        const Real logarithmic_slope = power + k4_ + k3_ * power * t;
        return std::exp(t) / (x * logarithmic_slope);
    }

private:

    [[nodiscard]] Real log_forward(Real t) const {
        const Real exp_term = std::exp(k3_ * t);
        return (k2_ * exp_term + k4_) * t;
    }

    [[nodiscard]] Real solve_log_uptake(Real log_activity) const {
        // A bounded log-uptake interval avoids both overflow in q^K3 and
        // the failure of Newton iterations on the historical model.
        const Real lower_limit = std::log(std::numeric_limits<Real>::denorm_min());
        const Real upper_limit = std::log(std::numeric_limits<Real>::max());
        if (log_activity < log_forward(lower_limit) ||
            log_activity > log_forward(upper_limit)) {
            throw isotherm_error(error_code::outside_domain,
                                 "WeberVanVliet uptake is outside representable range");
        }
        Real lower = lower_limit;
        Real upper = upper_limit;
        for (int iteration = 0; iteration < 160; ++iteration) {
            const Real middle = lower + (upper - lower) * 0.5;
            if (middle == lower || middle == upper) break;
            if (log_forward(middle) < log_activity) lower = middle;
            else upper = middle;
        }
        return lower + (upper - lower) * 0.5;
    }
    Real k1_, k2_, k3_, k4_;
};
} // namespace isotherm::models

namespace isotherm {
template<> struct model_traits<models::WeberVanVliet> {
    static constexpr std::size_t parameter_count = 4;
    static constexpr std::array<std::string_view, 4> parameter_names{"K1", "K2", "K3", "K4"};
    static constexpr std::string_view identifier = "weber_van_vliet";
    static constexpr independent_variable variable = independent_variable::concentration;
    static constexpr equation_type equation = equation_type::implicit_equation;
    static constexpr bool temperature_dependent = false;
};
} // namespace isotherm
