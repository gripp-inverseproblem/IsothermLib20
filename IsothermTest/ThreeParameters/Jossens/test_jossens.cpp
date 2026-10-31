//==============================================================================
// Name        : test_jossens.cpp
// Authors     : Joao Flavio Vasconcellos
// Version     : 2.0.0
// Description : Independent reference, derivative and domain tests for Jossens.
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
using isotherm::Real;
using isotherm::models::Jossens;
using isotherm::models::RedlichPeterson;

// Reference values: 50-digit mpmath solutions of C = (q/H) exp(F q^p),
// cross-checked with the Lambert-W closed form q = [W(F p (H C)^p)/(F p)]^(1/p).
constexpr Real H = 5.0, F = 0.65, P = 1.15;

TEST(JossensModel, TraitsAndConcept) {
    static_assert(isotherm::model_traits<Jossens>::parameter_count == 3);
    static_assert(isotherm::scalar_isotherm<Jossens>);
    static_assert(isotherm::differentiable_scalar_isotherm<Jossens>);
    static_assert(isotherm::model_traits<Jossens>::temperature_dependent == false);
    static_assert(isotherm::model_traits<Jossens>::equation == isotherm::equation_type::implicit_equation);
}

TEST(JossensModel, IndependentReferenceValues) {
    const Jossens model{H, F, P};
    EXPECT_NEAR(isotherm::evaluate(model, 0.3), 0.86518009865276424, 1e-14);
    EXPECT_NEAR(isotherm::evaluate(model, 0.8), 1.4623238458543007, 1e-14);
    EXPECT_NEAR(isotherm::evaluate(model, 2.0), 2.1266979615901124, 1e-14);
    EXPECT_DOUBLE_EQ(isotherm::evaluate(model, 0.0), 0.0);
}

TEST(JossensModel, ImplicitResidualVanishes) {
    const Jossens model{H, F, P};
    for (const Real c : {1e-3, 0.5, 3.0, 50.0, 1e4}) {
        const Real q = isotherm::evaluate(model, c);
        const Real residual = std::log(q) - std::log(H) + F * std::pow(q, P) - std::log(c);
        EXPECT_NEAR(residual, 0.0, 1e-13 * (1.0 + std::abs(std::log(c))));
    }
}

TEST(JossensModel, AnalyticalDerivative) {
    const Jossens model{H, F, P};
    EXPECT_NEAR(isotherm::derivative(model, 1.3), 0.5611244929409796, 1e-14);
    const Real h = 1e-5;
    const Real numerical = (isotherm::evaluate(model, 1.3 + h) -
                            isotherm::evaluate(model, 1.3 - h)) / (2.0 * h);
    EXPECT_NEAR(isotherm::derivative(model, 1.3), numerical, 1e-8);
    // Henry-law limit at zero concentration.
    EXPECT_DOUBLE_EQ(isotherm::derivative(model, 0.0), H);
}

TEST(JossensModel, HenryLimitAtVerySmallConcentration) {
    const Jossens model{H, F, P};
    for (const Real c : {1e-60, 1e-200}) {
        EXPECT_NEAR(isotherm::evaluate(model, c) / (H * c), 1.0, 1e-13);
        EXPECT_NEAR(isotherm::derivative(model, c) / H, 1.0, 1e-13);
    }
}

TEST(JossensModel, IsNotRedlichPeterson) {
    // The original Jossens relation must not reduce to the Redlich-Peterson form
    // used by earlier versions of this class.
    const Jossens jossens{H, F, P};
    const RedlichPeterson rp{H, F, P};
    EXPECT_GT(std::abs(isotherm::evaluate(jossens, 2.0) - isotherm::evaluate(rp, 2.0)), 1e-3);
}

TEST(JossensModel, RejectsInvalidParametersAndInputs) {
    EXPECT_THROW((Jossens{0.0, F, P}), isotherm::isotherm_error);
    EXPECT_THROW((Jossens{H, -1.0, P}), isotherm::isotherm_error);
    EXPECT_THROW((Jossens{H, F, 0.0}), isotherm::isotherm_error);
    const Jossens model{H, F, P};
    EXPECT_THROW(static_cast<void>(isotherm::evaluate(model, -0.1)), isotherm::isotherm_error);
    EXPECT_THROW(static_cast<void>(isotherm::evaluate(model, std::numeric_limits<Real>::infinity())), isotherm::isotherm_error);
}

TEST(JossensModel, SpanEvaluation) {
    const Jossens model{H, F, P};
    std::array<Real, 3> input{0.3, 0.8, 2.0}, output{-1.0, -1.0, -1.0};
    isotherm::evaluate_into(model, std::span<const Real>(input), std::span<Real>(output));
    EXPECT_NEAR(output[1], 1.4623238458543007, 1e-14);
}
} // namespace
