//==============================================================================
// Name        : OBrienMyers.hpp
// Authors     : Joao Flavio Vasconcellos
// Version     : 2.0.0
// Description : OBrienMyers mathematical adsorption/sorption isotherm.
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
class OBrienMyers final {
public:
    OBrienMyers(Real p1, Real p2, Real p3) : qmax_(p1), b_(p2), sigma_(p3) {
        require_positive(qmax_, "q_max");
        require_positive(b_, "b");
        require_positive(sigma_, "sigma");
        if (sigma_ >= 1.0) throw isotherm_error(error_code::outside_domain, "OBrienMyers requires sigma < 1");
    }

    [[nodiscard]] Real evaluate(Real x) const {
        require_nonnegative(x, "independent variable");
        
        if (x == 0.0) return 0.0;
        const Real z = b_ * x;
        return qmax_ * (z / (1.0 + z) + sigma_ * sigma_ *
               z * (1.0 - z) / (2.0 * std::pow(1.0 + z, 3.0)));
    }

    [[nodiscard]] Real derivative(Real x) const {
        require_positive(x, "independent variable for derivative");
        
        const Real z = b_ * x;
        return qmax_ * b_ * (1.0 / std::pow(1.0 + z, 2.0) +
               sigma_ * sigma_ * (1.0 - 4.0*z + z*z) /
               (2.0 * std::pow(1.0 + z, 4.0)));
    }
private:
    const Real qmax_;
    const Real b_;
    const Real sigma_;
    
};
} // namespace isotherm::models

namespace isotherm {
template<> struct model_traits<models::OBrienMyers> {
    static constexpr std::size_t parameter_count = 3;
    static constexpr std::array<std::string_view, 3> parameter_names{"q_max", "b", "sigma"};
    static constexpr std::string_view identifier = "o_brien_myers";
    static constexpr independent_variable variable = independent_variable::concentration;
    static constexpr equation_type equation = equation_type::explicit_equation;
    static constexpr bool temperature_dependent = false;
};
} // namespace isotherm
