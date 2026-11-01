//==============================================================================
// Name        : test_contract.cpp
// Authors     : Joao Flavio Vasconcellos
// Version     : 2.0.0
// Description : GoogleTest validation of the C++20 model contracts.
//
// Copyright   : Copyright (C) 2026 Joao Flavio Vasconcellos
// Licence     : MIT License (see LICENSE.txt)
// SPDX-License-Identifier: MIT
//==============================================================================

//==============================================================================
// C++ standard library headers
//==============================================================================

#include <array>
#include <limits>
#include <span>


//==============================================================================
// Local headers
//==============================================================================

#include "probe.hpp"


//==============================================================================
// GoogleTest headers
//==============================================================================

#include <gtest/gtest.h>

//==============================================================================
// Test suite: model contracts and C++20 concepts
//==============================================================================

using namespace isotherm;

/**
 * @brief Verifies model concepts and compile-time traits.
 *
 * Checks that valid scalar and temperature-dependent models satisfy their
 * respective concepts, while a model without the required interface does not.
 */
TEST(ModelContract, StaticScalarAndThermalCapabilities) {
    EXPECT_TRUE(scalar_isotherm<contract_probe::linear_model>);
    EXPECT_TRUE(thermal_isotherm<contract_probe::thermal_model>);
    EXPECT_FALSE(isotherm_model<contract_probe::invalid_model>);
    EXPECT_EQ(model_traits<contract_probe::linear_model>::parameter_count, 1U);
}

/**
 * @brief Verifies the numerical dispatch for scalar and thermal models.
 */
TEST(ModelContract, ScalarAndThermalEvaluation) {
    EXPECT_DOUBLE_EQ(evaluate(contract_probe::linear_model{2.0}, 3.0), 6.0);
    EXPECT_NEAR(evaluate(contract_probe::thermal_model{}, 10.0, 300.0), 1.0 / 30.0, 1e-14);
}

/**
 * @brief Verifies scalar-model batch evaluation using std::span.
 */
TEST(ModelContract, SpanEvaluation) {
    const std::array<double, 3> xs{0., 1., 2.};
    std::array<double, 3> output{};
    evaluate_into(contract_probe::linear_model{2.}, std::span<const double>{xs}, std::span<double>{output});
    EXPECT_EQ(output, (std::array<double, 3>{0., 2., 4.}));
}

/**
 * @brief Checks that a rejected batch size leaves the output unchanged.
 */
TEST(ModelContract, InvalidBatchSizeDoesNotModifyOutput) {
    const std::array<double, 2> xs{1., 2.};
    std::array<double, 1> output{42.};
    EXPECT_THROW(evaluate_into(contract_probe::linear_model{2.}, std::span<const double>{xs}, std::span<double>{output}), isotherm_error);
    EXPECT_DOUBLE_EQ(output[0], 42.);
}

//==============================================================================
// Test suite: invalid domains and non-finite numerical values
//==============================================================================

/**
 * @brief Verifies that invalid model parameters and negative inputs are rejected.
 */
TEST(ModelContract, ModelSpecificDomainIsEnforced) {
    EXPECT_THROW(contract_probe::linear_model{-1.}, isotherm_error);
    EXPECT_THROW(static_cast<void>(evaluate(contract_probe::linear_model{1.}, -2.)), isotherm_error);
}

/**
 * @brief Verifies the rejection of non-finite inputs and invalid temperatures.
 */
TEST(ModelContract, NonFiniteInputsAreRejected) {
    EXPECT_THROW(static_cast<void>(evaluate(contract_probe::linear_model{1.}, std::numeric_limits<double>::quiet_NaN())), isotherm_error);
    EXPECT_THROW(static_cast<void>(evaluate(contract_probe::thermal_model{}, 1., 0.)), isotherm_error);
}

/**
 * @brief Verifies the error category reported for a non-finite model result.
 */
TEST(ModelContract, NonFiniteResultsAreRejected) {
    try {
        (void)evaluate(contract_probe::divergent_model{}, 2.);
        FAIL();
    } catch (const isotherm_error& e) {
        EXPECT_EQ(e.code(), error_code::non_finite_result);
    }
}
