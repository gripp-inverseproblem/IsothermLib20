//==============================================================================
// Name        : test_weber_van_vliet.cpp
// Authors     : Joao Flavio Vasconcellos
// Version     : 2.0.0
// Description : Independent mathematical and domain tests for WeberVanVliet.
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
using isotherm::models::WeberVanVliet;
TEST(WeberVanVlietModel, TraitsAndConcept) {
    static_assert(isotherm::model_traits<WeberVanVliet>::parameter_count == 4);
    static_assert(isotherm::scalar_isotherm<WeberVanVliet>);
    static_assert(isotherm::differentiable_scalar_isotherm<WeberVanVliet>);
    static_assert(!isotherm::model_traits<WeberVanVliet>::temperature_dependent);
}
TEST(WeberVanVlietModel, IndependentReferenceValues) {
    const WeberVanVliet model{2, 0.7, 0.6, 0.8};
    EXPECT_NEAR(isotherm::evaluate(model, 0.2), 0.09266525675154362, 2e-11);
    EXPECT_NEAR(isotherm::evaluate(model, 0.8), 0.48075325450206674, 2e-11);
    EXPECT_NEAR(isotherm::evaluate(model, 1.3), 0.73194202941976882, 2e-11);
    EXPECT_NEAR(isotherm::evaluate(model, 2), 1, 2e-11);
    EXPECT_DOUBLE_EQ(isotherm::evaluate(model, 0.0), 0.0);
}
TEST(WeberVanVlietModel, DerivativeMatchesCentralDifference) {
    const WeberVanVliet model{2, 0.7, 0.6, 0.8};
    for (const isotherm::Real x : {0.25, 0.9, 2.4}) {
        const isotherm::Real h = 1e-5 * x;
        const isotherm::Real numerical = (isotherm::evaluate(model, x+h) -
                                  isotherm::evaluate(model, x-h)) / (2.0*h);
        const isotherm::Real analytical = isotherm::derivative(model, x);
        EXPECT_NEAR(analytical, numerical, 2e-7 * (1.0 + std::abs(analytical)));
    }
}
TEST(WeberVanVlietModel, InvalidParametersAndInputs) {
    EXPECT_THROW((WeberVanVliet{0.0, 0.7, 0.5, 0.4}), isotherm::isotherm_error);
    EXPECT_THROW((WeberVanVliet{5.0, -1.0, 0.5, 0.4}), isotherm::isotherm_error);
    EXPECT_THROW((WeberVanVliet{5.0, 0.7, -1.0, 0.4}), isotherm::isotherm_error);
    EXPECT_THROW((WeberVanVliet{5.0, 0.7, 0.5, -1.0}), isotherm::isotherm_error);
    EXPECT_THROW((WeberVanVliet{2.0, 0.7, 0.6, 0.01}), isotherm::isotherm_error);
    const WeberVanVliet model{2, 0.7, 0.6, 0.8};
    EXPECT_THROW((isotherm::evaluate(model, -0.001)), isotherm::isotherm_error);
    EXPECT_THROW((isotherm::evaluate(model, std::numeric_limits<isotherm::Real>::infinity())), isotherm::isotherm_error);
    EXPECT_THROW((isotherm::evaluate(model, std::numeric_limits<isotherm::Real>::quiet_NaN())), isotherm::isotherm_error);
    EXPECT_THROW((isotherm::derivative(model, 0.0)), isotherm::isotherm_error);
}
TEST(WeberVanVlietModel, SpanEvaluation) {
    const WeberVanVliet model{2, 0.7, 0.6, 0.8};
    std::array<isotherm::Real, 4> values{0.2, 0.8, 1.3, 2.0};
    std::array<isotherm::Real, 4> output{-1.,-1.,-1.,-1.};
    isotherm::evaluate_into(model, std::span<const isotherm::Real>(values), std::span<isotherm::Real>(output));
    EXPECT_NEAR(output[1], 0.48075325450206674, 2e-11);
}
TEST(WeberVanVlietModel, InverseForwardResidualAndUniqueness) {
    const WeberVanVliet model{2.0, 0.7, 0.6, 0.8};
    for (const isotherm::Real q : {0.12, 0.5, 1.0, 2.0, 5.0}) {
        const isotherm::Real c = 2.0 * std::pow(q, 0.7 * std::pow(q, 0.6) + 0.8);
        EXPECT_NEAR(isotherm::evaluate(model, c), q, 5e-12 * (1.0 + q));
    }
    EXPECT_NEAR(isotherm::evaluate(model, 2.0), 1.0, 2e-13);
    EXPECT_NEAR(isotherm::derivative(model, 2.0), 1.0/(2.0*(0.7+0.8)), 2e-13);
}
TEST(WeberVanVlietModel, RejectsNonUniqueParameterRegion) {
    EXPECT_THROW((WeberVanVliet{1.0, 2.0, 0.7, 0.15}), isotherm::isotherm_error);
    const WeberVanVliet near_boundary{1.0, 2.0, 0.7, 0.30};
    EXPECT_NEAR(isotherm::evaluate(near_boundary, 1.0), 1.0, 1e-12);
}
TEST(WeberVanVlietModel, ArchivedNumericalRegression) {
    // Expected uptake is recorded in the historical IsothermLibTest case;
    // this test does not invoke or embed the historical implementation.
    const WeberVanVliet model{0.087093, 3.19814, 0.0647897, 0.769393};
    EXPECT_NEAR(isotherm::evaluate(model, 1.56792), 2.01818633, 5e-8);
}
} // namespace
