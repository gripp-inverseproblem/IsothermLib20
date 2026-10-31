<!--
==============================================================================
Name        : RELEASE_AND_CLEANUP.md
Authors     : Joao Flavio Vasconcellos
Version     : 2.0.0
Description : Manifest of excluded legacy files and safe manual migration.

Copyright   : Copyright (C) 2026 Joao Flavio Vasconcellos
Licence     : MIT License (see LICENSE)
SPDX-License-Identifier: MIT
==============================================================================
-->

# Isotherm++ 2.0: consolidated source package and cleanup

## Scope

This is a **complete, clean, standalone tree** containing current I01–I02.4
sources, not an incremental overlay. All code, tests, examples, numerical
plotting inputs, Sphinx reStructuredText sources, images, approved palette and
build scripts remain. No historical model source is claimed to be migrated.

## Deliberately excluded from this ZIP

- Old patch and progress Markdown reports: `APPLY_INCREMENT.md`,
  `BUILD_VALIDATION.md`, `CHANGES_I02_1_I02_2.md`,
  `I02_4_*_FIX.md`, `I02_4_RELEASE_NOTES.md`,
  `IMPLEMENTATION_STATUS.md`, `UPGRADE_FROM_I02.md`, and `docs/README.md`.
- Duplicate earlier HTML/CSS prototypes including
  `docs/design/approved_html_reference.html` and `docs/_static/isothermlib.css`.
- Old generated output directories, including `DoxyFiles/`, `docs/html/`,
  `docs/latex/`, `IsothermLib/doxygen/`, and any cached `build/docs/latex/`.
- Old CMake/Doxygen configuration under `docs/Doxyfile.in`, and
  duplicated LaTeX-only RST figure variants and PDF figure copies.
- `.git/` history, virtual environments, compiled binaries and cache files.
- Old trees `IsothermLib/` and `IsothermLibTest/` are **not part of this new
  distribution**, but should be preserved separately for planned model migration.

## What to do with an existing mixed directory

**Recommended:** extract the ZIP into a *new* directory. This requires no
manual deletion and leaves the historical checkout unchanged. Do not extract
into the old directory assuming that ZIP extraction deletes files: it does not.

If you intentionally reuse the mixed `~/Programas/Isotherm` checkout,
**manually** remove obsolete files/directories only after comparing to the
new ZIP and backing up the originals. In particular:

- Can generally be removed *after review* (if present):
  `docs/Doxyfile.in`, `docs/_static/isothermlib.css`,
  `docs/design/approved_html_reference.html`, `LICENSE_REVIEW_REQUIRED.md`,
  and the superseded incremental `.md` files named above.
- Generated documentation may be deleted and regenerated:
  `DoxyFiles/`, `docs/html/`, `docs/latex/`, `build/docs/latex/`.
- **Keep in a separate archive, do not delete:** `IsothermLib/`,
  `IsothermLibTest/`, and any unported model/exemplar source, historical
  licence notices, or work that is not contained in this ZIP.
- `include/isothermlib/` is superseded by `include/isotherm/` for the
  new scaffold. Review and archive before removing if it contains local edits.

A fresh out-of-source build directory avoids stale CMake cache and Doxygen
paths. Neither this ZIP nor its build scripts automatically delete user files.

## Document pipeline change

Sphinx still validates/builds the HTML using PyData. The A4 PDF is now
printed from those HTML pages by WeasyPrint with `isotherm-print.css`, so the
PDF shares the visual identity of the HTML. Equations in the PDF are rendered
as vector SVGs from the LaTeX source strings by Matplotlib mathtext.
The print renderer rejects unrenderable equations rather than silently
omitting them. Complex future LaTeX expressions may require adaptation of the
math renderer; PDF parity must be reviewed visually on WSL.

## Verification status

The previous I02.3 suite passed 9/9 CTest tests on the user's WSL. After
this re-packaging the source header check and core CMake/CTest were rerun in
a separate environment; those local checks do not replace a full Sphinx build
on the user's WSL. The print layout is validated independently with a local
HTML fixture and WeasyPrint.
