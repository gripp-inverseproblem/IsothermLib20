//==============================================================================
// Name        : concepts.hpp
// Authors     : Joao Flavio Vasconcellos
// Version     : 2.0.0
// Description : Isotherm++ 2.0 project source and documentation.
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
#include <concepts>
#include <cstddef>
#include <string_view>
#include <type_traits>
#include <utility>


//==============================================================================
// Isotherm++ headers
//==============================================================================

#include "isotherm/core/traits.hpp"
#include "isotherm/core/type.hpp"

namespace isotherm {
namespace detail {
template<class T>
concept described_model = requires {
    { model_traits<T>::parameter_count } -> std::convertible_to<std::size_t>;
    { model_traits<T>::identifier } -> std::convertible_to<std::string_view>;
    { model_traits<T>::variable } -> std::convertible_to<independent_variable>;
    { model_traits<T>::equation } -> std::convertible_to<equation_type>;
    { model_traits<T>::temperature_dependent } -> std::convertible_to<bool>;
    // The names must match the number of parameters at compile time.
    requires std::same_as<std::remove_cvref_t<decltype(model_traits<T>::parameter_names)>,
        std::array<std::string_view, model_traits<T>::parameter_count>>;
};
}
template<class T>
concept scalar_isotherm = detail::described_model<T> &&
    (!model_traits<T>::temperature_dependent) &&
    requires(const T& model, Real x) {
        { model.evaluate(x) } -> std::convertible_to<Real>;
    };
template<class T>
concept thermal_isotherm = detail::described_model<T> &&
    model_traits<T>::temperature_dependent &&
    requires(const T& model, Real x, Real temperature) {
        { model.evaluate(x, temperature) } -> std::convertible_to<Real>;
    };
/** @brief A scalar model exposing an analytical concentration derivative. */
template<class T>
concept differentiable_scalar_isotherm = scalar_isotherm<T> &&
    requires(const T& model, Real concentration) {
        { model.derivative(concentration) } -> std::convertible_to<Real>;
    };

/** @brief Thermal model exposing an analytical concentration derivative. */
template<class T>
concept differentiable_thermal_isotherm = thermal_isotherm<T> &&
    requires(const T& model, Real concentration, Real temperature) {
        { model.derivative(concentration, temperature) } -> std::convertible_to<Real>;
    };

template<class T>
concept isotherm_model = scalar_isotherm<T> || thermal_isotherm<T>;
} // namespace isotherm
