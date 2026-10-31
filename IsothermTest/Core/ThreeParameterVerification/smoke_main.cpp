//==============================================================================
// Name        : smoke_main.cpp
// Authors     : Joao Flavio Vasconcellos
// Version     : 2.0.0
// Description : GoogleTest-independent reference verification of 22 three-parameter models.
//
// Copyright   : Copyright (C) 2026 Joao Flavio Vasconcellos
// Licence     : MIT License (see LICENSE)
// SPDX-License-Identifier: MIT
//==============================================================================

//==============================================================================
// C++ standard library headers
//==============================================================================
#include <cmath>
#include <exception>
#include <iostream>
//==============================================================================
// Isotherm++ headers
//==============================================================================
#include "isotherm/isotherm.hpp"

int main() {
    int failures = 0;
    const auto check = [&](const char* name, double actual, double reference) {
        if (!std::isfinite(actual) || std::abs(actual - reference) > 1e-10 * (1.0 + std::abs(reference))) {
            std::cerr << "Reference mismatch: " << name << " (" << actual << " versus " << reference << ")\n";
            ++failures;
        }
    };
    check("BrouersSotolongo", isotherm::evaluate(isotherm::models::BrouersSotolongo{5.0, 0.65, 1.3}, 0.8), 1.9256031995330558);
    check("BrunauerEmmettTeller", isotherm::evaluate(isotherm::models::BrunauerEmmettTeller{5.0, 2.0, 8.0}, 0.8), 1.0101010101010099);
    check("FowlerGuggenheim", isotherm::evaluate(isotherm::models::FowlerGuggenheim{5.0, 0.8, 900.0}, 0.8, 298.15), 1.7982748715071861);
    check("FritzSchlunder", isotherm::evaluate(isotherm::models::FritzSchlunder{0.9, 0.65, 1.15}, 0.8), 0.27589469860476251);
    check("Hill", isotherm::evaluate(isotherm::models::Hill{5.0, 1.2, 1.4}, 0.8), 1.8939084666186496);
    check("HillDeBoer", isotherm::evaluate(isotherm::models::HillDeBoer{5.0, 0.8, 900.0}, 0.8, 298.15), 1.5633426491065827);
    check("HollKrich", isotherm::evaluate(isotherm::models::HollKrich{5.0, 0.65, 1.3}, 0.8), 1.6360076731367519);
    check("Jossens", isotherm::evaluate(isotherm::models::Jossens{5.0, 0.65, 1.15}, 0.8), 2.6615513966935613);
    check("Khan", isotherm::evaluate(isotherm::models::Khan{5.0, 0.65, 1.15}, 0.8), 1.6063982481016781);
    check("Kiselev", isotherm::evaluate(isotherm::models::Kiselev{5.0, 0.8, 0.6}, 0.8), 2.240755142422779);
    check("KobleCorrigan", isotherm::evaluate(isotherm::models::KobleCorrigan{5.0, 0.65, 1.3}, 0.8), 2.5169348817488491);
    check("LangmuirFreundlich", isotherm::evaluate(isotherm::models::LangmuirFreundlich{5.0, 0.65, 1.3}, 0.8), 1.6360076731367519);
    check("MacMillanTeller", isotherm::evaluate(isotherm::models::MacMillanTeller{5.0, 0.7, 8.0}, 0.8), 3.3619980311644495);
    check("RadkePrausnitsI", isotherm::evaluate(isotherm::models::RadkePrausnitsI{5.0, 0.65, 1.15}, 0.8), 1.6063982481016781);
    check("RadkePrausnitsII", isotherm::evaluate(isotherm::models::RadkePrausnitsII{5.0, 0.65, 1.15}, 0.8), 1.7300084078508149);
    check("RadkePrausnitsIII", isotherm::evaluate(isotherm::models::RadkePrausnitsIII{5.0, 0.65, 1.3}, 0.8), 1.5123008808764822);
    check("RedlichPeterson", isotherm::evaluate(isotherm::models::RedlichPeterson{5.0, 0.65, 1.15}, 0.8), 2.6615513966935613);
    check("Sips", isotherm::evaluate(isotherm::models::Sips{5.0, 0.65, 1.3}, 0.8), 1.8841556803417239);
    check("Toth", isotherm::evaluate(isotherm::models::Toth{5.0, 0.65, 1.3}, 0.8), 2.1171470751873813);
    check("Unilan", isotherm::evaluate(isotherm::models::Unilan{5.0, 0.65, 0.7}, 0.8), 1.7383849369463829);
    check("ValenzuelaMyers", isotherm::evaluate(isotherm::models::ValenzuelaMyers{5.0, 0.65, 0.7}, 0.8), 2.7486537522640004);
    check("ViethSladek", isotherm::evaluate(isotherm::models::ViethSladek{5.0, 0.65, 0.2}, 0.8), 1.8705263157894736);
    if (failures) return 1;
    std::cout << "Three-parameter reference values OK: 22 models\n";
    return 0;
}
