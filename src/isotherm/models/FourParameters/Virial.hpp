//==============================================================================
// Name        : Virial.hpp
// Authors     : Joao Flavio Vasconcellos
// Version     : 2.0.0
// Description : Virial mathematical adsorption/sorption isotherm.
//
// Copyright   : Copyright (C) 2026 Joao Flavio Vasconcellos
// Licence     : MIT License (see LICENSE.txt)
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
class Virial final {
public:
    Virial(Real p1, Real p2, Real p3, Real p4) : kh_(p1), a_(p2), b_(p3), c_(p4) {
        require_positive(kh_, "K_H");
        require_positive(a_, "A");
        require_positive(b_, "B");
        require_positive(c_, "C");
        
    }

    [[nodiscard]] Real evaluate(Real x) const {
        require_nonnegative(x, "independent variable");
        
        if (x == 0.0) return 0.0;
        const Real lx = std::log(x) + std::log(kh_);
        Real lo = 0.0, hi = std::max(1.0, kh_ * x);
        // Bisect to full double resolution: a fixed iteration count cannot resolve
        // roots far below the initial bracket width (e.g. K*C = 1e-60).
        for (int i = 0; i < 2200; ++i) {
            const Real q = lo + (hi - lo) * 0.5;
            if (q == lo || q == hi) break;
            const Real f = std::log(q) + a_ * q + b_ * q*q + c_ * q*q*q;
            if (f < lx) lo = q;
            else hi = q;
        }
        return lo + (hi - lo) * 0.5;
    }

    [[nodiscard]] Real derivative(Real x) const {
        require_positive(x, "independent variable for derivative");
        
        const Real q = evaluate(x);
        return q / (x * (1.0 + a_ * q + 2.0*b_*q*q + 3.0*c_*q*q*q));
    }
    /** @brief Parameters in the order of model_traits::parameter_names. */
    [[nodiscard]] std::array<Real, 4> parameters() const noexcept {
        return {kh_, a_, b_, c_};
    }

    /** @brief Copy with new parameters; the constructor validates them. */
    [[nodiscard]] Virial with_parameters(const std::array<Real, 4>& p) const {
        return Virial{p[0], p[1], p[2], p[3]};
    }

private:
    Real kh_;
    Real a_;
    Real b_;
    Real c_;
    
};
} // namespace isotherm::models

namespace isotherm {
template<> struct model_traits<models::Virial> {
    static constexpr std::size_t parameter_count = 4;
    static constexpr std::array<std::string_view, 4> parameter_names{"K_H", "A", "B", "C"};
    static constexpr std::string_view identifier = "virial";
    static constexpr independent_variable variable = independent_variable::concentration;
    static constexpr equation_type equation = equation_type::implicit_equation;
    static constexpr bool temperature_dependent = false;
};
} // namespace isotherm
