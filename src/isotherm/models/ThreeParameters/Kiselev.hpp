//==============================================================================
// Name        : Kiselev.hpp
// Authors     : Joao Flavio Vasconcellos
// Version     : 2.0.0
// Description : Implicit Kiselev occupancy relation solved with a bracketed method.
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
#include "isotherm/models/ThreeParameters/detail/coverage_solver.hpp"

namespace isotherm::models {
/** @brief Implicit Kiselev occupancy relation solved with a bracketed method. */
class Kiselev final {
public:
    Kiselev(Real p1, Real p2, Real p3) : a_(p1), b_(p2), c_(p3) {
        require_positive(a_, "Kiselev a");
        require_positive(b_, "Kiselev b");
        require_positive(c_, "Kiselev c");
    }

    [[nodiscard]] Real evaluate(Real x) const {
        return a_ * coverage(x);
    }
    [[nodiscard]] Real derivative(Real x) const {
        require_positive(x, "concentration for the derivative");
        const Real theta = coverage(x);
        const Real n = c_;
        const Real log_slope = 1.0 / theta - n / (1.0 + n * theta) + 1.0 / (1.0 - theta);
        return a_ / (x * log_slope);
    }

    /** @brief Parameters in the order of model_traits::parameter_names. */
    [[nodiscard]] std::array<Real, 3> parameters() const noexcept {
        return {a_, b_, c_};
    }

    /** @brief Copy with new parameters; the constructor validates them. */
    [[nodiscard]] Kiselev with_parameters(const std::array<Real, 3>& p) const {
        return Kiselev{p[0], p[1], p[2]};
    }

private:
    [[nodiscard]] Real coverage(Real x) const {
        require_nonnegative(x, "equilibrium concentration");
        if (x == 0.0) return 0.0;
        const Real n = c_;
        const Real log_activity = std::log(b_) + std::log(x);
        return detail::solve_coverage(log_activity, [&](Real theta) {
            return std::log(theta) - std::log1p(n * theta) - std::log1p(-theta);
        });
    }
    Real a_, b_, c_;
};
} // namespace isotherm::models

namespace isotherm {
template<> struct model_traits<models::Kiselev> {
    static constexpr std::size_t parameter_count = 3;
    static constexpr std::array<std::string_view, 3> parameter_names{"q_max", "K", "a"};
    static constexpr std::string_view identifier = "kiselev";
    static constexpr independent_variable variable = independent_variable::concentration;
    static constexpr equation_type equation = equation_type::implicit_equation;
    static constexpr bool temperature_dependent = false;
};
} // namespace isotherm
