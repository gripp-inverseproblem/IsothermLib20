//==============================================================================
// Name        : test_flory_huggins.cpp
// Authors     : Joao Flavio Vasconcellos
// Version     : 2.0.0
// Description : Mathematical and domain verification for FloryHuggins.
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
using isotherm::models::FloryHuggins;
TEST(FloryHugginsModel, ConceptsTraitsAndReferences) {
    static_assert(isotherm::scalar_isotherm<FloryHuggins>);
    static_assert(isotherm::differentiable_scalar_isotherm<FloryHuggins>);
    static_assert(isotherm::model_traits<FloryHuggins>::parameter_count == 3);
    const FloryHuggins model{4.2, 0.55, 1.4};
    EXPECT_NEAR(isotherm::evaluate(model, 0.21), 0.4187641888404135, 2e-9*(1.0+std::abs(0.4187641888404135)));
    EXPECT_NEAR(isotherm::evaluate(model, 0.35), 0.6411558099445096, 2e-9*(1.0+std::abs(0.6411558099445096)));
    EXPECT_NEAR(isotherm::evaluate(model, 0.61), 0.97394318744724429, 2e-9*(1.0+std::abs(0.97394318744724429)));
    EXPECT_NEAR(isotherm::evaluate(model, 0.8), 1.1699768797577885, 2e-9*(1.0+std::abs(1.1699768797577885)));
    EXPECT_NEAR(isotherm::evaluate(model, 0.0), 0, 1e-13);
}
TEST(FloryHugginsModel, IndependentFiniteDifferences) {
    const FloryHuggins model{4.2, 0.55, 1.4};
    for (isotherm::Real x : {0.25, 0.5, 0.75}) {
        const isotherm::Real h = x * 1e-5;
        const isotherm::Real fd=(isotherm::evaluate(model,x+h)-isotherm::evaluate(model,x-h))/(2*h);
        const isotherm::Real analytical=isotherm::derivative(model,x);
        EXPECT_NEAR(analytical,fd,3e-6*(1.0+std::abs(fd)));
    }
}
TEST(FloryHugginsModel, RejectsNonPhysicalInputs) {
    const FloryHuggins model{4.2, 0.55, 1.4};
    EXPECT_THROW(isotherm::evaluate(model, -0.1), isotherm::isotherm_error);
    EXPECT_THROW(isotherm::evaluate(model, std::numeric_limits<isotherm::Real>::infinity()), isotherm::isotherm_error);
    EXPECT_THROW(isotherm::evaluate(model, std::numeric_limits<isotherm::Real>::quiet_NaN()), isotherm::isotherm_error);
    EXPECT_THROW(isotherm::derivative(model, 0.0), isotherm::isotherm_error);
    EXPECT_THROW((FloryHuggins{0.0, 0.55, 1.4}), isotherm::isotherm_error);
}
TEST(FloryHugginsModel, SpanBatchMatchesScalar) {
    const FloryHuggins model{4.2, 0.55, 1.4};
    std::array<isotherm::Real,3> input{0.21,0.41,0.71}, output{-1.,-1.,-1.};
    isotherm::evaluate_into(model,std::span<const isotherm::Real>(input),std::span<isotherm::Real>(output));
    for(unsigned i=0;i<3;++i) EXPECT_NEAR(output[i],isotherm::evaluate(model,input[i]),1e-13);
}
} // namespace
