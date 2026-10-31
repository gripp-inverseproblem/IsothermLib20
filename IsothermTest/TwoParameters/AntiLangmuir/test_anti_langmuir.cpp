//==============================================================================
// Name        : test_anti_langmuir.cpp
// Authors     : Joao Flavio Vasconcellos
// Version     : 2.0.0
// Description : Mathematical and domain verification for AntiLangmuir.
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
#include <concepts>
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
using isotherm::Real;
using isotherm::models::AntiLangmuir;
TEST(AntiLangmuirModel, ConceptsTraitsAndReferences) {
    static_assert(isotherm::scalar_isotherm<AntiLangmuir>);
    static_assert(isotherm::differentiable_scalar_isotherm<AntiLangmuir>);
    static_assert(isotherm::model_traits<AntiLangmuir>::parameter_count == 2);
    static_assert(std::same_as<decltype(AntiLangmuir{1.6, 0.12}.evaluate(0.5)), Real>);
    static_assert(std::same_as<decltype(AntiLangmuir{1.6, 0.12}.derivative(0.5)), Real>);
    const AntiLangmuir model{1.6, 0.12};
    EXPECT_NEAR(isotherm::evaluate(model, 0.21), 0.34468608945424706, 2e-9*(1.0+std::abs(0.34468608945424706)));
    EXPECT_NEAR(isotherm::evaluate(model, 0.35), 0.58455114822546972, 2e-9*(1.0+std::abs(0.58455114822546972)));
    EXPECT_NEAR(isotherm::evaluate(model, 0.61), 1.0530858869227449, 2e-9*(1.0+std::abs(1.0530858869227449)));
    EXPECT_NEAR(isotherm::evaluate(model, 0.8), 1.4159292035398232, 2e-9*(1.0+std::abs(1.4159292035398232)));
    EXPECT_NEAR(isotherm::evaluate(model, 0.0), 0, 1e-13);
}
TEST(AntiLangmuirModel, IndependentFiniteDifferences) {
    const AntiLangmuir model{1.6, 0.12};
    for (Real x : {0.25, 0.5, 0.75}) {
        const Real h = x * 1e-5;
        const Real fd=(isotherm::evaluate(model,x+h)-isotherm::evaluate(model,x-h))/(2*h);
        const Real analytical=isotherm::derivative(model,x);
        EXPECT_NEAR(analytical,fd,3e-6*(1.0+std::abs(fd)));
    }
}
TEST(AntiLangmuirModel, RejectsNonPhysicalInputs) {
    const AntiLangmuir model{1.6, 0.12};
    EXPECT_THROW(isotherm::evaluate(model, -0.1), isotherm::isotherm_error);
    EXPECT_THROW(isotherm::evaluate(model, std::numeric_limits<Real>::infinity()), isotherm::isotherm_error);
    EXPECT_THROW(isotherm::evaluate(model, std::numeric_limits<Real>::quiet_NaN()), isotherm::isotherm_error);
    EXPECT_NEAR(isotherm::derivative(model, 0.0), Real{1.6}, 1e-14);
    EXPECT_THROW((AntiLangmuir{0.0, 0.12}), isotherm::isotherm_error);
}
TEST(AntiLangmuirModel, SpanBatchMatchesScalar) {
    const AntiLangmuir model{1.6, 0.12};
    std::array<Real,3> input{0.21,0.41,0.71}, output{-1.,-1.,-1.};
    isotherm::evaluate_into(model,std::span<const Real>(input),std::span<Real>(output));
    for(unsigned i=0;i<3;++i) EXPECT_NEAR(output[i],isotherm::evaluate(model,input[i]),1e-13);
}
} // namespace
