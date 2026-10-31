//==============================================================================
// Name        : test_g_a_b.cpp
// Authors     : Joao Flavio Vasconcellos
// Version     : 2.0.0
// Description : Mathematical and domain verification for GAB.
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
using isotherm::models::GAB;
TEST(GABModel, ConceptsTraitsAndReferences) {
    static_assert(isotherm::scalar_isotherm<GAB>);
    static_assert(isotherm::differentiable_scalar_isotherm<GAB>);
    static_assert(isotherm::model_traits<GAB>::parameter_count == 3);
    const GAB model{3.4, 1.8, 0.72};
    EXPECT_NEAR(isotherm::evaluate(model, 0.21), 0.97254056910422548, 2e-9*(1.0+std::abs(0.97254056910422548)));
    EXPECT_NEAR(isotherm::evaluate(model, 0.35), 1.7158939595690594, 2e-9*(1.0+std::abs(1.7158939595690594)));
    EXPECT_NEAR(isotherm::evaluate(model, 0.61), 3.5467835773326084, 2e-9*(1.0+std::abs(3.5467835773326084)));
    EXPECT_NEAR(isotherm::evaluate(model, 0.8), 5.6913761392051887, 2e-9*(1.0+std::abs(5.6913761392051887)));
    EXPECT_NEAR(isotherm::evaluate(model, 0.0), 0, 1e-13);
}
TEST(GABModel, IndependentFiniteDifferences) {
    const GAB model{3.4, 1.8, 0.72};
    for (isotherm::Real x : {0.25, 0.5, 0.75}) {
        const isotherm::Real h = x * 1e-5;
        const isotherm::Real fd=(isotherm::evaluate(model,x+h)-isotherm::evaluate(model,x-h))/(2*h);
        const isotherm::Real analytical=isotherm::derivative(model,x);
        EXPECT_NEAR(analytical,fd,3e-6*(1.0+std::abs(fd)));
    }
}
TEST(GABModel, RejectsNonPhysicalInputs) {
    const GAB model{3.4, 1.8, 0.72};
    EXPECT_THROW(isotherm::evaluate(model, -0.1), isotherm::isotherm_error);
    EXPECT_THROW(isotherm::evaluate(model, std::numeric_limits<isotherm::Real>::infinity()), isotherm::isotherm_error);
    EXPECT_THROW(isotherm::evaluate(model, std::numeric_limits<isotherm::Real>::quiet_NaN()), isotherm::isotherm_error);
    EXPECT_THROW(isotherm::derivative(model, 0.0), isotherm::isotherm_error);
    EXPECT_THROW((GAB{0.0, 1.8, 0.72}), isotherm::isotherm_error);
}
TEST(GABModel, SpanBatchMatchesScalar) {
    const GAB model{3.4, 1.8, 0.72};
    std::array<isotherm::Real,3> input{0.21,0.41,0.71}, output{-1.,-1.,-1.};
    isotherm::evaluate_into(model,std::span<const isotherm::Real>(input),std::span<isotherm::Real>(output));
    for(unsigned i=0;i<3;++i) EXPECT_NEAR(output[i],isotherm::evaluate(model,input[i]),1e-13);
}
} // namespace
