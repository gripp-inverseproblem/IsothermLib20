# Smith, FourParameters migration specification

**Project:** Isotherm++ 2.0  
**Status:** design and reference tests prepared; integration with the current repository **not verified**.  
**Language:** Australian English for code-facing prose and scientific documentation.

## 1. Provenance and model identity

Sherman E. Smith (1947), *The Sorption of Water Vapor by High Polymers*, Journal of the American Chemical Society, 69, 646–651, equation (6), p. 647. The original high-activity branch can be written as

\[
q_e(a_w)=\alpha_b-\alpha'\ln(1-a_w).
\]

This original form contains **two coefficients**. The temperature-dependent model already chosen for Isotherm++ is a **modified** Smith formulation. Adding `aw` to the stored state changes the software class category but **does not add a fourth empirical coefficient or change the mathematical model**.

## 2. Stored parameters and independent variable

\[
\boxed{q_e(T;A,B,C,a_w)=A+BT-C\ln(1-a_w)}
\]

| Symbol | Role | Units or restrictions |
| --- | --- | --- |
| `A` | Stored empirical coefficient | Same units as `q_e`, positive in the present project policy |
| `B` | Stored temperature coefficient | Units of `q_e/K`, finite |
| `C` | Stored log-term coefficient | Same units as `q_e`, strictly positive |
| `aw` | Stored water activity | Dimensionless, `0 <= aw < 1` |
| `T` | Independent evaluation variable | Kelvin, finite, `T > 0` |

The current project convention additionally requires `A + B*T > 0` at every evaluated temperature. The applicability of the original Smith branch is restricted to a suitable water-activity regime; the algebraic validity of `aw=0` does not constitute physical validation of a zero-activity extrapolation.

## 3. C++20 API target

```cpp
namespace isotherm::models {
    // Adapt to the existing class conventions and base type.
    // Smith(Real A, Real B, Real C, Real aw);
}

// New caller interface:
Smith model(A, B, C, aw);
const Real q = evaluate(model, T);
const Real dq_dT = derivative(model, T);
```

Do not keep an ambiguous `evaluate(model, aw, T)` overload unless deliberately required by a separately documented compatibility policy. Do not invent `D`. Preserve the project-wide `isotherm::Real` alias and conventions for exceptions, source locations, metadata, factories and immutable parameter storage.

## 4. Derivatives and scaled sensitivities

\[
\frac{\partial q_e}{\partial T}=B,\qquad
\frac{\partial q_e}{\partial a_w}=\frac{C}{1-a_w}.
\]

The generic `derivative(model,T)` must denote the derivative with respect to the **independent variable T**, not the internally stored `aw`.

The agreed scaled sensitivities, one for each stored parameter, are

\[
S_A=A\frac{\partial q_e}{\partial A}=A,\qquad
S_B=B\frac{\partial q_e}{\partial B}=BT,
\]
\[
S_C=C\frac{\partial q_e}{\partial C}=-C\ln(1-a_w),\qquad
S_{a_w}=a_w\frac{\partial q_e}{\partial a_w}=\frac{C a_w}{1-a_w}.
\]

At `aw=0`, `S_aw=0`. At `aw` approaching 1 from below, the logarithmic term and `S_aw` become unbounded. Do not sample exactly `aw=1` in any figure or test.

**Important:** a plot of `q_e` against `aw` or its `aw` sensitivity must be generated from a **sequence of new Smith instances with varied stored `aw`**, keeping `(A,B,C,T)` fixed. A plot against `T` uses one instance with fixed `aw`. Likewise, the temperature derivative graph is a horizontal line at `B`.

## 5. Regression and numerical acceptance

Use the following independent reference case:

- `A=0.3`, `B=0.002`, `C=0.1`, `aw=0.4`, `T=300 K`.
- `q_e=0.9510825623765989`.
- `derivative(model,300)=0.002`.
- `S_A=0.3`, `S_B=0.6`, `S_C=0.051082562376599076`, `S_aw=0.06666666666666668`.

At fixed `aw`, numerical outputs before and after migration must agree within the existing tolerance; use the old three-parameter calculation as an independent regression oracle. Recheck the derivative convention explicitly.

Include tests for constructor parameter ordering and metadata, copy/move semantics where relevant, accessors, valid/invalid domains, boundary `aw=0`, near-saturation `aw`, zero or negative `T`, non-finite inputs, `A+B*T<=0`, analytic/numerical derivatives, scaled sensitivities, and CMake/CTest test discovery.

## 6. Files and integration points

The actual repository paths and conventions must be discovered by `tools/preflight_smith_migration.py`. Inspect at least:

- Smith class declaration and implementation, `ThreeParameters` to `FourParameters`.
- Corresponding `CMakeLists.txt`, discovery targets, registries, model metadata, counts, factories, IDs and exporters.
- GoogleTest cases previously associated with Smith.
- `Examples/` and existing figure-generation programmes; retain the English folder naming decision.
- Sphinx `docs/scientific_catalogue/models/ThreeParameters/Smith.rst`, target `FourParameters`, and all `toctree` links.
- Scientific figure data generated from compiled C++20 implementations, SVG/PNG generation, derivative and scaled sensitivities.
- Source-browser paths, Sphinx dependencies and verification checks.
- Remaining call sites using `evaluate(model,aw,T)` or `Smith(A,B,C)`.

Make the smallest necessary changes, preserve unrelated work, and avoid broad formatting refactors or global text replacement.

## 7. Evidence required for 'migration complete'

1. `git diff --check` succeeds, and the diff is restricted to Smith and necessary index/registry/build files.
2. Current library and targeted Smith GoogleTests compile and pass.
3. Global `ctest --test-dir build --output-on-failure` passes.
4. Figure-generation targets run using compiled C++20 model code.
5. `cmake --build build --target docs-html -j` passes with warnings treated as errors.
6. Source-browser references resolve to the new path, with no stale `ThreeParameters/Smith` link.
7. Old and new numerical results agree at fixed `aw` for a representative `T` grid.
8. The documentation clearly distinguishes the 1947 two-coefficient original from the present modified thermal formulation with four stored values.

Never claim these conditions passed until the corresponding command outputs have been observed on the current tree.
