//==============================================================================
// Name        : test_dubinin_radushkevich.cpp
// Authors     : Joao Flavio Vasconcellos
// Version     : 2.0.0
// Description : Independent numerical and domain tests of DubininRadushkevich.
//
// Copyright   : Copyright (C) 2026 Joao Flavio Vasconcellos
// Licence     : MIT License (see LICENSE.txt)
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
using Model = isotherm::models::DubininRadushkevich;

/** @brief Verify compile-time metadata and the model's parameter count. */
TEST(DubininRadushkevichModel, ModelTraits) {
    static_assert(isotherm::model_traits<Model>::parameter_count == 2);
    static_assert(isotherm::differentiable_thermal_isotherm<Model>);
    EXPECT_EQ(isotherm::model_traits<Model>::parameter_names.size(), 2U);
    EXPECT_EQ(isotherm::model_traits<Model>::temperature_dependent, true);
}

/** @brief Independently verify a finite positive example and its derivative. */
TEST(DubininRadushkevichModel, DerivativeFiniteDifference) {
    const Model model{2.7, 0.000001};
    const isotherm::Real x = 0.8;
    constexpr isotherm::Real h = 1e-5;
    const isotherm::Real value = isotherm::evaluate(model, x, 298.15);
    const isotherm::Real slope = isotherm::derivative(model, x, 298.15);
    const isotherm::Real fd = (isotherm::evaluate(model, x+h, 298.15) -
        isotherm::evaluate(model, x-h, 298.15))/(2*h);
    EXPECT_TRUE(std::isfinite(value));
    EXPECT_TRUE(std::isfinite(slope));
    EXPECT_NEAR(slope, fd, 2e-4 * (1 + std::abs(slope)));
}

/** @brief Reject invalid finite and nonfinite parameters at construction. */
TEST(DubininRadushkevichModel, ParameterValidation) {
    EXPECT_THROW(static_cast<void>(Model{0.0, 0.000001}), isotherm::isotherm_error);
    EXPECT_THROW(static_cast<void>(Model{2.7, 0.0}), isotherm::isotherm_error);
    EXPECT_THROW(static_cast<void>(Model{-1.0, 0.000001}), isotherm::isotherm_error);
    EXPECT_THROW(static_cast<void>(Model{2.7, std::numeric_limits<isotherm::Real>::infinity()}), isotherm::isotherm_error);
}

/** @brief Guard the documented physical or mathematical input domain. */
TEST(DubininRadushkevichModel, InputValidation) {
    const Model model{2.7, 0.000001};
    EXPECT_THROW(static_cast<void>(isotherm::evaluate(model, -1.0, 298.15)), isotherm::isotherm_error);
    EXPECT_THROW(static_cast<void>(isotherm::evaluate(model, std::numeric_limits<isotherm::Real>::quiet_NaN(), 298.15)), isotherm::isotherm_error);
    EXPECT_THROW(static_cast<void>(isotherm::evaluate(model, 1.0, 0.0)), isotherm::isotherm_error);
}

/** @brief Check the defining equation using independent reference arithmetic. */
TEST(DubininRadushkevichModel, EnergyUnitEquivalence) {
    using isotherm::gas_constant_unit;
    constexpr isotherm::Real temperature_k = 298.15;
    constexpr isotherm::Real beta_si = 1e-6;
    const Model si{2.7, beta_si};
    const Model kj{2.7, beta_si * 1e6,
                   gas_constant_unit::kilojoule_per_mole_kelvin};
    const Model atm{2.7, beta_si * 101.325 * 101.325,
                    gas_constant_unit::litre_atmosphere_per_mole_kelvin};
    for (isotherm::Real c : {isotherm::Real{0.1}, isotherm::Real{0.5}, isotherm::Real{1.0}}) {
        EXPECT_NEAR(isotherm::evaluate(si, c, temperature_k),
                    isotherm::evaluate(kj, c, temperature_k), 1e-12);
        EXPECT_NEAR(isotherm::evaluate(si, c, temperature_k),
                    isotherm::evaluate(atm, c, temperature_k), 1e-12);
    }
    EXPECT_THROW(static_cast<void>(Model{2.7, beta_si,
        static_cast<gas_constant_unit>(-1)}), isotherm::isotherm_error);
}

TEST(DubininRadushkevichModel, ZeroConcentrationLimit) {
    const Model model{2.7, 0.000001};
    EXPECT_EQ(isotherm::evaluate(model, 0.0, 298.15), isotherm::Real{0.0});
    EXPECT_EQ(isotherm::derivative(model, 0.0, 298.15), isotherm::Real{0.0});
}

TEST(DubininRadushkevichModel, IndependentIdentity) {
    const Model model{2.7, 0.000001};
    constexpr isotherm::Real x = 1.0;
    const isotherm::Real q = isotherm::evaluate(model, x, 298.15);
    const isotherm::Real e = 8.31446261815324*298.15*std::log1p(1.0/x);
    EXPECT_NEAR(q, 2.7*std::exp(-0.000001*e*e), 1e-13);
}
}
