//==============================================================================
// Name        : test_volmer.cpp
// Authors     : Joao Flavio Vasconcellos
// Version     : 2.0.0
// Description : Mathematical and domain verification for Volmer.
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

using isotherm::Real;

namespace {
using isotherm::models::Volmer;
TEST(VolmerModel, ConceptsTraitsAndReferences) {
    static_assert(isotherm::scalar_isotherm<Volmer>);
    static_assert(isotherm::differentiable_scalar_isotherm<Volmer>);
    static_assert(isotherm::model_traits<Volmer>::parameter_count == 2);
    const Volmer model{4.5, 0.7};
    EXPECT_NEAR(isotherm::evaluate(model, 0.21), 0.51482569996173799, 2e-9*(1.0+std::abs(0.51482569996173799)));
    EXPECT_NEAR(isotherm::evaluate(model, 0.35), 0.75153262762680595, 2e-9*(1.0+std::abs(0.75153262762680595)));
    EXPECT_NEAR(isotherm::evaluate(model, 0.61), 1.0712292666161132, 2e-9*(1.0+std::abs(1.0712292666161132)));
    EXPECT_NEAR(isotherm::evaluate(model, 0.8), 1.2441856663567672, 2e-9*(1.0+std::abs(1.2441856663567672)));
    EXPECT_NEAR(isotherm::evaluate(model, 0.0), 0, 1e-13);
}
TEST(VolmerModel, IndependentFiniteDifferences) {
    const Volmer model{4.5, 0.7};
    for (Real x : {0.25, 0.5, 0.75}) {
        const Real h = x * 1e-5;
        const Real fd=(isotherm::evaluate(model,x+h)-isotherm::evaluate(model,x-h))/(2*h);
        const Real analytical=isotherm::derivative(model,x);
        EXPECT_NEAR(analytical,fd,3e-6*(1.0+std::abs(fd)));
    }
}
TEST(VolmerModel, RejectsNonPhysicalInputs) {
    const Volmer model{4.5, 0.7};
    EXPECT_THROW(static_cast<void>(isotherm::evaluate(model, -0.1)), isotherm::isotherm_error);
    EXPECT_THROW(static_cast<void>(isotherm::evaluate(model, std::numeric_limits<Real>::infinity())), isotherm::isotherm_error);
    EXPECT_THROW(static_cast<void>(isotherm::evaluate(model, std::numeric_limits<Real>::quiet_NaN())), isotherm::isotherm_error);
    EXPECT_THROW(static_cast<void>(isotherm::derivative(model, 0.0)), isotherm::isotherm_error);
    EXPECT_THROW((Volmer{0.0, 0.7}), isotherm::isotherm_error);
}
TEST(VolmerModel, SpanBatchMatchesScalar) {
    const Volmer model{4.5, 0.7};
    std::array<Real,3> input{0.21,0.41,0.71}, output{-1.,-1.,-1.};
    isotherm::evaluate_into(model,std::span<const Real>(input),std::span<Real>(output));
    for(unsigned i=0;i<3;++i) EXPECT_NEAR(output[i],isotherm::evaluate(model,input[i]),1e-13);
}
} // namespace
