//==============================================================================
// Name        : Temkin.hpp
// Authors     : Joao Flavio Vasconcellos
// Version     : 2.0.0
// Description : Two-parameter Temkin formulation with strict domain validation.
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

#include "isotherm/core/gas_constant.hpp"
#include "isotherm/core/traits.hpp"
#include "isotherm/core/type.hpp"
#include "isotherm/core/validation.hpp"

namespace isotherm::models {

/** @brief Thermal two-parameter Temkin model, evaluated at a supplied absolute temperature; temperature does not count as a fitted parameter. */
class Temkin final {
public:
    explicit Temkin(
        Real affinity, Real energy_constant,
        gas_constant_unit energy_unit = gas_constant_unit::joule_per_mole_kelvin)
        : energy_unit_(energy_unit), gas_constant_(molar_gas_constant(energy_unit)),
          affinity_(affinity), energy_constant_(energy_constant) {
        require_positive(affinity_, "Temkin affinity");
        require_positive(energy_constant_, "Temkin energy constant");
    }

    /** @brief Evaluate the model on its documented mathematical domain. */
    [[nodiscard]] Real evaluate(Real concentration, Real temperature) const {
        require_positive(concentration, "equilibrium concentration");
        require_positive(temperature, "temperature in kelvin");
        const Real log_argument = std::log(affinity_) + std::log(concentration);
        if (log_argument < 0.0) throw isotherm_error(error_code::outside_domain, "Temkin uptake would be negative (K*C < 1)");
        return (gas_constant_ * temperature / energy_constant_) * log_argument;
    }

    /** @brief Analytical derivative with respect to concentration. */
    [[nodiscard]] Real derivative(Real concentration, Real temperature) const {
        require_positive(concentration, "equilibrium concentration");
        require_positive(temperature, "temperature in kelvin");
        if (std::log(affinity_) + std::log(concentration) < 0.0) throw isotherm_error(error_code::outside_domain, "Temkin uptake would be negative (K*C < 1)");
        return gas_constant_ * temperature / (energy_constant_ * concentration);
    }

    /** @brief Parameters in the order of model_traits::parameter_names. */
    [[nodiscard]] std::array<Real, 2> parameters() const noexcept {
        return {affinity_, energy_constant_};
    }

    /** @brief Copy with new parameters; the constructor validates them. The energy unit is kept. */
    [[nodiscard]] Temkin with_parameters(const std::array<Real, 2>& p) const {
        return Temkin{p[0], p[1], energy_unit_};
    }

private:
    gas_constant_unit energy_unit_;
    Real gas_constant_;
    Real affinity_;
    Real energy_constant_;
};

} // namespace isotherm::models

namespace isotherm {

/** @brief Compile-time traits for the Temkin formulation. */
template<> struct model_traits<models::Temkin> {
    static constexpr std::size_t parameter_count = 2;
    static constexpr std::array<std::string_view, 2> parameter_names{"K_T", "b_T"};
    static constexpr std::string_view identifier = "temkin";
    static constexpr independent_variable variable = independent_variable::concentration;
    static constexpr equation_type equation = equation_type::explicit_equation;
    static constexpr bool temperature_dependent = true;
};

} // namespace isotherm
