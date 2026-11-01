//==============================================================================
// Name        : main.cpp
// Authors     : Joao Flavio Vasconcellos
// Version     : 2.0.0
// Description : External-model example demonstrating core evaluation.
//
// Copyright   : Copyright (C) 2026 Joao Flavio Vasconcellos
// Licence     : MIT License (see LICENSE.txt)
// SPDX-License-Identifier: MIT
//==============================================================================

// Contract demonstration only; this is not a validated catalogue model.
// The CSV on stdout can be plotted by the adjacent plot.py script.

//==============================================================================
// C++ standard library headers
//==============================================================================

#include <array>
#include <iostream>
#include <string_view>


//==============================================================================
// Isotherm++ headers
//==============================================================================

#include "isotherm/isotherm.hpp"

struct demo_model final {
    explicit demo_model(double coefficient) : coefficient_(coefficient) {
        isotherm::require_nonnegative(coefficient, "coefficient");
    }
    [[nodiscard]] double evaluate(double concentration) const {
        isotherm::require_nonnegative(concentration, "concentration");
        return coefficient_ * concentration;
    }
private:
    double coefficient_;
};
namespace isotherm {
template<> struct model_traits<demo_model> {
    static constexpr std::size_t parameter_count = 1;
    static constexpr std::array<std::string_view, 1> parameter_names{"coefficient"};
    static constexpr std::string_view identifier = "external-model-contract-demo";
    static constexpr independent_variable variable = independent_variable::concentration;
    static constexpr equation_type equation = equation_type::explicit_equation;
    static constexpr bool temperature_dependent = false;
};
}
static_assert(isotherm::scalar_isotherm<demo_model>);
int main() {
    const demo_model model{2.0};
    std::cout << "concentration,uptake\n";
    for (int i = 0; i <= 100; ++i) {
        const double x = static_cast<double>(i) / 10.0;
        std::cout << x << ',' << isotherm::evaluate(model, x) << '\n';
    }
}
