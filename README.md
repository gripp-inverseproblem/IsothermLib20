<!--
==============================================================================
Name        : README.md
Authors     : Joao Flavio Vasconcellos
Version     : 2.0.0
Description : Current Isotherm++ 2.0 build, documentation and release status.

Copyright   : Copyright (C) 2026 Joao Flavio Vasconcellos
Licence     : MIT License (see LICENSE)
SPDX-License-Identifier: MIT
==============================================================================
-->

# Isotherm++ 2.0

Isotherm++ 2.0 is a C++20 scientific library under development for
adsorption isotherms and moisture-sorption models. The **I07** source
archive provides **56 named model identifiers**: 37 historical IsothermLib
identifiers (including two classes with the same five-parameter formula)
plus 19 literature-derived identifiers. They are not necessarily 56
mathematically independent theories. Each published identifier has a model
header, analytical concentration or activity derivative, domain validation,
GoogleTest source, compiled CSV example, vector SVG figures and a Sphinx page.

The formulations are specified explicitly, including restrictions on implicit
models where mathematical uniqueness is guaranteed. The literature-derived
moisture sorption models accept water activity rather than arbitrary solute
concentration. Reference formulas are documented, but original-publication
priority, independent scientific benchmarking and experimental validation are
separate tasks and have **not** been completed for all 56 entries.

## Build and test

```sh
cmake -S . -B build -DISOTHERM_REQUIRE_GTEST=ON
cmake --build build --parallel
cmake --build build --target run_all_tests
```

With the CMake Makefiles generator, `make -C build run_all_tests` is equivalent.
The models and C++20 model contracts use concepts and traits, not inheritance,
virtual methods or CRTP. The fitting module is planned separately. New example
and GoogleTest directories are discovered automatically by CMake.

## Documentation, HTML and matching PDF

On WSL, activate the Python virtual environment already used for Sphinx:

```sh
source .venv-docs/bin/activate
python -m pip install -r docs/requirements.txt
cmake -S . -B build -DISOTHERM_BUILD_DOCS=ON \
  -DISOTHERM_SPHINX_PYTHON="$PWD/.venv-docs/bin/python"
cmake --build build --target docs-check
cmake --build build --target docs-html
cmake --build build --target docs-pdf
```

The HTML manual is written to `build/docs/html/index.html`. The A4 PDF is
written to `build/docs/pdf/IsothermPP-Scientific-Manual.pdf`. **PDF generation
now prints the generated HTML using WeasyPrint and a dedicated print CSS**;
it no longer uses Sphinx LaTeX or XeLaTeX. Both outputs originate from the
same `docs/*.rst` files. Models appear first in parameter-count groups, then
alphabetically within each group in both HTML and PDF. Matplotlib produces vector equation images for the PDF; the interactive HTML continues to use MathJax.

The documentation and code comments use Australian English. The visual
identity is specified in `docs/design/VISUAL_STANDARD.md`.

## Examples and tests

Henry, the eight two-parameter models, all 22 three-parameter formulations and all four historical four-parameter models and the two five-parameter source identifiers are implemented in I07, together with 19 additional literature-based formulations.
New models are grouped by their number of parameters, consistently across
`include/isotherm/models/`, `src/models/`, `Examples/` and `IsothermTest/`.
Use `Examples/<Group>/<Model>/main.cpp` for examples and
`IsothermTest/<Group>/<Model>/test_*.cpp` for model tests. The standalone
`ExternalModelDemo` remains an integration-contract demonstration, not a
scientific model. Regenerate scientific figures using:

```sh
cmake --build build --target docs-figures
```

This target runs the compiled examples for all 56 model identifiers and sends
their CSV output through the per-model `plot.py` scripts.

## Packaging, cleanup and licensing

This ZIP contains **only the modern I01–I07 source tree**. It intentionally
omits the old generated Doxygen HTML, the older IsothermLib implementation,
obsolete LaTeX output, duplicate prototypes and incremental repair reports.
The historical source must be **archived, not destroyed**, until its model
migration and rights review are complete. Use a separate directory rather than overlaying an older mixed tree. MIT covers only content for
which the project holds sufficient rights; see `LICENSING_NOTICE.md`.

## Installation into another CMake project

```sh
cmake -S . -B build/release -DISOTHERM_BUILD_TESTS=OFF
cmake --build build/release
cmake --install build/release --prefix install
```

Consumers use `find_package(IsothermPP CONFIG REQUIRED)` and the
`Isotherm::Isotherm` CMake target.

## Literature extension scope

I07 implements 37 historical IsothermLib identifiers plus 19 specified
literature-derived conventions, for 56 public model identifiers (not 56
algebraically independent families). The original FrenkelHalseyHill and
FritzSchlunderV source files contain identical five-parameter equations.
All 19 shortlisted literature-derived model identifiers have explicit
mathematical conventions, parameters, admissible domains, tests, examples and
scientific pages. A model count alone is not a scientific validation result.
