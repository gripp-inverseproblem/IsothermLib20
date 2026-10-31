//==============================================================================
// Name        : GAB.hpp
// Authors     : Joao Flavio Vasconcellos
// Version     : 2.0.0
// Description : GAB mathematical adsorption/sorption isotherm.
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
class GAB final {
public:
    GAB(Real p1, Real p2, Real p3) : qm_(p1), c_(p2), k_(p3) {
        require_positive(qm_, "q_m");
        require_positive(c_, "C");
        require_positive(k_, "K");
        if (k_ >= 1.0 || c_ < 1.0) throw isotherm_error(error_code::outside_domain, "GAB requires 0<K<1 and C>=1");
    }

    [[nodiscard]] Real evaluate(Real x) const {
        require_nonnegative(x, "independent variable");
        if (x > 1.0) throw isotherm_error(error_code::outside_domain, "Water activity exceeds one");
        if (x == 0.0) return 0.0;
        const Real d1 = 1.0 - k_ * x;
        const Real d2 = 1.0 + (c_ - 1.0) * k_ * x;
        return qm_ * c_ * k_ * x / (d1 * d2);
    }

    [[nodiscard]] Real derivative(Real x) const {
        require_positive(x, "independent variable for derivative");
        if (x > 1.0) throw isotherm_error(error_code::outside_domain, "Water activity exceeds one");
        const Real d1 = 1.0 - k_ * x;
        const Real d2 = 1.0 + (c_ - 1.0) * k_ * x;
        return qm_ * c_ * k_ * (1.0 + (c_ - 1.0) * k_ * k_ * x * x)
               / (d1 * d1 * d2 * d2);
    }
    /** @brief Parameters in the order of model_traits::parameter_names. */
    [[nodiscard]] std::array<Real, 3> parameters() const noexcept {
        return {qm_, c_, k_};
    }

    /** @brief Copy with new parameters; the constructor validates them. */
    [[nodiscard]] GAB with_parameters(const std::array<Real, 3>& p) const {
        return GAB{p[0], p[1], p[2]};
    }

private:
    Real qm_;
    Real c_;
    Real k_;
    
};
} // namespace isotherm::models

namespace isotherm {
template<> struct model_traits<models::GAB> {
    static constexpr std::size_t parameter_count = 3;
    static constexpr std::array<std::string_view, 3> parameter_names{"q_m", "C", "K"};
    static constexpr std::string_view identifier = "g_a_b";
    static constexpr independent_variable variable = independent_variable::relative_pressure;
    static constexpr equation_type equation = equation_type::explicit_equation;
    static constexpr bool temperature_dependent = false;
};
} // namespace isotherm
