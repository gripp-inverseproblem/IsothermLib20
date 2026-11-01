//==============================================================================
// Name        : test_redlich_peterson.cpp
// Authors     : Joao Flavio Vasconcellos
// Version     : 2.0.0
// Description : Independent reference, derivative and domain tests for RedlichPeterson.
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
using isotherm::models::RedlichPeterson;
TEST(RedlichPetersonModel, TraitsAndConcept) {
    static_assert(isotherm::model_traits<RedlichPeterson>::parameter_count == 3);
    static_assert(isotherm::scalar_isotherm<RedlichPeterson>);
    static_assert(isotherm::differentiable_scalar_isotherm<RedlichPeterson>);
    static_assert(isotherm::model_traits<RedlichPeterson>::temperature_dependent == false);
}
TEST(RedlichPetersonModel, IndependentReferenceValues) {
    const RedlichPeterson model{5, 0.65, 1.15};
    EXPECT_NEAR(isotherm::evaluate(model, 0.8), 2.6615513966935613, 1e-10 * (1.0 + std::abs(2.6615513966935613)));
    EXPECT_NEAR(isotherm::evaluate(model, 2.0), 4.094265863512355, 1e-10 * (1.0 + std::abs(4.094265863512355)));
    EXPECT_DOUBLE_EQ(isotherm::evaluate(model, 0.0), 0.0);
}
TEST(RedlichPetersonModel, DerivativeMatchesCentralDifference) {
    const RedlichPeterson model{5, 0.65, 1.15};
    const isotherm::Real h = 1e-5;
    const isotherm::Real numerical = (isotherm::evaluate(model, 1.3 + h) -
                              isotherm::evaluate(model, 1.3 - h)) / (2.0 * h);
    const isotherm::Real analytical = isotherm::derivative(model, 1.3);
    EXPECT_NEAR(analytical, numerical, 2e-6 * (1.0 + std::abs(analytical)));
}
TEST(RedlichPetersonModel, RejectsInvalidParametersAndInputs) {
    EXPECT_THROW((RedlichPeterson{0.0, 0.65, 1.15}), isotherm::isotherm_error);
    EXPECT_THROW((RedlichPeterson{5, -1.0, 1.15}), isotherm::isotherm_error);
    const RedlichPeterson model{5, 0.65, 1.15};
    EXPECT_THROW(static_cast<void>(isotherm::evaluate(model, -0.1)), isotherm::isotherm_error);
    EXPECT_THROW(static_cast<void>(isotherm::evaluate(model, std::numeric_limits<isotherm::Real>::infinity())), isotherm::isotherm_error);
    EXPECT_THROW(static_cast<void>(isotherm::derivative(model, 0.0)), isotherm::isotherm_error);
    
}
TEST(RedlichPetersonModel, SpanEvaluation) {
    const RedlichPeterson model{5, 0.65, 1.15};
    std::array<isotherm::Real,3> input{0.3,0.8,2.0}, output{-1.0,-1.0,-1.0};
    isotherm::evaluate_into(model, std::span<const isotherm::Real>(input), std::span<isotherm::Real>(output));
    EXPECT_NEAR(output[1], 2.6615513966935613, 1e-10);
}
} // namespace
