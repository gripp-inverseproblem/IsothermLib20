//==============================================================================
// Name        : test_langmuir.cpp
// Authors     : Joao Flavio Vasconcellos
// Version     : 2.0.0
// Description : Independent mathematical and domain tests of the Langmuir model.
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

using isotherm::models::Langmuir;

/** @brief Compile-time two-parameter model contract. */
TEST(LangmuirModel, TraitsAndConcepts) {
    static_assert(isotherm::scalar_isotherm<Langmuir>);
    static_assert(isotherm::differentiable_scalar_isotherm<Langmuir>);
    static_assert(isotherm::model_traits<Langmuir>::parameter_count == 2);
    EXPECT_EQ(isotherm::model_traits<Langmuir>::parameter_names[0], "q_max");
    EXPECT_EQ(isotherm::model_traits<Langmuir>::parameter_names[1], "K");
    const Langmuir model{5.0, 0.4};
    EXPECT_DOUBLE_EQ(model.capacity(), 5.0);
    EXPECT_DOUBLE_EQ(model.affinity(), 0.4);
}

/** @brief Independently calculated q values for qmax=10 and K=0.5. */
TEST(LangmuirModel, ExactReferenceValues) {
    const Langmuir model{10.0, 0.5};
    EXPECT_DOUBLE_EQ(isotherm::evaluate(model, 0.0), 0.0);
    EXPECT_DOUBLE_EQ(isotherm::evaluate(model, 2.0), 5.0);
    EXPECT_DOUBLE_EQ(isotherm::evaluate(model, 6.0), 7.5);
}

/** @brief The solution grows monotonically and remains below qmax. */
TEST(LangmuirModel, MonotonicityAndSaturation) {
    const Langmuir model{8.0, 2.0};
    Real previous = 0.0;
    for (int n = 1; n <= 150; ++n) {
        const Real concentration = static_cast<Real>(n) / 10.0;
        const Real value = isotherm::evaluate(model, concentration);
        EXPECT_GT(value, previous);
        EXPECT_LT(value, 8.0);
        previous = value;
    }
    EXPECT_NEAR(isotherm::evaluate(model, 1e8), 8.0, 1e-7);
}

/** @brief qmax/2 is attained at C=1/K. */
TEST(LangmuirModel, HalfSaturation) {
    const Langmuir model{6.0, 0.25};
    EXPECT_DOUBLE_EQ(isotherm::evaluate(model, 4.0), 3.0);
}

/** @brief Independent exact analytic derivative and finite-difference check. */
TEST(LangmuirModel, Derivative) {
    const Langmuir model{10.0, 0.5};
    EXPECT_DOUBLE_EQ(isotherm::derivative(model, 0.0), 5.0);
    EXPECT_DOUBLE_EQ(isotherm::derivative(model, 2.0), 1.25);
    const Real x = 1.5;
    const Real h = 1e-5;
    const Real numerical = (isotherm::evaluate(model, x + h) -
                              isotherm::evaluate(model, x - h)) / (2.0 * h);
    EXPECT_NEAR(isotherm::derivative(model, x), numerical, 2e-10);
}

/** @brief Four linearised identities hold algebraically at C>0. */
TEST(LangmuirModel, FourLinearisedIdentities) {
    const Real qmax = 10.0;
    const Real affinity = 0.5;
    const Langmuir model{qmax, affinity};
    for (Real concentration : {0.5, 1.0, 2.0, 5.0}) {
        const Real q = isotherm::evaluate(model, concentration);
        EXPECT_NEAR(concentration / q, 1.0 / (affinity * qmax) + concentration / qmax, 1e-12);
        EXPECT_NEAR(1.0 / q, 1.0 / qmax + 1.0 / (affinity * qmax * concentration), 1e-12);
        EXPECT_NEAR(q / concentration, affinity * qmax - affinity * q, 1e-12);
        EXPECT_NEAR(q, qmax - (q / concentration) / affinity, 1e-12);
    }
}

/** @brief A large intermediate K*C must saturate rather than cause NaN. */
TEST(LangmuirModel, FiniteInputsWithOverflowingIntermediate) {
    const Langmuir model{7.0, 1e308};
    EXPECT_DOUBLE_EQ(isotherm::evaluate(model, 1e308), 7.0);
    EXPECT_DOUBLE_EQ(isotherm::derivative(model, 1e308), 0.0);
}

/** @brief The model supports batch evaluation without changing the core API. */
TEST(LangmuirModel, SpanEvaluation) {
    const Langmuir model{10.0, 0.5};
    const std::array<Real, 3> concentration{0.0, 2.0, 6.0};
    std::array<Real, 3> uptake{};
    isotherm::evaluate_into(model, std::span<const Real>{concentration},
                            std::span<Real>{uptake});
    EXPECT_EQ(uptake, (std::array<Real, 3>{0.0, 5.0, 7.5}));
}

/** @brief Parameters must be strictly positive and finite. */
TEST(LangmuirModel, InvalidParameters) {
    EXPECT_THROW(static_cast<void>(Langmuir{0.0, 1.0}), isotherm::isotherm_error);
    EXPECT_THROW(static_cast<void>(Langmuir{1.0, 0.0}), isotherm::isotherm_error);
    EXPECT_THROW(static_cast<void>(Langmuir{-1.0, 1.0}), isotherm::isotherm_error);
    EXPECT_THROW(static_cast<void>(Langmuir{1.0, -1.0}), isotherm::isotherm_error);
    EXPECT_THROW(static_cast<void>(Langmuir{1.0, std::numeric_limits<Real>::infinity()}),
                 isotherm::isotherm_error);
    EXPECT_THROW(static_cast<void>(Langmuir{std::numeric_limits<Real>::quiet_NaN(), 1.0}),
                 isotherm::isotherm_error);
}

/** @brief Reject invalid concentrations on both value and derivative paths. */
TEST(LangmuirModel, InvalidConcentrations) {
    const Langmuir model{1.0, 2.0};
    EXPECT_THROW(static_cast<void>(isotherm::evaluate(model, -1.0)), isotherm::isotherm_error);
    EXPECT_THROW(static_cast<void>(isotherm::derivative(model, -1.0)), isotherm::isotherm_error);
    EXPECT_THROW(static_cast<void>(isotherm::evaluate(model, std::numeric_limits<Real>::quiet_NaN())),
                 isotherm::isotherm_error);
}

} // namespace
