//==============================================================================
// Name        : main.cpp
// Authors     : Joao Flavio Vasconcellos
// Version     : 2.0.0
// Description : Smith temperature response, derivative and scaled sensitivities.
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
using Model = isotherm::models::Smith;
using Parameters = std::array<Real, 4>;

[[nodiscard]] Real uptake(const Parameters& p, Real temperature_k) {
    return isotherm::evaluate(Model{p[0], p[1], p[2], p[3]}, temperature_k);
}

[[nodiscard]] Real scaled_sensitivity(Parameters parameters,
                                      Real temperature_k, std::size_t index) {
    const Real nominal = parameters.at(index);
    const Real step = std::max(std::abs(nominal), Real{1})
                    * std::cbrt(std::numeric_limits<Real>::epsilon());
    parameters[index] = nominal + step;
    const Real positive = uptake(parameters, temperature_k);
    const Real upper = parameters[index];
    parameters[index] = nominal - step;
    try {
        const Real negative = uptake(parameters, temperature_k);
        return nominal * (positive - negative) / (upper - parameters[index]);
    } catch (const isotherm::isotherm_error&) {
        // Use a one-sided difference when a perturbed parameter is inadmissible.
        parameters[index] = nominal;
        return nominal * (positive - uptake(parameters, temperature_k))
               / (upper - nominal);
    }
}
} // namespace

int main() {
    constexpr Parameters parameters{1.4, -0.001, 0.55, 0.5};
    const Model model{parameters[0], parameters[1], parameters[2], parameters[3]};
    std::cout << "temperature_k,uptake,derivative,sensitivity_1,sensitivity_2,"
                 "sensitivity_3,sensitivity_4\n"
              << std::setprecision(std::numeric_limits<Real>::max_digits10);
    for (int index = 0; index < 81; ++index) {
        const Real temperature = Real{273.15}
            + static_cast<Real>(index) * Real{0.625};
        std::cout << temperature << ','
                  << isotherm::evaluate(model, temperature) << ','
                  << isotherm::derivative(model, temperature);
        for (std::size_t parameter = 0; parameter < parameters.size(); ++parameter) {
            std::cout << ',' << scaled_sensitivity(parameters, temperature, parameter);
        }
        std::cout << '\n';
    }
}
