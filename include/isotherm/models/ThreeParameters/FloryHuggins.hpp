//==============================================================================
// Name        : FloryHuggins.hpp
// Authors     : Joao Flavio Vasconcellos
// Version     : 2.0.0
// Description : FloryHuggins mathematical adsorption/sorption isotherm.
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
class FloryHuggins final {
public:
    FloryHuggins(Real p1, Real p2, Real p3) : qmax_(p1), k_(p2), n_(p3) {
        require_positive(qmax_, "q_max");
        require_positive(k_, "K");
        require_positive(n_, "n");
        
    }

    [[nodiscard]] Real evaluate(Real x) const {
        require_nonnegative(x, "independent variable");
        
        if (x == 0.0) return 0.0;
        Real lo = 0.0, hi = 1.0;
        // Bisect to full double resolution: a fixed iteration count cannot resolve
        // roots far below the initial bracket width (e.g. K*C = 1e-60).
        for (int i = 0; i < 2200; ++i) {
            const Real t = lo + (hi - lo) * 0.5;
            if (t == lo || t == hi) break;
            if (t < k_ * x * std::pow(1.0 - t, n_)) lo = t;
            else hi = t;
        }
        return qmax_ * (lo + (hi - lo) * 0.5);
    }

    [[nodiscard]] Real derivative(Real x) const {
        require_positive(x, "independent variable for derivative");
        
        const Real theta = evaluate(x) / qmax_;
        return qmax_ * theta * (1.0 - theta) / (x * (1.0 + (n_ - 1.0) * theta));
    }
    /** @brief Parameters in the order of model_traits::parameter_names. */
    [[nodiscard]] std::array<Real, 3> parameters() const noexcept {
        return {qmax_, k_, n_};
    }

    /** @brief Copy with new parameters; the constructor validates them. */
    [[nodiscard]] FloryHuggins with_parameters(const std::array<Real, 3>& p) const {
        return FloryHuggins{p[0], p[1], p[2]};
    }

private:
    Real qmax_;
    Real k_;
    Real n_;
    
};
} // namespace isotherm::models

namespace isotherm {
template<> struct model_traits<models::FloryHuggins> {
    static constexpr std::size_t parameter_count = 3;
    static constexpr std::array<std::string_view, 3> parameter_names{"q_max", "K", "n"};
    static constexpr std::string_view identifier = "flory_huggins";
    static constexpr independent_variable variable = independent_variable::concentration;
    static constexpr equation_type equation = equation_type::implicit_equation;
    static constexpr bool temperature_dependent = false;
};
} // namespace isotherm
