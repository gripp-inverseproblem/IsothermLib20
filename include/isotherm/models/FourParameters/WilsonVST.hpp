//==============================================================================
// Name        : WilsonVST.hpp
// Authors     : Joao Flavio Vasconcellos
// Version     : 2.0.0
// Description : WilsonVST mathematical adsorption/sorption isotherm.
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
class WilsonVST final {
public:
    WilsonVST(Real p1, Real p2, Real p3, Real p4) : qmax_(p1), kh_(p2), l1v_(p3), lv1_(p4) {
        require_positive(qmax_, "q_max");
        require_positive(kh_, "K_H");
        require_positive(l1v_, "L1v");
        require_positive(lv1_, "Lv1");
        if (l1v_ < 0.8 || l1v_ > 1.2 || lv1_ < 0.8 || lv1_ > 1.2) throw isotherm_error(error_code::outside_domain, "WilsonVST restricted to [0.8,1.2] factors");
    }

    [[nodiscard]] Real evaluate(Real x) const {
        require_nonnegative(x, "independent variable");
        
        if (x == 0.0) return 0.0;
        const Real lx = std::log(x) + std::log(kh_) - std::log(qmax_);
        Real lo = 0.0, hi = 1.0;
        // Bisect to full double resolution: a fixed iteration count cannot resolve
        // roots far below the initial bracket width (e.g. K*C = 1e-60).
        for (int i = 0; i < 2200; ++i) {
            const Real t = lo + (hi - lo) * 0.5;
            if (t == lo || t == hi) break;
            if (log_wilson(t) < lx) lo = t; else hi = t;
        }
        return qmax_ * (lo + (hi - lo) * 0.5);
    }

    [[nodiscard]] Real derivative(Real x) const {
        require_positive(x, "independent variable for derivative");
        
        const Real theta = evaluate(x) / qmax_;
        return qmax_ / (x * log_wilson_derivative(theta));
    }
    /** @brief Parameters in the order of model_traits::parameter_names. */
    [[nodiscard]] std::array<Real, 4> parameters() const noexcept {
        return {qmax_, kh_, l1v_, lv1_};
    }

    /** @brief Copy with new parameters; the constructor validates them. */
    [[nodiscard]] WilsonVST with_parameters(const std::array<Real, 4>& p) const {
        return WilsonVST{p[0], p[1], p[2], p[3]};
    }

private:
    Real qmax_;
    Real kh_;
    Real l1v_;
    Real lv1_;
        [[nodiscard]] Real log_wilson(Real theta) const {
        const Real u = 1.0 - (1.0 - lv1_) * theta;
        const Real v = l1v_ + (1.0 - l1v_) * theta;
        return std::log(theta) - std::log1p(-theta)
               + std::log(l1v_) + std::log(u) - std::log(v)
               - lv1_ * (1.0 - lv1_) * theta / u
               - (1.0 - l1v_) * theta / v;
    }
    [[nodiscard]] Real log_wilson_derivative(Real theta) const {
        const Real u = 1.0 - (1.0 - lv1_) * theta;
        const Real v = l1v_ + (1.0 - l1v_) * theta;
        return 1.0 / (theta * (1.0 - theta))
               - (1.0 - lv1_) / u - (1.0 - l1v_) / v
               - lv1_ * (1.0 - lv1_) / (u * u)
               - l1v_ * (1.0 - l1v_) / (v * v);
    }

};
} // namespace isotherm::models

namespace isotherm {
template<> struct model_traits<models::WilsonVST> {
    static constexpr std::size_t parameter_count = 4;
    static constexpr std::array<std::string_view, 4> parameter_names{"q_max", "K_H", "L1v", "Lv1"};
    static constexpr std::string_view identifier = "wilson_v_s_t";
    static constexpr independent_variable variable = independent_variable::concentration;
    static constexpr equation_type equation = equation_type::implicit_equation;
    static constexpr bool temperature_dependent = false;
};
} // namespace isotherm
