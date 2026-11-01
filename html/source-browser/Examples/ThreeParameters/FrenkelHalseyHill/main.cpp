//==============================================================================
// Name        : main.cpp
// Authors     : Joao Flavio Vasconcellos
// Version     : 2.0.0
// Description : Compiled concentration-based FHH curve and sensitivities.
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
#include <iomanip>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <utility>
//==============================================================================
// Isotherm++ headers
//==============================================================================
#include "isotherm/isotherm.hpp"

namespace {
using isotherm::Real;
using Model = isotherm::models::FrenkelHalseyHill;
constexpr std::size_t parameter_count = 3;
using Parameters = std::array<Real, parameter_count>;

[[nodiscard]] Real uptake_from_class(const Parameters& parameters, Real C_e) {
    return std::apply([C_e](auto... coefficients) {
        const Model model{coefficients...};
        return isotherm::evaluate(model, C_e);
    }, parameters);
}

[[nodiscard]] Real numerical_scaled_sensitivity(
    Parameters values, Real C_e, std::size_t index) {
    const Real nominal = values.at(index);
    const Real step = std::abs(nominal) *
        std::cbrt(std::numeric_limits<Real>::epsilon());
    const Real upper = nominal + step;
    const Real lower = nominal - step;
    const Real baseline = uptake_from_class(values, C_e);
    bool upper_valid = false;
    bool lower_valid = false;
    Real upper_value = Real{0};
    Real lower_value = Real{0};
    try {
        values[index] = upper;
        upper_value = uptake_from_class(values, C_e);
        upper_valid = std::isfinite(upper_value);
    } catch (const isotherm::isotherm_error&) { }
    try {
        values[index] = lower;
        lower_value = uptake_from_class(values, C_e);
        lower_valid = std::isfinite(lower_value);
    } catch (const isotherm::isotherm_error&) { }
    if (upper_valid && lower_valid) {
        return nominal * (upper_value - lower_value) / (upper - lower);
    }
    if (upper_valid) {
        return nominal * (upper_value - baseline) / (upper - nominal);
    }
    if (lower_valid) {
        return nominal * (baseline - lower_value) / (nominal - lower);
    }
    throw std::runtime_error("No valid perturbation for FHH sensitivity");
}
} // namespace

int main() {
    // Illustrative, dimensionally consistent concentration convention.
    const Parameters parameters{Real{2.4}, Real{1.2}, Real{2.5}};
    const Model model{parameters[0], parameters[1], parameters[2]};
    std::cout << "concentration,uptake,derivative,sensitivity_1,"
                 "sensitivity_2,sensitivity_3\n" << std::setprecision(17);
    for (int i = 0; i <= 80; ++i) {
        const Real C_e = Real{0.1} + i * Real{0.01};
        std::cout << C_e << ','
                  << isotherm::evaluate(model, C_e) << ','
                  << isotherm::derivative(model, C_e);
        for (std::size_t j = 0; j != parameter_count; ++j) {
            std::cout << ',' << numerical_scaled_sensitivity(parameters,
                                                               C_e, j);
        }
        std::cout << '\n';
    }
}
