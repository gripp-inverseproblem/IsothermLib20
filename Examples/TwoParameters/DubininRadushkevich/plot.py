#==============================================================================
# Name        : plot.py
# Authors     : Joao Flavio Vasconcellos
# Version     : 2.0.0
# Description : Plot DubininRadushkevich compiled CSV data and linearised identity.
#
# Copyright   : Copyright (C) 2026 Joao Flavio Vasconcellos
# Licence     : MIT License (see LICENSE)
# SPDX-License-Identifier: MIT
#==============================================================================

#==============================================================================
# Python standard library imports
#==============================================================================
import argparse
import csv
import io
import math
import subprocess
from pathlib import Path
#==============================================================================
# Third-party library imports
#==============================================================================
import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt

matplotlib.rcParams["svg.hashsalt"] = "isothermpp-scientific-figures"


def draw(path, xs, ys, xlab, ylab, title):
    """Render vector graphics without recalculating the isotherm in Python."""
    fig, ax = plt.subplots(figsize=(7.6, 4.3), layout="constrained")
    ax.plot(xs, ys, color="#087F82", linewidth=2.5)
    ax.scatter(xs[::10], ys[::10], color="#DF735F", s=16, zorder=3)
    ax.set(xlabel=xlab, ylabel=ylab, title=title)
    ax.grid(True, alpha=0.20)
    ax.spines[["right", "top"]].set_visible(False)
    fig.savefig(path, format="svg", transparent=True, metadata={"Date": None})
    plt.close(fig)


def draw_sensitivities(path, concentrations, lambda_qs, lambda_beta):
    """Only render sensitivities already calculated by the compiled C++ class."""
    fig, ax = plt.subplots(figsize=(7.6, 4.3), layout="constrained")
    ax.plot(concentrations, lambda_qs, color="#087F82", linewidth=2.4,
            label=r"$\lambda_{q_s}$")
    ax.plot(concentrations, lambda_beta, color="#DF735F", linewidth=2.4,
            label=r"$\lambda_{\beta}$")
    ax.set(xlabel=r"Equilibrium concentration, $C_e$ (dimensionless)",
           ylabel=r"Scaled sensitivity, $\lambda_i$",
           title="Dubinin–Radushkevich — numerical parameter sensitivities")
    ax.legend(frameon=False)
    ax.grid(True, alpha=0.20)
    ax.spines[["right", "top"]].set_visible(False)
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
    expected = {"concentration", "uptake", "derivative", "sensitivity_qs", "sensitivity_beta"}
    if len(records) != 81 or set(records[0]) != expected:
        raise ValueError("Unexpected Dubinin–Radushkevich compiled CSV data")
    xx = [float(row["concentration"]) for row in records]
    qq = [float(row["uptake"]) for row in records]
    dy = [float(row["derivative"]) for row in records]
    sq = [float(row["sensitivity_qs"]) for row in records]
    sb = [float(row["sensitivity_beta"]) for row in records]
    if not all(math.isfinite(z) for z in xx + qq + dy + sq + sb):
        raise ValueError("Non-finite compiled Dubinin–Radushkevich data")
    prefix = "dubinin_radushkevich"
    (args.output / f"{prefix}_data.csv").write_text(raw, encoding="utf-8")
    draw(args.output / f"{prefix}_curve.svg", xx, qq,
         r"Equilibrium concentration, $C_e$ (dimensionless)",
         r"Equilibrium uptake, $q_e$", "Dubinin–Radushkevich isotherm plot")
    draw(args.output / f"{prefix}_derivative.svg", xx, dy,
         r"Equilibrium concentration, $C_e$ (dimensionless)",
         r"Concentration derivative, $\mathrm{d}q_e/\mathrm{d}C_e$",
         "Dubinin–Radushkevich — concentration derivative")
    draw_sensitivities(args.output / f"{prefix}_sensitivity.svg", xx, sq, sb)


if __name__ == "__main__":
    main()
