#==============================================================================
# Name        : plot.py
# Authors     : Joao Flavio Vasconcellos
# Version     : 2.0.0
# Description : Plot Elovich compiled uptake, derivative and sensitivities.
#
# Copyright   : Copyright (C) 2026 Joao Flavio Vasconcellos
# Licence     : MIT License (see LICENSE.txt)
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


def draw_sensitivities(path, xs, lambda_qref, lambda_ke):
    """Plot only values calculated by C++ model perturbations."""
    fig, ax = plt.subplots(figsize=(7.6, 4.3), layout="constrained")
    ax.plot(xs, lambda_qref, color="#087F82", linewidth=2.4,
            label=r"$\lambda_{q_{\mathrm{ref}}}$")
    ax.plot(xs, lambda_ke, color="#DF735F", linewidth=2.4,
            label=r"$\lambda_{K_E}$")
    ax.set(xlabel=r"Equilibrium concentration, $C_e$",
           ylabel=r"Scaled sensitivity, $\lambda_i$",
           title="Elovich — numerical parameter sensitivities")
    ax.grid(True, alpha=0.20)
    ax.legend(frameon=False)
    ax.spines[["right", "top"]].set_visible(False)
    fig.savefig(path, format="svg", transparent=True, metadata={"Date": None})
    plt.close(fig)


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("executable", type=Path)
    parser.add_argument("--output", type=Path, required=True)
    arguments = parser.parse_args()
    arguments.output.mkdir(parents=True, exist_ok=True)
    raw = subprocess.run([str(arguments.executable.resolve())], check=True,
                         capture_output=True, text=True).stdout
    records = list(csv.DictReader(io.StringIO(raw)))
    expected = {"concentration", "uptake", "derivative", "sensitivity_qref", "sensitivity_ke"}
    if len(records) != 81 or set(records[0]) != expected:
        raise ValueError("Unexpected compiled Elovich CSV data")
    data = {key: [float(row[key]) for row in records] for key in expected}
    if not all(math.isfinite(v) for series in data.values() for v in series):
        raise ValueError("Non-finite compiled Elovich data")
    prefix = "elovich"
    (arguments.output / f"{prefix}_data.csv").write_text(raw, encoding="utf-8")
    xs = data["concentration"]
    draw(arguments.output / f"{prefix}_curve.svg", xs, data["uptake"],
         r"Equilibrium concentration, $C_e$", r"Equilibrium uptake, $q_e$",
         "Elovich isotherm plot")
    draw(arguments.output / f"{prefix}_derivative.svg", xs, data["derivative"],
         r"Equilibrium concentration, $C_e$",
         r"Concentration derivative, $\mathrm{d}q_e/\mathrm{d}C_e$",
         "Elovich — concentration derivative")
    draw_sensitivities(arguments.output / f"{prefix}_sensitivity.svg", xs,
                       data["sensitivity_qref"], data["sensitivity_ke"])


if __name__ == "__main__":
    main()
