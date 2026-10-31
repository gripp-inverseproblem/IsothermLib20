//==============================================================================
// Name        : test_frenkel_halsey_hill.cpp
// Authors     : Joao Flavio Vasconcellos
// Version     : 2.0.0
// Description : Concentration FHH: references, derivative, domain and traits.
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
#include <string_view>
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
using isotherm::models::FrenkelHalseyHill;

TEST(FrenkelHalseyHillModel, ConceptsTraitsAndIndependentReferences) {
    static_assert(isotherm::scalar_isotherm<FrenkelHalseyHill>);
    static_assert(isotherm::differentiable_scalar_isotherm<FrenkelHalseyHill>);
    using Traits = isotherm::model_traits<FrenkelHalseyHill>;
    static_assert(Traits::parameter_count == 3);
    static_assert(Traits::variable ==
                  isotherm::independent_variable::concentration);
    static_assert(Traits::parameter_names[0] == "q_m");
    static_assert(Traits::parameter_names[1] == "C_s");
    static_assert(Traits::parameter_names[2] == "s");
    const FrenkelHalseyHill model{2.4, 1.2, 2.5};
    EXPECT_NEAR(isotherm::evaluate(model, 0.2),
                1.9006377828700347, 1e-13);
    EXPECT_NEAR(isotherm::evaluate(model, 0.5),
                2.5311331095035494, 1e-13);
    EXPECT_NEAR(isotherm::evaluate(model, 0.8),
                3.4437359692335974, 1e-13);
    EXPECT_NEAR(isotherm::derivative(model, 0.5),
                2.3129398015092003, 1e-12);
}

TEST(FrenkelHalseyHillModel, IndependentlyCalculatedDerivative) {
    const FrenkelHalseyHill model{2.4, 1.2, 2.5};
    for (Real x : {Real{0.2}, Real{0.5}, Real{0.8}}) {
        const Real h = Real{1e-5};
        const Real central = (isotherm::evaluate(model, x + h) -
                              isotherm::evaluate(model, x - h)) / (2 * h);
        EXPECT_NEAR(isotherm::derivative(model, x), central,
                    2e-7 * (1 + std::abs(central)));
        EXPECT_GT(isotherm::derivative(model, x), 0.0);
    }
}

TEST(FrenkelHalseyHillModel, FHHLogarithmicRelationAndLimits) {
    const Real q_m = Real{2.4};
    const Real C_s = Real{1.2};
    const Real s = Real{2.5};
    const FrenkelHalseyHill model{q_m, C_s, s};
    for (Real x : {Real{0.2}, Real{0.5}, Real{0.8}}) {
        const Real coverage = isotherm::evaluate(model, x) / q_m;
        EXPECT_NEAR(std::log(C_s / x),
                    std::pow(coverage, -s), 1e-12);
    }
    EXPECT_LT(isotherm::evaluate(model, Real{1e-5}),
              isotherm::evaluate(model, Real{0.2}));
    EXPECT_GT(isotherm::evaluate(model, Real{1.199}),
              isotherm::evaluate(model, Real{0.8}));
}

TEST(FrenkelHalseyHillModel, ConcentrationScaleIsNotRedundant) {
    // C_s changes the shape, and cannot be absorbed into q_m uniformly.
    const FrenkelHalseyHill baseline{2.4, 1.2, 2.5};
    const FrenkelHalseyHill altered{2.4, 1.5, 2.5};
    const Real ratio_low = isotherm::evaluate(altered, 0.2) /
                           isotherm::evaluate(baseline, 0.2);
    const Real ratio_high = isotherm::evaluate(altered, 0.8) /
                            isotherm::evaluate(baseline, 0.8);
    EXPECT_GT(std::abs(ratio_low - ratio_high), 1e-2);
}

TEST(FrenkelHalseyHillModel, DomainAndNumericalStability) {
    const FrenkelHalseyHill model{2.4, 1.2, 2.5};
    for (Real x : {Real{-0.1}, Real{0}, Real{1.2}, Real{1.3},
                   std::numeric_limits<Real>::infinity(),
                   std::numeric_limits<Real>::quiet_NaN()}) {
        EXPECT_THROW((void)isotherm::evaluate(model, x),
                     isotherm::isotherm_error);
        EXPECT_THROW((void)isotherm::derivative(model, x),
                     isotherm::isotherm_error);
    }
    EXPECT_THROW((FrenkelHalseyHill{0.0, 1.2, 2.5}),
                 isotherm::isotherm_error);
    EXPECT_THROW((FrenkelHalseyHill{2.4, 0.0, 2.5}),
                 isotherm::isotherm_error);
    EXPECT_THROW((FrenkelHalseyHill{2.4, 1.2, -2.5}),
                 isotherm::isotherm_error);
    EXPECT_THROW((FrenkelHalseyHill{2.4, 1.2,
        std::numeric_limits<Real>::quiet_NaN()}),
        isotherm::isotherm_error);
    EXPECT_THROW((FrenkelHalseyHill{2.4,
        std::numeric_limits<Real>::infinity(), 2.5}),
        isotherm::isotherm_error);
    const Real near_saturation = std::nextafter(Real{1.2}, Real{0});
    EXPECT_TRUE(std::isfinite(isotherm::evaluate(model, near_saturation)));
    EXPECT_TRUE(std::isfinite(isotherm::derivative(model, near_saturation)));
    const FrenkelHalseyHill extreme_scale{2.4, 1e300, 2.5};
    EXPECT_TRUE(std::isfinite(isotherm::evaluate(extreme_scale, 1e-300)));
}

TEST(FrenkelHalseyHillModel, ScaledSensitivitiesAgainstFiniteDifferences) {
    const Real q_m = Real{2.4};
    const Real C_s = Real{1.2};
    const Real s = Real{2.5};
    const Real C_e = Real{0.5};
    const FrenkelHalseyHill model{q_m, C_s, s};
    const Real q = isotherm::evaluate(model, C_e);
    const Real L = std::log(C_s / C_e);
    const Real exact[3] = {
        q,
        -q / (s * L),
        q * std::log(L) / s,
    };
    std::array<Real, 3> parameters{q_m, C_s, s};
    for (std::size_t j = 0; j < parameters.size(); ++j) {
        auto lower = parameters;
        auto upper = parameters;
        const Real step = parameters[j] * Real{1e-5};
        lower[j] -= step;
        upper[j] += step;
        const FrenkelHalseyHill lo{lower[0], lower[1], lower[2]};
        const FrenkelHalseyHill hi{upper[0], upper[1], upper[2]};
        const Real estimate = parameters[j] *
            (isotherm::evaluate(hi, C_e) - isotherm::evaluate(lo, C_e)) /
            (Real{2} * step);
        EXPECT_NEAR(estimate, exact[j], 2e-9 * (Real{1} + std::abs(exact[j])));
    }
}

TEST(FrenkelHalseyHillModel, SpanBatchMatchesScalar) {
    const FrenkelHalseyHill model{2.4, 1.2, 2.5};
    std::array<Real, 3> input{Real{0.2}, Real{0.5}, Real{0.8}};
    std::array<Real, 3> output{Real{-1}, Real{-1}, Real{-1}};
    isotherm::evaluate_into(model, std::span<const Real>(input),
                            std::span<Real>(output));
    for (std::size_t i = 0; i < input.size(); ++i) {
        EXPECT_NEAR(output[i], isotherm::evaluate(model, input[i]), 1e-13);
    }
}
} // namespace
