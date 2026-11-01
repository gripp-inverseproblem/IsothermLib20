//==============================================================================
// Name        : test_triple_site_langmuir.cpp
// Authors     : Joao Flavio Vasconcellos
// Version     : 2.0.0
// Description : Mathematical and domain verification for TripleSiteLangmuir.
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
using isotherm::models::TripleSiteLangmuir;
TEST(TripleSiteLangmuirModel, ConceptsTraitsAndReferences) {
    static_assert(isotherm::scalar_isotherm<TripleSiteLangmuir>);
    static_assert(isotherm::differentiable_scalar_isotherm<TripleSiteLangmuir>);
    static_assert(isotherm::model_traits<TripleSiteLangmuir>::parameter_count == 6);
    const TripleSiteLangmuir model{1.5, 0.9, 2.1, 0.3, 0.7, 0.08};
    EXPECT_NEAR(isotherm::evaluate(model, 0.21), 0.37446043460169859, 2e-9*(1.0+std::abs(0.37446043460169859)));
    EXPECT_NEAR(isotherm::evaluate(model, 0.35), 0.57792924852575156, 2e-9*(1.0+std::abs(0.57792924852575156)));
    EXPECT_NEAR(isotherm::evaluate(model, 0.61), 0.88905593964684049, 2e-9*(1.0+std::abs(0.88905593964684049)));
    EXPECT_NEAR(isotherm::evaluate(model, 0.8), 1.0764638528053065, 2e-9*(1.0+std::abs(1.0764638528053065)));
    EXPECT_NEAR(isotherm::evaluate(model, 0.0), 0, 1e-13);
}
TEST(TripleSiteLangmuirModel, IndependentFiniteDifferences) {
    const TripleSiteLangmuir model{1.5, 0.9, 2.1, 0.3, 0.7, 0.08};
    for (isotherm::Real x : {0.25, 0.5, 0.75}) {
        const isotherm::Real h = x * 1e-5;
        const isotherm::Real fd=(isotherm::evaluate(model,x+h)-isotherm::evaluate(model,x-h))/(2*h);
        const isotherm::Real analytical=isotherm::derivative(model,x);
        EXPECT_NEAR(analytical,fd,3e-6*(1.0+std::abs(fd)));
    }
}
TEST(TripleSiteLangmuirModel, RejectsNonPhysicalInputs) {
    const TripleSiteLangmuir model{1.5, 0.9, 2.1, 0.3, 0.7, 0.08};
    EXPECT_THROW(static_cast<void>(isotherm::evaluate(model, -0.1)), isotherm::isotherm_error);
    EXPECT_THROW(static_cast<void>(isotherm::evaluate(model, std::numeric_limits<isotherm::Real>::infinity())), isotherm::isotherm_error);
    EXPECT_THROW(static_cast<void>(isotherm::evaluate(model, std::numeric_limits<isotherm::Real>::quiet_NaN())), isotherm::isotherm_error);
    EXPECT_THROW(static_cast<void>(isotherm::derivative(model, 0.0)), isotherm::isotherm_error);
    EXPECT_THROW((TripleSiteLangmuir{0.0, 0.9, 2.1, 0.3, 0.7, 0.08}), isotherm::isotherm_error);
}
TEST(TripleSiteLangmuirModel, SpanBatchMatchesScalar) {
    const TripleSiteLangmuir model{1.5, 0.9, 2.1, 0.3, 0.7, 0.08};
    std::array<isotherm::Real,3> input{0.21,0.41,0.71}, output{-1.,-1.,-1.};
    isotherm::evaluate_into(model,std::span<const isotherm::Real>(input),std::span<isotherm::Real>(output));
    for(unsigned i=0;i<3;++i) EXPECT_NEAR(output[i],isotherm::evaluate(model,input[i]),1e-13);
}
} // namespace
