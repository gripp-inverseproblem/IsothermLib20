//==============================================================================
// Name        : test_fowler_guggenheim.cpp
// Authors     : Joao Flavio Vasconcellos
// Version     : 2.0.0
// Description : Independent reference, derivative and domain tests for FowlerGuggenheim.
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
// Isotherm++ headers
//==============================================================================
#include "isotherm/isotherm.hpp"
//==============================================================================
// GoogleTest headers
//==============================================================================
#include <gtest/gtest.h>

namespace {
using isotherm::models::FowlerGuggenheim;
using isotherm::Real;
TEST(FowlerGuggenheimModel, TraitsAndConcept) {
    static_assert(isotherm::model_traits<FowlerGuggenheim>::parameter_count == 3);
    static_assert(isotherm::thermal_isotherm<FowlerGuggenheim>);
    static_assert(isotherm::differentiable_thermal_isotherm<FowlerGuggenheim>);
    static_assert(isotherm::model_traits<FowlerGuggenheim>::temperature_dependent == true);
}
TEST(FowlerGuggenheimModel, IndependentReferenceValues) {
    const FowlerGuggenheim model{5, 0.8, 900};
    EXPECT_NEAR(isotherm::evaluate(model, 0.8, 298.15), 1.7982748715071861, 1e-10 * (1.0 + std::abs(1.7982748715071861)));
    EXPECT_NEAR(isotherm::evaluate(model, 2.0, 298.15), 2.8288321805371308, 1e-10 * (1.0 + std::abs(2.8288321805371308)));
    EXPECT_DOUBLE_EQ(isotherm::evaluate(model, 0.0, 298.15), 0.0);
}
TEST(FowlerGuggenheimModel, DerivativeMatchesCentralDifference) {
    const FowlerGuggenheim model{5, 0.8, 900};
    const Real h = 1e-5;
    const Real numerical = (isotherm::evaluate(model, 1.3 + h, 298.15) -
                              isotherm::evaluate(model, 1.3 - h, 298.15)) / (2.0 * h);
    const Real analytical = isotherm::derivative(model, 1.3, 298.15);
    EXPECT_NEAR(analytical, numerical, 2e-6 * (1.0 + std::abs(analytical)));
}
TEST(FowlerGuggenheimModel, RejectsInvalidParametersAndInputs) {
    EXPECT_THROW((FowlerGuggenheim{0.0, 0.8, 900}), isotherm::isotherm_error);
    EXPECT_THROW((FowlerGuggenheim{5, -1.0, 900}), isotherm::isotherm_error);
    const FowlerGuggenheim model{5, 0.8, 900};
    EXPECT_THROW((isotherm::evaluate(model, -0.1, 298.15)), isotherm::isotherm_error);
    EXPECT_THROW((isotherm::evaluate(model, std::numeric_limits<Real>::infinity(), 298.15)), isotherm::isotherm_error);
    EXPECT_THROW((isotherm::derivative(model, 0.0, 298.15)), isotherm::isotherm_error);
    EXPECT_THROW((isotherm::evaluate(model, 1.0, 0.0)), isotherm::isotherm_error);
}
TEST(FowlerGuggenheimModel, EnergyUnitsAndKelvin) {
    const FowlerGuggenheim si{5, 0.8, 900};
    const FowlerGuggenheim kj{5, 0.8, 0.9,
        isotherm::gas_constant_unit::kilojoule_per_mole_kelvin};
    const FowlerGuggenheim latm{5, 0.8, 900.0 / 101.325,
        isotherm::gas_constant_unit::litre_atmosphere_per_mole_kelvin};
    for (Real concentration : {Real{0.3}, Real{0.8}, Real{2.0}}) {
        const Real q = isotherm::evaluate(si, concentration, 298.15);
        EXPECT_NEAR(q, isotherm::evaluate(kj, concentration, 298.15), 1e-12);
        EXPECT_NEAR(q, isotherm::evaluate(latm, concentration, 298.15), 1e-12);
    }
    EXPECT_THROW(((FowlerGuggenheim{5, 0.8, 900.0,
        static_cast<isotherm::gas_constant_unit>(-1)})), isotherm::isotherm_error);
}
TEST(FowlerGuggenheimModel, SpanEvaluation) {
    const FowlerGuggenheim model{5, 0.8, 900};
    std::array<Real,3> input{0.3,0.8,2.0}, output{-1.0,-1.0,-1.0};
    isotherm::evaluate_into(model, std::span<const Real>(input), 298.15, std::span<Real>(output));
    EXPECT_NEAR(output[1], 1.7982748715071861, 1e-10);
}
} // namespace
