#!/usr/bin/env python3
#==============================================================================
# Name        : check_elovich_sensitivity.py
# Authors     : Joao Flavio Vasconcellos
# Version     : 2.0.0
# Description : Check the compiled Elovich example numerically.
#
# Copyright   : Copyright (C) 2026 Joao Flavio Vasconcellos
# Licence     : MIT License (see LICENSE)
# SPDX-License-Identifier: MIT
#==============================================================================

"""Check C++ Elovich data using an independent monotone bisection solver."""
import math
import sys
from _checks import run_csv, check_approx, success

rows = run_csv(sys.argv[1],
    ("concentration", "uptake", "derivative", "sensitivity_qref", "sensitivity_ke"), 81)
qref, ke = 2.4, 0.8
for i, row in enumerate(rows):
    c = row["concentration"]
    lo, hi = 0.0, 1.0
    for _ in range(100):
        mid = (lo + hi)/2
        if mid*math.exp(mid) > ke*c:hi = mid
        else:lo = mid
    theta = (lo+hi)/2 if c else 0.0
    q = qref*theta
    dqdc = q/(c*(1+theta)) if c else qref*ke
    check_approx(row["uptake"], q, f"Elovich q row {i}")
    check_approx(row["derivative"], dqdc, f"Elovich derivative row {i}")
    check_approx(row["sensitivity_qref"], q, f"Elovich q_ref sensitivity row {i}")
    check_approx(row["sensitivity_ke"], qref*theta/(1+theta),
                 f"Elovich K_E sensitivity row {i}")
success("Compiled Elovich CSV: implicit solution, derivative, both sensitivities")
