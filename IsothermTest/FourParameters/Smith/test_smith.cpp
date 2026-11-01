//==============================================================================
// Name        : test_smith.cpp
// Authors     : Joao Flavio Vasconcellos
// Version     : 2.0.0
// Description : Four-stored-parameter modified Smith temperature-section tests.
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
#include <string_view>

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
using isotherm::models::Smith;

TEST(SmithModel, MetadataAndFixedActivityTemperatureSection) {
    static_assert(isotherm::scalar_isotherm<Smith>);
    static_assert(isotherm::differentiable_scalar_isotherm<Smith>);
    static_assert(!isotherm::thermal_isotherm<Smith>);
    static_assert(isotherm::model_traits<Smith>::parameter_count == 4);
    static_assert(!isotherm::model_traits<Smith>::temperature_dependent);
    static_assert(isotherm::model_traits<Smith>::variable ==
                  isotherm::independent_variable::temperature);
    static_assert(isotherm::model_traits<Smith>::parameter_names ==
                  std::array<std::string_view, 4>{"A", "B", "C", "a_w"});
    const Smith model{1.4, -0.001, 0.55, 0.5};
    constexpr Real temperature_k = 300.0;
    EXPECT_NEAR(isotherm::evaluate(model, temperature_k),
                1.1 - 0.55 * std::log(0.5), 1e-13);
    EXPECT_NEAR(isotherm::derivative(model, temperature_k), -0.001, 1e-13);
    EXPECT_NEAR(isotherm::evaluate(Smith{1.4, -0.001, 0.55, 0.0}, temperature_k),
                1.1, 1e-13);
    EXPECT_NEAR(isotherm::evaluate(model, 310.0)
                - isotherm::evaluate(model, 300.0), -0.01, 1e-13);
}

TEST(SmithModel, TemperatureDerivativeMatchesCentredDifference) {
    const Smith model{1.4, -0.001, 0.55, 0.5};
    for (Real temperature : {273.15, 300.0, 323.15}) {
        constexpr Real h = 0.001;
        const Real centred = (isotherm::evaluate(model, temperature + h)
                            - isotherm::evaluate(model, temperature - h)) / (2*h);
        EXPECT_NEAR(isotherm::derivative(model, temperature), centred, 1e-11);
    }
}

TEST(SmithModel, StoredActivityChangesTheResponseAsExpected) {
    constexpr Real h = 1e-5;
    for (Real activity : {0.25, 0.5, 0.75}) {
        const Smith plus{1.4, -0.001, 0.55, activity + h};
        const Smith minus{1.4, -0.001, 0.55, activity - h};
        const Real centred = (isotherm::evaluate(plus, 300.0)
                            - isotherm::evaluate(minus, 300.0)) / (2*h);
        EXPECT_NEAR(centred, Real{0.55} / (Real{1} - activity), 3e-8);
    }
}

TEST(SmithModel, ScaledSensitivitiesAndIndependentReference) {
    constexpr Real A = 0.3, B = 0.002, C = 0.1, aw = 0.4, T = 300.0;
    const Smith model{A, B, C, aw};
    EXPECT_NEAR(isotherm::evaluate(model, T), 0.9510825623765989, 1e-13);
    EXPECT_NEAR(isotherm::derivative(model, T), B, 1e-13);
    EXPECT_NEAR(A, 0.3, 1e-14);
    EXPECT_NEAR(B * T, 0.6, 1e-14);
    EXPECT_NEAR(-C * std::log1p(-aw), 0.051082562376599076, 1e-13);
    EXPECT_NEAR(aw * C / (Real{1} - aw), 0.06666666666666668, 1e-13);
}

TEST(SmithModel, RejectsInvalidStoredParametersAndTemperatures) {
    const Smith model{1.4, -0.001, 0.55, 0.5};
    EXPECT_THROW((Smith{0.0, -0.001, 0.55, 0.5}), isotherm::isotherm_error);
    EXPECT_THROW((Smith{1.4, -0.001, 0.0, 0.5}), isotherm::isotherm_error);
    EXPECT_THROW((Smith{1.4, -0.001, 0.55, -0.1}), isotherm::isotherm_error);
    EXPECT_THROW((Smith{1.4, -0.001, 0.55, 1.0}), isotherm::isotherm_error);
    EXPECT_THROW((Smith{1.4, -0.001, 0.55, std::numeric_limits<Real>::infinity()}),
                 isotherm::isotherm_error);
    EXPECT_THROW((Smith{1.4, std::numeric_limits<Real>::infinity(), 0.55, 0.5}),
                 isotherm::isotherm_error);
    EXPECT_THROW((Smith{1.4, -0.001, 0.55, std::numeric_limits<Real>::quiet_NaN()}),
                 isotherm::isotherm_error);
    EXPECT_THROW((void)isotherm::evaluate(model, 0.0), isotherm::isotherm_error);
    EXPECT_THROW((void)isotherm::evaluate(model, -20.0), isotherm::isotherm_error);
    EXPECT_THROW((void)isotherm::evaluate(model, 2000.0), isotherm::isotherm_error);
    EXPECT_THROW((void)isotherm::evaluate(model, std::numeric_limits<Real>::infinity()),
                 isotherm::isotherm_error);
    EXPECT_THROW((void)isotherm::derivative(model, 0.0), isotherm::isotherm_error);
    EXPECT_THROW((void)isotherm::derivative(model, 2000.0), isotherm::isotherm_error);
}

TEST(SmithModel, TemperatureSpanEvaluationMatchesScalar) {
    const Smith model{1.4, -0.001, 0.55, 0.5};
    std::array<Real, 3> temperatures{273.15, 298.15, 323.15};
    std::array<Real, 3> output{};
    isotherm::evaluate_into(model, std::span<const Real>(temperatures),
                            std::span<Real>(output));
    for (std::size_t i = 0; i < temperatures.size(); ++i) {
        EXPECT_NEAR(output[i], isotherm::evaluate(model, temperatures[i]), 1e-13);
    }
}
} // namespace
