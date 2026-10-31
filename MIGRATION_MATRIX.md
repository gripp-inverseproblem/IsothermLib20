# Smith migration, traceability matrix

| Priority | Component | Change needed | Verification |
| --- | --- | --- | --- |
| P0 | Model equation | Keep `A + B*T - C*log(1-aw)`, no extra `D` | Reference value and old/new regression |
| P0 | Model state | Store `(A,B,C,aw)` in `FourParameters` | Constructor and accessors test |
| P0 | API | `Smith(A,B,C,aw)`, `evaluate(model,T)` | Client build |
| P0 | Derivative | `derivative(model,T) = B` | Analytic and finite-difference checks |
| P0 | Input validation | `0<=aw<1`, finite T>0, finite coefficients, positive admissible offset | Domain tests |
| P0 | Build/registration | Remove old and add new Smith source/test registrations | CMake configure + discovery |
| P1 | Gaussian/other numerical callers | Find all call sites and update signature | Repository-wide references report |
| P1 | Unit tests | Move/adapt GoogleTest suite to category FourParameters | CTest |
| P1 | Scientific figures | Re-instantiate on activity sweep; plot over T at fixed aw | Check numerical C++ generated CSV and figures |
| P1 | Sensitivities | `A`, `BT`, `-C ln(1-aw)`, `C aw/(1-aw)` | Analytic/numerical comparison |
| P1 | Sphinx documentation | Move Smith.rst, update equations, provenance, references, index | Sphinx warnings-as-errors |
| P1 | Source browser | Update paths/links to FourParameters | HTML link inspection |
| P2 | User guides/examples | Replace previous constructor/evaluate use and label aw as stored | Example builds |
| P2 | Removed paths | Ensure no outdated ThreeParameters Smith files/indices persist | `rg` for stale path |

## Suggested local integration sequence

1. Run read-only preflight; inspect `git status` and current Smith implementation.
2. Compare the current source with Henderson/Oswin migrations; preserve their actual current conventions.
3. Freeze the numerical regression grid using existing three-parameter Smith.
4. Move the model and update stored state, evaluator and derivative with minimal changes.
5. Fix call sites and target-level CMake registrations.
6. Migrate the existing test suite and add four-parameter validation.
7. Adapt current compiled-C++ data generator and sensitivity plots.
8. Relocate and update Smith.rst and its Sphinx indexes/source-browser references.
9. Run targeted tests, global tests, figure targets and strict docs build.
10. Inspect diffs and compare old/new data; only then declare success.
