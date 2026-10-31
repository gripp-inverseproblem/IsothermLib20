//==============================================================================
// Name        : Aranovich.hpp
// Authors     : Joao Flavio Vasconcellos
// Version     : 2.0.0
// Description : Aranovich mathematical adsorption/sorption isotherm.
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
class Aranovich final {
public:
    Aranovich(Real p1, Real p2, Real p3) : qmax_(p1), b_(p2), n_(p3) {
        require_positive(qmax_, "q_max");
        require_positive(b_, "b");
        require_positive(n_, "n");
        
    }

    [[nodiscard]] Real evaluate(Real x) const {
        require_nonnegative(x, "independent variable");
        if (x >= 1.0) throw isotherm_error(error_code::outside_domain, "Aranovich requires x<1");
        if (x == 0.0) return 0.0;
        return qmax_ * b_ * x / ((1.0 + b_ * x) * std::pow(1.0 - x, n_));
    }

    [[nodiscard]] Real derivative(Real x) const {
        require_positive(x, "independent variable for derivative");
        if (x >= 1.0) throw isotherm_error(error_code::outside_domain, "Aranovich requires x<1");
        return evaluate(x) * (1.0 / x - b_ / (1.0 + b_ * x) + n_ / (1.0 - x));
    }
private:
    const Real qmax_;
    const Real b_;
    const Real n_;
    
};
} // namespace isotherm::models

namespace isotherm {
template<> struct model_traits<models::Aranovich> {
    static constexpr std::size_t parameter_count = 3;
    static constexpr std::array<std::string_view, 3> parameter_names{"q_max", "b", "n"};
    static constexpr std::string_view identifier = "aranovich";
    static constexpr independent_variable variable = independent_variable::relative_pressure;
    static constexpr equation_type equation = equation_type::explicit_equation;
    static constexpr bool temperature_dependent = false;
};
} // namespace isotherm
