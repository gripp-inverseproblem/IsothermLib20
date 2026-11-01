//==============================================================================
// Name        : test_flory_huggins_v_s_t.cpp
// Authors     : Joao Flavio Vasconcellos
// Version     : 2.0.0
// Description : Mathematical and domain verification for FloryHugginsVST.
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
//==============================================================================
// Isotherm++ headers
//==============================================================================
#include "isotherm/isotherm.hpp"
//==============================================================================
// GoogleTest headers
//==============================================================================
#include <gtest/gtest.h>

namespace {
using isotherm::models::FloryHugginsVST;
TEST(FloryHugginsVSTModel, ConceptsTraitsAndReferences) {
    static_assert(isotherm::scalar_isotherm<FloryHugginsVST>);
    static_assert(isotherm::differentiable_scalar_isotherm<FloryHugginsVST>);
    static_assert(isotherm::model_traits<FloryHugginsVST>::parameter_count == 3);
    const FloryHugginsVST model{4.2, 1.3, 0.35};
    EXPECT_NEAR(isotherm::evaluate(model, 0.21), 0.25459343627684822, 2e-9*(1.0+std::abs(0.25459343627684822)));
    EXPECT_NEAR(isotherm::evaluate(model, 0.35), 0.40630020539555178, 2e-9*(1.0+std::abs(0.40630020539555178)));
    EXPECT_NEAR(isotherm::evaluate(model, 0.61), 0.65692390087003683, 2e-9*(1.0+std::abs(0.65692390087003683)));
    EXPECT_NEAR(isotherm::evaluate(model, 0.8), 0.81875207087775514, 2e-9*(1.0+std::abs(0.81875207087775514)));
    EXPECT_NEAR(isotherm::evaluate(model, 0.0), 0, 1e-13);
}
TEST(FloryHugginsVSTModel, IndependentFiniteDifferences) {
    const FloryHugginsVST model{4.2, 1.3, 0.35};
    for (isotherm::Real x : {0.25, 0.5, 0.75}) {
        const isotherm::Real h = x * 1e-5;
        const isotherm::Real fd=(isotherm::evaluate(model,x+h)-isotherm::evaluate(model,x-h))/(2*h);
        const isotherm::Real analytical=isotherm::derivative(model,x);
        EXPECT_NEAR(analytical,fd,3e-6*(1.0+std::abs(fd)));
    }
}
TEST(FloryHugginsVSTModel, RejectsNonPhysicalInputs) {
    const FloryHugginsVST model{4.2, 1.3, 0.35};
    EXPECT_THROW(static_cast<void>(isotherm::evaluate(model, -0.1)), isotherm::isotherm_error);
    EXPECT_THROW(static_cast<void>(isotherm::evaluate(model, std::numeric_limits<isotherm::Real>::infinity())), isotherm::isotherm_error);
    EXPECT_THROW(static_cast<void>(isotherm::evaluate(model, std::numeric_limits<isotherm::Real>::quiet_NaN())), isotherm::isotherm_error);
    EXPECT_THROW(static_cast<void>(isotherm::derivative(model, 0.0)), isotherm::isotherm_error);
    EXPECT_THROW((FloryHugginsVST{0.0, 1.3, 0.35}), isotherm::isotherm_error);
}
TEST(FloryHugginsVSTModel, SpanBatchMatchesScalar) {
    const FloryHugginsVST model{4.2, 1.3, 0.35};
    std::array<isotherm::Real,3> input{0.21,0.41,0.71}, output{-1.,-1.,-1.};
    isotherm::evaluate_into(model,std::span<const isotherm::Real>(input),std::span<isotherm::Real>(output));
    for(unsigned i=0;i<3;++i) EXPECT_NEAR(output[i],isotherm::evaluate(model,input[i]),1e-13);
}
} // namespace
