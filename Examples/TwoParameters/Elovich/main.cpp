//==============================================================================
// Name        : main.cpp
// Authors     : Joao Flavio Vasconcellos
// Version     : 2.0.0
// Description : Generate independently reproducible Elovich values as CSV.
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

/** @brief Central-difference scaled sensitivity from the public model API. */
[[nodiscard]] isotherm::Real scaled_sensitivity_reference(
    isotherm::Real capacity, isotherm::Real affinity,
    isotherm::Real concentration) {
    using isotherm::Real;
    const Real step = capacity * std::cbrt(std::numeric_limits<Real>::epsilon());
    const isotherm::models::Elovich minus{capacity - step, affinity};
    const isotherm::models::Elovich plus{capacity + step, affinity};
    return capacity * (isotherm::evaluate(plus, concentration) -
                       isotherm::evaluate(minus, concentration)) /
           ((capacity + step) - (capacity - step));
}

[[nodiscard]] isotherm::Real scaled_sensitivity_affinity(
    isotherm::Real capacity, isotherm::Real affinity,
    isotherm::Real concentration) {
    using isotherm::Real;
    const Real step = affinity * std::cbrt(std::numeric_limits<Real>::epsilon());
    const isotherm::models::Elovich minus{capacity, affinity - step};
    const isotherm::models::Elovich plus{capacity, affinity + step};
    return affinity * (isotherm::evaluate(plus, concentration) -
                       isotherm::evaluate(minus, concentration)) /
           ((affinity + step) - (affinity - step));
}

} // namespace

/** @brief Generate reproducible uptake, concentration slope and sensitivities. */
int main() {
    using isotherm::Real;
    constexpr Real capacity = 2.4;
    constexpr Real affinity = 0.8;
    const isotherm::models::Elovich model{capacity, affinity};
    std::cout << "concentration,uptake,derivative,sensitivity_qref,sensitivity_ke\n"
              << std::setprecision(17);
    for (int index = 0; index <= 80; ++index) {
        const Real concentration = static_cast<Real>(index) / Real{120.0};
        std::cout << concentration << ','
                  << isotherm::evaluate(model, concentration) << ','
                  << isotherm::derivative(model, concentration) << ','
                  << scaled_sensitivity_reference(capacity, affinity, concentration) << ','
                  << scaled_sensitivity_affinity(capacity, affinity, concentration) << '\n';
    }
}
