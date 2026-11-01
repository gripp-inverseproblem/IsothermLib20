..
   ==============================================================================
   Name        : index.rst
   Authors     : Joao Flavio Vasconcellos
   Version     : 2.0.0
   Description : Scope and evidence of current software tests.

   Copyright   : Copyright (C) 2026 Joao Flavio Vasconcellos
   Licence     : MIT License (see LICENSE.txt)
   SPDX-License-Identifier: MIT
   ==============================================================================

Verification and validation
===========================

The CTest suite combines a GoogleTest test file for every model, smoke
programmes that check reference values without GoogleTest, tests of the
parameter interface and of the declared algebraic equivalences, residual
checks of the implicit equations, and checks of the documentation sources.
It is run with ``cmake --build build --target run_all_tests``. The library
compiles without warnings under GCC 13 and 15 and Clang 18 with strict warning
flags, and the test suite runs cleanly under AddressSanitizer and
UndefinedBehaviorSanitizer.

An independent audit compared every equation and derivative with reference
implementations written from the published equations and evaluated with
60-digit arithmetic, and checked the derivatives symbolically. The audit
included extreme inputs, from about 1e-300 to 1e300, near poles and in the
limits of each domain.

Model-specific conventions that differ from some published forms, such as the
decreasing Jovanovic branch, the singular Halsey form, the dimensionless input
of Dubinin–Radushkevich and the constrained implicit Elovich coverage branch,
are documented on the corresponding model pages. The tests verify the
implementation of the stated equations; they are not a substitute for
experimental validation.

The documentation target ``docs-figures`` regenerates the model plots from the
compiled C++20 examples rather than from independent Python model equations.
``docs-html`` builds the HTML manual with Sphinx, and ``docs-pdf`` renders the
A4 PDF from HTML and CSS with WeasyPrint, redrawing each model's plots from the
same example data.
