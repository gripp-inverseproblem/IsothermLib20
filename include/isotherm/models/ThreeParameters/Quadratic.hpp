//==============================================================================
// Name        : Quadratic.hpp
// Authors     : Joao Flavio Vasconcellos
// Version     : 2.0.0
// Description : Quadratic mathematical adsorption/sorption isotherm.
//
// Copyright   : Copyright (C) 2026 Joao Flavio Vasconcellos
// Licence     : MIT License (see LICENSE)
// SPDX-License-Identifier: MIT
//==============================================================================

#pragma once
//==============================================================================
// C++ standard library headers
//==============================================================================
#include <algorithm>
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
class Quadratic final {
public:
    Quadratic(Real p1, Real p2, Real p3) : qmax_(p1), ka_(p2), kb_(p3) {
        require_positive(qmax_, "q_max");
        require_positive(ka_, "K_a");
        require_positive(kb_, "K_b");
        
    }

    [[nodiscard]] Real evaluate(Real x) const {
        require_nonnegative(x, "independent variable");
        
        if (x == 0.0) return 0.0;
        const Real d = 1.0 + ka_ * x + kb_ * x * x;
        return qmax_ * x * (ka_ + 2.0 * kb_ * x) / d;
    }

    [[nodiscard]] Real derivative(Real x) const {
        require_positive(x, "independent variable for derivative");
        
        const Real d = 1.0 + ka_ * x + kb_ * x * x;
        const Real n = x * (ka_ + 2.0 * kb_ * x);
        const Real nd = ka_ + 4.0 * kb_ * x;
        const Real dd = ka_ + 2.0 * kb_ * x;
        return qmax_ * (nd * d - n * dd) / (d * d);
    }
private:
    const Real qmax_;
    const Real ka_;
    const Real kb_;
    
};
} // namespace isotherm::models

namespace isotherm {
template<> struct model_traits<models::Quadratic> {
    static constexpr std::size_t parameter_count = 3;
    static constexpr std::array<std::string_view, 3> parameter_names{"q_max", "K_a", "K_b"};
    static constexpr std::string_view identifier = "quadratic";
    static constexpr independent_variable variable = independent_variable::concentration;
    static constexpr equation_type equation = equation_type::explicit_equation;
    static constexpr bool temperature_dependent = false;
};
} // namespace isotherm
