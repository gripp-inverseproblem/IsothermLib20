..
   ==============================================================================
   Name        : index.rst
   Authors     : Joao Flavio Vasconcellos
   Version     : 2.0.0
   Description : Contents and release scope of the shared scientific manual.

   Copyright   : Copyright (C) 2026 Joao Flavio Vasconcellos
   Licence     : MIT License (see LICENSE.txt)
   SPDX-License-Identifier: MIT
   ==============================================================================

Isotherm++ 2.0
==============

.. container:: isotherm-hero

   .. image:: _static/mark_transparent.png
      :width: 125px
      :alt: Official circular Isotherm++ logo, with three ascending adsorption curves.
      :align: right
      :class: isotherm-brand-icon

   .. container:: isotherm-eyebrow

      Scientific software | Australian English

   **Adsorption isotherm models, mathematical reference and C++20 integration.**

   The scientific documentation and C++ API reference are maintained from
   a common reStructuredText source tree for the HTML and PDF manuals.

Each model provides the adsorbed amount, its exact derivative with respect to
the independent variable, validated parameter domains and read and replace
access to its parameters. All models share one interface based on C++20
concepts and traits, without inheritance or virtual functions, so the isotherm
used by a transport solver can be changed without modifying the solver.

Each model page gives the original publication, the equation implemented, its
derivative, the admissible domain, the numerical parameter sensitivities and a
compiled C++20 example. The library covers single-component equilibrium; it
does not include a parameter-fitting module.

.. toctree::
   :maxdepth: 2
   :caption: Scientific manual

   scientific_catalogue/index
   user_guide/index
   architecture/index
   developers/index
   verification/index
