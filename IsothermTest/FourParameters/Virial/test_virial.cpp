//==============================================================================
// Name        : test_virial.cpp
// Authors     : Joao Flavio Vasconcellos
// Version     : 2.0.0
// Description : Mathematical and domain verification for Virial.
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
using isotherm::models::Virial;
TEST(VirialModel, ConceptsTraitsAndReferences) {
    static_assert(isotherm::scalar_isotherm<Virial>);
    static_assert(isotherm::differentiable_scalar_isotherm<Virial>);
    static_assert(isotherm::model_traits<Virial>::parameter_count == 4);
    const Virial model{1, 0.25, 0.045, 0.01};
    EXPECT_NEAR(isotherm::evaluate(model, 0.21), 0.19941441612090133, 2e-9*(1.0+std::abs(0.19941441612090133)));
    EXPECT_NEAR(isotherm::evaluate(model, 0.35), 0.32137533345901181, 2e-9*(1.0+std::abs(0.32137533345901181)));
    EXPECT_NEAR(isotherm::evaluate(model, 0.61), 0.52724830038093939, 2e-9*(1.0+std::abs(0.52724830038093939)));
    EXPECT_NEAR(isotherm::evaluate(model, 0.8), 0.66266791864810304, 2e-9*(1.0+std::abs(0.66266791864810304)));
    EXPECT_NEAR(isotherm::evaluate(model, 0.0), 0, 1e-13);
}
TEST(VirialModel, IndependentFiniteDifferences) {
    const Virial model{1, 0.25, 0.045, 0.01};
    for (isotherm::Real x : {0.25, 0.5, 0.75}) {
        const isotherm::Real h = x * 1e-5;
        const isotherm::Real fd=(isotherm::evaluate(model,x+h)-isotherm::evaluate(model,x-h))/(2*h);
        const isotherm::Real analytical=isotherm::derivative(model,x);
        EXPECT_NEAR(analytical,fd,3e-6*(1.0+std::abs(fd)));
    }
}
TEST(VirialModel, RejectsNonPhysicalInputs) {
    const Virial model{1, 0.25, 0.045, 0.01};
    EXPECT_THROW(isotherm::evaluate(model, -0.1), isotherm::isotherm_error);
    EXPECT_THROW(isotherm::evaluate(model, std::numeric_limits<isotherm::Real>::infinity()), isotherm::isotherm_error);
    EXPECT_THROW(isotherm::evaluate(model, std::numeric_limits<isotherm::Real>::quiet_NaN()), isotherm::isotherm_error);
    EXPECT_THROW(isotherm::derivative(model, 0.0), isotherm::isotherm_error);
    EXPECT_THROW((Virial{0.0, 0.25, 0.045, 0.01}), isotherm::isotherm_error);
}
TEST(VirialModel, SpanBatchMatchesScalar) {
    const Virial model{1, 0.25, 0.045, 0.01};
    std::array<isotherm::Real,3> input{0.21,0.41,0.71}, output{-1.,-1.,-1.};
    isotherm::evaluate_into(model,std::span<const isotherm::Real>(input),std::span<isotherm::Real>(output));
    for(unsigned i=0;i<3;++i) EXPECT_NEAR(output[i],isotherm::evaluate(model,input[i]),1e-13);
}
} // namespace
