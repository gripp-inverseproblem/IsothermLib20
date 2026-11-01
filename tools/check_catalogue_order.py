#!/usr/bin/env python3
#==============================================================================
# Name        : check_catalogue_order.py
# Authors     : Joao Flavio Vasconcellos
# Version     : 2.0.0
# Description : Validate catalogue grouping, order and path coverage.
#
# Copyright   : Copyright (C) 2026 Joao Flavio Vasconcellos
# Licence     : MIT License (see LICENSE.txt)
# SPDX-License-Identifier: MIT
#==============================================================================

"""Verify catalogue groups, alphabetic order, and exact model membership."""
import re
from _checks import ROOT, PARAMETER_COUNTS, require, success, text

src = text("docs/scientific_catalogue/index.rst")
items = re.findall(r"^\s+models/([A-Za-z]+Parameters?)/([A-Za-z0-9]+)\s*$", src, re.M)
require(bool(items), "No model entries in the catalogue")
require(len(set(items)) == len(items), "Duplicate models in catalogue")
for group, count in PARAMETER_COUNTS.items():
    actual = [name for grp, name in items if grp == group]
    expected = sorted(p.stem for p in (ROOT / "include/isotherm/models" / group).glob("*.hpp"))
    require(actual == sorted(actual), f"Non-alphabetical {group} entries")
    require(actual == expected, f"Catalogue/{group} mismatch: {set(actual)^set(expected)}")
    for name in actual:
        require((ROOT / "docs/scientific_catalogue/models" / group / (name + ".rst")).is_file(),
                f"Missing catalogue page: {group}/{name}")
require([g for g, _ in items] == sorted([g for g, _ in items],
        key=lambda g: list(PARAMETER_COUNTS).index(g)), "Misordered parameter groups")
success(f"Alphabetical catalogue verified for {len(items)} model entries")
