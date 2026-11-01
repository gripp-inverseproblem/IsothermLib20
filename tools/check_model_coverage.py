#!/usr/bin/env python3
#==============================================================================
# Name        : check_model_coverage.py
# Authors     : Joao Flavio Vasconcellos
# Version     : 2.0.0
# Description : Cross-check model headers, tests, examples and docs.
#
# Copyright   : Copyright (C) 2026 Joao Flavio Vasconcellos
# Licence     : MIT License (see LICENSE.txt)
# SPDX-License-Identifier: MIT
#==============================================================================

"""Confirm all published models have header, documentation, examples, and tests."""
from _checks import ROOT, PARAMETER_COUNTS, require, success, model_files, text

models = list(model_files())
require(len(models) >= 55, f"Model count unexpectedly low: {len(models)}")
for group, count, name, path in models:
    header = path.read_text(encoding="utf-8")
    require(f"model_traits<models::{name}>" in header,
            f"Missing traits: {group}/{name}")
    require(f"parameter_count = {count}" in header,
            f"Parameter count mismatch: {group}/{name}")
    require((ROOT / "Examples" / group / name / "main.cpp").is_file(),
            f"Missing compiled example: {group}/{name}")
    require(list((ROOT / "IsothermTest" / group / name).glob("test_*.cpp")),
            f"Missing unit test: {group}/{name}")
    require((ROOT / "docs/scientific_catalogue/models" / group / (name + ".rst")).is_file(),
            f"Missing catalogue document: {group}/{name}")
    require((ROOT / "Examples" / group / name / "plot.py").is_file(),
            f"Missing figure generator: {group}/{name}")
    require(f"models/{group}/{name}" in text("docs/scientific_catalogue/index.rst"),
            f"Unlisted model: {group}/{name}")
# FHH is a three-parameter model: the obsolete five-parameter files must
# not be rediscovered by CMake or mask the concentration-based implementation.
require((ROOT / "src/isotherm/models/ThreeParameters/FrenkelHalseyHill.hpp").is_file(),
        "FHH migration missing ThreeParameters header")
for obsolete in (
    "src/isotherm/models/FiveParameters/FrenkelHalseyHill.hpp",
    "Examples/FiveParameters/FrenkelHalseyHill/main.cpp",
    "Examples/FiveParameters/FrenkelHalseyHill/plot.py",
    "IsothermTest/FiveParameters/FrenkelHalseyHill/test_frenkel_halsey_hill.cpp",
):
    require(not (ROOT / obsolete).exists(), f"Obsolete FHH file remains: {obsolete}")
require('"isotherm/models/ThreeParameters/FrenkelHalseyHill.hpp"'
        in text("src/isotherm/isotherm.hpp"),
        "Umbrella header must expose the three-parameter FHH")
require('example_FiveParameters_FrenkelHalseyHill'
        not in text("docs/CMakeLists.txt"),
        "Figure generator must not reference the old five-parameter FHH")
require('example_ThreeParameters_FrenkelHalseyHill'
        in text("docs/CMakeLists.txt"),
        "Figure generator must reference the three-parameter FHH")
# Specific Smith migration invariants.
require((ROOT / "src/isotherm/models/FourParameters/Smith.hpp").is_file(),
        "Smith migration missing FourParameters header")
require(not (ROOT / "src/isotherm/models/ThreeParameters/Smith.hpp").exists(),
        "Obsolete ThreeParameters/Smith.hpp remains")
success(f"Source, example, test, and documentation coverage for {len(models)} models")
