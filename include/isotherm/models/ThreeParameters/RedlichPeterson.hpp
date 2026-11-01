//==============================================================================
// Name        : RedlichPeterson.hpp
// Authors     : Joao Flavio Vasconcellos
// Version     : 2.0.0
// Description : Redlich–Peterson equation q = A C/(1 + B C^n).
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
/** @brief Redlich–Peterson equation q = A C/(1 + B C^n). */
class RedlichPeterson final {
public:
    RedlichPeterson(Real p1, Real p2, Real p3) : a_(p1), b_(p2), c_(p3) {
        require_positive(a_, "RedlichPeterson a");
        require_positive(b_, "RedlichPeterson b");
        require_positive(c_, "RedlichPeterson c");
    }

    [[nodiscard]] Real evaluate(Real x) const {
        require_nonnegative(x, "equilibrium concentration");
        const Real q = a_, k = b_, n = c_;
        if (x == 0.0) return 0.0;
        return q * x / (1.0 + k * std::pow(x, n));
    }
    [[nodiscard]] Real derivative(Real x) const {
        require_positive(x, "concentration for the derivative");
        const Real q = a_, k = b_, n = c_;
        return q * (1.0 + (1.0 - n) * k * std::pow(x, n)) / std::pow(1.0 + k * std::pow(x, n), 2.0);
    }

    /** @brief Parameters in the order of model_traits::parameter_names. */
    [[nodiscard]] std::array<Real, 3> parameters() const noexcept {
        return {a_, b_, c_};
    }

    /** @brief Copy with new parameters; the constructor validates them. */
    [[nodiscard]] RedlichPeterson with_parameters(const std::array<Real, 3>& p) const {
        return RedlichPeterson{p[0], p[1], p[2]};
    }

private:
    Real a_, b_, c_;
};
} // namespace isotherm::models

namespace isotherm {
template<> struct model_traits<models::RedlichPeterson> {
    static constexpr std::size_t parameter_count = 3;
    static constexpr std::array<std::string_view, 3> parameter_names{"A", "B", "n"};
    static constexpr std::string_view identifier = "redlich_peterson";
    static constexpr independent_variable variable = independent_variable::concentration;
    static constexpr equation_type equation = equation_type::explicit_equation;
    static constexpr bool temperature_dependent = false;
};
} // namespace isotherm
