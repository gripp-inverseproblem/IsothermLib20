//==============================================================================
// Name        : Jossens.hpp
// Authors     : Joao Flavio Vasconcellos
// Version     : 2.0.0
// Description : Loading-implicit Jossens isotherm C = (q/H) exp(F q^p).
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
#include "isotherm/core/error.hpp"
#include "isotherm/core/traits.hpp"
#include "isotherm/core/type.hpp"
#include "isotherm/core/validation.hpp"

namespace isotherm::models {
/**
 * @brief Jossens isotherm in its original loading-implicit form.
 *
 * C_e = (q_e/H) exp(F q_e^p), with H > 0 (Henry coefficient), F > 0, p > 0.
 *
 * ln C_e = ln q_e - ln H + F q_e^p is strictly increasing in q_e, so every
 * C_e > 0 has exactly one root, bracketed by 0 < q_e < H C_e. The root is
 * found by bisection to full double resolution.
 */
class Jossens final {
public:
    Jossens(Real henry, Real f, Real p) : h_(henry), f_(f), p_(p) {
        require_positive(h_, "Jossens H");
        require_positive(f_, "Jossens F");
        require_positive(p_, "Jossens p");
    }

    [[nodiscard]] Real evaluate(Real x) const {
        require_nonnegative(x, "equilibrium concentration");
        if (x == 0.0) return 0.0;
        return solve_uptake(x);
    }

    /** @brief dq/dC = q / [C (1 + F p q^p)]; its limit at C = 0 is H. */
    [[nodiscard]] Real derivative(Real x) const {
        require_nonnegative(x, "equilibrium concentration");
        if (x == 0.0) return h_;
        const Real q = solve_uptake(x);
        return q / (x * (1.0 + f_ * p_ * std::pow(q, p_)));
    }

    [[nodiscard]] std::array<Real, 3> parameters() const noexcept {
        return {h_, f_, p_};
    }

    [[nodiscard]] Jossens with_parameters(const std::array<Real, 3>& p) const {
        return Jossens{p[0], p[1], p[2]};
    }

private:
    [[nodiscard]] Real solve_uptake(Real x) const {
        const Real target = std::log(h_) + std::log(x);   // ln(H C)
        Real lo = 0.0;
        Real hi = std::exp(target);                       // q < H C
        if (!std::isfinite(hi)) {
            throw isotherm_error(error_code::outside_domain,
                                 "Jossens: H*C exceeds the representable range");
        }
        for (int i = 0; i < 2200; ++i) {
            const Real q = lo + (hi - lo) * 0.5;
            if (q == lo || q == hi) break;
            if (std::log(q) + f_ * std::pow(q, p_) < target) lo = q;
            else hi = q;
        }
        return lo + (hi - lo) * 0.5;
    }

    Real h_;
    Real f_;
    Real p_;
};
} // namespace isotherm::models

namespace isotherm {
template<> struct model_traits<models::Jossens> {
    static constexpr std::size_t parameter_count = 3;
    static constexpr std::array<std::string_view, 3> parameter_names{"H", "F", "p"};
    static constexpr std::string_view identifier = "jossens";
    static constexpr independent_variable variable = independent_variable::concentration;
    static constexpr equation_type equation = equation_type::implicit_equation;
    static constexpr bool temperature_dependent = false;
};
} // namespace isotherm
