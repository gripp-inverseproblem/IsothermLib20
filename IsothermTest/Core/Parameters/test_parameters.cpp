//==============================================================================
// Name        : test_parameters.cpp
// Authors     : Joao Flavio Vasconcellos
// Version     : 2.0.0
// Description : Read and replace access to the parameters of every model.
//
// Copyright   : Copyright (C) 2026 Joao Flavio Vasconcellos
// Licence     : MIT License (see LICENSE.txt)
// SPDX-License-Identifier: MIT
//==============================================================================

//==============================================================================
// C++ standard library headers
//==============================================================================
#include <cstddef>
#include <limits>
#include <type_traits>
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

constexpr Real nan = std::numeric_limits<Real>::quiet_NaN();

// Contract shared by every model: read access, round trip, single-parameter
// replacement, validation of replacements and copy assignment.
template<class Model>
void check_parameter_api(const Model& model) {
    static_assert(isotherm::parameterised_model<Model>);
    static_assert(std::is_copy_assignable_v<Model>);
    constexpr std::size_t count = isotherm::model_traits<Model>::parameter_count;

    const auto values = isotherm::parameters(model);
    ASSERT_EQ(values.size(), count);
    for (std::size_t i = 0; i < count; ++i) {
        EXPECT_EQ(isotherm::parameter(model, i), values[i]);
    }
    EXPECT_THROW(static_cast<void>(isotherm::parameter(model, count)),
                 isotherm::isotherm_error);

    const Model rebuilt = isotherm::with_parameters(model, values);
    EXPECT_EQ(isotherm::parameters(rebuilt), values);

    for (std::size_t i = 0; i < count; ++i) {
        const Model same = isotherm::with_parameter(model, i, values[i]);
        EXPECT_EQ(isotherm::parameters(same), values);
        EXPECT_THROW(static_cast<void>(isotherm::with_parameter(model, i, nan)),
                     isotherm::isotherm_error);
    }
    EXPECT_THROW(static_cast<void>(isotherm::with_parameter(model, count, 1.0)),
                 isotherm::isotherm_error);

    Model assigned = rebuilt;
    assigned = isotherm::with_parameters(model, values);
    EXPECT_EQ(isotherm::parameters(assigned), values);
    // The source object is never modified.
    EXPECT_EQ(isotherm::parameters(model), values);
}
} // namespace

#define ISOTHERM_PARAMETER_TEST(Model, ...)                         \
    TEST(ParameterAccess, Model) { check_parameter_api(Model{__VA_ARGS__}); }

