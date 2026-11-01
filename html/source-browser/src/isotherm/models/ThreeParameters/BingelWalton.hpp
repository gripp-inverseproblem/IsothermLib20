//==============================================================================
// Name        : BingelWalton.hpp
// Authors     : Joao Flavio Vasconcellos
// Version     : 2.0.0
// Description : BingelWalton mathematical adsorption/sorption isotherm.
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
class BingelWalton final {
public:
    BingelWalton(Real p1, Real p2, Real p3) : qmax_(p1), a_(p2), b_(p3) {
        require_positive(qmax_, "q_max");
        require_positive(a_, "a");
        require_positive(b_, "b");
        
    }

    [[nodiscard]] Real evaluate(Real x) const {
        require_nonnegative(x, "independent variable");
        
        if (x == 0.0) return 0.0;
        const Real e = std::exp(-(a_ + b_) * x);
        return qmax_ * (-std::expm1(-(a_ + b_) * x)) / (1.0 + (b_ / a_) * e);
    }

    [[nodiscard]] Real derivative(Real x) const {
        require_positive(x, "independent variable for derivative");
        
        const Real e = std::exp(-(a_ + b_) * x);
        const Real d = 1.0 + (b_ / a_) * e;
        return qmax_ * (a_ + b_) * (1.0 + b_ / a_) * e / (d * d);
    }
    /** @brief Parameters in the order of model_traits::parameter_names. */
    [[nodiscard]] std::array<Real, 3> parameters() const noexcept {
        return {qmax_, a_, b_};
    }

    /** @brief Copy with new parameters; the constructor validates them. */
    [[nodiscard]] BingelWalton with_parameters(const std::array<Real, 3>& p) const {
        return BingelWalton{p[0], p[1], p[2]};
    }

private:
    Real qmax_;
    Real a_;
    Real b_;
    
};
} // namespace isotherm::models

namespace isotherm {
template<> struct model_traits<models::BingelWalton> {
    static constexpr std::size_t parameter_count = 3;
    static constexpr std::array<std::string_view, 3> parameter_names{"q_max", "a", "b"};
    static constexpr std::string_view identifier = "bingel_walton";
    static constexpr independent_variable variable = independent_variable::concentration;
    static constexpr equation_type equation = equation_type::explicit_equation;
    static constexpr bool temperature_dependent = false;
};
} // namespace isotherm
