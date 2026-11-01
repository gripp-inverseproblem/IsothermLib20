#!/usr/bin/env python3
#==============================================================================
# Name        : check_henry_sensitivity.py
# Authors     : Joao Flavio Vasconcellos
# Version     : 2.0.0
# Description : Check the compiled Henry example against references.
#
# Copyright   : Copyright (C) 2026 Joao Flavio Vasconcellos
# Licence     : MIT License (see LICENSE.txt)
# SPDX-License-Identifier: MIT
#==============================================================================

"""Check actual C++ Henry CSV against an independently evaluated reference."""
import sys
from _checks import run_csv, check_approx, success

rows = run_csv(sys.argv[1], ("concentration", "uptake", "derivative", "sensitivity_kh"), 121)
for i, row in enumerate(rows):
    c = row["concentration"]
    check_approx(row["uptake"], 1.25 * c, f"Henry q row {i}")
    check_approx(row["derivative"], 1.25, f"Henry dq/dc row {i}")
    check_approx(row["sensitivity_kh"], 1.25 * c, f"Henry sensitivity row {i}")
success("Compiled Henry CSV: 121 rows, model and numerical sensitivity")
