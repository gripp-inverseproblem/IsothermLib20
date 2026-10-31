//==============================================================================
// Name        : FritzSchlunderIV.hpp
// Authors     : Joao Flavio Vasconcellos
// Version     : 2.0.0
// Description : Legacy four-parameter Fritz–Schlunder response.
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
/** @brief Legacy four-parameter Fritz–Schlunder response. */
class FritzSchlunderIV final {
public:
    FritzSchlunderIV(Real p1, Real p2, Real p3, Real p4) : qmax_(p1), k1_(p2), k2_(p3), k3_(p4) {
        require_positive(qmax_, "FritzSchlunderIV q_max");
        require_positive(k1_, "FritzSchlunderIV K1");
        require_positive(k2_, "FritzSchlunderIV K2");
        require_positive(k3_, "FritzSchlunderIV K3");
        if (k3_ > 1.0) {
            throw isotherm_error(error_code::outside_domain,
                                 "FritzSchlunderIV requires K3 <= 1");
        }
    }

    [[nodiscard]] Real evaluate(Real x) const {
        require_nonnegative(x, "equilibrium concentration");
        if (x == 0.0) return 0.0;
        const Real z = k1_ * std::pow(x, k3_);
        return qmax_ * std::pow(x, k2_) / (1.0 + z);
    }

    [[nodiscard]] Real derivative(Real x) const {
        require_positive(x, "concentration for the derivative");
        const Real z = k1_ * std::pow(x, k3_);
        return evaluate(x) / x * (k2_ - k3_ * z / (1.0 + z));
    }

private:
    Real qmax_, k1_, k2_, k3_;
};
} // namespace isotherm::models

namespace isotherm {
template<> struct model_traits<models::FritzSchlunderIV> {
    static constexpr std::size_t parameter_count = 4;
    static constexpr std::array<std::string_view, 4> parameter_names{"q_max", "K1", "K2", "K3"};
    static constexpr std::string_view identifier = "fritz_schlunder_i_v";
    static constexpr independent_variable variable = independent_variable::concentration;
    static constexpr equation_type equation = equation_type::explicit_equation;
    static constexpr bool temperature_dependent = false;
};
} // namespace isotherm
