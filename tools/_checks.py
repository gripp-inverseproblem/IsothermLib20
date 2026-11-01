#==============================================================================
# Name        : _checks.py
# Authors     : Joao Flavio Vasconcellos
# Version     : 2.0.0
# Description : Shared consistency assertions for repository and CSV checks.
#
# Copyright   : Copyright (C) 2026 Joao Flavio Vasconcellos
# Licence     : MIT License (see LICENSE.txt)
# SPDX-License-Identifier: MIT
#==============================================================================

"""Shared standard-library assertions for Isotherm++ CTest validation.

Reconstructed 2026-10-30: the provided source archive omitted tools/.
These are new independently specified checks, not recovered originals.
"""
from __future__ import annotations

import csv
import io
import math
import subprocess
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
PARAMETER_COUNTS = {
    "OneParameter": 1,
    "TwoParameters": 2,
    "ThreeParameters": 3,
    "FourParameters": 4,
    "FiveParameters": 5,
    "SixParameters": 6,
}


def require(condition: bool, message: str) -> None:
    if not condition:
        raise AssertionError(message)


def text(rel: str | Path) -> str:
    p = ROOT / rel
    require(p.is_file(), f"Required file missing: {p}")
    return p.read_text(encoding="utf-8")


def check_approx(actual: float, expected: float, label: str,
                 rtol: float = 2e-6, atol: float = 1e-10) -> None:
    require(math.isclose(actual, expected, rel_tol=rtol, abs_tol=atol),
            f"{label}: observed {actual:.17g}, expected {expected:.17g}")


def model_files():
    base = ROOT / "include/isotherm/models"
    for group, count in PARAMETER_COUNTS.items():
        for path in sorted((base / group).glob("*.hpp")):
            yield group, count, path.stem, path


def run_csv(executable: str, columns: tuple[str, ...], rows: int):
    proc = subprocess.run([str(Path(executable).resolve())], capture_output=True,
                          text=True, check=True, timeout=60)
    reader = csv.DictReader(io.StringIO(proc.stdout))
    require(tuple(reader.fieldnames or ()) == columns,
            f"Unexpected CSV header: {reader.fieldnames}; expected {columns}")
    records = list(reader)
    require(len(records) == rows,
            f"Expected {rows} records, found {len(records)}")
    result = []
    for index, record in enumerate(records):
        parsed = {}
        for key in columns:
            try:
                value = float(record[key])
            except (ValueError, TypeError, KeyError) as exc:
                raise AssertionError(f"Invalid {key} at row {index}") from exc
            require(math.isfinite(value), f"Non-finite {key} at row {index}")
            parsed[key] = value
        result.append(parsed)
    xs = [r[columns[0]] for r in result]
    require(all(x1 < x2 for x1, x2 in zip(xs, xs[1:])),
            f"Independent variable {columns[0]} is not strictly increasing")
    return result


def success(message: str) -> None:
    print(f"PASS: {message}")
