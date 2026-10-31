#!/usr/bin/env python3
#==============================================================================
# Name        : check_henry_source_browser.py
# Authors     : Joao Flavio Vasconcellos
# Version     : 2.0.0
# Description : Validate generated Henry source pages and raw files.
#
# Copyright   : Copyright (C) 2026 Joao Flavio Vasconcellos
# Licence     : MIT License (see LICENSE)
# SPDX-License-Identifier: MIT
#==============================================================================

"""Build source browser in a temporary directory and check raw-source fidelity."""
import subprocess
import sys
import tempfile
from pathlib import Path
from _checks import ROOT, require, success

with tempfile.TemporaryDirectory(prefix="isotherm_source_browser_") as directory:
    output = Path(directory)
    subprocess.run([sys.executable, str(ROOT / "docs/scripts/build_source_browser.py"),
                    "--output", str(output)], check=True, timeout=120)
    rel = "include/isotherm/models/OneParameter/Henry.hpp"
    original = ROOT / rel
    html = output / (rel + ".html")
    require(html.is_file(), f"Missing source page: {html}")
    page = html.read_text(encoding="utf-8")
    require("Henry" in page and "coefficient" in page,
            "Henry source page does not expose implementation symbols")
    raw_candidates = [output / rel, output / (rel + ".txt"),
                      output / (rel + ".raw")]
    raws = list(output.rglob("Henry.hpp"))
    require(any(p.read_bytes() == original.read_bytes() for p in raws),
            "No byte-identical raw Henry header in source browser")
    require((output / "index.html").is_file(), "Source browser index missing")
success("Henry source-browser generation and raw file fidelity")
