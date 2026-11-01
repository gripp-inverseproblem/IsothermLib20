//==============================================================================
// Name        : test_equivalences.cpp
// Authors     : Joao Flavio Vasconcellos
// Version     : 2.0.0
// Description : Check algebraic equivalences and implicit occupancy residuals.
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
// Isotherm++ headers
//==============================================================================
#include "isotherm/isotherm.hpp"
//==============================================================================
// GoogleTest headers
//==============================================================================
#include <gtest/gtest.h>

namespace {
TEST(ThreeParameterEquivalences, DirectSameFormulaFamilies) {
    using namespace isotherm::models;
    const HollKrich holl{5, .7, 1.25};
    const LangmuirFreundlich lf{5, .7, 1.25};
    const Khan khan{5, .7, 1.25};
    const RadkePrausnitsI radke{5, .7, 1.25};
    for (double x : {.1, .3, 1., 3.}) {
        EXPECT_NEAR(isotherm::evaluate(holl,x), isotherm::evaluate(lf,x), 1e-13);
        EXPECT_NEAR(isotherm::evaluate(khan,x), isotherm::evaluate(radke,x), 1e-13);
    }
}
TEST(ThreeParameterEquivalences, ReparameterisedSameFamily) {
    using namespace isotherm::models;
    const Hill hill{5, 2.0, 1.25};
    const LangmuirFreundlich lf{5, .5, 1.25};
    const KobleCorrigan kc{4.0, .8, 1.25};
    const LangmuirFreundlich lf2{5.0, .8, 1.25};
    const Sips sips{5.0, .75, 1.25};
    const LangmuirFreundlich lf3{5.0, std::pow(.75, .8), .8};
    const FritzSchlunder fritz{.75, 1.2, 1.25};
    const RedlichPeterson rp{.9, .75, 1.25};
    for (double x : {.1, .3, 1., 3.}) {
        EXPECT_NEAR(isotherm::evaluate(hill,x), isotherm::evaluate(lf,x), 1e-13);
        EXPECT_NEAR(isotherm::evaluate(kc,x), isotherm::evaluate(lf2,x), 1e-13);
        EXPECT_NEAR(isotherm::evaluate(sips,x), isotherm::evaluate(lf3,x), 1e-13);
        EXPECT_NEAR(isotherm::evaluate(fritz,x), isotherm::evaluate(rp,x), 1e-13);
    }
}
TEST(ThreeParameterImplicit, EquationResiduals) {
    using namespace isotherm::models;
    const double temperature=298.15, gas=8.31446261815324;
    const FowlerGuggenheim fg{5.,.8,900.};
    const HillDeBoer hd{5.,.8,900.};
    const Kiselev kis{5.,.8,.6};
    for (double c : {.2, .8, 2.}) {
        const double theta_f=isotherm::evaluate(fg,c,temperature)/5.;
        const double theta_h=isotherm::evaluate(hd,c,temperature)/5.;
        const double theta_k=isotherm::evaluate(kis,c)/5.;
        EXPECT_NEAR(theta_f/(1.-theta_f)*std::exp(900.*theta_f/(gas*temperature)), .8*c, 1e-12);
        EXPECT_NEAR(theta_h/(1.-theta_h)*std::exp(theta_h/(1.-theta_h)-900.*theta_h/(gas*temperature)), .8*c, 1e-12);
        EXPECT_NEAR(theta_k/((1.+.6*theta_k)*(1.-theta_k)), .8*c, 1e-12);
    }
    EXPECT_THROW(static_cast<void>(isotherm::evaluate(HillDeBoer{5.,.8,15000.},.5,298.15)), isotherm::isotherm_error);
}
TEST(ThreeParameterDomain, SaturationBoundaryIsExcluded) {
    using namespace isotherm::models;
    EXPECT_THROW(static_cast<void>(isotherm::evaluate(BrunauerEmmettTeller{5,2,8},8)), isotherm::isotherm_error);
    EXPECT_THROW(static_cast<void>(isotherm::evaluate(MacMillanTeller{5,.7,8},8)), isotherm::isotherm_error);
}
} // namespace
