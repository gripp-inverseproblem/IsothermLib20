#!/usr/bin/env python3
#==============================================================================
# Name        : check_remaining_models.py
# Authors     : Joao Flavio Vasconcellos
# Version     : 2.0.0
# Description : Validate traits and types in other parameter groups.
#
# Copyright   : Copyright (C) 2026 Joao Flavio Vasconcellos
# Licence     : MIT License (see LICENSE.txt)
# SPDX-License-Identifier: MIT
#==============================================================================

"""Check one-, four-, five- and six-parameter traits and numeric interfaces."""
from _checks import model_files, require, success

models = [(group, count, name, path) for group, count, name, path in model_files()
          if group not in {"TwoParameters", "ThreeParameters"}]
require(len(models) >= 10, "Remaining-parameter model inventory is incomplete")
for group, count, name, path in models:
    src = path.read_text(encoding="utf-8")
    require(f"parameter_count = {count}" in src, f"{name}: incorrect traits count")
    require('"isotherm/core/type.hpp"' in src, f"{name}: missing Real alias")
    require("Real " in src, f"{name}: missing Real in interface")
success(f"Shared Real and traits checked in {len(models)} remaining models")
