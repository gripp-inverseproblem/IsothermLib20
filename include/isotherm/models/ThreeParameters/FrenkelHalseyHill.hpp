//==============================================================================
// Name        : FrenkelHalseyHill.hpp
// Authors     : Joao Flavio Vasconcellos
// Version     : 2.0.0
// Description : Concentration-based, three-parameter FHH multilayer isotherm.
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
/**
 * @brief Three-parameter concentration-based FHH convention.
 *
 * q_e(C_e) = q_m [ln(C_s / C_e)]^(-1/s), 0 < C_e < C_s.
 *
 * This convention fixes the additional FHH coefficient k to unity;
 * q_m is an uptake scaling coefficient, not a finite saturation capacity.
 * C_s and C_e must be expressed in the same concentration units.
 */
class FrenkelHalseyHill final {
public:
    FrenkelHalseyHill(Real q_m, Real C_s, Real s)
        : q_m_(q_m), c_s_(C_s), s_(s) {
        require_positive(q_m_, "q_m");
        require_positive(c_s_, "C_s");
        require_positive(s_, "s");
    }

    [[nodiscard]] Real evaluate(Real C_e) const {
        const Real log_ratio = log_saturation_ratio(C_e);
        const Real result = q_m_ * std::pow(log_ratio, -Real{1} / s_);
        require_finite(result, "FHH uptake");
        return result;
    }

    [[nodiscard]] Real derivative(Real C_e) const {
        const Real log_ratio = log_saturation_ratio(C_e);
        const Real uptake = q_m_ * std::pow(log_ratio, -Real{1} / s_);
        // Splitting the factors avoids an intermediate overflow in C_e * L.
        const Real result = (uptake / s_) / C_e / log_ratio;
        require_finite(result, "FHH concentration derivative");
        return result;
    }

private:
    [[nodiscard]] Real log_saturation_ratio(Real C_e) const {
        require_positive(C_e, "equilibrium concentration");
        if (!(C_e < c_s_)) {
            throw isotherm_error(error_code::outside_domain,
                                 "C_e must be below C_s");
        }
        // For C_e near C_s, log1p avoids cancellation. The difference
        // (C_s - C_e) is exact in this interval by Sterbenz's lemma.
        if (C_e >= c_s_ * Real{0.5}) {
            return -std::log1p((C_e - c_s_) / c_s_);
        }
        // Avoid overflow in C_s / C_e for very small positive C_e.
        return std::log(c_s_) - std::log(C_e);
    }

    const Real q_m_;
    const Real c_s_;
    const Real s_;
};
} // namespace isotherm::models

namespace isotherm {
template<> struct model_traits<models::FrenkelHalseyHill> {
    static constexpr std::size_t parameter_count = 3;
    static constexpr std::array<std::string_view, 3> parameter_names{
        "q_m", "C_s", "s"};
    static constexpr std::string_view identifier = "frenkel_halsey_hill";
    static constexpr independent_variable variable =
        independent_variable::concentration;
    static constexpr equation_type equation = equation_type::explicit_equation;
    static constexpr bool temperature_dependent = false;
};
} // namespace isotherm
