//==============================================================================
// Name        : main.cpp
// Authors     : Joao Flavio Vasconcellos
// Version     : 2.0.0
// Description : Generate scientific CSV observations from compiled Baudu C++20 code.
//
// Copyright   : Copyright (C) 2026 Joao Flavio Vasconcellos
// Licence     : MIT License (see LICENSE)
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
#include <stdexcept>
#include <utility>
//==============================================================================
// Isotherm++ headers
//==============================================================================
#include "isotherm/isotherm.hpp"

namespace {
using isotherm::Real;
using Model = isotherm::models::Baudu;
constexpr std::size_t parameter_count = 4;
using Parameters = std::array<Real, parameter_count>;

[[nodiscard]] Real uptake_from_class(const Parameters& values, Real concentration) {
    return std::apply([concentration](auto... coefficients) {
        const Model model{coefficients...};
        return isotherm::evaluate(model, concentration);
    }, values);
}

[[nodiscard]] Real numerical_scaled_sensitivity(Parameters values, Real concentration, std::size_t index) {
    const Real nominal = values.at(index);
    // Use a relative numerical increment, with the actual perturbed values in
    // denominators to avoid cancellation from rounded parameter positions.
    const Real step = std::abs(nominal) *
        std::cbrt(std::numeric_limits<Real>::epsilon());
    const Real upper = nominal + step;
    const Real lower = nominal - step;
    const Real baseline = uptake_from_class(values, concentration);
    bool upper_valid = false;
    bool lower_valid = false;
    Real upper_value = Real{0};
    Real lower_value = Real{0};
    try {
        values[index] = upper;
        upper_value = uptake_from_class(values, concentration);
        upper_valid = std::isfinite(upper_value);
    } catch (const isotherm::isotherm_error&) { }
    try {
        values[index] = lower;
        lower_value = uptake_from_class(values, concentration);
        lower_valid = std::isfinite(lower_value);
    } catch (const isotherm::isotherm_error&) { }
    if (upper_valid && lower_valid)
        return nominal * (upper_value - lower_value) / (upper - lower);
    if (upper_valid)
        return nominal * (upper_value - baseline) / (upper - nominal);
    if (lower_valid)
        return nominal * (baseline - lower_value) / (nominal - lower);
    throw std::runtime_error("No valid parameter perturbation for numerical sensitivity");
}
} // namespace

int main() {
    const isotherm::models::Baudu model{5, 0.6, 0.4, 0.35};
    const std::array<isotherm::Real, 4> parameters{5, 0.6, 0.4, 0.35};
    std::cout << "concentration,uptake,derivative,sensitivity_1,sensitivity_2,sensitivity_3,sensitivity_4\n" << std::setprecision(17);
    for (int index = 0; index <= 80; ++index) {
        const isotherm::Real x = 0.1 + static_cast<isotherm::Real>(index) * 0.04;
        std::cout << x << ',' << isotherm::evaluate(model, x)
                  << ',' << isotherm::derivative(model, x)  << ',' << numerical_scaled_sensitivity(parameters, x, 0) << ',' << numerical_scaled_sensitivity(parameters, x, 1) << ',' << numerical_scaled_sensitivity(parameters, x, 2) << ',' << numerical_scaled_sensitivity(parameters, x, 3) << '\n';
    }
}
