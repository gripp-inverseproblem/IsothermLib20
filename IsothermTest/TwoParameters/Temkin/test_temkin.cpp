//==============================================================================
// Name        : test_temkin.cpp
// Authors     : Joao Flavio Vasconcellos
// Version     : 2.0.0
// Description : Independent numerical and domain tests of Temkin.
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
using Model = isotherm::models::Temkin;
using isotherm::Real;

/** @brief Verify compile-time metadata and the model's parameter count. */
TEST(TemkinModel, ModelTraits) {
    static_assert(isotherm::model_traits<Model>::parameter_count == 2);
    static_assert(isotherm::differentiable_thermal_isotherm<Model>);
    EXPECT_EQ(isotherm::model_traits<Model>::parameter_names.size(), 2U);
    EXPECT_EQ(isotherm::model_traits<Model>::temperature_dependent, true);
}

/** @brief Independently verify a finite positive example and its derivative. */
TEST(TemkinModel, DerivativeFiniteDifference) {
    const Model model{4.0, 1800.0};
    const Real x = 0.8;
    constexpr Real h = 1e-5;
    const Real value = isotherm::evaluate(model, x, 298.15);
    const Real slope = isotherm::derivative(model, x, 298.15);
    const Real fd = (isotherm::evaluate(model, x+h, 298.15) -
        isotherm::evaluate(model, x-h, 298.15))/(2*h);
    EXPECT_TRUE(std::isfinite(value));
    EXPECT_TRUE(std::isfinite(slope));
    EXPECT_NEAR(slope, fd, 2e-4 * (1 + std::abs(slope)));
}

/** @brief Reject invalid finite and nonfinite parameters at construction. */
TEST(TemkinModel, ParameterValidation) {
    EXPECT_THROW(static_cast<void>(Model{0.0, 1800.0}), isotherm::isotherm_error);
    EXPECT_THROW(static_cast<void>(Model{4.0, 0.0}), isotherm::isotherm_error);
    EXPECT_THROW(static_cast<void>(Model{-1.0, 1800.0}), isotherm::isotherm_error);
    EXPECT_THROW(static_cast<void>(Model{4.0, std::numeric_limits<Real>::infinity()}), isotherm::isotherm_error);
}

/** @brief Guard the documented physical or mathematical input domain. */
TEST(TemkinModel, InputValidation) {
    const Model model{4.0, 1800.0};
    EXPECT_THROW(static_cast<void>(isotherm::evaluate(model, -1.0, 298.15)), isotherm::isotherm_error);
    EXPECT_THROW(static_cast<void>(isotherm::evaluate(model, std::numeric_limits<Real>::quiet_NaN(), 298.15)), isotherm::isotherm_error);
    EXPECT_THROW(static_cast<void>(isotherm::evaluate(model, 1.0, 0.0)), isotherm::isotherm_error);
}

/** @brief Check the defining equation using independent reference arithmetic. */
TEST(TemkinModel, EnergyUnitEquivalence) {
    constexpr Real temperature_k = 298.15;
    using isotherm::gas_constant_unit;
    const Model si{4.0, 1800.0};
    const Model kj{4.0, 1.8, gas_constant_unit::kilojoule_per_mole_kelvin};
    const Model atm{4.0, 1800.0 / 101.325,
                    gas_constant_unit::litre_atmosphere_per_mole_kelvin};
    for (Real c : {Real{0.5}, Real{1.0}, Real{2.0}}) {
        EXPECT_NEAR(isotherm::evaluate(si, c, temperature_k),
                    isotherm::evaluate(kj, c, temperature_k), 1e-12);
        EXPECT_NEAR(isotherm::evaluate(si, c, temperature_k),
                    isotherm::evaluate(atm, c, temperature_k), 1e-12);
    }
    EXPECT_THROW(static_cast<void>(Model{4.0, 1800.0,
        static_cast<gas_constant_unit>(-1)}), isotherm::isotherm_error);
}

TEST(TemkinModel, IndependentIdentity) {
    const Model model{4.0, 1800.0};
    constexpr Real x = 1.0;
    const Real q = isotherm::evaluate(model, x, 298.15);
    EXPECT_NEAR(q, (8.31446261815324*298.15/1800.0)*std::log(4*x), 1e-12);
}
}
