#==============================================================================
# Name        : plot.py
# Authors     : Joao Flavio Vasconcellos
# Version     : 2.0.0
# Description : Scientific SVG figures from compiled Anti-Langmuir CSV output.
#
# Copyright   : Copyright (C) 2026 Joao Flavio Vasconcellos
# Licence     : MIT License (see LICENSE)
# SPDX-License-Identifier: MIT
#==============================================================================

import argparse
import csv
import io
import math
import subprocess
from pathlib import Path

import matplotlib
matplotlib.use("Agg")
from matplotlib import pyplot as plt

# Keep generated SVG identifiers deterministic across repeated builds.
matplotlib.rcParams["svg.hashsalt"] = "isothermpp-scientific-figures"


def render_chart(path, xx, yy, ylabel, title):
    """Plot independently compiled output; never re-evaluate the C++ formula."""
    fig, ax = plt.subplots(figsize=(7.6, 4.3), layout="constrained")
    ax.plot(xx, yy, color="#087F82", linewidth=2.3)
    ax.scatter(xx[::10], yy[::10], color="#DF735F", s=17, zorder=3)
    ax.set(xlabel="Equilibrium concentration", ylabel=ylabel, title=title)
    ax.grid(True, alpha=.20)
    ax.spines[["top", "right"]].set_visible(False)
    fig.savefig(path, format="svg", transparent=True, metadata={"Date": None})
    plt.close(fig)


def render_sensitivities(path, xx, lambda_h, lambda_b):
    """Compare both numerical sensitivities produced by the C++ model."""
    fig, ax = plt.subplots(figsize=(7.6, 4.3), layout="constrained")
    ax.plot(xx, lambda_h, label=r"$\lambda_H$", color="#087F82", linewidth=2.3)
    ax.plot(xx, lambda_b, label=r"$\lambda_b$", color="#DF735F", linewidth=2.3)
    ax.set(xlabel=r"Equilibrium concentration, $C_e$",
           ylabel=r"Scaled sensitivity, $\lambda_i$",
           title="Anti-Langmuir — numerical parameter sensitivities")
    ax.grid(True, alpha=.20)
    ax.legend(frameon=False)
    ax.spines[["top", "right"]].set_visible(False)
    fig.savefig(path, format="svg", transparent=True, metadata={"Date": None})
    plt.close(fig)


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("executable", type=Path)
    parser.add_argument("--output", type=Path, required=True)
    args = parser.parse_args()
    args.output.mkdir(parents=True, exist_ok=True)
    raw = subprocess.run([str(args.executable.resolve())], check=True,
                         capture_output=True, text=True).stdout
    records = list(csv.DictReader(io.StringIO(raw)))
    expected = {"concentration", "uptake", "derivative", "sensitivity_h", "sensitivity_b"}
    if len(records) != 81 or set(records[0]) != expected:
        raise ValueError("Unexpected compiled Anti-Langmuir CSV data")
    xx = [float(row["concentration"]) for row in records]
    yy = [float(row["uptake"]) for row in records]
    dy = [float(row["derivative"]) for row in records]
    sh = [float(row["sensitivity_h"]) for row in records]
    sb = [float(row["sensitivity_b"]) for row in records]
    if not all(math.isfinite(z) for z in xx + yy + dy + sh + sb):
        raise ValueError("Non-finite compiled figure data")
    prefix = "anti_langmuir"
    (args.output / f"{prefix}_data.csv").write_text(raw, encoding="utf-8")
    render_chart(args.output / f"{prefix}_curve.svg", xx, yy,
                 "Equilibrium uptake", "Anti-Langmuir isotherm plot")
    render_chart(args.output / f"{prefix}_derivative.svg", xx, dy,
                 r"$\mathrm{d}q_e/\mathrm{d}C_e$", "Anti-Langmuir — concentration derivative")
    render_sensitivities(args.output / f"{prefix}_sensitivity.svg", xx, sh, sb)


if __name__ == "__main__":
    main()
