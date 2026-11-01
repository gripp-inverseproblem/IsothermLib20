//==============================================================================
// Name        : test_oswin.cpp
// Authors     : Joao Flavio Vasconcellos
// Version     : 2.0.0
// Description : Four-parameter Oswin numerical and domain regression tests.
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
using isotherm::models::Oswin;

TEST(OswinModel, TraitsAndFourCoefficients) {
    static_assert(isotherm::thermal_isotherm<Oswin>);
    static_assert(isotherm::differentiable_thermal_isotherm<Oswin>);
    static_assert(isotherm::model_traits<Oswin>::parameter_count == 4);
    constexpr auto names = isotherm::model_traits<Oswin>::parameter_names;
    static_assert(names[0] == std::string_view{"A"});
    static_assert(names[3] == std::string_view{"D"});
    const Oswin model{2.0, -0.002, 1.7, 0.001};
    EXPECT_NEAR(isotherm::evaluate(model, 0.5, 300.0), 1.4, 1e-12);
    EXPECT_NEAR(isotherm::derivative(model, 0.5, 300.0), 2.8, 1e-12);
    EXPECT_NEAR(isotherm::evaluate(model, 0.0, 300.0), 0.0, 1e-12);
    EXPECT_NEAR(isotherm::evaluate(model, 0.2, 300.0), 0.7, 1e-12);
    const Real exponent_at_400 = Real{1} / (Real{1.7} + Real{0.001} * Real{400});
    EXPECT_NEAR(isotherm::evaluate(model, 0.2, 400.0),
                Real{1.2} * std::pow(Real{0.25}, exponent_at_400), 1e-12);
}

TEST(OswinModel, ReducesToThreeParameterFormWhenDIsZero) {
    const Oswin modified{2.0, -0.002, 2.0, 0.0};
    for (Real temperature : {300.0, 450.0}) {
        for (Real activity : {0.2, 0.5, 0.8}) {
            const Real expected = (Real{2} - Real{0.002} * temperature)
                * std::pow(activity / (Real{1} - activity), Real{0.5});
            EXPECT_NEAR(isotherm::evaluate(modified, activity, temperature),
                        expected, 1e-12);
        }
    }
    const Oswin original{2.0, 0.0, 2.0, 0.0};
    EXPECT_NEAR(isotherm::evaluate(original, 0.2, 500.0), 1.0, 1e-12);
}

TEST(OswinModel, ActivityDerivativeMatchesFiniteDifferences) {
    const Oswin model{2.0, -0.002, 1.7, 0.001};
    for (Real temperature : {300.0, 400.0}) {
        for (Real activity : {0.25, 0.5, 0.75}) {
            const Real delta = activity * 1e-5;
            const Real numerical =
                (isotherm::evaluate(model, activity + delta, temperature)
                 - isotherm::evaluate(model, activity - delta, temperature))
                / (Real{2} * delta);
            EXPECT_NEAR(isotherm::derivative(model, activity, temperature),
                        numerical, 3e-6 * (1.0 + std::abs(numerical)));
        }
    }
}

TEST(OswinModel, AllParametersAffectIndependentPredictions) {
    const Oswin base{2.0, -0.002, 1.7, 0.001};
    constexpr Real activity = 0.2;
    constexpr Real temperature = 300.0;
    const Real reference = isotherm::evaluate(base, activity, temperature);
    for (const auto model : {Oswin{2.05, -0.002, 1.7, 0.001},
                             Oswin{2.0, -0.0019, 1.7, 0.001},
                             Oswin{2.0, -0.002, 1.8, 0.001},
                             Oswin{2.0, -0.002, 1.7, 0.0011}}) {
        EXPECT_GT(std::abs(isotherm::evaluate(model, activity, temperature) - reference),
                  1e-6);
    }
}

TEST(OswinModel, RejectsInvalidParametersAndDomain) {
    const Oswin model{2.0, -0.002, 1.7, 0.001};
    EXPECT_THROW((Oswin{0.0, 0.0, 2.0, 0.0}), isotherm::isotherm_error);
    EXPECT_THROW((Oswin{2.0, 0.0, 0.0, 0.0}), isotherm::isotherm_error);
    EXPECT_THROW((Oswin{2.0, 0.0, 2.0,
                        std::numeric_limits<Real>::infinity()}),
                 isotherm::isotherm_error);
    EXPECT_THROW((Oswin{2.0, std::numeric_limits<Real>::quiet_NaN(), 2.0, 0.0}),
                 isotherm::isotherm_error);
    EXPECT_THROW(static_cast<void>(isotherm::evaluate(model, -0.01, 300.0)), isotherm::isotherm_error);
    EXPECT_THROW(static_cast<void>(isotherm::evaluate(model, 1.0, 300.0)), isotherm::isotherm_error);
    EXPECT_THROW(static_cast<void>(isotherm::evaluate(model, 0.5, 0.0)), isotherm::isotherm_error);
    EXPECT_THROW(static_cast<void>(isotherm::evaluate(model, 0.5, 1200.0)), isotherm::isotherm_error);
    const Oswin bad_exponent{2.0, 0.0, 1.7, -0.01};
    EXPECT_THROW(static_cast<void>(isotherm::evaluate(bad_exponent, 0.5, 300.0)),
                 isotherm::isotherm_error);
    EXPECT_THROW(static_cast<void>(isotherm::derivative(model, 0.0, 300.0)),
                 isotherm::isotherm_error);
}

TEST(OswinModel, ThermalBatchMatchesScalar) {
    const Oswin model{2.0, -0.002, 1.7, 0.001};
    std::array<Real, 3> activities{0.21, 0.41, 0.71};
    std::array<Real, 3> output{-1.0, -1.0, -1.0};
    isotherm::evaluate_into(model, std::span<const Real>(activities), 300.0,
                            std::span<Real>(output));
    for (std::size_t i = 0; i < activities.size(); ++i) {
        EXPECT_NEAR(output[i], isotherm::evaluate(model, activities[i], 300.0),
                    1e-12);
    }
}
