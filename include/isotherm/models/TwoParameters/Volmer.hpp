//==============================================================================
// Name        : Volmer.hpp
// Authors     : Joao Flavio Vasconcellos
// Version     : 2.0.0
// Description : Volmer mathematical adsorption/sorption isotherm.
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
class Volmer final {
public:
    Volmer(Real p1, Real p2) : qmax_(p1), k_(p2) {
        require_positive(qmax_, "q_max");
        require_positive(k_, "K");
        
    }

    [[nodiscard]] Real evaluate(Real x) const {
        require_nonnegative(x, "independent variable");
        
        if (x == 0.0) return 0.0;
        // w exp(w) = Kx, with theta=w/(1+w). Bracketed solution.
        const Real lx = std::log(k_) + std::log(x);
        Real lo = 0.0, hi = std::max(1.0, k_ * x);
        for (int i = 0; i < 160; ++i) {
            const Real w = 0.5 * (lo + hi);
            if (w == 0.0 || std::log(w) + w < lx) lo = w;
            else hi = w;
        }
        const Real w = 0.5 * (lo + hi);
        return qmax_ * w / (1.0 + w);
    }

    [[nodiscard]] Real derivative(Real x) const {
        require_positive(x, "independent variable for derivative");
        
        const Real theta = evaluate(x) / qmax_;
        return qmax_ * theta * (1.0 - theta) * (1.0 - theta) / x;
    }
    /** @brief Parameters in the order of model_traits::parameter_names. */
    [[nodiscard]] std::array<Real, 2> parameters() const noexcept {
        return {qmax_, k_};
    }

    /** @brief Copy with new parameters; the constructor validates them. */
    [[nodiscard]] Volmer with_parameters(const std::array<Real, 2>& p) const {
        return Volmer{p[0], p[1]};
    }

private:
    Real qmax_;
    Real k_;
    
};
} // namespace isotherm::models

namespace isotherm {
template<> struct model_traits<models::Volmer> {
    static constexpr std::size_t parameter_count = 2;
    static constexpr std::array<std::string_view, 2> parameter_names{"q_max", "K"};
    static constexpr std::string_view identifier = "volmer";
    static constexpr independent_variable variable = independent_variable::concentration;
    static constexpr equation_type equation = equation_type::implicit_equation;
    static constexpr bool temperature_dependent = false;
};
} // namespace isotherm
