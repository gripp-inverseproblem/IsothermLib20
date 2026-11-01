//==============================================================================
// Name        : AntiLangmuir.hpp
// Authors     : Joao Flavio Vasconcellos
// Version     : 2.0.0
// Description : AntiLangmuir mathematical adsorption/sorption isotherm.
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
class AntiLangmuir final {
public:
    AntiLangmuir(Real p1, Real p2) : h_(p1), b_(p2) {
        require_positive(h_, "H");
        require_positive(b_, "b");
        
    }

    [[nodiscard]] Real evaluate(Real x) const {
        require_nonnegative(x, "independent variable");
        if (x >= 1.0 / b_) throw isotherm_error(error_code::outside_domain, "AntiLangmuir requires x < 1/b");
        if (x == 0.0) return 0.0;
        return h_ * x / (1.0 - b_ * x);
    }

    [[nodiscard]] Real derivative(Real x) const {
        require_nonnegative(x, "independent variable for derivative");
        if (x >= 1.0 / b_) throw isotherm_error(error_code::outside_domain, "AntiLangmuir requires x < 1/b");
        const Real d = 1.0 - b_ * x;
        return h_ / (d * d);
    }
    /** @brief Parameters in the order of model_traits::parameter_names. */
    [[nodiscard]] std::array<Real, 2> parameters() const noexcept {
        return {h_, b_};
    }

    /** @brief Copy with new parameters; the constructor validates them. */
    [[nodiscard]] AntiLangmuir with_parameters(const std::array<Real, 2>& p) const {
        return AntiLangmuir{p[0], p[1]};
    }

private:
    Real h_;
    Real b_;
    
};
} // namespace isotherm::models

namespace isotherm {
template<> struct model_traits<models::AntiLangmuir> {
    static constexpr std::size_t parameter_count = 2;
    static constexpr std::array<std::string_view, 2> parameter_names{"H", "b"};
    static constexpr std::string_view identifier = "anti_langmuir";
    static constexpr independent_variable variable = independent_variable::concentration;
    static constexpr equation_type equation = equation_type::explicit_equation;
    static constexpr bool temperature_dependent = false;
};
} // namespace isotherm
