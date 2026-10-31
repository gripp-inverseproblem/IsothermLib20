//==============================================================================
// Name        : test_jovanovic.cpp
// Authors     : Joao Flavio Vasconcellos
// Version     : 2.0.0
// Description : Independent numerical and domain tests of Jovanovic.
//
// Copyright   : Copyright (C) 2026 Joao Flavio Vasconcellos
// Licence     : MIT License (see LICENSE)
// SPDX-License-Identifier: MIT
//==============================================================================

//==============================================================================
// C++ standard library headers
//==============================================================================
#include <cmath>
#include <limits>
//==============================================================================
// Isotherm++ headers
//==============================================================================
#include "isotherm/isotherm.hpp"
//==============================================================================
// GoogleTest headers
//==============================================================================
#include <gtest/gtest.h>

using isotherm::Real;

namespace {
using Model = isotherm::models::Jovanovic;

/** @brief Verify compile-time metadata and the model's parameter count. */
TEST(JovanovicModel, ModelTraits) {
    static_assert(isotherm::model_traits<Model>::parameter_count == 2);
    static_assert(isotherm::differentiable_scalar_isotherm<Model>);
    EXPECT_EQ(isotherm::model_traits<Model>::parameter_names.size(), 2U);
    EXPECT_EQ(isotherm::model_traits<Model>::parameter_names[0], "q_sat");
    EXPECT_EQ(isotherm::model_traits<Model>::temperature_dependent, false);
}

/** @brief Independently verify a finite positive example and its derivative. */
TEST(JovanovicModel, DerivativeFiniteDifference) {
    const Model model{2.5, 0.35};
    const Real x = 0.8;
    constexpr Real h = 1e-5;
    const Real value = isotherm::evaluate(model, x);
    const Real slope = isotherm::derivative(model, x);
    const Real fd = (isotherm::evaluate(model, x+h) -
        isotherm::evaluate(model, x-h))/(2*h);
    EXPECT_TRUE(std::isfinite(value));
    EXPECT_GT(slope, 0.0);
    EXPECT_TRUE(std::isfinite(slope));
    EXPECT_NEAR(slope, fd, 2e-4 * (1 + std::abs(slope)));
}

/** @brief Reject invalid finite and nonfinite parameters at construction. */
TEST(JovanovicModel, ParameterValidation) {
    EXPECT_THROW(static_cast<void>(Model{0.0, 0.35}), isotherm::isotherm_error);
    EXPECT_THROW(static_cast<void>(Model{2.5, 0.0}), isotherm::isotherm_error);
    EXPECT_THROW(static_cast<void>(Model{-1.0, 0.35}), isotherm::isotherm_error);
    EXPECT_THROW(static_cast<void>(Model{2.5, std::numeric_limits<Real>::infinity()}), isotherm::isotherm_error);
}

/** @brief Guard the documented physical or mathematical input domain. */
TEST(JovanovicModel, InputValidation) {
    const Model model{2.5, 0.35};
    EXPECT_THROW(static_cast<void>(isotherm::evaluate(model, -1.0)), isotherm::isotherm_error);
    EXPECT_THROW(static_cast<void>(isotherm::evaluate(model, std::numeric_limits<Real>::quiet_NaN())), isotherm::isotherm_error);
    EXPECT_THROW(static_cast<void>(isotherm::derivative(model, -0.1)), isotherm::isotherm_error);
}

/** @brief Check the defining equation using independent reference arithmetic. */
TEST(JovanovicModel, IndependentIdentity) {
    const Model model{2.5, 0.35};
    constexpr Real x = 1.0;
    const Real q = isotherm::evaluate(model, x);
    EXPECT_NEAR(q, 2.5 * (1.0 - std::exp(-0.35 * x)), 1e-14);
    EXPECT_NEAR(isotherm::derivative(model, x),
                2.5 * 0.35 * std::exp(-0.35 * x), 1e-14);
}

/** @brief Confirm the zero-uptake boundary and finite saturation limit. */
TEST(JovanovicModel, MonotonicityAndSaturation) {
    const Model model{2.5, 0.35};
    EXPECT_DOUBLE_EQ(isotherm::evaluate(model, 0.0), 0.0);
    EXPECT_NEAR(isotherm::derivative(model, 0.0), 2.5 * 0.35, 1e-14);
    const Real small = isotherm::evaluate(model, 0.5);
    const Real larger = isotherm::evaluate(model, 2.0);
    EXPECT_GT(larger, small);
    EXPECT_LT(larger, 2.5);
    EXPECT_NEAR(isotherm::evaluate(model, 200.0), 2.5, 1e-14);
    EXPECT_NEAR(isotherm::derivative(model, 200.0), 0.0, 1e-14);
}
}
