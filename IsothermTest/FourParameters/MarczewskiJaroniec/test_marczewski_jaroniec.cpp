//==============================================================================
// Name        : test_marczewski_jaroniec.cpp
// Authors     : Joao Flavio Vasconcellos
// Version     : 2.0.0
// Description : Independent mathematical and domain tests for MarczewskiJaroniec.
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
using isotherm::models::MarczewskiJaroniec;
TEST(MarczewskiJaroniecModel, TraitsAndConcept) {
    static_assert(isotherm::model_traits<MarczewskiJaroniec>::parameter_count == 4);
    static_assert(isotherm::scalar_isotherm<MarczewskiJaroniec>);
    static_assert(isotherm::differentiable_scalar_isotherm<MarczewskiJaroniec>);
    static_assert(!isotherm::model_traits<MarczewskiJaroniec>::temperature_dependent);
}
TEST(MarczewskiJaroniecModel, IndependentReferenceValues) {
    const MarczewskiJaroniec model{5, 0.7, 0.8, 0.6};
    EXPECT_NEAR(isotherm::evaluate(model, 0.2), 1.3342781607892527, 2e-11);
    EXPECT_NEAR(isotherm::evaluate(model, 0.8), 2.4488998778679631, 2e-11);
    EXPECT_NEAR(isotherm::evaluate(model, 1.3), 2.8885388309712416, 2e-11);
    EXPECT_NEAR(isotherm::evaluate(model, 2), 3.2665905418388785, 2e-11);
    EXPECT_DOUBLE_EQ(isotherm::evaluate(model, 0.0), 0.0);
}
TEST(MarczewskiJaroniecModel, DerivativeMatchesCentralDifference) {
    const MarczewskiJaroniec model{5, 0.7, 0.8, 0.6};
    for (const isotherm::Real x : {0.25, 0.9, 2.4}) {
        const isotherm::Real h = 1e-5 * x;
        const isotherm::Real numerical = (isotherm::evaluate(model, x+h) -
                                  isotherm::evaluate(model, x-h)) / (2.0*h);
        const isotherm::Real analytical = isotherm::derivative(model, x);
        EXPECT_NEAR(analytical, numerical, 2e-7 * (1.0 + std::abs(analytical)));
    }
}
TEST(MarczewskiJaroniecModel, InvalidParametersAndInputs) {
    EXPECT_THROW((MarczewskiJaroniec{0.0, 0.7, 0.5, 0.4}), isotherm::isotherm_error);
    EXPECT_THROW((MarczewskiJaroniec{5.0, -1.0, 0.5, 0.4}), isotherm::isotherm_error);
    EXPECT_THROW((MarczewskiJaroniec{5.0, 0.7, -1.0, 0.4}), isotherm::isotherm_error);
    EXPECT_THROW((MarczewskiJaroniec{5.0, 0.7, 0.5, -1.0}), isotherm::isotherm_error);
    EXPECT_THROW((MarczewskiJaroniec{5.0, 0.7, 0.8, 1.0}), isotherm::isotherm_error);
    const MarczewskiJaroniec model{5, 0.7, 0.8, 0.6};
    EXPECT_THROW((isotherm::evaluate(model, -0.001)), isotherm::isotherm_error);
    EXPECT_THROW((isotherm::evaluate(model, std::numeric_limits<isotherm::Real>::infinity())), isotherm::isotherm_error);
    EXPECT_THROW((isotherm::evaluate(model, std::numeric_limits<isotherm::Real>::quiet_NaN())), isotherm::isotherm_error);
    EXPECT_THROW((isotherm::derivative(model, 0.0)), isotherm::isotherm_error);
}
TEST(MarczewskiJaroniecModel, SpanEvaluation) {
    const MarczewskiJaroniec model{5, 0.7, 0.8, 0.6};
    std::array<isotherm::Real, 4> values{0.2, 0.8, 1.3, 2.0};
    std::array<isotherm::Real, 4> output{-1.,-1.,-1.,-1.};
    isotherm::evaluate_into(model, std::span<const isotherm::Real>(values), std::span<isotherm::Real>(output));
    EXPECT_NEAR(output[1], 2.4488998778679631, 2e-11);
}
TEST(MarczewskiJaroniecModel, SaturationLimit) {
    const MarczewskiJaroniec model{5.0, 0.7, 0.8, 0.6};
    EXPECT_NEAR(isotherm::evaluate(model, 1e12), 5.0, 1e-8);
}
TEST(MarczewskiJaroniecModel, ArchivedNumericalRegression) {
    // Expected uptake is recorded in the historical IsothermLibTest case;
    // this test does not invoke or embed the historical implementation.
    const MarczewskiJaroniec model{4.99159, 0.240965, 0.0686414, 0.0818727};
    EXPECT_NEAR(isotherm::evaluate(model, 1.68377), 2.103278904, 5e-8);
}
} // namespace
