//==============================================================================
// Name        : error.hpp
// Authors     : Joao Flavio Vasconcellos
// Version     : 2.0.0
// Description : Isotherm++ 2.0 project source and documentation.
//
// Copyright   : Copyright (C) 2026 Joao Flavio Vasconcellos
// Licence     : MIT License (see LICENSE)
// SPDX-License-Identifier: MIT
//==============================================================================

#pragma once

//==============================================================================
// C++ standard library headers
//==============================================================================

#include <string>
#include <utility>

namespace isotherm {

enum class error_code {
    invalid_input,
    invalid_parameters,
    outside_domain,
    non_finite_result,
    no_convergence
};

// Typed error value; intentionally does not inherit from any base class.
class isotherm_error final {
public:
    isotherm_error(error_code code, std::string message)
        : message_(std::move(message)), code_(code) {}

    [[nodiscard]] error_code code() const noexcept { return code_; }
    [[nodiscard]] const char* what() const noexcept { return message_.c_str(); }

private:
    std::string message_;
    error_code code_;
};

} // namespace isotherm
