#!/usr/bin/env python3
#==============================================================================
# Name        : check_two_parameter_real.py
# Authors     : Joao Flavio Vasconcellos
# Version     : 2.0.0
# Description : Check shared Real type in two-parameter model APIs.
#
# Copyright   : Copyright (C) 2026 Joao Flavio Vasconcellos
# Licence     : MIT License (see LICENSE.txt)
# SPDX-License-Identifier: MIT
#==============================================================================

"""Check reviewed two-coefficient models use the shared Real type."""
from _checks import ROOT, model_files, require, success

models = [(name, path) for group, _, name, path in model_files() if group == "TwoParameters"]
require(len(models) >= 8, "Unexpectedly few two-parameter models")
for name, path in models:
    src = path.read_text(encoding="utf-8")
    require('"isotherm/core/type.hpp"' in src, f"{name}: missing Real alias include")
    require("Real " in src, f"{name}: missing Real in numeric API")
    require("parameter_count = 2" in src, f"{name}: wrong parameter count")
success(f"Shared Real API checked in {len(models)} two-parameter models")
