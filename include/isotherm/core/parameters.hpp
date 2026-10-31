//==============================================================================
// Name        : parameters.hpp
// Authors     : Joao Flavio Vasconcellos
// Version     : 2.0.0
// Description : Generic read and replace access to model parameters.
//
// Copyright   : Copyright (C) 2026 Joao Flavio Vasconcellos
// Licence     : MIT License (see LICENSE)
// SPDX-License-Identifier: MIT
//==============================================================================

#pragma once

//==============================================================================
// C++ standard library headers
//==============================================================================

#include <array>
#include <concepts>
#include <cstddef>
#include <string>


//==============================================================================
// Isotherm++ headers
//==============================================================================

#include "isotherm/core/concepts.hpp"
#include "isotherm/core/error.hpp"
#include "isotherm/core/traits.hpp"
#include "isotherm/core/type.hpp"
#include "isotherm/core/validation.hpp"

namespace isotherm {

/** @brief Fixed-size parameter vector of a model, in trait order. */
template<class Model>
using parameter_array = std::array<Real, model_traits<Model>::parameter_count>;

/**
 * @brief A model whose parameters can be read and replaced.
 *
 * parameters() returns the values in the order of
 * model_traits<Model>::parameter_names. with_parameters() returns a new
 * object built through the model constructor, so every replacement passes
 * the same validation as the original construction. Models remain
 * immutable: parameters are never modified in place.
 */
template<class Model>
concept parameterised_model = detail::described_model<Model> &&
    std::copy_constructible<Model> &&
    requires(const Model& model, const parameter_array<Model>& values) {
        { model.parameters() } -> std::same_as<parameter_array<Model>>;
        { model.with_parameters(values) } -> std::same_as<Model>;
    };

/** @brief All parameters, in the order of model_traits::parameter_names. */
template<parameterised_model Model>
[[nodiscard]] parameter_array<Model> parameters(const Model& model) noexcept {
    return model.parameters();
}

/** @brief Parameter at a zero-based index. */
template<parameterised_model Model>
[[nodiscard]] Real parameter(const Model& model, std::size_t index) {
    constexpr std::size_t count = model_traits<Model>::parameter_count;
    if (index >= count) {
        throw isotherm_error(error_code::invalid_input,
            "Parameter index " + std::to_string(index) +
            " is out of range for a model with " + std::to_string(count) +
            " parameters");
    }
    return model.parameters()[index];
}

/** @brief New model with all parameters replaced; validated by the constructor. */
template<parameterised_model Model>
[[nodiscard]] Model with_parameters(const Model& model,
                                    const parameter_array<Model>& values) {
    for (const Real value : values) {
        require_finite(value, "model parameter");
    }
    return model.with_parameters(values);
}

/** @brief New model with one parameter replaced; validated by the constructor. */
template<parameterised_model Model>
[[nodiscard]] Model with_parameter(const Model& model, std::size_t index,
                                   Real value) {
    auto values = model.parameters();
    if (index >= values.size()) {
        throw isotherm_error(error_code::invalid_input,
            "Parameter index " + std::to_string(index) +
            " is out of range for a model with " +
            std::to_string(values.size()) + " parameters");
    }
    values[index] = value;
    return with_parameters(model, values);
}

} // namespace isotherm
