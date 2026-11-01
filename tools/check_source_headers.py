#!/usr/bin/env python3
#==============================================================================
# Name        : check_source_headers.py
# Authors     : Joao Flavio Vasconcellos
# Version     : 2.0.0
# Description : Verify standard provenance headers in reviewed files.
#
# Copyright   : Copyright (C) 2026 Joao Flavio Vasconcellos
# Licence     : MIT License (see LICENSE.txt)
# SPDX-License-Identifier: MIT
#==============================================================================

"""Check mandatory source provenance metadata in reviewed source files."""
from _checks import ROOT, require, success

keys = ("Name", "Authors", "Version", "Description", "Copyright", "Licence", "SPDX-License-Identifier")
paths = list((ROOT / "src/isotherm").rglob("*.hpp"))
paths += list((ROOT / "Examples").rglob("*.cpp"))
paths += list((ROOT / "Examples").rglob("plot.py"))
paths += list((ROOT / "IsothermTest").rglob("*.cpp"))
paths += list((ROOT / "docs/scientific_catalogue").rglob("*.rst"))
require(bool(paths), "No source files found")
for path in paths:
    heading = path.read_text(encoding="utf-8")[:1400]
    for key in keys:
        require(key in heading, f"{path.relative_to(ROOT)}: missing {key}")
success(f"Source-header fields present in {len(paths)} checked files")
