..
   ==============================================================================
   Name        : index.rst
   Authors     : Joao Flavio Vasconcellos
   Version     : 2.0.0
   Description : Scope and evidence of current software tests.

   Copyright   : Copyright (C) 2026 Joao Flavio Vasconcellos
   Licence     : MIT License (see LICENSE)
   SPDX-License-Identifier: MIT
   ==============================================================================

Verification and validation
===========================

The I03 validation showed 28/28 successful CTest registrations in the
user's WSL environment. The I05 code adds 22 three-parameter suites with
five tests apiece, plus algebraic-equivalence and implicit-residual tests.
A GoogleTest-independent smoke programme independently checks the reference
uptake of all 22 new models.

In the package-construction environment, GCC built the examples and the
smoke programme, 5/5 available CTest registrations passed, and 114 new
GoogleTest-style cases passed through a temporary local compatibility harness.
This harness is not a substitute for running the actual GoogleTest framework.
Full GoogleTest, Sphinx HTML and PDF verification remain necessary on the WSL
installation before release. The compiled-figure target was exercised for all
22 additional models.

Each historical two- and three-parameter equation has been compared algebraically with
the corresponding original Isotherm++ source file. This does not remove the
need to investigate discrepancies between published model conventions,
particularly the decreasing legacy Jovanovic branch, the singular Halsey
form, dimensionless input in Dubinin–Radushkevich and the constrained
implicit Elovich coverage branch. Current tests cover finite-difference
slopes, independently evaluated identities and invalid input/parameter
domains; they are not a substitute for experimental validation.

The documentation target ``docs-figures`` regenerates model plots from
compiled C++20 executables rather than from independent Python model
equations. The catalogue-order check enforces ascending parameter-group order
and case-insensitive alphabetical model order; the PDF assembly uses the same
explicit ordering. ``docs-html`` builds Sphinx HTML and ``docs-pdf`` renders
A4 PDF from HTML/CSS using WeasyPrint. Both are pending confirmation with the
user's Sphinx virtual environment.
