//==============================================================================
// Name        : smoke_main.cpp
// Authors     : Joao Flavio Vasconcellos
// Version     : 2.0.0
// Description : Standalone CTest verification of the model core.
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
#include <limits>
#include <span>


//==============================================================================
// Local headers
//==============================================================================

#include "probe.hpp"

//==============================================================================
// Standalone smoke test: model evaluation and error propagation
//==============================================================================

/**
 * @brief Runs dependency-free CTest checks for the C++20 model core.
 * @return Zero on success; a positive code identifying the first failed check.
 */
int main() {
    using namespace isotherm;
    const contract_probe::linear_model model{2.0};
    if (std::abs(evaluate(model, 3.0) - 6.0) > 1e-12) return 1;
    const std::array<double, 3> xs{0.0, 1.0, 2.0};
    std::array<double, 3> ys{};
    evaluate_into(model, std::span<const double>{xs}, std::span<double>{ys});
    if (ys != std::array<double, 3>{0.0, 2.0, 4.0}) return 2;
    if (std::abs(evaluate(contract_probe::thermal_model{}, 10.0, 300.0) - (1.0 / 30.0)) > 1e-12) return 3;
    try { (void)evaluate(model, std::numeric_limits<double>::infinity()); return 4; }
    catch (const isotherm_error& ex) { if (ex.code() != error_code::invalid_input) return 5; }
    try { (void)evaluate(contract_probe::divergent_model{}, 1.0); return 6; }
    catch (const isotherm_error& ex) { if (ex.code() != error_code::non_finite_result) return 7; }
    return 0;
}
