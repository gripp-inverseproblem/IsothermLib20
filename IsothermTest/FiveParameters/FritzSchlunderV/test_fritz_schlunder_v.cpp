//==============================================================================
// Name        : test_fritz_schlunder_v.cpp
// Authors     : Joao Flavio Vasconcellos
// Version     : 2.0.0
// Description : Mathematical and domain verification for FritzSchlunderV.
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
using isotherm::models::FritzSchlunderV;
TEST(FritzSchlunderVModel, ConceptsTraitsAndReferences) {
    static_assert(isotherm::scalar_isotherm<FritzSchlunderV>);
    static_assert(isotherm::differentiable_scalar_isotherm<FritzSchlunderV>);
    static_assert(isotherm::model_traits<FritzSchlunderV>::parameter_count == 5);
    const FritzSchlunderV model{6.05758, 0.217337, 0.0885359, 0.0169304, 0.0746286};
    EXPECT_NEAR(isotherm::evaluate(model, 0.21), 19.921790267276098, 2e-9*(1.0+std::abs(19.921790267276098)));
    EXPECT_NEAR(isotherm::evaluate(model, 0.35), 19.889174863807312, 2e-9*(1.0+std::abs(19.889174863807312)));
    EXPECT_NEAR(isotherm::evaluate(model, 0.61), 19.847254768828925, 2e-9*(1.0+std::abs(19.847254768828925)));
    EXPECT_NEAR(isotherm::evaluate(model, 0.8), 19.824328100723395, 2e-9*(1.0+std::abs(19.824328100723395)));
    EXPECT_NEAR(isotherm::evaluate(model, 0.0), 0, 1e-13);
}
TEST(FritzSchlunderVModel, IndependentFiniteDifferences) {
    const FritzSchlunderV model{6.05758, 0.217337, 0.0885359, 0.0169304, 0.0746286};
    for (isotherm::Real x : {0.25, 0.5, 0.75}) {
        const isotherm::Real h = x * 1e-5;
        const isotherm::Real fd=(isotherm::evaluate(model,x+h)-isotherm::evaluate(model,x-h))/(2*h);
        const isotherm::Real analytical=isotherm::derivative(model,x);
        EXPECT_NEAR(analytical,fd,3e-6*(1.0+std::abs(fd)));
    }
}
TEST(FritzSchlunderVModel, RejectsNonPhysicalInputs) {
    const FritzSchlunderV model{6.05758, 0.217337, 0.0885359, 0.0169304, 0.0746286};
    EXPECT_THROW(static_cast<void>(isotherm::evaluate(model, -0.1)), isotherm::isotherm_error);
    EXPECT_THROW(static_cast<void>(isotherm::evaluate(model, std::numeric_limits<isotherm::Real>::infinity())), isotherm::isotherm_error);
    EXPECT_THROW(static_cast<void>(isotherm::evaluate(model, std::numeric_limits<isotherm::Real>::quiet_NaN())), isotherm::isotherm_error);
    EXPECT_THROW(static_cast<void>(isotherm::derivative(model, 0.0)), isotherm::isotherm_error);
    EXPECT_THROW((FritzSchlunderV{0.0, 0.217337, 0.0885359, 0.0169304, 0.0746286}), isotherm::isotherm_error);
}
TEST(FritzSchlunderVModel, SpanBatchMatchesScalar) {
    const FritzSchlunderV model{6.05758, 0.217337, 0.0885359, 0.0169304, 0.0746286};
    std::array<isotherm::Real,3> input{0.21,0.41,0.71}, output{-1.,-1.,-1.};
    isotherm::evaluate_into(model,std::span<const isotherm::Real>(input),std::span<isotherm::Real>(output));
    for(unsigned i=0;i<3;++i) EXPECT_NEAR(output[i],isotherm::evaluate(model,input[i]),1e-13);
}
} // namespace
