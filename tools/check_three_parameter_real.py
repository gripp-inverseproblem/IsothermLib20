#!/usr/bin/env python3
#==============================================================================
# Name        : check_three_parameter_real.py
# Authors     : Joao Flavio Vasconcellos
# Version     : 2.0.0
# Description : Check shared Real type in three-parameter APIs.
#
# Copyright   : Copyright (C) 2026 Joao Flavio Vasconcellos
# Licence     : MIT License (see LICENSE)
# SPDX-License-Identifier: MIT
#==============================================================================

"""Check three-stored-parameter models use the shared Real alias."""
from _checks import model_files, require, success

models = [(name, path) for group, _, name, path in model_files() if group == "ThreeParameters"]
require(len(models) >= 25, "Unexpectedly few three-parameter models")
for name, path in models:
    src = path.read_text(encoding="utf-8")
    require('"isotherm/core/type.hpp"' in src, f"{name}: missing Real alias include")
    require("Real " in src, f"{name}: missing Real in API")
    require("parameter_count = 3" in src, f"{name}: wrong parameter count")
success(f"Shared Real API checked in {len(models)} three-parameter models")
