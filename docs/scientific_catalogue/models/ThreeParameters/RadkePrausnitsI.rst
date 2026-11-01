..
   ==============================================================================
   Name        : RadkePrausnitsI.rst
   Authors     : Joao Flavio Vasconcellos
   Version     : 2.0.0
   Description : Model equation, validity, code example and reproducible figures.

   Copyright   : Copyright (C) 2026 Joao Flavio Vasconcellos
   Licence     : MIT License (see LICENSE.txt)
   SPDX-License-Identifier: MIT
   ==============================================================================

Radke–Prausnitz I isotherm
==========================

Original publication and scope
------------------------------

The original adsorption study is Radke and Prausnitz (1972),
*Adsorption of Organic Solutes from Dilute Aqueous Solution of Activated
Carbon*, *Industrial & Engineering Chemistry Fundamentals*, **11** (4),
445–451 (`DOI <https://doi.org/10.1021/i160044a003>`_). It concerns
the equilibrium adsorption of organic solutes from dilute aqueous
solutions onto activated carbon. The three expressions labelled I, II
and III in this catalogue are specific mathematical parametrisations;
their common historical attribution does not establish that they are
interchangeable.

Tran, Bollinger, Salvestrini, Chu and Juang (2023), *Critical Review and
Discussion of the Nonlinear Form of Radke–Prausnitz Model in Adsorption
Solid–Liquid Phases*, *Journal of Environmental Engineering*, **149** (3),
03122006 (`DOI: 10.1061/JOEEDU.EEENG-7074
<https://doi.org/10.1061/JOEEDU.EEENG-7074>`_), examine the nonlinear
expression, parameter dimensions and correct attribution to Radke and
Prausnitz (1972). In particular, their analysis specifies the admissible
exponent range for the correctly parametrised Radke–Prausnitz expression.

Tran, Bollinger, Lima and Juang (2023), *How to avoid mistakes in treating
adsorption isotherm data (liquid and solid phases): Some comments about
correctly using Radke-Prausnitz nonlinear model and Langmuir equilibrium
constant*, *Journal of Environmental Management*, **325** (Part A), 116475
(`DOI: 10.1016/j.jenvman.2022.116475
<https://doi.org/10.1016/j.jenvman.2022.116475>`_), identify incorrect
nonlinear forms and errors in the dimensional interpretation of adsorption
equilibrium constants.

The implemented form I is :math:`q_e=q_{\max}KC_e/(1+KC_e)^n`.
It has a Henry-law limit at low concentration. At :math:`n=1` it reduces
to Langmuir; for :math:`0<n<1` its high-concentration asymptote is
proportional to :math:`C_e^{1-n}`. For :math:`n>1`, its derivative can
become negative at sufficiently high concentration, so the numerical
domain :math:`n>0` accepted by the C++ class is broader than the range
normally used for monotonic adsorption curves. The exponent convention
is specific to this form and must not be transferred unchanged to
forms II or III.


Mathematical formulation
------------------------

.. math::

   q_e=\frac{q_{\max}Kc_e}{(1+Kc_e)^n}

The three adjustable parameters are ``q_max``, ``K`` and ``n``.
All must be strictly positive and finite. The model accepts finite,
non-negative equilibrium concentrations, while the derivative requires
strictly positive concentration.

An analytical concentration derivative is implemented and checked against an
independently calculated centred difference.

The implementation requires the following admissible parameter and input domain:

.. math::

   q_{\mathrm{max}}>0,\quad K>0,\quad n>0,\quad C_e\geq0.

C++20 usage
-----------

.. code-block:: cpp

   #include <isotherm/isotherm.hpp>

   const isotherm::models::RadkePrausnitsI model{5, 0.65, 1.15};
   const isotherm::Real uptake = isotherm::evaluate(model, 0.8);   // 1.6063982
   const isotherm::Real slope = isotherm::derivative(model, 0.8);  // 1.2180092

.. raw:: html

   <div class="isotherm-source-tools" role="navigation" aria-label="RadkePrausnitsI source code">
     <a class="isotherm-source-action" href="../../../source-browser/src/isotherm/models/ThreeParameters/RadkePrausnitsI.hpp.html">Implementation</a>
     <a class="isotherm-source-action" href="../../../source-browser/IsothermTest/ThreeParameters/RadkePrausnitsI/test_radke_prausnits_i.cpp.html">Tests</a>
     <a class="isotherm-source-action" href="../../../source-browser/Examples/ThreeParameters/RadkePrausnitsI/main.cpp.html">Example</a>
   </div>

RadkePrausnitsI isotherm plot
-----------------------------

.. figure:: ../../../_static/figures/radke_prausnits_i_curve.svg
   :width: 95%
   :alt: Uptake curve computed with the compiled C++20 RadkePrausnitsI implementation.

   Radke–Prausnitz I uptake for :math:`q_{\max}=5`, :math:`K=0.65`, :math:`n=1.15`.

Concentration derivative
------------------------

.. figure:: ../../../_static/figures/radke_prausnits_i_derivative.svg
   :width: 95%
   :alt: Analytical slope from the compiled C++20 RadkePrausnitsI model.

   Concentration derivative of Radke–Prausnitz I uptake for :math:`q_{\max}=5`,
   :math:`K=0.65`, :math:`n=1.15`.

Numerical sensitivity
---------------------

For each coefficient :math:`k_i`, the numerical scaled sensitivity is

.. math::

   \lambda_i(C_e)=k_i\frac{\partial q_e}{\partial k_i}.

Numerical values are obtained by perturbing individual parameters and evaluating new instances of the C++ model. The accompanying C++20 example performs this procedure.

.. figure:: ../../../_static/figures/radke_prausnits_i_sensitivity.svg
   :width: 95%
   :alt: Comparative numerical parameter sensitivities for the RadkePrausnitsI model.

   Scaled sensitivities :math:`\lambda_{q_{\max}}`, :math:`\lambda_{K}`, :math:`\lambda_{n}`
   versus equilibrium concentration.

Validation and limitations
--------------------------

.. math::

   q_{\mathrm{max}}>0,\quad K>0,\quad n>0,\quad C_e\geq0.
