//==============================================================================
// Name        : gas_constant.hpp
// Authors     : Joao Flavio Vasconcellos
// Version     : 2.0.0
// Description : Molar gas constant expressed in selected consistent energy units.
//
// Copyright   : Copyright (C) 2026 Joao Flavio Vasconcellos
// Licence     : MIT License (see LICENSE.txt)
// SPDX-License-Identifier: MIT
//==============================================================================
#pragma once

#include "isotherm/core/error.hpp"
#include "isotherm/core/type.hpp"

namespace isotherm {

/** @brief Energy unit of R*T; temperature is always supplied in kelvin. */
enum class gas_constant_unit {
    joule_per_mole_kelvin,             ///< R*T in J/mol (default).
    kilojoule_per_mole_kelvin,         ///< R*T in kJ/mol.
    litre_atmosphere_per_mole_kelvin   ///< R*T in L atm/mol.
};

/** @brief Select numerical representation of the same physical constant.
 *  Parameter energy units must match the units of R*T.
 *  The kelvin unit of temperature does not change with this selection.
 *  Reference: 2022 CODATA/NIST R = 8.314462618... J mol^-1 K^-1.
 */
[[nodiscard]] constexpr Real molar_gas_constant(
    gas_constant_unit unit = gas_constant_unit::joule_per_mole_kelvin) {
    constexpr Real R_SI = 8.31446261815324;
    switch (unit) {
        case gas_constant_unit::joule_per_mole_kelvin:
            return R_SI;
        case gas_constant_unit::kilojoule_per_mole_kelvin:
            return R_SI / Real{1000.0};
        case gas_constant_unit::litre_atmosphere_per_mole_kelvin:
            // 1 L atm = 101.325 J exactly (standard atmosphere).
            return R_SI / Real{101.325};
    }
    throw isotherm_error(error_code::invalid_input,
                         "Unknown molar gas constant unit");
}

} // namespace isotherm
