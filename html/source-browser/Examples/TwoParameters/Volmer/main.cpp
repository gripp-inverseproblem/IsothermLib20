//==============================================================================
// Name        : main.cpp
// Authors     : Joao Flavio Vasconcellos
// Version     : 2.0.0
// Description : C++20 uptake, independent-variable derivative and numerical parameter sensitivities for Volmer.
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
using isotherm::Real;
using Model = isotherm::models::Volmer;

/** @brief Central differences across independent model instances, never an analytic parameter derivative. */
[[nodiscard]] Real scaled_sensitivity(Real first, Real second, Real x, int parameter) {
    const Real nominal = parameter == 0 ? first : second;
    const Real step = nominal * std::cbrt(std::numeric_limits<Real>::epsilon());
    const Real lower = nominal - step;
    const Real upper = nominal + step;
    const Model below{parameter == 0 ? lower : first, parameter == 1 ? lower : second};
    const Model above{parameter == 0 ? upper : first, parameter == 1 ? upper : second};
    return nominal * (isotherm::evaluate(above, x) - isotherm::evaluate(below, x))
           / (upper - lower);
}
} // namespace

int main() {
    using isotherm::Real;
    constexpr Real first = 4.5;
    constexpr Real second = 0.7;
    const isotherm::models::Volmer model{first, second};
    std::cout << "concentration,uptake,derivative,sensitivity_1,sensitivity_2\n"
              << std::setprecision(std::numeric_limits<Real>::max_digits10);
    for (int index = 0; index < 81; ++index) {
        const Real concentration = Real{0.1} + static_cast<Real>(index) * Real{0.04};
        std::cout << concentration << ',' << isotherm::evaluate(model, concentration)
                  << ',' << isotherm::derivative(model, concentration)
                  << ',' << scaled_sensitivity(first, second, concentration, 0)
                  << ',' << scaled_sensitivity(first, second, concentration, 1) << '\n';
    }
}