ISOTHERM_PARAMETER_TEST(Henry, 2.5)
ISOTHERM_PARAMETER_TEST(AntiLangmuir, 1.6, 0.12)
ISOTHERM_PARAMETER_TEST(DubininRadushkevich, 2.7, 1.0e-6)
ISOTHERM_PARAMETER_TEST(Elovich, 2.4, 0.8)
ISOTHERM_PARAMETER_TEST(Freundlich, 1.4, 2.0)
ISOTHERM_PARAMETER_TEST(Halsey, 3.0, 2.3)
ISOTHERM_PARAMETER_TEST(HarkinJura, 1.7, 1.2)
ISOTHERM_PARAMETER_TEST(Jovanovic, 2.5, 0.35)
ISOTHERM_PARAMETER_TEST(Langmuir, 5.0, 0.4)
ISOTHERM_PARAMETER_TEST(Temkin, 4.0, 1800.0)
ISOTHERM_PARAMETER_TEST(Volmer, 4.5, 0.7)
ISOTHERM_PARAMETER_TEST(Aranovich, 3.5, 0.8, 0.5)
ISOTHERM_PARAMETER_TEST(BingelWalton, 4.0, 0.35, 0.8)
ISOTHERM_PARAMETER_TEST(BrouersSotolongo, 5.0, 0.65, 1.3)
ISOTHERM_PARAMETER_TEST(BrunauerEmmettTeller, 5.0, 2.0, 8.0)
ISOTHERM_PARAMETER_TEST(DubininAstakhov, 4.8, 0.3, 2.0)
ISOTHERM_PARAMETER_TEST(FloryHuggins, 4.2, 0.55, 1.4)
ISOTHERM_PARAMETER_TEST(FloryHugginsVST, 4.2, 1.3, 0.35)
ISOTHERM_PARAMETER_TEST(FowlerGuggenheim, 5.0, 0.8, 900.0)
ISOTHERM_PARAMETER_TEST(FrenkelHalseyHill, 2.4, 1.2, 2.5)
ISOTHERM_PARAMETER_TEST(FritzSchlunder, 0.9, 0.65, 1.15)
ISOTHERM_PARAMETER_TEST(Frumkin, 4.1, 0.7, 1.0)
ISOTHERM_PARAMETER_TEST(GAB, 3.4, 1.8, 0.72)
ISOTHERM_PARAMETER_TEST(Henderson, 0.015, 1.7, 0.65)
ISOTHERM_PARAMETER_TEST(Hill, 5.0, 1.2, 1.4)
ISOTHERM_PARAMETER_TEST(HillDeBoer, 5.0, 0.8, 900.0)
ISOTHERM_PARAMETER_TEST(HollKrich, 5.0, 0.65, 1.3)
ISOTHERM_PARAMETER_TEST(Jossens, 5.0, 0.65, 1.15)
ISOTHERM_PARAMETER_TEST(Khan, 5.0, 0.65, 1.15)
ISOTHERM_PARAMETER_TEST(Kiselev, 5.0, 0.8, 0.6)
ISOTHERM_PARAMETER_TEST(KobleCorrigan, 5.0, 0.65, 1.3)
ISOTHERM_PARAMETER_TEST(LangmuirFreundlich, 5.0, 0.65, 1.3)
ISOTHERM_PARAMETER_TEST(MacMillanTeller, 5.0, 0.7, 8.0)
ISOTHERM_PARAMETER_TEST(OBrienMyers, 4.5, 0.7, 0.5)
ISOTHERM_PARAMETER_TEST(Quadratic, 4.0, 0.8, 0.16)
ISOTHERM_PARAMETER_TEST(RadkePrausnitsI, 5.0, 0.65, 1.15)
ISOTHERM_PARAMETER_TEST(RadkePrausnitsII, 5.0, 0.65, 1.15)
ISOTHERM_PARAMETER_TEST(RadkePrausnitsIII, 5.0, 0.65, 1.3)
ISOTHERM_PARAMETER_TEST(RedlichPeterson, 5.0, 0.65, 1.15)
ISOTHERM_PARAMETER_TEST(Sips, 5.0, 0.65, 1.3)
ISOTHERM_PARAMETER_TEST(Toth, 5.0, 0.65, 1.3)
ISOTHERM_PARAMETER_TEST(Unilan, 5.0, 0.65, 0.7)
ISOTHERM_PARAMETER_TEST(ValenzuelaMyers, 5.0, 0.65, 0.7)
ISOTHERM_PARAMETER_TEST(ViethSladek, 5.0, 0.65, 0.2)
ISOTHERM_PARAMETER_TEST(Baudu, 5.0, 0.6, 0.4, 0.35)
ISOTHERM_PARAMETER_TEST(DualSiteLangmuir, 2.5, 0.85, 1.5, 0.15)
ISOTHERM_PARAMETER_TEST(FritzSchlunderIV, 5.0, 0.7, 0.9, 0.6)
ISOTHERM_PARAMETER_TEST(JensenSeaton, 1.4, 3.0, 0.18, 1.4)
ISOTHERM_PARAMETER_TEST(MarczewskiJaroniec, 5.0, 0.7, 0.8, 0.6)
ISOTHERM_PARAMETER_TEST(Oswin, 2.0, -0.002, 1.7, 0.001)
ISOTHERM_PARAMETER_TEST(Smith, 0.3, 0.002, 0.1, 0.4)
ISOTHERM_PARAMETER_TEST(Virial, 1.0, 0.25, 0.045, 0.01)
ISOTHERM_PARAMETER_TEST(WeberVanVliet, 2.0, 0.7, 0.6, 0.8)
ISOTHERM_PARAMETER_TEST(WilsonVST, 4.0, 1.2, 0.9, 1.1)
ISOTHERM_PARAMETER_TEST(FritzSchlunderV, 6.05758, 0.217337, 0.0885359, 0.0169304, 0.0746286)
ISOTHERM_PARAMETER_TEST(TripleSiteLangmuir, 1.5, 0.9, 2.1, 0.3, 0.7, 0.08)

TEST(ParameterAccess, ReplacementChangesTheModel) {
    Langmuir model{5.0, 0.4};
    EXPECT_EQ(isotherm::parameter(model, 0), 5.0);
    EXPECT_EQ(isotherm::parameter(model, 1), 0.4);

    model = Langmuir{12.0, 0.8};
    EXPECT_EQ(isotherm::parameters(model), (isotherm::parameter_array<Langmuir>{12.0, 0.8}));

    model = isotherm::with_parameter(model, 1, 0.9);
    const Langmuir expected{12.0, 0.9};
    EXPECT_EQ(isotherm::evaluate(model, 1.5), isotherm::evaluate(expected, 1.5));
    EXPECT_EQ(isotherm::derivative(model, 1.5), isotherm::derivative(expected, 1.5));
}

TEST(ParameterAccess, InvalidReplacementLeavesTheModelUnchanged) {
    Langmuir model{5.0, 0.4};
    EXPECT_THROW(model = isotherm::with_parameter(model, 0, -1.0),
                 isotherm::isotherm_error);
    EXPECT_EQ(isotherm::parameters(model), (isotherm::parameter_array<Langmuir>{5.0, 0.4}));
}

TEST(ParameterAccess, ReplacementKeepsTheEnergyUnit) {
    using isotherm::gas_constant_unit;
    const Temkin kj{4.0, 1.8, gas_constant_unit::kilojoule_per_mole_kelvin};
    const Temkin replaced = isotherm::with_parameter(kj, 0, 4.0);
    EXPECT_EQ(isotherm::evaluate(replaced, 0.7, 300.0),
              isotherm::evaluate(kj, 0.7, 300.0));

    const DubininRadushkevich dr{2.7, 1.0e-6, gas_constant_unit::kilojoule_per_mole_kelvin};
    EXPECT_EQ(isotherm::evaluate(isotherm::with_parameters(dr, isotherm::parameters(dr)), 0.5, 300.0),
              isotherm::evaluate(dr, 0.5, 300.0));
}
