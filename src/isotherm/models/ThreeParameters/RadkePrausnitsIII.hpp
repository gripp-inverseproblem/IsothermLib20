//==============================================================================
// Name        : RadkePrausnitsIII.hpp
// Authors     : Joao Flavio Vasconcellos
// Version     : 2.0.0
// Description : Legacy Radke–Prausnitz III form with a concentration power in the denominator.
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
/** @brief Legacy Radke–Prausnitz III form with a concentration power in the denominator. */
class RadkePrausnitsIII final {
public:
    RadkePrausnitsIII(Real p1, Real p2, Real p3) : a_(p1), b_(p2), c_(p3) {
        require_positive(a_, "RadkePrausnitsIII a");
        require_positive(b_, "RadkePrausnitsIII b");
        require_positive(c_, "RadkePrausnitsIII c");
    }

    [[nodiscard]] Real evaluate(Real x) const {
        require_nonnegative(x, "equilibrium concentration");
        const Real q = a_, k = b_, n = c_;
        if (x == 0.0) return 0.0;
        return q * k * std::pow(x, n) / (1.0 + k * std::pow(x, n - 1.0));
    }
    [[nodiscard]] Real derivative(Real x) const {
        require_positive(x, "concentration for the derivative");
        const Real q = a_, k = b_, n = c_;
        return q * k * std::pow(x, n - 1.0) * (n + k * std::pow(x, n - 1.0)) / std::pow(1.0 + k * std::pow(x, n - 1.0), 2.0);
    }

    /** @brief Parameters in the order of model_traits::parameter_names. */
    [[nodiscard]] std::array<Real, 3> parameters() const noexcept {
        return {a_, b_, c_};
    }

    /** @brief Copy with new parameters; the constructor validates them. */
    [[nodiscard]] RadkePrausnitsIII with_parameters(const std::array<Real, 3>& p) const {
        return RadkePrausnitsIII{p[0], p[1], p[2]};
    }

private:
    Real a_, b_, c_;
};
} // namespace isotherm::models

namespace isotherm {
template<> struct model_traits<models::RadkePrausnitsIII> {
    static constexpr std::size_t parameter_count = 3;
    static constexpr std::array<std::string_view, 3> parameter_names{"q_max", "K", "n"};
    static constexpr std::string_view identifier = "radke_prausnits_i_i_i";
    static constexpr independent_variable variable = independent_variable::concentration;
    static constexpr equation_type equation = equation_type::explicit_equation;
    static constexpr bool temperature_dependent = false;
};
} // namespace isotherm
