# Smith migration audit, Isotherm++ 2.0

## Source inspected

The user-supplied `Isotherm(3).zip` was inspected. The archive already contains
`MIGRATION_SPEC_EN_AU.md` and `MIGRATION_MATRIX.md`, but its compiled Smith
model remains a three-coefficient thermal model. The preparation files are not
evidence that the migration itself was carried out.

## Confirmed inconsistencies

1. Smith is implemented in `include/isotherm/models/ThreeParameters/Smith.hpp`
   as `Smith(A,B,C)` with `evaluate(a_w,T)` and `derivative(a_w,T)`.
2. `model_traits<Smith>` reports `parameter_count=3`,
   `variable=relative_pressure`, and `temperature_dependent=true`.
3. Its existing derivative is `dq_e/da_w = C/(1-a_w)`, not the requested
   temperature derivative `B`.
4. The existing tests, compiled example and CSV/plot script use the old API
   and three scaled sensitivity columns.
5. `docs/scientific_catalogue/models/ThreeParameters/Smith.rst`, the catalogue
   index, the umbrella header, and `docs/CMakeLists.txt` refer to Smith in
   the old category and/or depict activity as the abscissa.
6. Model registration and test discovery use filesystem scanning; they do not
   require explicit per-model CMake entries. The figures target is explicitly
   referenced and must be updated.
7. Source-browser indexing is also based on dynamically enumerated paths, so
   the script does not need changes, but links in the RST file must be updated.

## Chosen correction

Store A, B, C and fixed water activity in the class. Use temperature as the
sole runtime input and `derivative(model,T)` as the temperature derivative.
The category now means four **stored** values, not four freely fitted
empirical coefficients. Update the catalogue's blanket 'adjustable parameter'
language to avoid misrepresenting the physical interpretation.

The original Smith (1947) relation has two coefficients. The temperature term
in the current library is a modification; adding stored `a_w` does not
constitute a new historical four-coefficient Smith equation.

## Analytic checks

\[q_e(T;A,B,C,a_w) = A+BT-C\ln(1-a_w).\]
\[\partial q_e/\partial T=B,\qquad
  \partial q_e/\partial a_w=C/(1-a_w).\]
\[S_A=A,\quad S_B=BT,\quad S_C=-C\ln(1-a_w),\quad
  S_{a_w}=Ca_w/(1-a_w).\]

At constant `a_w`, only the intercept `A-C*ln(1-a_w)` and the slope B
can be separately inferred from a temperature series. Separately fitting
A and C requires observations at different water activities or additional
information.

## Verification limits

The updated example compiled, generated data and figures, and passed a
243-point numerical parity comparison in the sandbox. The source-browser
script completed. The six new GTest source cases were exercised with a
local substitute harness because the genuine GTest framework was not
installed. Full Sphinx, GTest, CTest and user's local build must be verified
in the actual repository. No unrelated model was changed.
