#!/usr/bin/env python3
#==============================================================================
# Name        : check_external_links.py
# Authors     : Joao Flavio Vasconcellos
# Version     : 2.0.0
# Description : Check offline syntax of external catalogue hyperlinks.
#
# Copyright   : Copyright (C) 2026 Joao Flavio Vasconcellos
# Licence     : MIT License (see LICENSE)
# SPDX-License-Identifier: MIT
#==============================================================================

"""Verify documented external URLs are syntactically valid, never file:// links.

This checker is deliberately offline: it does NOT assert remote availability.
"""
import re
from urllib.parse import urlsplit
from _checks import ROOT, require, success

pages = list((ROOT / "docs").rglob("*.rst"))
require(bool(pages), "No RST documentation")
count = 0
for path in pages:
    src = path.read_text(encoding="utf-8")
    require("file://" not in src, f"Local file URL in {path}")
    # Verify all explicit linked URLs in reStructuredText link targets.
    for raw in re.findall(r"<((?:https?://)[^<>\s]+)>", src):
        parsed = urlsplit(raw)
        require(parsed.scheme in ("http", "https") and bool(parsed.netloc),
                f"Invalid external URL {raw!r} in {path}")
        count += 1
require(count > 20, "External-link coverage unexpectedly low")
success(f"Offline syntax of {count} external URLs checked; not a live-link test")
