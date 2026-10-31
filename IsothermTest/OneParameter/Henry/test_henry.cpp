//==============================================================================
// Name        : test_henry.cpp
// Authors     : Joao Flavio Vasconcellos
// Version     : 2.0.0
// Description : Independent mathematical and domain tests of the Henry model.
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
#include <concepts>
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

using isotherm::Real;
using isotherm::models::Henry;

/** @brief The one-parameter catalogue entry satisfies the static API. */
TEST(HenryModel, TraitsAndConcepts) {
    static_assert(isotherm::scalar_isotherm<Henry>);
    static_assert(isotherm::differentiable_scalar_isotherm<Henry>);
    static_assert(isotherm::model_traits<Henry>::parameter_count == 1);
    EXPECT_EQ(isotherm::model_traits<Henry>::parameter_names[0], "K_H");
    EXPECT_DOUBLE_EQ(Henry{2.5}.coefficient(), 2.5);
}

/** @brief Independent reference values from q=K_H*C. */
TEST(HenryModel, ExactReferenceValues) {
    const Henry model{2.5};
    EXPECT_DOUBLE_EQ(isotherm::evaluate(model, 0.0), 0.0);
    EXPECT_DOUBLE_EQ(isotherm::evaluate(model, 2.0), 5.0);
    EXPECT_DOUBLE_EQ(isotherm::evaluate(model, 4.0), 10.0);
}

/** @brief Henry uptake is exactly proportional to concentration. */
TEST(HenryModel, HomogeneityAndMonotonicity) {
    const Henry model{0.75};
    const Real q1 = isotherm::evaluate(model, 3.0);
    const Real q2 = isotherm::evaluate(model, 9.0);
    EXPECT_NEAR(q2, 3.0 * q1, 1e-14);
    EXPECT_GT(q2, q1);
}

/** @brief Analytical derivative matches the central difference. */
TEST(HenryModel, DerivativeAndFiniteDifference) {
    const Henry model{3.25};
    const Real x = 1.7;
    const Real h = 1e-5;
    const Real approximate = (isotherm::evaluate(model, x + h) -
                                isotherm::evaluate(model, x - h)) / (2.0 * h);
    EXPECT_DOUBLE_EQ(isotherm::derivative(model, 0.0), 3.25);
    EXPECT_NEAR(isotherm::derivative(model, x), approximate, 1e-10);
}

/** @brief Batch evaluation uses the same result as scalar dispatch. */
TEST(HenryModel, SpanEvaluation) {
    const Henry model{4.0};
    const std::array<Real, 3> concentrations{0.0, 0.25, 1.0};
    std::array<Real, 3> result{};
    isotherm::evaluate_into(model, std::span<const Real>{concentrations},
                            std::span<Real>{result});
    EXPECT_EQ(result, (std::array<Real, 3>{0.0, 1.0, 4.0}));
}

/** @brief Scaled coefficient sensitivity is obtained by numerical differences. */
TEST(HenryModel, NumericalParameterSensitivity) {
    // Unlike the concentration derivative, parameter sensitivity is obtained
    // exclusively from evaluations at perturbed coefficient values.
    const Real coefficient = 1.25;
    const Real step = 1e-5;
    for (const Real concentration : {0.0, 0.1, 1.0, 3.0, 6.0}) {
        const Henry high{coefficient + step};
        const Henry low{coefficient - step};
        const Real difference = (isotherm::evaluate(high, concentration) -
                                 isotherm::evaluate(low, concentration)) / (2.0 * step);
        const Real sensitivity = coefficient * difference;
        const Real expected = isotherm::evaluate(Henry{coefficient}, concentration);
        EXPECT_NEAR(sensitivity, expected, 1e-10);
    }
}

TEST(HenryModel, RealAliasAndBoundaries) {
    static_assert(std::same_as<decltype(Henry{1.0}.evaluate(1.0)), Real>);
    const Real smallest = std::numeric_limits<Real>::denorm_min();
    EXPECT_NO_THROW(static_cast<void>(Henry{smallest}));
    EXPECT_NO_THROW(static_cast<void>(isotherm::evaluate(Henry{1.0}, 0.0)));
    EXPECT_THROW(static_cast<void>(Henry{-std::numeric_limits<Real>::infinity()}),
                 isotherm::isotherm_error);
    EXPECT_THROW(static_cast<void>(isotherm::evaluate(Henry{1.0},
                         std::numeric_limits<Real>::quiet_NaN())),
                 isotherm::isotherm_error);
}

/** @brief Parameters must be strictly positive and finite. */
TEST(HenryModel, InvalidParameters) {
    EXPECT_THROW(static_cast<void>(Henry{0.0}), isotherm::isotherm_error);
    EXPECT_THROW(static_cast<void>(Henry{-1.0}), isotherm::isotherm_error);
    EXPECT_THROW(static_cast<void>(Henry{std::numeric_limits<Real>::infinity()}),
                 isotherm::isotherm_error);
    EXPECT_THROW(static_cast<void>(Henry{std::numeric_limits<Real>::quiet_NaN()}),
                 isotherm::isotherm_error);
}

/** @brief Negative and non-finite concentrations are not admissible. */
TEST(HenryModel, InvalidConcentrations) {
    const Henry model{1.0};
    EXPECT_THROW(static_cast<void>(isotherm::evaluate(model, -1.0)), isotherm::isotherm_error);
    EXPECT_THROW(static_cast<void>(isotherm::derivative(model, -1.0)), isotherm::isotherm_error);
    EXPECT_THROW(static_cast<void>(isotherm::evaluate(model, std::numeric_limits<Real>::infinity())),
                 isotherm::isotherm_error);
}

/** @brief Overflow in a mathematically unbounded law is reported, not hidden. */
TEST(HenryModel, OverflowIsReported) {
    const Henry model{1e308};
    try {
        static_cast<void>(isotherm::evaluate(model, 10.0));
        FAIL() << "Expected a non-finite-result error";
    } catch (const isotherm::isotherm_error& error) {
        EXPECT_EQ(error.code(), isotherm::error_code::non_finite_result);
    }
}

} // namespace
