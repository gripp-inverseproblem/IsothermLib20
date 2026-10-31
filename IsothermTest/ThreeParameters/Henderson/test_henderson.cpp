//==============================================================================
// Name        : test_henderson.cpp
// Authors     : Joao Flavio Vasconcellos
// Version     : 2.0.0
// Description : Three-input Henderson thermal-section and domain regression.
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

using isotherm::Real;
namespace {
using isotherm::models::Henderson;

TEST(HendersonModel, ThreeParametersAndConstitutiveRelation) {
    static_assert(isotherm::scalar_isotherm<Henderson>);
    static_assert(isotherm::differentiable_scalar_isotherm<Henderson>);
    static_assert(isotherm::model_traits<Henderson>::parameter_count == 3);
    const Henderson model{0.015, 1.7, 0.65};
    for (Real temperature : {Real{273.15}, Real{298.15}, Real{323.15}}) {
        const Real q = isotherm::evaluate(model, temperature);
        EXPECT_NEAR(std::exp(-Real{0.015} * temperature * std::pow(q, Real{1.7})),
                    Real{0.35}, Real{2e-13});
        const Real h = Real{0.001};
        const Real numerical = (isotherm::evaluate(model, temperature + h)
                               - isotherm::evaluate(model, temperature - h)) / (2*h);
        EXPECT_NEAR(isotherm::derivative(model, temperature), numerical,
                    Real{1e-7} * (Real{1} + std::abs(numerical)));
    }
}
TEST(HendersonModel, DomainAndWaterActivityEndPoint) {
    const Henderson model{0.015, 1.7, 0.65};
    EXPECT_THROW((Henderson{0.0, 1.7, 0.65}), isotherm::isotherm_error);
    EXPECT_THROW((Henderson{0.015, 0.0, 0.65}), isotherm::isotherm_error);
    EXPECT_THROW((Henderson{0.015, 1.7, -0.1}), isotherm::isotherm_error);
    EXPECT_THROW((Henderson{0.015, 1.7, 1.0}), isotherm::isotherm_error);
    EXPECT_THROW(isotherm::evaluate(model, 0.0), isotherm::isotherm_error);
    EXPECT_THROW(isotherm::evaluate(model, -5.0), isotherm::isotherm_error);
    EXPECT_THROW(isotherm::evaluate(model, std::numeric_limits<Real>::infinity()),
                 isotherm::isotherm_error);
    EXPECT_NEAR(isotherm::evaluate(Henderson{0.015, 1.7, 0.0}, 298.15), 0, 1e-14);
}
TEST(HendersonModel, BatchMatchesScalar) {
    const Henderson model{0.015, 1.7, 0.65};
    std::array<Real,3> inputs{273.15, 298.15, 323.15}, results{0,0,0};
    isotherm::evaluate_into(model, std::span<const Real>(inputs), std::span<Real>(results));
    for (std::size_t i = 0; i < inputs.size(); ++i)
        EXPECT_NEAR(results[i], isotherm::evaluate(model, inputs[i]), 1e-13);
}
} // namespace
