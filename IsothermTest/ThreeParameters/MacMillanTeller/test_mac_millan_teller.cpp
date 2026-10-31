//==============================================================================
// Name        : test_mac_millan_teller.cpp
// Authors     : Joao Flavio Vasconcellos
// Version     : 2.0.0
// Description : Independent reference, derivative and domain tests for MacMillanTeller.
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
using isotherm::models::MacMillanTeller;
TEST(MacMillanTellerModel, TraitsAndConcept) {
    static_assert(isotherm::model_traits<MacMillanTeller>::parameter_count == 3);
    static_assert(isotherm::scalar_isotherm<MacMillanTeller>);
    static_assert(isotherm::differentiable_scalar_isotherm<MacMillanTeller>);
    static_assert(isotherm::model_traits<MacMillanTeller>::temperature_dependent == false);
}
TEST(MacMillanTellerModel, IndependentReferenceValues) {
    const MacMillanTeller model{5, 0.7, 8};
    EXPECT_NEAR(isotherm::evaluate(model, 0.8), 3.3619980311644495, 1e-10 * (1.0 + std::abs(3.3619980311644495)));
    EXPECT_NEAR(isotherm::evaluate(model, 2.0), 3.981538003916334, 1e-10 * (1.0 + std::abs(3.981538003916334)));
    EXPECT_DOUBLE_EQ(isotherm::evaluate(model, 0.0), 0.0);
}
TEST(MacMillanTellerModel, LiteratureExponentOneThird) {
    const MacMillanTeller model{5, 0.7, 8};
    const isotherm::Real x = 0.8;
    const isotherm::Real log_ratio = std::log(8.0 / x);
    const isotherm::Real expected = 5.0 * std::cbrt(0.7 / log_ratio);
    const isotherm::Real expected_slope = expected / (3.0 * x * log_ratio);
    EXPECT_NEAR(isotherm::evaluate(model, x), expected, 1e-12);
    EXPECT_NEAR(isotherm::derivative(model, x), expected_slope, 1e-12);
}
TEST(MacMillanTellerModel, DerivativeMatchesCentralDifference) {
    const MacMillanTeller model{5, 0.7, 8};
    const isotherm::Real h = 1e-5;
    const isotherm::Real numerical = (isotherm::evaluate(model, 1.3 + h) -
                              isotherm::evaluate(model, 1.3 - h)) / (2.0 * h);
    const isotherm::Real analytical = isotherm::derivative(model, 1.3);
    EXPECT_NEAR(analytical, numerical, 2e-6 * (1.0 + std::abs(analytical)));
}
TEST(MacMillanTellerModel, RejectsInvalidParametersAndInputs) {
    EXPECT_THROW((MacMillanTeller{0.0, 0.7, 8}), isotherm::isotherm_error);
    EXPECT_THROW((MacMillanTeller{5, -1.0, 8}), isotherm::isotherm_error);
    const MacMillanTeller model{5, 0.7, 8};
    EXPECT_THROW((isotherm::evaluate(model, -0.1)), isotherm::isotherm_error);
    EXPECT_THROW((isotherm::evaluate(model, std::numeric_limits<isotherm::Real>::infinity())), isotherm::isotherm_error);
    EXPECT_THROW((isotherm::derivative(model, 0.0)), isotherm::isotherm_error);
    
}
TEST(MacMillanTellerModel, SpanEvaluation) {
    const MacMillanTeller model{5, 0.7, 8};
    std::array<isotherm::Real,3> input{0.3,0.8,2.0}, output{-1.0,-1.0,-1.0};
    isotherm::evaluate_into(model, std::span<const isotherm::Real>(input), std::span<isotherm::Real>(output));
    EXPECT_NEAR(output[1], 3.3619980311644495, 1e-10);
}
} // namespace
