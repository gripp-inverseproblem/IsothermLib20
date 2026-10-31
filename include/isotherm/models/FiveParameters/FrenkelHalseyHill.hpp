//==============================================================================
// Name        : FrenkelHalseyHill.hpp
// Authors     : Joao Flavio Vasconcellos
// Version     : 2.0.0
// Description : FrenkelHalseyHill mathematical adsorption/sorption isotherm.
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
class FrenkelHalseyHill final {
public:
    FrenkelHalseyHill(Real p1, Real p2, Real p3, Real p4, Real p5) : qmax_(p1), k1_(p2), k2_(p3), k3_(p4), k4_(p5) {
        require_positive(qmax_, "q_max");
        require_positive(k1_, "K1");
        require_positive(k2_, "K2");
        require_positive(k3_, "K3");
        require_positive(k4_, "K4");
        
    }

    [[nodiscard]] Real evaluate(Real x) const {
        require_nonnegative(x, "independent variable");
        
        if (x == 0.0) return 0.0;
        const Real v = k2_ * std::pow(x, k4_);
        return qmax_ * std::pow(x, k3_) / (k1_ + v);
    }

    [[nodiscard]] Real derivative(Real x) const {
        require_positive(x, "independent variable for derivative");
        
        const Real v = k2_ * std::pow(x, k4_);
        return evaluate(x) / x * (k3_ - k4_ * v / (k1_ + v));
    }
private:
    const Real qmax_;
    const Real k1_;
    const Real k2_;
    const Real k3_;
    const Real k4_;
    
};
} // namespace isotherm::models

namespace isotherm {
template<> struct model_traits<models::FrenkelHalseyHill> {
    static constexpr std::size_t parameter_count = 5;
    static constexpr std::array<std::string_view, 5> parameter_names{"q_max", "K1", "K2", "K3", "K4"};
    static constexpr std::string_view identifier = "frenkel_halsey_hill";
    static constexpr independent_variable variable = independent_variable::concentration;
    static constexpr equation_type equation = equation_type::explicit_equation;
    static constexpr bool temperature_dependent = false;
};
} // namespace isotherm
