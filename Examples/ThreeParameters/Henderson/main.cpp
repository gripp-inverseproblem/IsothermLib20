//==============================================================================
// Name        : main.cpp
// Authors     : Joao Flavio Vasconcellos
// Version     : 2.0.0
// Description : Henderson three-parameter thermal-section data via compiled C++20 models.
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
#include <iomanip>
#include <iostream>
#include <limits>

//==============================================================================
// Isotherm++ headers
//==============================================================================
#include "isotherm/isotherm.hpp"

namespace {
using isotherm::Real;
using Model = isotherm::models::Henderson;
using Parameters = std::array<Real, 3>;

[[nodiscard]] Real uptake(const Parameters& parameters, Real temperature_k) {
    return isotherm::evaluate(Model{parameters[0], parameters[1], parameters[2]}, temperature_k);
}

[[nodiscard]] Real scaled_sensitivity(Parameters parameters, Real temperature_k, std::size_t index) {
    const Real nominal = parameters.at(index);
    const Real h = nominal * std::cbrt(std::numeric_limits<Real>::epsilon());
    parameters[index] = nominal + h;
    const Real above = uptake(parameters, temperature_k);
    parameters[index] = nominal - h;
    const Real below = uptake(parameters, temperature_k);
    return nominal * (above - below) / (2 * h);
}
} // namespace

int main() {
    using isotherm::Real;
    constexpr Parameters parameters{Real{0.015}, Real{1.7}, Real{0.65}};
    const Model model{parameters[0], parameters[1], parameters[2]};
    std::cout << "temperature_k,uptake,derivative,sensitivity_1,sensitivity_2,sensitivity_3\n"
              << std::setprecision(std::numeric_limits<Real>::max_digits10);
    for (int index = 0; index < 81; ++index) {
        const Real temperature = Real{273.15} + Real{0.625} * static_cast<Real>(index);
        std::cout << temperature << ',' << isotherm::evaluate(model, temperature)
                  << ',' << isotherm::derivative(model, temperature)
                  << ',' << scaled_sensitivity(parameters, temperature, 0)
                  << ',' << scaled_sensitivity(parameters, temperature, 1)
                  << ',' << scaled_sensitivity(parameters, temperature, 2) << '\n';
    }
}
