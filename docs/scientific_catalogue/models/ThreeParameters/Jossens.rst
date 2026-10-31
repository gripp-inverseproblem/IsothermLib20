..
   ==============================================================================
   Name        : Jossens.rst
   Authors     : Joao Flavio Vasconcellos
   Version     : 2.0.0
   Description : Model equation, validity, code example and reproducible figures.

   Copyright   : Copyright (C) 2026 Joao Flavio Vasconcellos
   Licence     : MIT License (see LICENSE)
   SPDX-License-Identifier: MIT
   ==============================================================================

Jossens isotherm
================

Original publication and scope
------------------------------

Jossens, L., Prausnitz, J. M., Fritz, W., Schlünder, E. U., and
Myers, A. L. (1978), *Thermodynamics of multi-solute adsorption from dilute
aqueous solutions*, *Chemical Engineering Science*, **33** (8), 1097–1106.
`DOI: 10.1016/0009-2509(78)85015-5
<https://doi.org/10.1016/0009-2509(78)85015-5>`_. The paper
introduces an adsorption relation in its treatment of adsorption from
dilute solutions.

Chong, B. Y. K., Hashim, M. A., and Chu, K. H. (2024), *The Jossens
isotherm: A critical look at water contaminant adsorption modeling*,
*Separation Science and Technology*, **59** (10–14), 1147–1157.
`DOI: 10.1080/01496395.2024.2368236
<https://doi.org/10.1080/01496395.2024.2368236>`_. The authors distinguish
the original, loading-implicit Jossens relation from the Redlich–Peterson
isotherm and discuss parameter identification.

The original Jossens relation is **loading-implicit**: the equilibrium
concentration is an explicit function of the uptake. It must not be
confused with the Redlich–Peterson form :math:`q_e=AC_e/(1+BC_e^n)`, which
is frequently reported under the Jossens name (Chong, Hashim and Chu, 2024).
This class implements the loading-implicit relation; earlier versions of the
library exposed the Redlich–Peterson form under this name.

Mathematical formulation
------------------------

.. math::

   C_e=\frac{q_e}{H}\exp\left(Fq_e^{\,p}\right)

The three adjustable parameters are ``H`` (Henry coefficient, uptake per
concentration), ``F`` and ``p``. All must be strictly positive and finite.
Because

.. math::

   \ln C_e=\ln q_e-\ln H+Fq_e^{\,p}

is strictly increasing in :math:`q_e`, every positive concentration has a
unique uptake, bracketed by :math:`0<q_e<HC_e`; the C++ class finds it by
bisection to full double resolution. An equivalent explicit form is
:math:`q_e=\left[W\!\left(Fp\,(HC_e)^p\right)/(Fp)\right]^{1/p}`, where
:math:`W` is the principal Lambert function; it is used only to verify the
implementation.

The analytical concentration derivative follows from implicit
differentiation:

.. math::

   \frac{\mathrm{d}q_e}{\mathrm{d}C_e}=\frac{q_e}{C_e\left(1+Fp\,q_e^{\,p}\right)},
   \qquad
   \lim_{C_e\to0^+}\frac{\mathrm{d}q_e}{\mathrm{d}C_e}=H.

The implementation returns the Henry-law limit :math:`H` at :math:`C_e=0`.

The implementation requires the following admissible parameter and input domain:

.. math::

   H>0,\quad F>0,\quad p>0,\quad C_e\geq0.

C++20 usage
-----------

.. code-block:: cpp

   #include <isotherm/isotherm.hpp>

   const isotherm::models::Jossens model{5, 0.65, 1.15};
   const isotherm::Real uptake = isotherm::evaluate(model, 0.8);   // 1.4623238
   const isotherm::Real slope = isotherm::derivative(model, 0.8);  // 0.84734764

.. raw:: html

   <div class="isotherm-source-tools" role="navigation" aria-label="Jossens source code">
     <a class="isotherm-source-action" href="../../../source-browser/include/isotherm/models/ThreeParameters/Jossens.hpp.html">Implementation</a>
     <a class="isotherm-source-action" href="../../../source-browser/IsothermTest/ThreeParameters/Jossens/test_jossens.cpp.html">Tests</a>
     <a class="isotherm-source-action" href="../../../source-browser/Examples/ThreeParameters/Jossens/main.cpp.html">Example</a>
   </div>

Jossens isotherm plot
---------------------

.. figure:: ../../../_static/figures/jossens_curve.svg
   :width: 95%
   :alt: Uptake curve computed with the compiled C++20 Jossens implementation.

   Jossens uptake for :math:`H=5`, :math:`F=0.65`, :math:`p=1.15`.

Concentration derivative
------------------------

.. figure:: ../../../_static/figures/jossens_derivative.svg
   :width: 95%
   :alt: Analytical slope from the compiled C++20 Jossens model.

   Concentration derivative of Jossens uptake for :math:`H=5`, :math:`F=0.65`,
   :math:`p=1.15`.

Numerical sensitivity
---------------------

For each coefficient :math:`k_i`, the numerical scaled sensitivity is

.. math::

   \lambda_i(C_e)=k_i\frac{\partial q_e}{\partial k_i}.

Numerical values are obtained by perturbing individual parameters and evaluating new instances of the C++ model. The accompanying C++20 example performs this procedure.

.. figure:: ../../../_static/figures/jossens_sensitivity.svg
   :width: 95%
   :alt: Comparative numerical parameter sensitivities for the Jossens model.

   Scaled sensitivities :math:`\lambda_{H}`, :math:`\lambda_{F}`, :math:`\lambda_{p}` versus
   equilibrium concentration.

Validation and limitations
--------------------------

.. math::

   H>0,\quad F>0,\quad p>0,\quad C_e\geq0.

Chong, Hashim and Chu (2024) report strong correlation between the Jossens
parameters when they are estimated from a single isotherm; the scaled
sensitivities above should be inspected before interpreting fitted values.
