//==============================================================================
// Name        : test_hill.cpp
// Authors     : Joao Flavio Vasconcellos
// Version     : 2.0.0
// Description : Independent reference, derivative and domain tests for Hill.
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
using isotherm::models::Hill;
TEST(HillModel, TraitsAndConcept) {
    static_assert(isotherm::model_traits<Hill>::parameter_count == 3);
    static_assert(isotherm::scalar_isotherm<Hill>);
    static_assert(isotherm::differentiable_scalar_isotherm<Hill>);
    static_assert(isotherm::model_traits<Hill>::temperature_dependent == false);
}
TEST(HillModel, IndependentReferenceValues) {
    const Hill model{5, 1.2, 1.4};
    EXPECT_NEAR(isotherm::evaluate(model, 0.8), 1.8939084666186496, 1e-10 * (1.0 + std::abs(1.8939084666186496)));
    EXPECT_NEAR(isotherm::evaluate(model, 2.0), 3.4370994340981675, 1e-10 * (1.0 + std::abs(3.4370994340981675)));
    EXPECT_DOUBLE_EQ(isotherm::evaluate(model, 0.0), 0.0);
}
TEST(HillModel, DerivativeMatchesCentralDifference) {
    const Hill model{5, 1.2, 1.4};
    const isotherm::Real h = 1e-5;
    const isotherm::Real numerical = (isotherm::evaluate(model, 1.3 + h) -
                              isotherm::evaluate(model, 1.3 - h)) / (2.0 * h);
    const isotherm::Real analytical = isotherm::derivative(model, 1.3);
    EXPECT_NEAR(analytical, numerical, 2e-6 * (1.0 + std::abs(analytical)));
}
TEST(HillModel, RejectsInvalidParametersAndInputs) {
    EXPECT_THROW((Hill{0.0, 1.2, 1.4}), isotherm::isotherm_error);
    EXPECT_THROW((Hill{5, -1.0, 1.4}), isotherm::isotherm_error);
    const Hill model{5, 1.2, 1.4};
    EXPECT_THROW(static_cast<void>(isotherm::evaluate(model, -0.1)), isotherm::isotherm_error);
    EXPECT_THROW(static_cast<void>(isotherm::evaluate(model, std::numeric_limits<isotherm::Real>::infinity())), isotherm::isotherm_error);
    EXPECT_THROW(static_cast<void>(isotherm::derivative(model, 0.0)), isotherm::isotherm_error);
    
}
TEST(HillModel, SpanEvaluation) {
    const Hill model{5, 1.2, 1.4};
    std::array<isotherm::Real,3> input{0.3,0.8,2.0}, output{-1.0,-1.0,-1.0};
    isotherm::evaluate_into(model, std::span<const isotherm::Real>(input), std::span<isotherm::Real>(output));
    EXPECT_NEAR(output[1], 1.8939084666186496, 1e-10);
}
} // namespace
