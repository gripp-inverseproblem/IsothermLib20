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

.. important:: Development status

   **I07** provides 56 named models across six groups of adjustable parameters.
   The scientific review is proceeding model by model. The current HTML
   preview includes the revised Henry documentation, a numerically evaluated
   parameter-sensitivity curve, and a first C++ source-browser prototype.
   Algebraic equivalences, historical formula conventions, primary-source
   verification and numerical robustness remain under scientific review.
   Parameter fitting is not yet part of the core release.

The new architecture uses C++20 concepts and traits, without model inheritance
or virtual functions. Parameter fitting is a planned, optional module.

.. toctree::
   :maxdepth: 2
   :caption: Scientific manual

   scientific_catalogue/index
   user_guide/index
   architecture/index
   developers/index
   verification/index
