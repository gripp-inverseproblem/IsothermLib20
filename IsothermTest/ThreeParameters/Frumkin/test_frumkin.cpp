//==============================================================================
// Name        : test_frumkin.cpp
// Authors     : Joao Flavio Vasconcellos
// Version     : 2.0.0
// Description : Mathematical and domain verification for Frumkin.
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
using isotherm::models::Frumkin;
TEST(FrumkinModel, ConceptsTraitsAndReferences) {
    static_assert(isotherm::scalar_isotherm<Frumkin>);
    static_assert(isotherm::differentiable_scalar_isotherm<Frumkin>);
    static_assert(isotherm::model_traits<Frumkin>::parameter_count == 3);
    const Frumkin model{4.1, 0.7, 1};
    EXPECT_NEAR(isotherm::evaluate(model, 0.21), 0.59568771690287259, 2e-9*(1.0+std::abs(0.59568771690287259)));
    EXPECT_NEAR(isotherm::evaluate(model, 0.35), 0.97142925379448453, 2e-9*(1.0+std::abs(0.97142925379448453)));
    EXPECT_NEAR(isotherm::evaluate(model, 0.61), 1.5815638655477315, 2e-9*(1.0+std::abs(1.5815638655477315)));
    EXPECT_NEAR(isotherm::evaluate(model, 0.8), 1.941051719111879, 2e-9*(1.0+std::abs(1.941051719111879)));
    EXPECT_NEAR(isotherm::evaluate(model, 0.0), 0, 1e-13);
}
TEST(FrumkinModel, IndependentFiniteDifferences) {
    const Frumkin model{4.1, 0.7, 1};
    for (isotherm::Real x : {0.25, 0.5, 0.75}) {
        const isotherm::Real h = x * 1e-5;
        const isotherm::Real fd=(isotherm::evaluate(model,x+h)-isotherm::evaluate(model,x-h))/(2*h);
        const isotherm::Real analytical=isotherm::derivative(model,x);
        EXPECT_NEAR(analytical,fd,3e-6*(1.0+std::abs(fd)));
    }
}
TEST(FrumkinModel, RejectsNonPhysicalInputs) {
    const Frumkin model{4.1, 0.7, 1};
    EXPECT_THROW(isotherm::evaluate(model, -0.1), isotherm::isotherm_error);
    EXPECT_THROW(isotherm::evaluate(model, std::numeric_limits<isotherm::Real>::infinity()), isotherm::isotherm_error);
    EXPECT_THROW(isotherm::evaluate(model, std::numeric_limits<isotherm::Real>::quiet_NaN()), isotherm::isotherm_error);
    EXPECT_THROW(isotherm::derivative(model, 0.0), isotherm::isotherm_error);
    EXPECT_THROW((Frumkin{0.0, 0.7, 1}), isotherm::isotherm_error);
}
TEST(FrumkinModel, SpanBatchMatchesScalar) {
    const Frumkin model{4.1, 0.7, 1};
    std::array<isotherm::Real,3> input{0.21,0.41,0.71}, output{-1.,-1.,-1.};
    isotherm::evaluate_into(model,std::span<const isotherm::Real>(input),std::span<isotherm::Real>(output));
    for(unsigned i=0;i<3;++i) EXPECT_NEAR(output[i],isotherm::evaluate(model,input[i]),1e-13);
}
} // namespace
