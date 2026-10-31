//==============================================================================
// Name        : FritzSchlunder.hpp
// Authors     : Joao Flavio Vasconcellos
// Version     : 2.0.0
// Description : Three-parameter legacy Fritz–Schlunder formula, algebraically linked to Redlich–Peterson.
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
/** @brief Three-parameter legacy Fritz–Schlunder formula, algebraically linked to Redlich–Peterson. */
class FritzSchlunder final {
public:
    FritzSchlunder(Real p1, Real p2, Real p3) : a_(p1), b_(p2), c_(p3) {
        require_positive(a_, "FritzSchlunder a");
        require_positive(b_, "FritzSchlunder b");
        require_positive(c_, "FritzSchlunder c");
    }

    [[nodiscard]] Real evaluate(Real x) const {
        require_nonnegative(x, "equilibrium concentration");
        const Real q = a_, k = b_, n = c_;
        if (x == 0.0) return 0.0;
        return q * k * x / (1.0 + q * std::pow(x, n));
    }
    [[nodiscard]] Real derivative(Real x) const {
        require_positive(x, "concentration for the derivative");
        const Real q = a_, k = b_, n = c_;
        return (q * k / (1.0 + q * std::pow(x, n))) * (1.0 - n * (q * std::pow(x, n)) / (1.0 + q * std::pow(x, n)));
    }

private:
    Real a_, b_, c_;
};
} // namespace isotherm::models

namespace isotherm {
template<> struct model_traits<models::FritzSchlunder> {
    static constexpr std::size_t parameter_count = 3;
    static constexpr std::array<std::string_view, 3> parameter_names{"Q", "K", "m"};
    static constexpr std::string_view identifier = "fritz_schlunder";
    static constexpr independent_variable variable = independent_variable::concentration;
    static constexpr equation_type equation = equation_type::explicit_equation;
    static constexpr bool temperature_dependent = false;
};
} // namespace isotherm
