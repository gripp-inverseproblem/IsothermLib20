..
   ==============================================================================
   Name        : index.rst
   Authors     : Joao Flavio Vasconcellos
   Version     : 2.0.0
   Description : User installation and C++ integration guide.

   Copyright   : Copyright (C) 2026 Joao Flavio Vasconcellos
   Licence     : MIT License (see LICENSE)
   SPDX-License-Identifier: MIT
   ==============================================================================

User guide
==========

Installing the C++20 interface
-------------------------------------------

.. code-block:: console

   cmake -S . -B build/release -DISOTHERM_BUILD_TESTS=OFF
   cmake --build build/release
   cmake --install build/release --prefix install

A consuming CMake application uses ``find_package(IsothermPP CONFIG REQUIRED)``
and links ``Isotherm::Isotherm``. The stable public header is
``isotherm/isotherm.hpp``.

The first implemented scientific models are ``isotherm::models::Henry`` and
``isotherm::models::Langmuir``. See the scientific catalogue for a mathematical
reference and the accompanying runnable examples. The independent GoogleTest
suites verify the principal identities and domains of both models.


Building the manuals
--------------------

Activate the WSL virtual environment where you installed Sphinx and run:

.. code-block:: console

   python -m sphinx --version
   cmake -S . -B build -DISOTHERM_BUILD_DOCS=ON \
     -DISOTHERM_SPHINX_PYTHON="$(command -v python)"
   cmake --build build --target docs-check
   cmake --build build --target docs-html
   cmake --build build --target docs-pdf

PDF generation uses HTML/CSS and WeasyPrint; both formats share the same RST pages.
