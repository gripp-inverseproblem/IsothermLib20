<!--
==============================================================================
Name        : README.md
Authors     : Joao Flavio Vasconcellos
Version     : 2.0.0
Description : Overview, installation and use of the Isotherm++ 2.0 library.

Copyright   : Copyright (C) 2026 Joao Flavio Vasconcellos
Licence     : MIT License (see LICENSE.txt)
SPDX-License-Identifier: MIT
==============================================================================
-->

# Isotherm++ 2.0

Isotherm++ is a header-only C++20 library of single-component adsorption
isotherm models, written for use inside mass transport simulations such as
fixed-bed adsorption, chromatography and diffusion in porous particles.

**Manual:** <https://gripp-inverseproblem.github.io/IsotermLib20/>
([PDF](https://gripp-inverseproblem.github.io/IsotermLib20/IsothermPP-Scientific-Manual.pdf))

For every model, the library provides:

- the adsorbed amount as a function of the model's independent variable
  (fluid-phase concentration, relative pressure or temperature);
- the derivative of the adsorbed amount with respect to that variable,
  computed from the exact expression;
- validation of parameters and inputs against the admissible domain;
- read and replace access to the parameters, for fitting and inverse problems;
- a page in the scientific manual with the equation, its derivative, the
  parameter sensitivities and the original publication.

All models share the same interface, so the isotherm used by a solver can be
changed without modifying the solver.

## Quick example

```cpp
#include <isotherm/isotherm.hpp>
using namespace isotherm;

int main() {
    models::Langmuir langmuir{60.8, 0.035};      // q_max, K
    Real q  = langmuir.evaluate(10.0);           // adsorbed amount
    Real dq = langmuir.derivative(10.0);         // dq/dC

    // Replace parameter 1 (K); the new model is validated again
    auto refitted = with_parameter(langmuir, 1, 0.040);
}
```

A function written once for any isotherm:

```cpp
template<differentiable_scalar_isotherm Model>
Real capacity(const Model& m, Real C, Real eps, Real rho_b) {
    return eps + rho_b * derivative(m, C);       // eps + rho_b dq/dC
}
```

Complete programmes are in `Examples/Usage/`:

- `SolverInterface`: basic use, a solver coefficient written once for any
  isotherm, and parameter replacement;
- `FixedBed`: four isotherms fitted to the same equilibrium data and used in a
  fixed-bed model (implicit finite volumes, Newton iterations with the exact
  derivative); `plot.py` draws the results.

## Available models

Models are grouped by number of parameters. (I) marks implicit models, solved
numerically to double precision; (T) marks models that also take the absolute
temperature as an argument.

| Parameters | Models |
|---|---|
| 1 | Henry |
| 2 | Anti-Langmuir, Dubinin–Radushkevich (T), Elovich (I), Freundlich, Halsey, Harkins–Jura, Jovanovic, Langmuir, Temkin (T), Volmer (I) |
| 3 | Aranovich, BET, Bingel–Walton, Brouers–Sotolongo, Dubinin–Astakhov, Flory–Huggins (I), Flory–Huggins VST (I), Fowler–Guggenheim (I, T), Frenkel–Halsey–Hill, Fritz–Schlünder, Frumkin (I), GAB, Henderson, Hill, Hill–de Boer (I, T), Holl–Krich, Jossens (I), Khan, Kiselev (I), Koble–Corrigan, Langmuir–Freundlich, MacMillan–Teller, O'Brien–Myers, Quadratic, Radke–Prausnitz I, II and III, Redlich–Peterson, Sips, Tóth, UNILAN, Valenzuela–Myers, Vieth–Sladek |
| 4 | Baudu, Dual-site Langmuir, Fritz–Schlünder IV, Jensen–Seaton, Marczewski–Jaroniec, Oswin (T), Smith, Virial (I), Weber–van Vliet (I), Wilson VST (I) |
| 5 | Fritz–Schlünder V |
| 6 | Triple-site Langmuir |

Some models are the same equation under a different parametrisation. They are
kept under their usual names, and the equivalence is stated in the manual. The
following equivalences are also checked by the tests:

- Hill, Sips, Koble–Corrigan and Holl–Krich are equivalent to Langmuir–Freundlich;
- the three-parameter Fritz–Schlünder model is equivalent to Redlich–Peterson;
- Khan is equivalent to Radke–Prausnitz I.

## Interface

| Function | Result |
|---|---|
| `m.evaluate(x)`, `evaluate(m, x)` | adsorbed amount |
| `m.derivative(x)`, `derivative(m, x)` | derivative with respect to `x` |
| `evaluate_into(m, xs, out)` | adsorbed amount for a range of inputs |
| `parameters(m)`, `parameter(m, i)` | parameter values |
| `with_parameter(m, i, v)`, `with_parameters(m, p)` | new model with replaced parameters |

Temperature-dependent models take the absolute temperature in kelvin as an
extra argument, for example `m.evaluate(x, T)`.

Each model is a value class with a `model_traits` specialisation that gives the
number and names of its parameters, its independent variable, whether it is
explicit or implicit and whether it depends on temperature. The models do not
use inheritance or virtual functions; C++20 concepts (`scalar_isotherm`,
`thermal_isotherm`, `differentiable_scalar_isotherm`, ...) check them at compile
time. A model written by the user works with the same interface if it provides
the same member functions and a `model_traits` specialisation; see
`Examples/OneParameter/ExternalModelDemo`.

Invalid parameters or inputs raise `isotherm::isotherm_error` with an error
code. The library never terminates the host program.

## Requirements

- C++20 compiler (tested with GCC 13 and 15, and Clang 18)
- CMake 3.28 or later
- Optional: GoogleTest, for the tests
- Optional: Python 3 with the packages in `docs/requirements.txt`, for the
  manual and the plotting scripts

Tested on Linux and on Windows through WSL.

## Build and test

```sh
cmake -S . -B build -DISOTHERM_REQUIRE_GTEST=ON
cmake --build build --parallel
cmake --build build --target run_all_tests
```

| CMake option | Default | Purpose |
|---|---|---|
| `ISOTHERM_BUILD_EXAMPLES` | ON | build the examples |
| `ISOTHERM_BUILD_TESTS` | ON | build the tests |
| `ISOTHERM_REQUIRE_GTEST` | OFF | stop if GoogleTest is not found |
| `ISOTHERM_BUILD_DOCS` | ON | configure the documentation targets |

## Use in another CMake project

Install the library:

```sh
cmake -S . -B build/release -DISOTHERM_BUILD_TESTS=OFF -DISOTHERM_BUILD_EXAMPLES=OFF
cmake --build build/release
cmake --install build/release --prefix <install-dir>
```

and link to it:

```cmake
find_package(IsothermPP CONFIG REQUIRED)
target_link_libraries(my_solver PRIVATE Isotherm::Isotherm)
```

The headers are installed in `<install-dir>/include/isotherm`. As the library
is header-only, adding the repository's `src` directory to the compiler search
path is also enough.

## Documentation

The scientific manual is published at
<https://gripp-inverseproblem.github.io/IsotermLib20/>. It is generated in HTML
and PDF from the same sources in `docs/`:

```sh
python -m venv .venv-docs
source .venv-docs/bin/activate
python -m pip install -r docs/requirements.txt
cmake -S . -B build -DISOTHERM_BUILD_DOCS=ON \
  -DISOTHERM_SPHINX_PYTHON="$PWD/.venv-docs/bin/python"
cmake --build build --target docs-html     # build/docs/html/index.html
cmake --build build --target docs-pdf      # build/docs/pdf/IsothermPP-Scientific-Manual.pdf
```

`docs-figures` regenerates the figures of the manual from the compiled
examples. To update the published manual, copy `build/docs/html/` and the PDF
into `html/` and push; the workflow in `.github/workflows/` publishes that
folder to GitHub Pages.

## Repository layout

```
src/isotherm/core/       interface, concepts, traits, validation and errors
src/isotherm/models/     models, grouped by number of parameters
Examples/                one example per model, ExternalModelDemo and Usage/
IsothermTest/            GoogleTest suite and smoke tests
docs/                    sources of the scientific manual (Sphinx)
tools/                   checks of the sources and of the manual, run by CTest
html/                    published manual (generated; do not edit by hand)
cmake/                   build configuration
```

## Citation

Citation metadata are given in `CITATION.cff`; GitHub shows them under
*Cite this repository*.

## Licence

MIT License. See `LICENSE.txt`. The IBM Plex fonts in `docs/_static/fonts`
are distributed under the SIL Open Font License 1.1 (`OFL.txt`).
