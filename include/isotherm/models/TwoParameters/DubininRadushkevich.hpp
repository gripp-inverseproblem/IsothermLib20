//==============================================================================
// Name        : DubininRadushkevich.hpp
// Authors     : Joao Flavio Vasconcellos
// Version     : 2.0.0
// Description : Two-parameter DubininRadushkevich formulation with strict domain validation.
//
// Copyright   : Copyright (C) 2026 Joao Flavio Vasconcellos
// Licence     : MIT License (see LICENSE)
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

#include "isotherm/core/gas_constant.hpp"
#include "isotherm/core/traits.hpp"
#include "isotherm/core/type.hpp"
#include "isotherm/core/validation.hpp"

namespace isotherm::models {

/** @brief Dubinin–Radushkevich pore-filling formulation with temperature as an independent input, not a fitted parameter. The standard concentration-based Polanyi potential is meaningful only with a dimensionless concentration ratio. */
class DubininRadushkevich final {
public:
    explicit DubininRadushkevich(
        Real capacity, Real energy_coefficient,
        gas_constant_unit energy_unit = gas_constant_unit::joule_per_mole_kelvin)
        : energy_unit_(energy_unit), gas_constant_(molar_gas_constant(energy_unit)),
          capacity_(capacity), energy_coefficient_(energy_coefficient) {
        require_positive(capacity_, "DubininRadushkevich capacity");
        require_positive(energy_coefficient_, "DubininRadushkevich energy coefficient");
    }

    /** @brief Evaluate the model on its documented mathematical domain. */
    [[nodiscard]] Real evaluate(Real concentration, Real temperature) const {
        require_nonnegative(concentration, "dimensionless equilibrium concentration");
        require_positive(temperature, "temperature in kelvin");
        if (concentration == 0.0) return 0.0;
        // log(1 + 1/C) == log1p(C) - log(C), without reciprocal overflow.
        const Real epsilon = gas_constant_ * temperature * (std::log1p(concentration) - std::log(concentration));
        return capacity_ * std::exp(-energy_coefficient_ * epsilon * epsilon);
    }

    /** @brief Analytical derivative with respect to concentration. */
    [[nodiscard]] Real derivative(Real concentration, Real temperature) const {
        require_nonnegative(concentration, "dimensionless equilibrium concentration");
        require_positive(temperature, "temperature in kelvin");
        if (concentration == 0.0) return 0.0;
        const Real epsilon = gas_constant_ * temperature * (std::log1p(concentration) - std::log(concentration));
        const Real value = evaluate(concentration, temperature);
        // Form the derivative from logarithmic uptake to avoid 0 * infinity.
        if (value == 0.0) return 0.0;
        const Real log_slope = std::log(2.0) + std::log(energy_coefficient_) +
            std::log(epsilon) + std::log(gas_constant_) + std::log(temperature) -
            std::log(concentration) - std::log1p(concentration);
        return std::exp(std::log(value) + log_slope);
    }

    /** @brief Parameters in the order of model_traits::parameter_names. */
    [[nodiscard]] std::array<Real, 2> parameters() const noexcept {
        return {capacity_, energy_coefficient_};
    }

    /** @brief Copy with new parameters; the constructor validates them. The energy unit is kept. */
    [[nodiscard]] DubininRadushkevich with_parameters(const std::array<Real, 2>& p) const {
        return DubininRadushkevich{p[0], p[1], energy_unit_};
    }

private:
    gas_constant_unit energy_unit_;
    Real gas_constant_;
    Real capacity_;
    Real energy_coefficient_;
};

} // namespace isotherm::models

namespace isotherm {

/** @brief Compile-time traits for the DubininRadushkevich formulation. */
template<> struct model_traits<models::DubininRadushkevich> {
    static constexpr std::size_t parameter_count = 2;
    static constexpr std::array<std::string_view, 2> parameter_names{"q_s", "beta"};
    static constexpr std::string_view identifier = "dubinin_radushkevich";
    static constexpr independent_variable variable = independent_variable::concentration;
    static constexpr equation_type equation = equation_type::explicit_equation;
    static constexpr bool temperature_dependent = true;
};

} // namespace isotherm
