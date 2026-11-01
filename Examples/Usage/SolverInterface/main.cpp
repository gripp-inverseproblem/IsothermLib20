//==============================================================================
// Name        : main.cpp
// Authors     : Joao Flavio Vasconcellos
// Version     : 2.0.0
// Description : Basic use, solver coefficient and parameter replacement.
//
// Copyright   : Copyright (C) 2026 Joao Flavio Vasconcellos
// Licence     : MIT License (see LICENSE.txt)
// SPDX-License-Identifier: MIT
//==============================================================================

// Code shown in the SoftwareX paper (Section 2.3), as a complete programme.

//==============================================================================
// C++ standard library headers
//==============================================================================

#include <iomanip>
#include <iostream>

//==============================================================================
// Isotherm++ headers
//==============================================================================

#include <isotherm/isotherm.hpp>

using namespace isotherm;

// Coefficient eps + rho_b dq/dC of the local-equilibrium mass balance,
// written once for any differentiable isotherm.
template<differentiable_scalar_isotherm Model>
Real capacity(const Model& m, Real C, Real eps, Real rho_b) {
    return eps + rho_b * derivative(m, C);
}

int main() {
    // Basic use: a model is an ordinary object.
    models::Langmuir langmuir{60.8, 0.035};    // q_max, K
    Real q  = langmuir.evaluate(10.0);         // adsorbed amount
    Real dq = langmuir.derivative(10.0);       // dq/dC

    // The same solver function accepts any isotherm.
    models::Toth toth{78.0, 0.15, 0.62};
    Real a = capacity(langmuir, 1e-60, 0.4, 500.0);
    Real b = capacity(toth,     1e-60, 0.4, 500.0);

    // Replace one parameter, as an estimation loop would do.
    toth = with_parameter(toth, 2, 0.70);      // checked again

    std::cout << std::setprecision(10)
              << "Langmuir q(10)            = " << q << '\n'
              << "Langmuir dq/dC(10)        = " << dq << '\n'
              << "capacity, Langmuir, 1e-60 = " << a << '\n'
              << "capacity, Toth, 1e-60     = " << b << '\n'
              << "Toth exponent after update = " << parameter(toth, 2) << '\n';
    return 0;
}
