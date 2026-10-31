#!/usr/bin/env python3
#==============================================================================
# Name        : check_anti_langmuir_sensitivity.py
# Authors     : Joao Flavio Vasconcellos
# Version     : 2.0.0
# Description : Check compiled Anti-Langmuir numeric sensitivities.
#
# Copyright   : Copyright (C) 2026 Joao Flavio Vasconcellos
# Licence     : MIT License (see LICENSE)
# SPDX-License-Identifier: MIT
#==============================================================================

"""Check Anti-Langmuir C++ sensitivities against analytic differentiation."""
import sys
from _checks import run_csv, check_approx, success

rows = run_csv(sys.argv[1],
    ("concentration", "uptake", "derivative", "sensitivity_h", "sensitivity_b"), 81)
H, b = 1.6, 0.12
for i, row in enumerate(rows):
    c = row["concentration"]
    denom = 1 - b * c
    check_approx(row["uptake"], H*c/denom, f"AntiLangmuir q row {i}")
    check_approx(row["derivative"], H/(denom*denom), f"AntiLangmuir derivative row {i}")
    check_approx(row["sensitivity_h"], H*c/denom, f"AntiLangmuir H sensitivity row {i}")
    check_approx(row["sensitivity_b"], H*b*c*c/(denom*denom), f"AntiLangmuir b sensitivity row {i}")
success("Compiled Anti-Langmuir CSV: physical domain and both sensitivities")
