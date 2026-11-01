//==============================================================================
// Name        : HarkinJura.hpp
// Authors     : Joao Flavio Vasconcellos
// Version     : 2.0.0
// Description : Two-parameter HarkinJura formulation with strict domain validation.
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
#include <cstddef>
#include <string_view>

//==============================================================================
// Isotherm++ headers
//==============================================================================

#include "isotherm/core/traits.hpp"
#include "isotherm/core/type.hpp"
#include "isotherm/core/validation.hpp"

namespace isotherm::models {

/** @brief Harkins–Jura formulation uses base-10 logarithms, unlike the natural-logarithm convention of most other models. The upper concentration boundary follows from B - log10(c_e) > 0. */
class HarkinJura final {
public:
    explicit HarkinJura(Real coefficient_a, Real coefficient_b) : coefficient_a_(coefficient_a), coefficient_b_(coefficient_b) {
        require_positive(coefficient_a_, "HarkinJura coefficient a");
        require_positive(coefficient_b_, "HarkinJura coefficient b");
    }

    /** @brief Evaluate the model on its documented mathematical domain. */
    [[nodiscard]] Real evaluate(Real concentration) const {
        require_nonnegative(concentration, "dimensionless equilibrium concentration");
        if (concentration == 0.0) return 0.0;
        const Real denominator = coefficient_b_ - std::log10(concentration);
        if (!(denominator > 0.0)) throw isotherm_error(error_code::outside_domain, "Harkins-Jura denominator must remain positive");
        return std::sqrt(coefficient_a_ / denominator);
    }

    /** @brief Analytical derivative with respect to concentration. */
    [[nodiscard]] Real derivative(Real concentration) const {
        require_positive(concentration, "dimensionless equilibrium concentration");
        const Real denominator = coefficient_b_ - std::log10(concentration);
        if (!(denominator > 0.0)) throw isotherm_error(error_code::outside_domain, "Harkins-Jura denominator must remain positive");
        return evaluate(concentration) / (2.0 * std::log(10.0) * concentration * denominator);
    }

    /** @brief Parameters in the order of model_traits::parameter_names. */
    [[nodiscard]] std::array<Real, 2> parameters() const noexcept {
        return {coefficient_a_, coefficient_b_};
    }

    /** @brief Copy with new parameters; the constructor validates them. */
    [[nodiscard]] HarkinJura with_parameters(const std::array<Real, 2>& p) const {
        return HarkinJura{p[0], p[1]};
    }

private:
    Real coefficient_a_;
    Real coefficient_b_;
};

} // namespace isotherm::models

namespace isotherm {

/** @brief Compile-time traits for the HarkinJura formulation. */
template<> struct model_traits<models::HarkinJura> {
    static constexpr std::size_t parameter_count = 2;
    static constexpr std::array<std::string_view, 2> parameter_names{"A", "B"};
    static constexpr std::string_view identifier = "harkin_jura";
    static constexpr independent_variable variable = independent_variable::concentration;
    static constexpr equation_type equation = equation_type::explicit_equation;
    static constexpr bool temperature_dependent = false;
};

} // namespace isotherm
