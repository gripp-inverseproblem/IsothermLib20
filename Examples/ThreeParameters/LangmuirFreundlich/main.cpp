//==============================================================================
// Name        : main.cpp
// Authors     : Joao Flavio Vasconcellos
// Version     : 2.0.0
// Description : Generate reproducible CSV observations using compiled LangmuirFreundlich.
//
// Copyright   : Copyright (C) 2026 Joao Flavio Vasconcellos
// Licence     : MIT License (see LICENSE)
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
using Model = isotherm::models::LangmuirFreundlich;

// Parameter sensitivities are numerical. The formula is never reimplemented.
[[nodiscard]] Real scaled_sensitivity(std::array<Real, 3> values, Real x, unsigned index) {
    const Real nominal = values.at(index);
    const Real step = std::max(std::abs(nominal), Real{1})
        * std::cbrt(std::numeric_limits<Real>::epsilon());
    values[index] = nominal + step;
    const Model above{values[0], values[1], values[2]};
    const Real upper = values[index];
    const Real positive = isotherm::evaluate(above, x);
    values[index] = nominal - step;
    try {
        const Model below{values[0], values[1], values[2]};
        const Real negative = isotherm::evaluate(below, x);
        return nominal * (positive - negative) / (upper - values[index]);
    } catch (const isotherm::isotherm_error&) {
        // A perturbed parameter may cross a physical domain boundary.
        values[index] = nominal;
        const Model baseline{values[0], values[1], values[2]};
        return nominal * (positive - isotherm::evaluate(baseline, x)) / (upper - nominal);
    }
}
} // namespace

int main() {
    const isotherm::models::LangmuirFreundlich model{5, 0.65, 1.3};
    const std::array<isotherm::Real, 3> nominal_parameters{5, 0.65, 1.3};
    std::cout << "concentration,uptake,derivative,sensitivity_1,sensitivity_2,sensitivity_3\n";
    std::cout << std::setprecision(17);
    for (int index = 0; index <= 80; ++index) {
        const isotherm::Real x = 0.1 + static_cast<isotherm::Real>(index) * 0.04;
        std::cout << x << ',' << isotherm::evaluate(model, x)
                  << ',' << isotherm::derivative(model, x)  << ',' << scaled_sensitivity(nominal_parameters, x, 0) << ',' << scaled_sensitivity(nominal_parameters, x, 1) << ',' << scaled_sensitivity(nominal_parameters, x, 2) << '\n';
    }
}
