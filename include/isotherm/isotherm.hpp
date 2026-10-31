//==============================================================================
// Name        : isotherm.hpp
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
// Isotherm++ headers
//==============================================================================

#include "isotherm/core/concepts.hpp"
#include "isotherm/core/error.hpp"
#include "isotherm/core/evaluation.hpp"
#include "isotherm/core/gas_constant.hpp"
#include "isotherm/core/traits.hpp"
#include "isotherm/core/type.hpp"
#include "isotherm/core/validation.hpp"
#include "isotherm/models/OneParameter/Henry.hpp"
#include "isotherm/models/TwoParameters/AntiLangmuir.hpp"
#include "isotherm/models/TwoParameters/DubininRadushkevich.hpp"
#include "isotherm/models/TwoParameters/Elovich.hpp"
#include "isotherm/models/TwoParameters/Freundlich.hpp"
#include "isotherm/models/TwoParameters/Halsey.hpp"
#include "isotherm/models/TwoParameters/HarkinJura.hpp"
#include "isotherm/models/ThreeParameters/Henderson.hpp"
#include "isotherm/models/TwoParameters/Jovanovic.hpp"
#include "isotherm/models/TwoParameters/Langmuir.hpp"
#include "isotherm/models/FourParameters/Oswin.hpp"
#include "isotherm/models/FourParameters/Smith.hpp"
#include "isotherm/models/TwoParameters/Temkin.hpp"
#include "isotherm/models/TwoParameters/Volmer.hpp"
#include "isotherm/models/ThreeParameters/Aranovich.hpp"
#include "isotherm/models/ThreeParameters/BingelWalton.hpp"
#include "isotherm/models/ThreeParameters/BrouersSotolongo.hpp"
#include "isotherm/models/ThreeParameters/BrunauerEmmettTeller.hpp"
#include "isotherm/models/ThreeParameters/DubininAstakhov.hpp"
#include "isotherm/models/ThreeParameters/FloryHuggins.hpp"
#include "isotherm/models/ThreeParameters/FloryHugginsVST.hpp"
#include "isotherm/models/ThreeParameters/FowlerGuggenheim.hpp"
#include "isotherm/models/ThreeParameters/FritzSchlunder.hpp"
#include "isotherm/models/ThreeParameters/Frumkin.hpp"
#include "isotherm/models/ThreeParameters/GAB.hpp"
#include "isotherm/models/ThreeParameters/Hill.hpp"
#include "isotherm/models/ThreeParameters/HillDeBoer.hpp"
#include "isotherm/models/ThreeParameters/HollKrich.hpp"
#include "isotherm/models/ThreeParameters/Jossens.hpp"
#include "isotherm/models/ThreeParameters/Khan.hpp"
#include "isotherm/models/ThreeParameters/Kiselev.hpp"
#include "isotherm/models/ThreeParameters/KobleCorrigan.hpp"
#include "isotherm/models/ThreeParameters/LangmuirFreundlich.hpp"
#include "isotherm/models/ThreeParameters/MacMillanTeller.hpp"
#include "isotherm/models/ThreeParameters/OBrienMyers.hpp"
#include "isotherm/models/ThreeParameters/Quadratic.hpp"
#include "isotherm/models/ThreeParameters/RadkePrausnitsI.hpp"
#include "isotherm/models/ThreeParameters/RadkePrausnitsII.hpp"
#include "isotherm/models/ThreeParameters/RadkePrausnitsIII.hpp"
#include "isotherm/models/ThreeParameters/RedlichPeterson.hpp"
#include "isotherm/models/ThreeParameters/Sips.hpp"
#include "isotherm/models/ThreeParameters/Toth.hpp"
#include "isotherm/models/ThreeParameters/Unilan.hpp"
#include "isotherm/models/ThreeParameters/ValenzuelaMyers.hpp"
#include "isotherm/models/ThreeParameters/ViethSladek.hpp"
#include "isotherm/models/FourParameters/Baudu.hpp"
#include "isotherm/models/FourParameters/DualSiteLangmuir.hpp"
#include "isotherm/models/FourParameters/FritzSchlunderIV.hpp"
#include "isotherm/models/FourParameters/JensenSeaton.hpp"
#include "isotherm/models/FourParameters/MarczewskiJaroniec.hpp"
#include "isotherm/models/FourParameters/Virial.hpp"
#include "isotherm/models/FourParameters/WeberVanVliet.hpp"
#include "isotherm/models/FourParameters/WilsonVST.hpp"
#include "isotherm/models/FiveParameters/FrenkelHalseyHill.hpp"
#include "isotherm/models/FiveParameters/FritzSchlunderV.hpp"
#include "isotherm/models/SixParameters/TripleSiteLangmuir.hpp"
