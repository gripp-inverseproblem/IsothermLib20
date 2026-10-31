//==============================================================================
// Name        : test_fritz_schlunder_i_v.cpp
// Authors     : Joao Flavio Vasconcellos
// Version     : 2.0.0
// Description : Independent mathematical and domain tests for FritzSchlunderIV.
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
using isotherm::models::FritzSchlunderIV;
TEST(FritzSchlunderIVModel, TraitsAndConcept) {
    static_assert(isotherm::model_traits<FritzSchlunderIV>::parameter_count == 4);
    static_assert(isotherm::scalar_isotherm<FritzSchlunderIV>);
    static_assert(isotherm::differentiable_scalar_isotherm<FritzSchlunderIV>);
    static_assert(!isotherm::model_traits<FritzSchlunderIV>::temperature_dependent);
}
TEST(FritzSchlunderIVModel, IndependentReferenceValues) {
    const FritzSchlunderIV model{5, 0.7, 0.9, 0.6};
    EXPECT_NEAR(isotherm::evaluate(model, 0.2), 0.92744431882270728, 2e-11);
    EXPECT_NEAR(isotherm::evaluate(model, 0.8), 2.5369375820167468, 2e-11);
    EXPECT_NEAR(isotherm::evaluate(model, 1.3), 3.4802080015550279, 2e-11);
    EXPECT_NEAR(isotherm::evaluate(model, 2), 4.5270852438705376, 2e-11);
    EXPECT_DOUBLE_EQ(isotherm::evaluate(model, 0.0), 0.0);
}
TEST(FritzSchlunderIVModel, DerivativeMatchesCentralDifference) {
    const FritzSchlunderIV model{5, 0.7, 0.9, 0.6};
    for (const isotherm::Real x : {0.25, 0.9, 2.4}) {
        const isotherm::Real h = 1e-5 * x;
        const isotherm::Real numerical = (isotherm::evaluate(model, x+h) -
                                  isotherm::evaluate(model, x-h)) / (2.0*h);
        const isotherm::Real analytical = isotherm::derivative(model, x);
        EXPECT_NEAR(analytical, numerical, 2e-7 * (1.0 + std::abs(analytical)));
    }
}
TEST(FritzSchlunderIVModel, InvalidParametersAndInputs) {
    EXPECT_THROW((FritzSchlunderIV{0.0, 0.7, 0.5, 0.4}), isotherm::isotherm_error);
    EXPECT_THROW((FritzSchlunderIV{5.0, -1.0, 0.5, 0.4}), isotherm::isotherm_error);
    EXPECT_THROW((FritzSchlunderIV{5.0, 0.7, -1.0, 0.4}), isotherm::isotherm_error);
    EXPECT_THROW((FritzSchlunderIV{5.0, 0.7, 0.5, -1.0}), isotherm::isotherm_error);
    EXPECT_THROW((FritzSchlunderIV{5.0, 0.7, 0.9, 1.001}), isotherm::isotherm_error);
    const FritzSchlunderIV model{5, 0.7, 0.9, 0.6};
    EXPECT_THROW((isotherm::evaluate(model, -0.001)), isotherm::isotherm_error);
    EXPECT_THROW((isotherm::evaluate(model, std::numeric_limits<isotherm::Real>::infinity())), isotherm::isotherm_error);
    EXPECT_THROW((isotherm::evaluate(model, std::numeric_limits<isotherm::Real>::quiet_NaN())), isotherm::isotherm_error);
    EXPECT_THROW((isotherm::derivative(model, 0.0)), isotherm::isotherm_error);
}
TEST(FritzSchlunderIVModel, SpanEvaluation) {
    const FritzSchlunderIV model{5, 0.7, 0.9, 0.6};
    std::array<isotherm::Real, 4> values{0.2, 0.8, 1.3, 2.0};
    std::array<isotherm::Real, 4> output{-1.,-1.,-1.,-1.};
    isotherm::evaluate_into(model, std::span<const isotherm::Real>(values), std::span<isotherm::Real>(output));
    EXPECT_NEAR(output[1], 2.5369375820167468, 2e-11);
}
TEST(FritzSchlunderIVModel, ArchivedNumericalRegression) {
    // Expected uptake is recorded in the historical IsothermLibTest case;
    // this test does not invoke or embed the historical implementation.
    const FritzSchlunderIV model{71.3166, 0.357335, 0.779255, 0.669959};
    EXPECT_NEAR(isotherm::evaluate(model, 1.3652), 63.11313414, 5e-8);
}
} // namespace
