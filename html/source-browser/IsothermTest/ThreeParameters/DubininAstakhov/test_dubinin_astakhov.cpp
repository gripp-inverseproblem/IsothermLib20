//==============================================================================
// Name        : test_dubinin_astakhov.cpp
// Authors     : Joao Flavio Vasconcellos
// Version     : 2.0.0
// Description : Mathematical and domain verification for DubininAstakhov.
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
using isotherm::models::DubininAstakhov;
TEST(DubininAstakhovModel, ConceptsTraitsAndReferences) {
    static_assert(isotherm::scalar_isotherm<DubininAstakhov>);
    static_assert(isotherm::differentiable_scalar_isotherm<DubininAstakhov>);
    static_assert(isotherm::model_traits<DubininAstakhov>::parameter_count == 3);
    const DubininAstakhov model{4.8, 0.3, 2};
    EXPECT_NEAR(isotherm::evaluate(model, 0.21), 2.3115758998539464, 2e-9*(1.0+std::abs(2.3115758998539464)));
    EXPECT_NEAR(isotherm::evaluate(model, 0.35), 3.448633170187128, 2e-9*(1.0+std::abs(3.448633170187128)));
    EXPECT_NEAR(isotherm::evaluate(model, 0.61), 4.4607515543778611, 2e-9*(1.0+std::abs(4.4607515543778611)));
    EXPECT_NEAR(isotherm::evaluate(model, 0.8), 4.7288308982593064, 2e-9*(1.0+std::abs(4.7288308982593064)));
    EXPECT_NEAR(isotherm::evaluate(model, 0.0), 0, 1e-13);
}
TEST(DubininAstakhovModel, IndependentFiniteDifferences) {
    const DubininAstakhov model{4.8, 0.3, 2};
    for (isotherm::Real x : {0.25, 0.5, 0.75}) {
        const isotherm::Real h = x * 1e-5;
        const isotherm::Real fd=(isotherm::evaluate(model,x+h)-isotherm::evaluate(model,x-h))/(2*h);
        const isotherm::Real analytical=isotherm::derivative(model,x);
        EXPECT_NEAR(analytical,fd,3e-6*(1.0+std::abs(fd)));
    }
}
TEST(DubininAstakhovModel, RejectsNonPhysicalInputs) {
    const DubininAstakhov model{4.8, 0.3, 2};
    EXPECT_THROW(static_cast<void>(isotherm::evaluate(model, -0.1)), isotherm::isotherm_error);
    EXPECT_THROW(static_cast<void>(isotherm::evaluate(model, std::numeric_limits<isotherm::Real>::infinity())), isotherm::isotherm_error);
    EXPECT_THROW(static_cast<void>(isotherm::evaluate(model, std::numeric_limits<isotherm::Real>::quiet_NaN())), isotherm::isotherm_error);
    EXPECT_THROW(static_cast<void>(isotherm::derivative(model, 0.0)), isotherm::isotherm_error);
    EXPECT_THROW((DubininAstakhov{0.0, 0.3, 2}), isotherm::isotherm_error);
}
TEST(DubininAstakhovModel, SpanBatchMatchesScalar) {
    const DubininAstakhov model{4.8, 0.3, 2};
    std::array<isotherm::Real,3> input{0.21,0.41,0.71}, output{-1.,-1.,-1.};
    isotherm::evaluate_into(model,std::span<const isotherm::Real>(input),std::span<isotherm::Real>(output));
    for(unsigned i=0;i<3;++i) EXPECT_NEAR(output[i],isotherm::evaluate(model,input[i]),1e-13);
}
} // namespace
