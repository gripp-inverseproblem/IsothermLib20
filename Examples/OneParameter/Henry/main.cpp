//==============================================================================
// Name        : main.cpp
// Authors     : Joao Flavio Vasconcellos
// Version     : 2.0.0
// Description : Generate validated Henry concentration and uptake data as CSV.
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

// Central difference of q_e with respect to K_H; never use the analytical
// concentration derivative to evaluate parameter sensitivity.
[[nodiscard]] isotherm::Real numerical_scaled_sensitivity(
    isotherm::Real coefficient, isotherm::Real concentration) {
    using isotherm::Real;
    const Real relative_step = std::cbrt(std::numeric_limits<Real>::epsilon());
    const Real step = coefficient * relative_step;
    const isotherm::models::Henry lower{coefficient - step};
    const isotherm::models::Henry upper{coefficient + step};
    const Real difference = (isotherm::evaluate(upper, concentration) -
                             isotherm::evaluate(lower, concentration)) /
                            ((coefficient + step) - (coefficient - step));
    return coefficient * difference;
}

} // namespace

/** @brief Reproducible reference and numerical sensitivity data. */
int main() {
    const isotherm::models::Henry model{1.25};
    std::cout << "concentration,uptake,derivative,sensitivity_kh\n";
    std::cout << std::setprecision(17);
    for (int index = 0; index <= 120; ++index) {
        const isotherm::Real concentration = static_cast<isotherm::Real>(index) / 20.0;
        std::cout << concentration << ','
                  << isotherm::evaluate(model, concentration) << ','
                  << isotherm::derivative(model, concentration) << ','
                  << numerical_scaled_sensitivity(model.coefficient(), concentration) << '\n';
    }
}
