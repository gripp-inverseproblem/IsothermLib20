//==============================================================================
// Name        : main.cpp
// Authors     : Joao Flavio Vasconcellos
// Version     : 2.0.0
// Description : Generate independently reproducible Temkin values as CSV.
//
// Copyright   : Copyright (C) 2026 Joao Flavio Vasconcellos
// Licence     : MIT License (see LICENSE.txt)
// SPDX-License-Identifier: MIT
//==============================================================================

//==============================================================================
// C++ standard library headers
//==============================================================================
#include <cmath>
#include <iomanip>
#include <iostream>
#include <limits>
//==============================================================================
// Isotherm++ headers
//==============================================================================
#include "isotherm/isotherm.hpp"

namespace {
/** @brief Scaled sensitivity to affinity using only the public model evaluation. */
[[nodiscard]] isotherm::Real sensitivity_affinity(
    isotherm::Real affinity, isotherm::Real energy,
    isotherm::Real concentration, isotherm::Real temperature) {
    using isotherm::Real;
    const Real step = affinity * std::cbrt(std::numeric_limits<Real>::epsilon());
    const isotherm::models::Temkin minus{affinity - step, energy};
    const isotherm::models::Temkin plus{affinity + step, energy};
    return affinity * (isotherm::evaluate(plus, concentration, temperature) -
                       isotherm::evaluate(minus, concentration, temperature)) /
           ((affinity + step) - (affinity - step));
}

/** @brief Scaled sensitivity to the energy coefficient via model instances. */
[[nodiscard]] isotherm::Real sensitivity_energy(
    isotherm::Real affinity, isotherm::Real energy,
    isotherm::Real concentration, isotherm::Real temperature) {
    using isotherm::Real;
    const Real step = energy * std::cbrt(std::numeric_limits<Real>::epsilon());
    const isotherm::models::Temkin minus{affinity, energy - step};
    const isotherm::models::Temkin plus{affinity, energy + step};
    return energy * (isotherm::evaluate(plus, concentration, temperature) -
                     isotherm::evaluate(minus, concentration, temperature)) /
           ((energy + step) - (energy - step));
}
} // namespace

/** @brief Generate reproducible values, slopes and parameter sensitivities for Temkin. */
int main() {
    constexpr isotherm::Real affinity = 4.0;
    constexpr isotherm::Real energy = 1800.0;
    const isotherm::models::Temkin model{affinity, energy};
    constexpr isotherm::Real temperature = 298.15; // ALWAYS kelvin (K).
    // The same physical R can be expressed in different energy units.
    // b_T must be converted consistently with R*T:
    const isotherm::models::Temkin model_kj{
        4.0, 1.8, isotherm::gas_constant_unit::kilojoule_per_mole_kelvin};
    const isotherm::models::Temkin model_latm{
        4.0, 1800.0 / 101.325,
        isotherm::gas_constant_unit::litre_atmosphere_per_mole_kelvin};
    static_cast<void>(model_kj);
    static_cast<void>(model_latm);
    std::cout << "concentration,uptake,derivative,sensitivity_kt,sensitivity_bt\n";
    std::cout << std::setprecision(17);
    for (int index = 0; index <= 80; ++index) {
        const isotherm::Real concentration = 0.5 + static_cast<isotherm::Real>(index) / 30.0;
        std::cout << concentration << ',' << isotherm::evaluate(model, concentration, temperature)
                  << ',' << isotherm::derivative(model, concentration, temperature)
                  << ',' << sensitivity_affinity(affinity, energy, concentration, temperature)
                  << ',' << sensitivity_energy(affinity, energy, concentration, temperature)
                  << '\n';
    }
}
