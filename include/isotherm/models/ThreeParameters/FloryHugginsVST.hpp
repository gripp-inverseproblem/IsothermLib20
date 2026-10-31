//==============================================================================
// Name        : FloryHugginsVST.hpp
// Authors     : Joao Flavio Vasconcellos
// Version     : 2.0.0
// Description : FloryHugginsVST mathematical adsorption/sorption isotherm.
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
class FloryHugginsVST final {
public:
    FloryHugginsVST(Real p1, Real p2, Real p3) : qmax_(p1), kh_(p2), alpha_(p3) {
        require_positive(qmax_, "q_max");
        require_positive(kh_, "K_H");
        require_positive(alpha_, "alpha");
        
    }

    [[nodiscard]] Real evaluate(Real x) const {
        require_nonnegative(x, "independent variable");
        
        if (x == 0.0) return 0.0;
        const Real lx = std::log(x * kh_ / qmax_);
        Real lo = 0.0, hi = 1.0;
        for (int i = 0; i < 160; ++i) {
            const Real t = lo + (hi - lo) * 0.5;
            const Real lhs = std::log(t) - std::log1p(-t)
                     + alpha_ * alpha_ * t / (1.0 + alpha_ * t);
            if (lhs < lx) lo = t; else hi = t;
        }
        return qmax_ * (lo + (hi - lo) * 0.5);
    }

    [[nodiscard]] Real derivative(Real x) const {
        require_positive(x, "independent variable for derivative");
        
        const Real theta = evaluate(x) / qmax_;
        const Real d = 1.0 / (theta * (1.0 - theta)) +
                         alpha_ * alpha_ / std::pow(1.0 + alpha_ * theta, 2.0);
        return qmax_ / (x * d);
    }
private:
    const Real qmax_;
    const Real kh_;
    const Real alpha_;
    
};
} // namespace isotherm::models

namespace isotherm {
template<> struct model_traits<models::FloryHugginsVST> {
    static constexpr std::size_t parameter_count = 3;
    static constexpr std::array<std::string_view, 3> parameter_names{"q_max", "K_H", "alpha"};
    static constexpr std::string_view identifier = "flory_huggins_v_s_t";
    static constexpr independent_variable variable = independent_variable::concentration;
    static constexpr equation_type equation = equation_type::implicit_equation;
    static constexpr bool temperature_dependent = false;
};
} // namespace isotherm
