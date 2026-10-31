#!/usr/bin/env python3
#==============================================================================
# Name        : check_dubinin_radushkevich_sensitivity.py
# Authors     : Joao Flavio Vasconcellos
# Version     : 2.0.0
# Description : Check the Dubinin-Radushkevich CSV sensitivities.
#
# Copyright   : Copyright (C) 2026 Joao Flavio Vasconcellos
# Licence     : MIT License (see LICENSE)
# SPDX-License-Identifier: MIT
#==============================================================================

"""Check Dubinin-Radushkevich C++ data, units, and both sensitivities."""
import math
import sys
from _checks import run_csv, check_approx, success

rows = run_csv(sys.argv[1],
    ("concentration", "uptake", "derivative", "sensitivity_qs", "sensitivity_beta"), 81)
qs, beta, R, T = 2.7, 1e-6, 8.31446261815324, 298.15
for i, row in enumerate(rows):
    c = row["concentration"]
    eps = R*T*(math.log1p(c) - math.log(c))
    q = qs*math.exp(-beta*eps*eps)
    d = q*2*beta*eps*R*T/(c*(1+c))
    check_approx(row["uptake"], q, f"DR q row {i}")
    check_approx(row["derivative"], d, f"DR derivative row {i}")
    check_approx(row["sensitivity_qs"], q, f"DR q_s sensitivity row {i}")
    check_approx(row["sensitivity_beta"], -beta*eps*eps*q,
                 f"DR beta sensitivity row {i}")
success("Compiled Dubinin-Radushkevich CSV: uptake, derivative, both sensitivities")
