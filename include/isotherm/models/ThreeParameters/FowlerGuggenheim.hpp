//==============================================================================
// Name        : FowlerGuggenheim.hpp
// Authors     : Joao Flavio Vasconcellos
// Version     : 2.0.0
// Description : Implicit Fowler–Guggenheim branch with non-negative interaction energy. Temperature is an independent input.
//
// Copyright   : Copyright (C) 2026 Joao Flavio Vasconcellos
// Licence     : MIT License (see LICENSE.txt)
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
#include "isotherm/core/gas_constant.hpp"
#include "isotherm/core/traits.hpp"
#include "isotherm/core/type.hpp"
#include "isotherm/core/validation.hpp"
#include "isotherm/models/ThreeParameters/detail/coverage_solver.hpp"

namespace isotherm::models {
/** @brief Implicit Fowler–Guggenheim branch with non-negative interaction energy. Temperature is an independent input. */
class FowlerGuggenheim final {
public:
    FowlerGuggenheim(Real p1, Real p2, Real p3,
        gas_constant_unit energy_unit = gas_constant_unit::joule_per_mole_kelvin)
        : energy_unit_(energy_unit), gas_constant_(molar_gas_constant(energy_unit)), a_(p1), b_(p2), c_(p3) {
        require_positive(a_, "FowlerGuggenheim a");
        require_positive(b_, "FowlerGuggenheim b");
        require_nonnegative(c_, "FowlerGuggenheim interaction energy");
    }

    [[nodiscard]] Real evaluate(Real x, Real temperature) const {
        return a_ * coverage(x, temperature);
    }
    [[nodiscard]] Real derivative(Real x, Real temperature) const {
        require_positive(x, "concentration for the derivative");
        require_positive(temperature, "temperature in kelvin");
        const Real interaction = c_ / (gas_constant_ * temperature);
        const Real theta = coverage(x, temperature);
        const Real log_slope = 1.0 / theta + 1.0 / (1.0 - theta) + interaction;
        return a_ / (x * log_slope);
    }

    /** @brief Parameters in the order of model_traits::parameter_names. */
    [[nodiscard]] std::array<Real, 3> parameters() const noexcept {
        return {a_, b_, c_};
    }

    /** @brief Copy with new parameters; the constructor validates them. The energy unit is kept. */
    [[nodiscard]] FowlerGuggenheim with_parameters(const std::array<Real, 3>& p) const {
        return FowlerGuggenheim{p[0], p[1], p[2], energy_unit_};
    }

private:
    [[nodiscard]] Real coverage(Real x, Real temperature) const {
        require_nonnegative(x, "equilibrium concentration");
        require_positive(temperature, "temperature in kelvin");
        if (x == 0.0) return 0.0;
        const Real interaction = c_ / (gas_constant_ * temperature);
        const Real log_activity = std::log(b_) + std::log(x);
        return detail::solve_coverage(log_activity, [&](Real theta) {
            return std::log(theta) - std::log1p(-theta) + theta * interaction;
        });
    }
    gas_constant_unit energy_unit_;
    Real gas_constant_;
    Real a_, b_, c_;
};
} // namespace isotherm::models

namespace isotherm {
template<> struct model_traits<models::FowlerGuggenheim> {
    static constexpr std::size_t parameter_count = 3;
    static constexpr std::array<std::string_view, 3> parameter_names{"q_max", "K", "W"};
    static constexpr std::string_view identifier = "fowler_guggenheim";
    static constexpr independent_variable variable = independent_variable::concentration;
    static constexpr equation_type equation = equation_type::implicit_equation;
    static constexpr bool temperature_dependent = true;
};
} // namespace isotherm
