//==============================================================================
// Name        : DubininAstakhov.hpp
// Authors     : Joao Flavio Vasconcellos
// Version     : 2.0.0
// Description : DubininAstakhov mathematical adsorption/sorption isotherm.
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
class DubininAstakhov final {
public:
    DubininAstakhov(Real p1, Real p2, Real p3) : qmax_(p1), d_(p2), n_(p3) {
        require_positive(qmax_, "q_max");
        require_positive(d_, "D");
        require_positive(n_, "n");
        if (n_ < 1.0) throw isotherm_error(error_code::outside_domain, "n must be >= 1");
    }

    [[nodiscard]] Real evaluate(Real x) const {
        require_nonnegative(x, "independent variable");
        if (x > 1.0) throw isotherm_error(error_code::outside_domain, "Relative pressure exceeds one");
        if (x == 0.0) return 0.0;
        const Real t = -std::log(x);
        return qmax_ * std::exp(-d_ * std::pow(t, n_));
    }

    [[nodiscard]] Real derivative(Real x) const {
        require_positive(x, "independent variable for derivative");
        if (x > 1.0) throw isotherm_error(error_code::outside_domain, "Relative pressure exceeds one");
        const Real t = -std::log(x);
        return evaluate(x) * d_ * n_ * std::pow(t, n_ - 1.0) / x;
    }
    /** @brief Parameters in the order of model_traits::parameter_names. */
    [[nodiscard]] std::array<Real, 3> parameters() const noexcept {
        return {qmax_, d_, n_};
    }

    /** @brief Copy with new parameters; the constructor validates them. */
    [[nodiscard]] DubininAstakhov with_parameters(const std::array<Real, 3>& p) const {
        return DubininAstakhov{p[0], p[1], p[2]};
    }

private:
    Real qmax_;
    Real d_;
    Real n_;
    
};
} // namespace isotherm::models

namespace isotherm {
template<> struct model_traits<models::DubininAstakhov> {
    static constexpr std::size_t parameter_count = 3;
    static constexpr std::array<std::string_view, 3> parameter_names{"q_max", "D", "n"};
    static constexpr std::string_view identifier = "dubinin_astakhov";
    static constexpr independent_variable variable = independent_variable::relative_pressure;
    static constexpr equation_type equation = equation_type::explicit_equation;
    static constexpr bool temperature_dependent = false;
};
} // namespace isotherm
