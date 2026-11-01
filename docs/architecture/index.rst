..
   ==============================================================================
   Name        : index.rst
   Authors     : Joao Flavio Vasconcellos
   Version     : 2.0.0
   Description : Isotherm++ 2.0 project source and documentation.
   
   Copyright   : Copyright (C) 2026 Joao Flavio Vasconcellos
   Licence     : MIT License (see LICENSE.txt)
   SPDX-License-Identifier: MIT
   ==============================================================================

C++20 architecture
==================

Core principles
---------------

The library is header-only and never uses inheritance or virtual functions in
its model architecture. A scientific model is a value type whose constructor
validates the parameters and whose member functions ``evaluate`` and
``derivative`` return the adsorbed amount and its derivative with respect to
the independent variable. Each model has an ``isotherm::model_traits``
specialisation that gives the number and names of its parameters, its
independent variable, whether the equation is explicit or implicit and whether
the model depends on temperature.

``scalar_isotherm``, ``thermal_isotherm``, their ``differentiable_`` variants
and ``parameterised_model`` are compile-time concepts. The core provides the
free functions ``evaluate``, ``derivative`` and ``evaluate_into``, which
validate the independent variable and reject non-finite results, and
``parameters``, ``parameter``, ``with_parameter`` and ``with_parameters``,
which read parameters or return a new, validated model with replaced values.
Individual model implementations remain responsible for their physical domain
and parameter restrictions.

Derivatives and implicit models
-------------------------------

Every model returns the derivative from its exact expression. Implicit models
compute the adsorbed amount numerically, in most cases by bisection on an
interval that contains the root, to the resolution of double precision; the
derivative then follows from implicit differentiation at that value.

Data and units
--------------

The scalar independent variable must be interpreted using the model's
scientific metadata: concentration, pressure, relative pressure or, for some
moisture-sorption sections, temperature. The library does not impose a
dimensional-analysis system. Temperature-dependent models accept an absolute
temperature in kelvin as an additional argument.

Error handling
--------------

Errors are raised as ``isotherm_error`` exceptions with explicit codes.
No library operation calls ``exit`` or terminates the host application.
Bulk evaluations write incrementally and do not provide rollback if a
later point fails.

Current scope
-------------

The library covers single-component equilibrium. It does not include a
parameter-fitting module; parameters are estimated with an optimiser chosen by
the user, through the parameter functions described above.
