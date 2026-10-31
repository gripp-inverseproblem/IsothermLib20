//==============================================================================
// Name        : test_baudu.cpp
// Authors     : Joao Flavio Vasconcellos
// Version     : 2.0.0
// Description : Independent mathematical and domain tests for Baudu.
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
using isotherm::models::Baudu;
TEST(BauduModel, TraitsAndConcept) {
    static_assert(isotherm::model_traits<Baudu>::parameter_count == 4);
    static_assert(isotherm::scalar_isotherm<Baudu>);
    static_assert(isotherm::differentiable_scalar_isotherm<Baudu>);
    static_assert(!isotherm::model_traits<Baudu>::temperature_dependent);
}
TEST(BauduModel, IndependentReferenceValues) {
    const Baudu model{5, 0.6, 0.4, 0.35};
    EXPECT_NEAR(isotherm::evaluate(model, 0.2), 0.52935417322122769, 2e-11);
    EXPECT_NEAR(isotherm::evaluate(model, 0.8), 1.5916219866110082, 2e-11);
    EXPECT_NEAR(isotherm::evaluate(model, 1.3), 2.2611718534352816, 2e-11);
    EXPECT_NEAR(isotherm::evaluate(model, 2), 3.035260866305292, 2e-11);
    EXPECT_DOUBLE_EQ(isotherm::evaluate(model, 0.0), 0.0);
}
TEST(BauduModel, DerivativeMatchesCentralDifference) {
    const Baudu model{5, 0.6, 0.4, 0.35};
    for (const isotherm::Real x : {0.25, 0.9, 2.4}) {
        const isotherm::Real h = 1e-5 * x;
        const isotherm::Real numerical = (isotherm::evaluate(model, x+h) -
                                  isotherm::evaluate(model, x-h)) / (2.0*h);
        const isotherm::Real analytical = isotherm::derivative(model, x);
        EXPECT_NEAR(analytical, numerical, 2e-7 * (1.0 + std::abs(analytical)));
    }
}
TEST(BauduModel, InvalidParametersAndInputs) {
    EXPECT_THROW((Baudu{0.0, 0.7, 0.5, 0.4}), isotherm::isotherm_error);
    EXPECT_THROW((Baudu{5.0, -1.0, 0.5, 0.4}), isotherm::isotherm_error);
    EXPECT_THROW((Baudu{5.0, 0.7, -1.0, 0.4}), isotherm::isotherm_error);
    EXPECT_THROW((Baudu{5.0, 0.7, 0.5, -1.0}), isotherm::isotherm_error);
    EXPECT_THROW((Baudu{5.0, 0.7, 1.7, 0.2}), isotherm::isotherm_error);
    EXPECT_THROW((Baudu{5.0, 0.7, 0.6, 1.0}), isotherm::isotherm_error);
    const Baudu model{5, 0.6, 0.4, 0.35};
    EXPECT_THROW((isotherm::evaluate(model, -0.001)), isotherm::isotherm_error);
    EXPECT_THROW((isotherm::evaluate(model, std::numeric_limits<isotherm::Real>::infinity())), isotherm::isotherm_error);
    EXPECT_THROW((isotherm::evaluate(model, std::numeric_limits<isotherm::Real>::quiet_NaN())), isotherm::isotherm_error);
    EXPECT_THROW((isotherm::derivative(model, 0.0)), isotherm::isotherm_error);
}
TEST(BauduModel, SpanEvaluation) {
    const Baudu model{5, 0.6, 0.4, 0.35};
    std::array<isotherm::Real, 4> values{0.2, 0.8, 1.3, 2.0};
    std::array<isotherm::Real, 4> output{-1.,-1.,-1.,-1.};
    isotherm::evaluate_into(model, std::span<const isotherm::Real>(values), std::span<isotherm::Real>(output));
    EXPECT_NEAR(output[1], 1.5916219866110082, 2e-11);
}
TEST(BauduModel, MonotonicBranch) {
    const Baudu model{5.0, 0.6, 0.4, 0.35};
    isotherm::Real previous = 0.0;
    for (int i=1;i<=80;++i) {
        const isotherm::Real current = isotherm::evaluate(model, i * 0.075);
        EXPECT_TRUE(current > previous);
        previous = current;
    }
}
TEST(BauduModel, ArchivedNumericalRegression) {
    // Expected uptake is recorded in the historical IsothermLibTest case;
    // this test does not invoke or embed the historical implementation.
    const Baudu model{1.95325, 0.513664, 0.0435246, 0.00605498};
    EXPECT_NEAR(isotherm::evaluate(model, 1.84657), 0.9412889085, 5e-8);
}
} // namespace
