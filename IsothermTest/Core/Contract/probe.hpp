//==============================================================================
// Name        : probe.hpp
// Authors     : Joao Flavio Vasconcellos
// Version     : 2.0.0
// Description : Independent model types used to test static contracts.
//
// Copyright   : Copyright (C) 2026 Joao Flavio Vasconcellos
// Licence     : MIT License (see LICENSE.txt)
// SPDX-License-Identifier: MIT
//==============================================================================

#pragma once

//==============================================================================
// C++ standard library headers
//==============================================================================

#include <array>
#include <limits>
#include <string_view>


//==============================================================================
// Isotherm++ headers
//==============================================================================

#include "isotherm/isotherm.hpp"

//==============================================================================
// Lightweight contract-probe models, without inheritance or virtual methods
//==============================================================================

namespace contract_probe {
/**
 * @brief Minimal one-parameter adsorption-style model used in contract tests.
 *
 * The response is a coefficient multiplied by a non-negative input.
 */
struct linear_model final {
    explicit linear_model(double coefficient) : coefficient_(coefficient) {
        isotherm::require_nonnegative(coefficient, "linear coefficient");
    }
    /**
     * @brief Evaluates the linear probe at a non-negative independent variable.
     * @param x Input value.
     * @return Product of the coefficient and the input value.
     * @throws isotherm::isotherm_error If the input violates the model domain.
     */
    [[nodiscard]] double evaluate(double x) const {
        isotherm::require_nonnegative(x, "independent variable");
        return coefficient_ * x;
    }
private:
    double coefficient_;
};
/** @brief Minimal temperature-dependent model used to test thermal dispatch. */
struct thermal_model final {
    [[nodiscard]] double evaluate(double x, double temperature) const {
        return x / temperature;
    }
};
/** @brief Deliberately incomplete model used to reject an invalid C++20 concept. */
struct invalid_model final {};
/** @brief Probe returning infinity to test non-finite result handling. */
struct divergent_model final {
    [[nodiscard]] double evaluate(double) const {
        return std::numeric_limits<double>::infinity();
    }
};
}
//==============================================================================
// Compile-time model traits for the independent probe types
//==============================================================================

namespace isotherm {
template<> struct model_traits<contract_probe::linear_model> {
    static constexpr std::size_t parameter_count = 1;
    static constexpr std::array<std::string_view, 1> parameter_names{"coefficient"};
    static constexpr std::string_view identifier = "contract-probe-linear";
    static constexpr independent_variable variable = independent_variable::concentration;
    static constexpr equation_type equation = equation_type::explicit_equation;
    static constexpr bool temperature_dependent = false;
};
template<> struct model_traits<contract_probe::thermal_model> {
    static constexpr std::size_t parameter_count = 0;
    static constexpr std::array<std::string_view, 0> parameter_names{};
    static constexpr std::string_view identifier = "contract-probe-thermal";
    static constexpr independent_variable variable = independent_variable::pressure;
    static constexpr equation_type equation = equation_type::explicit_equation;
    static constexpr bool temperature_dependent = true;
};
template<> struct model_traits<contract_probe::divergent_model> {
    static constexpr std::size_t parameter_count = 0;
    static constexpr std::array<std::string_view, 0> parameter_names{};
    static constexpr std::string_view identifier = "contract-probe-divergent";
    static constexpr independent_variable variable = independent_variable::concentration;
    static constexpr equation_type equation = equation_type::explicit_equation;
    static constexpr bool temperature_dependent = false;
};
}
//==============================================================================
// Compile-time conformance checks
//==============================================================================

static_assert(isotherm::scalar_isotherm<contract_probe::linear_model>);
static_assert(isotherm::thermal_isotherm<contract_probe::thermal_model>);
static_assert(!isotherm::scalar_isotherm<contract_probe::thermal_model>);
static_assert(!isotherm::isotherm_model<contract_probe::invalid_model>);
