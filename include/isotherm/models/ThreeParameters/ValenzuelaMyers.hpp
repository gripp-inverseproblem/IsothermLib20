//==============================================================================
// Name        : ValenzuelaMyers.hpp
// Authors     : Joao Flavio Vasconcellos
// Version     : 2.0.0
// Description : Valenzuela–Myers integrated heterogeneity equation; reciprocal affinity in log terms.
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
/** @brief Valenzuela–Myers integrated heterogeneity equation; reciprocal affinity in log terms. */
class ValenzuelaMyers final {
public:
    ValenzuelaMyers(Real p1, Real p2, Real p3) : a_(p1), b_(p2), c_(p3) {
        require_positive(a_, "ValenzuelaMyers a");
        require_positive(b_, "ValenzuelaMyers b");
        require_positive(c_, "ValenzuelaMyers c");
        if (c_ > 50.0) {
            throw isotherm_error(error_code::outside_domain,
                                 "Heterogeneity spread above the safe exponential range");
        }
    }

    [[nodiscard]] Real evaluate(Real x) const {
        require_nonnegative(x, "equilibrium concentration");
        const Real q = a_, k = b_, n = c_;
        if (x == 0.0) return 0.0;
        return (q / (2.0 * n)) * (std::log1p(x * std::exp(n) / k) - std::log1p(x * std::exp(-n) / k));
    }
    [[nodiscard]] Real derivative(Real x) const {
        require_positive(x, "concentration for the derivative");
        const Real q = a_, k = b_, n = c_;
        return (q / (2.0 * n)) * (std::exp(n) / (k + x * std::exp(n)) - std::exp(-n) / (k + x * std::exp(-n)));
    }

    /** @brief Parameters in the order of model_traits::parameter_names. */
    [[nodiscard]] std::array<Real, 3> parameters() const noexcept {
        return {a_, b_, c_};
    }

    /** @brief Copy with new parameters; the constructor validates them. */
    [[nodiscard]] ValenzuelaMyers with_parameters(const std::array<Real, 3>& p) const {
        return ValenzuelaMyers{p[0], p[1], p[2]};
    }

private:
    Real a_, b_, c_;
};
} // namespace isotherm::models

namespace isotherm {
template<> struct model_traits<models::ValenzuelaMyers> {
    static constexpr std::size_t parameter_count = 3;
    static constexpr std::array<std::string_view, 3> parameter_names{"q_max", "K", "a"};
    static constexpr std::string_view identifier = "valenzuela_myers";
    static constexpr independent_variable variable = independent_variable::concentration;
    static constexpr equation_type equation = equation_type::explicit_equation;
    static constexpr bool temperature_dependent = false;
};
} // namespace isotherm
