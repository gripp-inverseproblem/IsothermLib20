//==============================================================================
// Name        : main.cpp
// Authors     : Joao Flavio Vasconcellos
// Version     : 2.0.0
// Description : Compiled Anti-Langmuir response and scaled sensitivities as CSV.
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

// Parameter derivatives are estimated by central differences of uptake,
// not by the analytical derivative with respect to concentration.
[[nodiscard]] isotherm::Real scaled_sensitivity_h(
    isotherm::Real H, isotherm::Real b, isotherm::Real concentration) {
    using isotherm::Real;
    const Real h = H * std::cbrt(std::numeric_limits<Real>::epsilon());
    const isotherm::models::AntiLangmuir lower{H - h, b};
    const isotherm::models::AntiLangmuir upper{H + h, b};
    return H * (isotherm::evaluate(upper, concentration) -
                isotherm::evaluate(lower, concentration)) /
           ((H + h) - (H - h));
}

[[nodiscard]] isotherm::Real scaled_sensitivity_b(
    isotherm::Real H, isotherm::Real b, isotherm::Real concentration) {
    using isotherm::Real;
    const Real h = b * std::cbrt(std::numeric_limits<Real>::epsilon());
    const isotherm::models::AntiLangmuir lower{H, b - h};
    const isotherm::models::AntiLangmuir upper{H, b + h};
    return b * (isotherm::evaluate(upper, concentration) -
                isotherm::evaluate(lower, concentration)) /
           ((b + h) - (b - h));
}

} // namespace

int main() {
    const isotherm::Real H = 1.6;
    const isotherm::Real b = 0.12;
    const isotherm::models::AntiLangmuir model{H, b};
    std::cout << "concentration,uptake,derivative,sensitivity_h,sensitivity_b\n"
              << std::setprecision(17);
    // Retain the existing 81-concentration range and sampling grid.
    for (int i = 0; i <= 80; ++i) {
        const isotherm::Real x = 0.1 + i * 0.04;
        std::cout << x << ',' << isotherm::evaluate(model, x) << ','
                  << isotherm::derivative(model, x) << ','
                  << scaled_sensitivity_h(H, b, x) << ','
                  << scaled_sensitivity_b(H, b, x) << '\n';
    }
}
