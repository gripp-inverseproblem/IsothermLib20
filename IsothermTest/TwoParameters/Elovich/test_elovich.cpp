//==============================================================================
// Name        : test_elovich.cpp
// Authors     : Joao Flavio Vasconcellos
// Version     : 2.0.0
// Description : Independent numerical and domain tests of Elovich.
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

namespace {
using Model = isotherm::models::Elovich;
using isotherm::Real;

/** @brief Verify compile-time metadata and the model's parameter count. */
TEST(ElovichModel, ModelTraits) {
    static_assert(isotherm::model_traits<Model>::parameter_count == 2);
    static_assert(isotherm::differentiable_scalar_isotherm<Model>);
    EXPECT_EQ(isotherm::model_traits<Model>::parameter_names.size(), 2U);
    EXPECT_EQ(isotherm::model_traits<Model>::temperature_dependent, false);
}

/** @brief Independently verify a finite positive example and its derivative. */
TEST(ElovichModel, DerivativeFiniteDifference) {
    const Model model{2.4, 0.8};
    const Real x = 0.8;
    constexpr Real h = 1e-5;
    const Real value = isotherm::evaluate(model, x);
    const Real slope = isotherm::derivative(model, x);
    const Real fd = (isotherm::evaluate(model, x+h) -
        isotherm::evaluate(model, x-h))/(2*h);
    EXPECT_TRUE(std::isfinite(value));
    EXPECT_TRUE(std::isfinite(slope));
    EXPECT_NEAR(slope, fd, 2e-4 * (1 + std::abs(slope)));
}

/** @brief Reject invalid finite and nonfinite parameters at construction. */
TEST(ElovichModel, ParameterValidation) {
    EXPECT_THROW(static_cast<void>(Model{0.0, 0.8}), isotherm::isotherm_error);
    EXPECT_THROW(static_cast<void>(Model{2.4, 0.0}), isotherm::isotherm_error);
    EXPECT_THROW(static_cast<void>(Model{-1.0, 0.8}), isotherm::isotherm_error);
    EXPECT_THROW(static_cast<void>(Model{2.4, std::numeric_limits<Real>::infinity()}), isotherm::isotherm_error);
}

/** @brief Guard the documented physical or mathematical input domain. */
TEST(ElovichModel, InputValidation) {
    const Model model{2.4, 0.8};
    EXPECT_THROW(static_cast<void>(isotherm::evaluate(model, -1.0)), isotherm::isotherm_error);
    EXPECT_THROW(static_cast<void>(isotherm::evaluate(model, std::numeric_limits<Real>::quiet_NaN())), isotherm::isotherm_error);
    
}

/** @brief Check the defining equation using independent reference arithmetic. */
TEST(ElovichModel, BoundaryAndDomain) {
    const Model model{2.4, 0.8};
    EXPECT_EQ(isotherm::evaluate(model, 0.0), Real{0.0});
    EXPECT_NEAR(isotherm::derivative(model, 0.0), Real{2.4 * 0.8}, 1e-14);
    EXPECT_NEAR(isotherm::evaluate(model, std::exp(1.0) / 0.8), 2.4, 1e-12);
    EXPECT_THROW(static_cast<void>(isotherm::evaluate(model, 4.0)), isotherm::isotherm_error);
}

TEST(ElovichModel, IndependentIdentity) {
    const Model model{2.4, 0.8};
    constexpr Real x = 1.0;
    const Real q = isotherm::evaluate(model, x);
    const Real theta = q / 2.4;
    EXPECT_NEAR(theta * std::exp(theta), 0.8 * x, 1e-12);
}
}
