//==============================================================================
// Name        : Elovich.hpp
// Authors     : Joao Flavio Vasconcellos
// Version     : 2.0.0
// Description : Two-parameter Elovich formulation with strict domain validation.
//
// Copyright   : Copyright (C) 2026 Joao Flavio Vasconcellos
// Licence     : MIT License (see LICENSE.txt)
// SPDX-License-Identifier: MIT
//==============================================================================

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

/** @brief Implicit Elovich–Larionov relation solved on the physical 0 <= theta <= 1 branch using a bracketed algorithm. No Lambert-W dependency. */
class Elovich final {
public:
    explicit Elovich(Real reference_uptake, Real affinity) : reference_uptake_(reference_uptake), affinity_(affinity) {
        require_positive(reference_uptake_, "Elovich reference uptake");
        require_positive(affinity_, "Elovich affinity");
    }

    /** @brief Evaluate the model on its documented mathematical domain. */
    [[nodiscard]] Real evaluate(Real concentration) const {
        require_nonnegative(concentration, "equilibrium concentration");
        if (concentration == 0.0) return 0.0;
        const Real log_x = std::log(affinity_) + std::log(concentration);
        // On the physical coverage branch theta <= 1, K*C <= e.
        if (log_x > 1.0) throw isotherm_error(error_code::outside_domain, "Elovich coverage exceeds unity");
        if (log_x < -50.0) return reference_uptake_ * std::exp(log_x);
        const Real x = std::exp(log_x);
        Real lower = 0.0;
        Real upper = 1.0;
        // theta exp(theta) = x is strictly monotone for theta >= 0.
        for (int iteration = 0; iteration < 75; ++iteration) {
            const Real midpoint = 0.5 * (lower + upper);
            if (midpoint * std::exp(midpoint) > x) upper = midpoint;
            else lower = midpoint;
        }
        return reference_uptake_ * (0.5 * (lower + upper));
    }

    /** @brief Analytical derivative with respect to concentration. */
    [[nodiscard]] Real derivative(Real concentration) const {
        require_nonnegative(concentration, "equilibrium concentration");
        if (concentration == 0.0) return reference_uptake_ * affinity_;
        const Real uptake = evaluate(concentration);
        const Real theta = uptake / reference_uptake_;
        return uptake / (concentration * (1.0 + theta));
    }

    /** @brief Parameters in the order of model_traits::parameter_names. */
    [[nodiscard]] std::array<Real, 2> parameters() const noexcept {
        return {reference_uptake_, affinity_};
    }

    /** @brief Copy with new parameters; the constructor validates them. */
    [[nodiscard]] Elovich with_parameters(const std::array<Real, 2>& p) const {
        return Elovich{p[0], p[1]};
    }

private:
    Real reference_uptake_;
    Real affinity_;
};

} // namespace isotherm::models

namespace isotherm {

/** @brief Compile-time traits for the Elovich formulation. */
template<> struct model_traits<models::Elovich> {
    static constexpr std::size_t parameter_count = 2;
    static constexpr std::array<std::string_view, 2> parameter_names{"q_ref", "K_E"};
    static constexpr std::string_view identifier = "elovich";
    static constexpr independent_variable variable = independent_variable::concentration;
    static constexpr equation_type equation = equation_type::implicit_equation;
    static constexpr bool temperature_dependent = false;
};

} // namespace isotherm
