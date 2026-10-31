//==============================================================================
// Name        : MacMillanTeller.hpp
// Authors     : Joao Flavio Vasconcellos
// Version     : 2.0.0
// Description : MacMillan–Teller logarithmic formula; diverges as c_e approaches c_s.
//
// Copyright   : Copyright (C) 2026 Joao Flavio Vasconcellos
// Licence     : MIT License (see LICENSE)
// SPDX-License-Identifier: MIT
//==============================================================================

#pragma once
//==============================================================================
// C++ standard library headers
//==============================================================================
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
/** @brief MacMillan–Teller logarithmic formula; diverges as c_e approaches c_s. */
class MacMillanTeller final {
public:
    MacMillanTeller(Real p1, Real p2, Real p3) : a_(p1), b_(p2), c_(p3) {
        require_positive(a_, "MacMillanTeller a");
        require_positive(b_, "MacMillanTeller b");
        require_positive(c_, "MacMillanTeller c");
    }

    [[nodiscard]] Real evaluate(Real x) const {
        require_nonnegative(x, "equilibrium concentration");
        const Real q = a_, k = b_, n = c_;
        if (!(x < n)) {
            throw isotherm_error(error_code::outside_domain,
                                 "Concentration must be below the saturation concentration");
        }
        if (x == 0.0) return 0.0;
        return q * std::cbrt(k / std::log(n / x));
    }
    [[nodiscard]] Real derivative(Real x) const {
        require_positive(x, "concentration for the derivative");
        const Real q = a_, k = b_, n = c_;
        if (!(x < n)) {
            throw isotherm_error(error_code::outside_domain,
                                 "Concentration must be below the saturation concentration");
        }
        return q * std::cbrt(k / std::log(n / x)) / (3.0 * x * std::log(n / x));
    }

private:
    Real a_, b_, c_;
};
} // namespace isotherm::models

namespace isotherm {
template<> struct model_traits<models::MacMillanTeller> {
    static constexpr std::size_t parameter_count = 3;
    static constexpr std::array<std::string_view, 3> parameter_names{"q_max", "K", "c_s"};
    static constexpr std::string_view identifier = "mac_millan_teller";
    static constexpr independent_variable variable = independent_variable::concentration;
    static constexpr equation_type equation = equation_type::explicit_equation;
    static constexpr bool temperature_dependent = false;
};
} // namespace isotherm
