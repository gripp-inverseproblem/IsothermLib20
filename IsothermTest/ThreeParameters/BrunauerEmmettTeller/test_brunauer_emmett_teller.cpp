//==============================================================================
// Name        : test_brunauer_emmett_teller.cpp
// Authors     : Joao Flavio Vasconcellos
// Version     : 2.0.0
// Description : Independent reference, derivative and domain tests for BrunauerEmmettTeller.
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
using isotherm::models::BrunauerEmmettTeller;
TEST(BrunauerEmmettTellerModel, TraitsAndConcept) {
    static_assert(isotherm::model_traits<BrunauerEmmettTeller>::parameter_count == 3);
    static_assert(isotherm::scalar_isotherm<BrunauerEmmettTeller>);
    static_assert(isotherm::differentiable_scalar_isotherm<BrunauerEmmettTeller>);
    static_assert(isotherm::model_traits<BrunauerEmmettTeller>::temperature_dependent == false);
}
TEST(BrunauerEmmettTellerModel, IndependentReferenceValues) {
    const BrunauerEmmettTeller model{5, 2, 8};
    EXPECT_NEAR(isotherm::evaluate(model, 0.8), 1.0101010101010099, 1e-10 * (1.0 + std::abs(1.0101010101010099)));
    EXPECT_NEAR(isotherm::evaluate(model, 2.0), 2.6666666666666665, 1e-10 * (1.0 + std::abs(2.6666666666666665)));
    EXPECT_DOUBLE_EQ(isotherm::evaluate(model, 0.0), 0.0);
}
TEST(BrunauerEmmettTellerModel, DerivativeMatchesCentralDifference) {
    const BrunauerEmmettTeller model{5, 2, 8};
    const isotherm::Real h = 1e-5;
    const isotherm::Real numerical = (isotherm::evaluate(model, 1.3 + h) -
                              isotherm::evaluate(model, 1.3 - h)) / (2.0 * h);
    const isotherm::Real analytical = isotherm::derivative(model, 1.3);
    EXPECT_NEAR(analytical, numerical, 2e-6 * (1.0 + std::abs(analytical)));
}
TEST(BrunauerEmmettTellerModel, RejectsInvalidParametersAndInputs) {
    EXPECT_THROW((BrunauerEmmettTeller{0.0, 2, 8}), isotherm::isotherm_error);
    EXPECT_THROW((BrunauerEmmettTeller{5, -1.0, 8}), isotherm::isotherm_error);
    const BrunauerEmmettTeller model{5, 2, 8};
    EXPECT_THROW(static_cast<void>(isotherm::evaluate(model, -0.1)), isotherm::isotherm_error);
    EXPECT_THROW(static_cast<void>(isotherm::evaluate(model, std::numeric_limits<isotherm::Real>::infinity())), isotherm::isotherm_error);
    EXPECT_THROW(static_cast<void>(isotherm::derivative(model, 0.0)), isotherm::isotherm_error);
    
}
TEST(BrunauerEmmettTellerModel, SpanEvaluation) {
    const BrunauerEmmettTeller model{5, 2, 8};
    std::array<isotherm::Real,3> input{0.3,0.8,2.0}, output{-1.0,-1.0,-1.0};
    isotherm::evaluate_into(model, std::span<const isotherm::Real>(input), std::span<isotherm::Real>(output));
    EXPECT_NEAR(output[1], 1.0101010101010099, 1e-10);
}
} // namespace
