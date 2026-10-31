..
   ==============================================================================
   Name        : index.rst
   Authors     : Joao Flavio Vasconcellos
   Version     : 2.0.0
   Description : Isotherm++ 2.0 project source and documentation.
   
   Copyright   : Copyright (C) 2026 Joao Flavio Vasconcellos
   Licence     : MIT License (see LICENSE)
   SPDX-License-Identifier: MIT
   ==============================================================================

C++20 architecture
==================

Core principles
---------------

The library never uses inheritance or virtual functions in its model
architecture. A scientific model is a value type providing an
``evaluate`` function, together with an ``isotherm::model_traits``
specialisation. ``scalar_isotherm`` and ``thermal_isotherm`` are compile-time
concepts. The core provides validated ``evaluate`` and ``evaluate_into``
functions; individual model implementations remain responsible for their
physical domain and parameter restrictions.

Data and units
--------------

The scalar independent variable must be interpreted using the model's
scientific metadata: concentration, pressure or relative pressure. This
initial contract does not impose a full dimensional-analysis system.
Temperature-dependent models accept an absolute temperature in kelvin.

Error handling
--------------

Errors are raised as ``isotherm_error`` exceptions with explicit codes.
No library operation calls ``exit`` or terminates the host application.
Bulk evaluations write incrementally and do not provide rollback if a
later point fails.

Future work
-----------

The model factory, mathematical derivatives, implicit solvers, production
isotherms and optional fitting adapters belong to subsequent increments.
Do not treat them as implemented.
