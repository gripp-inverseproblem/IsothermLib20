//==============================================================================
// Name        : test_small_coverage.cpp
// Authors     : Joao Flavio Vasconcellos
// Version     : 2.0.0
// Description : Bisection-based implicit models at extreme concentrations.
//
// Copyright   : Copyright (C) 2026 Joao Flavio Vasconcellos
// Licence     : MIT License (see LICENSE.txt)
// SPDX-License-Identifier: MIT
//==============================================================================

//==============================================================================
// C++ standard library headers
//==============================================================================
#include <cmath>
//==============================================================================
// Third-party headers
//==============================================================================
#include <gtest/gtest.h>
//==============================================================================
// Isotherm++ headers
//==============================================================================
#include "isotherm/isotherm.hpp"

namespace {
using isotherm::Real;
using namespace isotherm::models;

// At very small concentration every model reduces to Henry's law, q = K_H C,
// and dq/dC = K_H. Concentrations of this order occur in the tails of
// diffusion fronts; the result must keep full relative accuracy.
template<class Model>
void check_henry_limit(const Model& model, Real henry) {
    for (const Real c : {1e-20, 1e-60, 1e-200, 1e-300}) {
        EXPECT_NEAR(isotherm::evaluate(model, c) / (henry * c), 1.0, 1e-12) << "C = " << c;
        EXPECT_NEAR(isotherm::derivative(model, c) / henry, 1.0, 1e-12) << "C = " << c;
    }
}
} // namespace

TEST(SmallCoverage, Virial) { check_henry_limit(Virial{1.0, 0.25, 0.045, 0.01}, 1.0); }
TEST(SmallCoverage, WilsonVST) { check_henry_limit(WilsonVST{4.0, 1.2, 0.9, 1.1}, 1.2); }
TEST(SmallCoverage, FloryHuggins) { check_henry_limit(FloryHuggins{4.2, 0.55, 1.4}, 4.2 * 0.55); }
TEST(SmallCoverage, FloryHugginsVST) { check_henry_limit(FloryHugginsVST{4.2, 1.3, 0.35}, 1.3); }
TEST(SmallCoverage, Frumkin) { check_henry_limit(Frumkin{4.1, 0.7, 1.0}, 4.1 * 0.7); }
TEST(SmallCoverage, Volmer) { check_henry_limit(Volmer{4.5, 0.7}, 4.5 * 0.7); }

// At very large concentration the implicit equation must still be satisfied.
TEST(LargeCoverage, VolmerResidual) {
    const Volmer model{4.5, 0.7};
    for (const Real c : {1e10, 1e50, 1e200}) {
        const Real theta = isotherm::evaluate(model, c) / 4.5;
        const Real w = theta / (1.0 - theta);
        const Real target = std::log(0.7) + std::log(c);
        EXPECT_NEAR(std::log(w) + w, target, 1e-12 * target) << "C = " << c;
    }
}

TEST(LargeCoverage, VirialResidual) {
    const Virial model{1.0, 0.25, 0.045, 0.01};
    for (const Real c : {1e10, 1e50, 1e200}) {
        const Real q = isotherm::evaluate(model, c);
        const Real target = std::log(c);
        EXPECT_NEAR(std::log(q) + 0.25 * q + 0.045 * q * q + 0.01 * q * q * q, target,
                    1e-12 * target) << "C = " << c;
    }
}
