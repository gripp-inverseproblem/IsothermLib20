..
   ==============================================================================
   Name        : index.rst
   Authors     : Joao Flavio Vasconcellos
   Version     : 2.0.0
   Description : Navigation by number of mathematical model parameters.

   Copyright   : Copyright (C) 2026 Joao Flavio Vasconcellos
   Licence     : MIT License (see LICENSE)
   SPDX-License-Identifier: MIT
   ==============================================================================

Scientific model catalogue
==========================

The catalogue is grouped **by the number of parameters stored by each C++
model** and then **alphabetically within each parameter group**. Some
fixed-activity temperature sections (such as Henderson and Smith) store water
activity as a physical condition, not an additional fitted coefficient.
Temperature supplied to an evaluation function is not a stored parameter.

.. toctree::
   :maxdepth: 1
   :caption: One parameter

   models/OneParameter/Henry

.. toctree::
   :maxdepth: 1
   :caption: Two parameters

   models/TwoParameters/AntiLangmuir
   models/TwoParameters/DubininRadushkevich
   models/TwoParameters/Elovich
   models/TwoParameters/Freundlich
   models/TwoParameters/Halsey
   models/TwoParameters/HarkinJura
   models/TwoParameters/Jovanovic
   models/TwoParameters/Langmuir
   models/TwoParameters/Temkin
   models/TwoParameters/Volmer

.. toctree::
   :maxdepth: 1
   :caption: Three parameters

   models/ThreeParameters/Aranovich
   models/ThreeParameters/BingelWalton
   models/ThreeParameters/BrouersSotolongo
   models/ThreeParameters/BrunauerEmmettTeller
   models/ThreeParameters/DubininAstakhov
   models/ThreeParameters/FloryHuggins
   models/ThreeParameters/FloryHugginsVST
   models/ThreeParameters/FowlerGuggenheim
   models/ThreeParameters/FrenkelHalseyHill
   models/ThreeParameters/FritzSchlunder
   models/ThreeParameters/Frumkin
   models/ThreeParameters/GAB
   models/ThreeParameters/Henderson
   models/ThreeParameters/Hill
   models/ThreeParameters/HillDeBoer
   models/ThreeParameters/HollKrich
   models/ThreeParameters/Jossens
   models/ThreeParameters/Khan
   models/ThreeParameters/Kiselev
   models/ThreeParameters/KobleCorrigan
   models/ThreeParameters/LangmuirFreundlich
   models/ThreeParameters/MacMillanTeller
   models/ThreeParameters/OBrienMyers
   models/ThreeParameters/Quadratic
   models/ThreeParameters/RadkePrausnitsI
   models/ThreeParameters/RadkePrausnitsII
   models/ThreeParameters/RadkePrausnitsIII
   models/ThreeParameters/RedlichPeterson
   models/ThreeParameters/Sips
   models/ThreeParameters/Toth
   models/ThreeParameters/Unilan
   models/ThreeParameters/ValenzuelaMyers
   models/ThreeParameters/ViethSladek

.. toctree::
   :maxdepth: 1
   :caption: Four parameters

   models/FourParameters/Baudu
   models/FourParameters/DualSiteLangmuir
   models/FourParameters/FritzSchlunderIV
   models/FourParameters/JensenSeaton
   models/FourParameters/MarczewskiJaroniec
   models/FourParameters/Oswin
   models/FourParameters/Smith
   models/FourParameters/Virial
   models/FourParameters/WeberVanVliet
   models/FourParameters/WilsonVST

.. toctree::
   :maxdepth: 1
   :caption: Five parameters

   models/FiveParameters/FritzSchlunderV

.. toctree::
   :maxdepth: 1
   :caption: Six parameters

   models/SixParameters/TripleSiteLangmuir

.. note::

   The historical IsothermLib catalogue comprises 37 names (one one-parameter,
   eight two-parameter, 22 three-parameter, four four-parameter, and two
   five-parameter entries). The two five-parameter names implement the very
   same algebraic formula in the archived source and are therefore not two
   independent mathematical model families.

   Additional models are explicitly marked as literature-derived conventions.
   The order is by stored parameter count and then by model name; a
   six-parameter group is necessary for triple-site Langmuir.
