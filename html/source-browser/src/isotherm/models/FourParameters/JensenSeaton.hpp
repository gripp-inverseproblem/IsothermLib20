//==============================================================================
// Name        : JensenSeaton.hpp
// Authors     : Joao Flavio Vasconcellos
// Version     : 2.0.0
// Description : JensenSeaton mathematical adsorption/sorption isotherm.
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
class JensenSeaton final {
public:
    JensenSeaton(Real p1, Real p2, Real p3, Real p4) : k_(p1), qm_(p2), kk_(p3), t_(p4) {
        require_positive(k_, "K");
        require_positive(qm_, "q_m");
        require_positive(kk_, "k");
        require_positive(t_, "t");
        
    }

    [[nodiscard]] Real evaluate(Real x) const {
        require_nonnegative(x, "independent variable");
        
        if (x == 0.0) return 0.0;
        const Real a = k_ * x / (qm_ * (1.0 + kk_ * x));
        return k_ * x / std::pow(1.0 + std::pow(a, t_), 1.0 / t_);
    }

    [[nodiscard]] Real derivative(Real x) const {
        require_positive(x, "independent variable for derivative");
        
        const Real a = k_ * x / (qm_ * (1.0 + kk_ * x));
        const Real v = std::pow(a, t_);
        return evaluate(x) / x * (1.0 - v / ((1.0 + v) * (1.0 + kk_ * x)));
    }
    /** @brief Parameters in the order of model_traits::parameter_names. */
    [[nodiscard]] std::array<Real, 4> parameters() const noexcept {
        return {k_, qm_, kk_, t_};
    }

    /** @brief Copy with new parameters; the constructor validates them. */
    [[nodiscard]] JensenSeaton with_parameters(const std::array<Real, 4>& p) const {
        return JensenSeaton{p[0], p[1], p[2], p[3]};
    }

private:
    Real k_;
    Real qm_;
    Real kk_;
    Real t_;
    
};
} // namespace isotherm::models

namespace isotherm {
template<> struct model_traits<models::JensenSeaton> {
    static constexpr std::size_t parameter_count = 4;
    static constexpr std::array<std::string_view, 4> parameter_names{"K", "q_m", "k", "t"};
    static constexpr std::string_view identifier = "jensen_seaton";
    static constexpr independent_variable variable = independent_variable::concentration;
    static constexpr equation_type equation = equation_type::explicit_equation;
    static constexpr bool temperature_dependent = false;
};
} // namespace isotherm
