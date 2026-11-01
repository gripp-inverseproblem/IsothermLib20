//==============================================================================
// Name        : main.cpp
// Authors     : Joao Flavio Vasconcellos
// Version     : 2.0.0
// Description : Evaluate four-parameter Oswin and numeric sensitivities.
//
// Copyright   : Copyright (C) 2026 Joao Flavio Vasconcellos
// Licence     : MIT License (see LICENSE.txt)
// SPDX-License-Identifier: MIT
//==============================================================================

//==============================================================================
// C++ standard library headers
//==============================================================================
#include <algorithm>
#include <array>
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
using Model = isotherm::models::Oswin;

[[nodiscard]] Real uptake_from_class(const std::array<Real, 4>& values,
                                     Real activity, Real temperature_k) {
    const Model model{values[0], values[1], values[2], values[3]};
    return isotherm::evaluate(model, activity, temperature_k);
}

[[nodiscard]] Real numerical_scaled_sensitivity(std::array<Real, 4> values,
                                                Real activity,
                                                Real temperature_k,
                                                std::size_t index) {
    const Real nominal = values.at(index);
    // Relative perturbation for nonzero parameters, with a fallback at zero.
    const Real scale = std::abs(nominal) > Real{0}
        ? std::abs(nominal) : Real{1};
    const Real step = scale
        * std::cbrt(std::numeric_limits<Real>::epsilon());
    values[index] = nominal + step;
    const Real positive = uptake_from_class(values, activity, temperature_k);
    const Real upper = values[index];
    values[index] = nominal - step;
    try {
        const Real negative = uptake_from_class(values, activity, temperature_k);
        return nominal * (positive - negative) / (upper - values[index]);
    } catch (const isotherm::isotherm_error&) {
        values[index] = nominal;
        const Real baseline = uptake_from_class(values, activity, temperature_k);
        return nominal * (positive - baseline) / (upper - nominal);
    }
}
} // namespace

int main() {
    constexpr std::array<Real, 4> parameters{2.0, -0.002, 1.7, 0.001};
    constexpr Real temperature_k = 300.0;  // Absolute temperature: K.
    const Model model{parameters[0], parameters[1],
                      parameters[2], parameters[3]};
    std::cout << "activity,uptake,derivative,sensitivity_1,sensitivity_2,"
                 "sensitivity_3,sensitivity_4\n"
              << std::setprecision(std::numeric_limits<Real>::max_digits10);
    for (int index = 0; index < 81; ++index) {
        const Real activity = Real{0.02}
            + static_cast<Real>(index) * Real{0.011625};
        std::cout << activity << ','
                  << isotherm::evaluate(model, activity, temperature_k) << ','
                  << isotherm::derivative(model, activity, temperature_k);
        for (std::size_t parameter = 0; parameter < parameters.size(); ++parameter) {
            std::cout << ',' << numerical_scaled_sensitivity(
                parameters, activity, temperature_k, parameter);
        }
        std::cout << '\n';
    }
}
