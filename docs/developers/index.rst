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

Developer guide
===============

Automatic CMake discovery
-------------------------

To add an example, create
``Examples/TwoParameters/ModelName/main.cpp``. CMake automatically finds it
at the next build. Additional ``*.cpp`` files in the same folder are linked
into that programme. No ``CMakeLists.txt`` changes are required.

For GoogleTest, create
``IsothermTest/TwoParameters/ModelName/test_model.cpp``. All
``test_*.cpp`` in that folder form one GoogleTest executable, with each
test registered in CTest. ``ISOTHERM_REQUIRE_GTEST=ON`` makes the
GoogleTest dependency mandatory.

The ``Core`` group is reserved for independent architectural tests.
Standalone smoke tests may use ``Core/Area/smoke_main.cpp``.

Documentation must follow Australian English, including graph labels.
Each model must be validated against independent mathematical references
before it can be listed as complete.
