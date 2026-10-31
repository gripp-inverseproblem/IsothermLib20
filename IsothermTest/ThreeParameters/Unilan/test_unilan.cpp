//==============================================================================
// Name        : test_unilan.cpp
// Authors     : Joao Flavio Vasconcellos
// Version     : 2.0.0
// Description : Independent reference, derivative and domain tests for Unilan.
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
using isotherm::models::Unilan;
TEST(UnilanModel, TraitsAndConcept) {
    static_assert(isotherm::model_traits<Unilan>::parameter_count == 3);
    static_assert(isotherm::scalar_isotherm<Unilan>);
    static_assert(isotherm::differentiable_scalar_isotherm<Unilan>);
    static_assert(isotherm::model_traits<Unilan>::temperature_dependent == false);
}
TEST(UnilanModel, IndependentReferenceValues) {
    const Unilan model{5, 0.65, 0.7};
    EXPECT_NEAR(isotherm::evaluate(model, 0.8), 1.7383849369463829, 1e-10 * (1.0 + std::abs(1.7383849369463829)));
    EXPECT_NEAR(isotherm::evaluate(model, 2.0), 2.8135946932358107, 1e-10 * (1.0 + std::abs(2.8135946932358107)));
    EXPECT_DOUBLE_EQ(isotherm::evaluate(model, 0.0), 0.0);
}
TEST(UnilanModel, DerivativeMatchesCentralDifference) {
    const Unilan model{5, 0.65, 0.7};
    const isotherm::Real h = 1e-5;
    const isotherm::Real numerical = (isotherm::evaluate(model, 1.3 + h) -
                              isotherm::evaluate(model, 1.3 - h)) / (2.0 * h);
    const isotherm::Real analytical = isotherm::derivative(model, 1.3);
    EXPECT_NEAR(analytical, numerical, 2e-6 * (1.0 + std::abs(analytical)));
}
TEST(UnilanModel, RejectsInvalidParametersAndInputs) {
    EXPECT_THROW((Unilan{0.0, 0.65, 0.7}), isotherm::isotherm_error);
    EXPECT_THROW((Unilan{5, -1.0, 0.7}), isotherm::isotherm_error);
    const Unilan model{5, 0.65, 0.7};
    EXPECT_THROW(static_cast<void>(isotherm::evaluate(model, -0.1)), isotherm::isotherm_error);
    EXPECT_THROW(static_cast<void>(isotherm::evaluate(model, std::numeric_limits<isotherm::Real>::infinity())), isotherm::isotherm_error);
    EXPECT_THROW(static_cast<void>(isotherm::derivative(model, 0.0)), isotherm::isotherm_error);
    
}
TEST(UnilanModel, SpanEvaluation) {
    const Unilan model{5, 0.65, 0.7};
    std::array<isotherm::Real,3> input{0.3,0.8,2.0}, output{-1.0,-1.0,-1.0};
    isotherm::evaluate_into(model, std::span<const isotherm::Real>(input), std::span<isotherm::Real>(output));
    EXPECT_NEAR(output[1], 1.7383849369463829, 1e-10);
}
} // namespace
