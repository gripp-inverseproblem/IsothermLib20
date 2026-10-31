//==============================================================================
// Name        : main.cpp
// Authors     : Joao Flavio Vasconcellos
// Version     : 2.0.0
// Description : Generate independently reproducible DubininRadushkevich values as CSV.
//
// Copyright   : Copyright (C) 2026 Joao Flavio Vasconcellos
// Licence     : MIT License (see LICENSE)
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

// Parameter sensitivities come from central differences of class evaluations.
// Temperature is held fixed; it is an independent input, not a fitted parameter.
[[nodiscard]] isotherm::Real scaled_sensitivity_capacity(
    isotherm::Real capacity, isotherm::Real beta,
    isotherm::Real concentration, isotherm::Real temperature) {
    using isotherm::Real;
    const Real step = capacity * std::cbrt(std::numeric_limits<Real>::epsilon());
    const isotherm::models::DubininRadushkevich lower{capacity - step, beta};
    const isotherm::models::DubininRadushkevich upper{capacity + step, beta};
    return capacity * (isotherm::evaluate(upper, concentration, temperature) -
                       isotherm::evaluate(lower, concentration, temperature)) /
           ((capacity + step) - (capacity - step));
}

[[nodiscard]] isotherm::Real scaled_sensitivity_beta(
    isotherm::Real capacity, isotherm::Real beta,
    isotherm::Real concentration, isotherm::Real temperature) {
    using isotherm::Real;
    const Real step = beta * std::cbrt(std::numeric_limits<Real>::epsilon());
    const isotherm::models::DubininRadushkevich lower{capacity, beta - step};
    const isotherm::models::DubininRadushkevich upper{capacity, beta + step};
    return beta * (isotherm::evaluate(upper, concentration, temperature) -
                   isotherm::evaluate(lower, concentration, temperature)) /
           ((beta + step) - (beta - step));
}

} // namespace

/** @brief Generate reproducible values, slopes and numerical sensitivities. */
int main() {
    using isotherm::Real;
    constexpr Real capacity = 2.7;
    constexpr Real beta = 0.000001;
    constexpr Real temperature = 298.15; // ALWAYS kelvin (K).
    // Equivalent choice: R in kJ/(mol K) requires beta in mol^2/kJ^2.
    const isotherm::models::DubininRadushkevich model_kj{
        capacity, beta * Real{1000000.0},
        isotherm::gas_constant_unit::kilojoule_per_mole_kelvin};
    // Another equivalent choice is R in L atm/(mol K):
    const isotherm::models::DubininRadushkevich model_latm{
        capacity, beta * Real{101.325} * Real{101.325},
        isotherm::gas_constant_unit::litre_atmosphere_per_mole_kelvin};
    // These local models demonstrate unit selection, without altering the CSV
    // schema consumed by the figures and reproducibility checks.
    static_cast<void>(model_kj);
    static_cast<void>(model_latm);
    const isotherm::models::DubininRadushkevich model{capacity, beta};
    std::cout << "concentration,uptake,derivative,sensitivity_qs,sensitivity_beta\n"
              << std::setprecision(17);
    // Preserve the existing concentration range and its 81 samples.
    for (int index = 0; index <= 80; ++index) {
        const Real concentration = 0.04 + static_cast<Real>(index) / 65.0;
        std::cout << concentration << ','
                  << isotherm::evaluate(model, concentration, temperature) << ','
                  << isotherm::derivative(model, concentration, temperature) << ','
                  << scaled_sensitivity_capacity(capacity, beta, concentration, temperature) << ','
                  << scaled_sensitivity_beta(capacity, beta, concentration, temperature) << '\n';
    }
}
