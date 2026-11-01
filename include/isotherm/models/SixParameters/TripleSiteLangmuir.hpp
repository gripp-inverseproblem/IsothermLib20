//==============================================================================
// Name        : TripleSiteLangmuir.hpp
// Authors     : Joao Flavio Vasconcellos
// Version     : 2.0.0
// Description : TripleSiteLangmuir mathematical adsorption/sorption isotherm.
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
class TripleSiteLangmuir final {
public:
    TripleSiteLangmuir(Real p1, Real p2, Real p3, Real p4, Real p5, Real p6) : q1_(p1), k1_(p2), q2_(p3), k2_(p4), q3_(p5), k3_(p6) {
        require_positive(q1_, "q_1");
        require_positive(k1_, "K_1");
        require_positive(q2_, "q_2");
        require_positive(k2_, "K_2");
        require_positive(q3_, "q_3");
        require_positive(k3_, "K_3");
        
    }

    [[nodiscard]] Real evaluate(Real x) const {
        require_nonnegative(x, "independent variable");
        
        if (x == 0.0) return 0.0;
        return q1_ * k1_ * x / (1.0 + k1_ * x)
             + q2_ * k2_ * x / (1.0 + k2_ * x)
             + q3_ * k3_ * x / (1.0 + k3_ * x);
    }

    [[nodiscard]] Real derivative(Real x) const {
        require_positive(x, "independent variable for derivative");
        
        return q1_ * k1_ / std::pow(1.0 + k1_ * x, 2.0)
             + q2_ * k2_ / std::pow(1.0 + k2_ * x, 2.0)
             + q3_ * k3_ / std::pow(1.0 + k3_ * x, 2.0);
    }
    /** @brief Parameters in the order of model_traits::parameter_names. */
    [[nodiscard]] std::array<Real, 6> parameters() const noexcept {
        return {q1_, k1_, q2_, k2_, q3_, k3_};
    }

    /** @brief Copy with new parameters; the constructor validates them. */
    [[nodiscard]] TripleSiteLangmuir with_parameters(const std::array<Real, 6>& p) const {
        return TripleSiteLangmuir{p[0], p[1], p[2], p[3], p[4], p[5]};
    }

private:
    Real q1_;
    Real k1_;
    Real q2_;
    Real k2_;
    Real q3_;
    Real k3_;
    
};
} // namespace isotherm::models

namespace isotherm {
template<> struct model_traits<models::TripleSiteLangmuir> {
    static constexpr std::size_t parameter_count = 6;
    static constexpr std::array<std::string_view, 6> parameter_names{"q_1", "K_1", "q_2", "K_2", "q_3", "K_3"};
    static constexpr std::string_view identifier = "triple_site_langmuir";
    static constexpr independent_variable variable = independent_variable::concentration;
    static constexpr equation_type equation = equation_type::explicit_equation;
    static constexpr bool temperature_dependent = false;
};
} // namespace isotherm
