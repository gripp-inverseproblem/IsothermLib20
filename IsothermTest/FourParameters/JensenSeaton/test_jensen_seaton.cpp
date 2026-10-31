//==============================================================================
// Name        : test_jensen_seaton.cpp
// Authors     : Joao Flavio Vasconcellos
// Version     : 2.0.0
// Description : Mathematical and domain verification for JensenSeaton.
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
using isotherm::models::JensenSeaton;
TEST(JensenSeatonModel, ConceptsTraitsAndReferences) {
    static_assert(isotherm::scalar_isotherm<JensenSeaton>);
    static_assert(isotherm::differentiable_scalar_isotherm<JensenSeaton>);
    static_assert(isotherm::model_traits<JensenSeaton>::parameter_count == 4);
    const JensenSeaton model{1.4, 3, 0.18, 1.4};
    EXPECT_NEAR(isotherm::evaluate(model, 0.21), 0.28651945446736976, 2e-9*(1.0+std::abs(0.28651945446736976)));
    EXPECT_NEAR(isotherm::evaluate(model, 0.35), 0.46606241907692297, 2e-9*(1.0+std::abs(0.46606241907692297)));
    EXPECT_NEAR(isotherm::evaluate(model, 0.61), 0.7734184072696606, 2e-9*(1.0+std::abs(0.7734184072696606)));
    EXPECT_NEAR(isotherm::evaluate(model, 0.8), 0.97828714669852135, 2e-9*(1.0+std::abs(0.97828714669852135)));
    EXPECT_NEAR(isotherm::evaluate(model, 0.0), 0, 1e-13);
}
TEST(JensenSeatonModel, IndependentFiniteDifferences) {
    const JensenSeaton model{1.4, 3, 0.18, 1.4};
    for (isotherm::Real x : {0.25, 0.5, 0.75}) {
        const isotherm::Real h = x * 1e-5;
        const isotherm::Real fd=(isotherm::evaluate(model,x+h)-isotherm::evaluate(model,x-h))/(2*h);
        const isotherm::Real analytical=isotherm::derivative(model,x);
        EXPECT_NEAR(analytical,fd,3e-6*(1.0+std::abs(fd)));
    }
}
TEST(JensenSeatonModel, RejectsNonPhysicalInputs) {
    const JensenSeaton model{1.4, 3, 0.18, 1.4};
    EXPECT_THROW(static_cast<void>(isotherm::evaluate(model, -0.1)), isotherm::isotherm_error);
    EXPECT_THROW(static_cast<void>(isotherm::evaluate(model, std::numeric_limits<isotherm::Real>::infinity())), isotherm::isotherm_error);
    EXPECT_THROW(static_cast<void>(isotherm::evaluate(model, std::numeric_limits<isotherm::Real>::quiet_NaN())), isotherm::isotherm_error);
    EXPECT_THROW(static_cast<void>(isotherm::derivative(model, 0.0)), isotherm::isotherm_error);
    EXPECT_THROW((JensenSeaton{0.0, 3, 0.18, 1.4}), isotherm::isotherm_error);
}
TEST(JensenSeatonModel, SpanBatchMatchesScalar) {
    const JensenSeaton model{1.4, 3, 0.18, 1.4};
    std::array<isotherm::Real,3> input{0.21,0.41,0.71}, output{-1.,-1.,-1.};
    isotherm::evaluate_into(model,std::span<const isotherm::Real>(input),std::span<isotherm::Real>(output));
    for(unsigned i=0;i<3;++i) EXPECT_NEAR(output[i],isotherm::evaluate(model,input[i]),1e-13);
}
} // namespace
