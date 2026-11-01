#==============================================================================
# Name        : plot.py
# Authors     : Joao Flavio Vasconcellos
# Version     : 2.0.0
# Description : Plot Temkin compiled CSV data and linearised identity.
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
    fig.savefig(path, transparent=True, metadata={"Date": None})
    plt.close(fig)


def draw_sensitivities(path, concentrations, lambda_kt, lambda_bt):
    """Plot numerical sensitivities provided by the compiled C++ example."""
    fig, ax = plt.subplots(figsize=(7.6, 4.3), layout="constrained")
    ax.plot(concentrations, lambda_kt, label=r"$\lambda_{K_T}$", linewidth=2.4)
    ax.plot(concentrations, lambda_bt, label=r"$\lambda_{b_T}$", linewidth=2.4)
    ax.set(xlabel=r"Equilibrium concentration, $C_e$",
           ylabel=r"Scaled sensitivity, $\lambda_i$",
           title="Temkin — numerical parameter sensitivities")
    ax.legend(frameon=False)
    ax.grid(True, alpha=0.20)
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
    expected = {"concentration", "uptake", "derivative", "sensitivity_kt", "sensitivity_bt"}
    if len(records) != 81 or set(records[0]) != expected:
        raise ValueError("Unexpected compiled Temkin CSV values")
    prefix = "temkin"
    (arguments.output / (prefix + "_data.csv")).write_text(raw, encoding="utf-8")
    xs = [float(row["concentration"]) for row in records]
    qs = [float(row["uptake"]) for row in records]
    sk = [float(row["sensitivity_kt"]) for row in records]
    sb = [float(row["sensitivity_bt"]) for row in records]
    if not all(math.isfinite(z) for z in xs + qs + sk + sb):
        raise ValueError("Non-finite compiled Temkin figure data")
    draw(arguments.output / (prefix + "_curve.svg"), xs, qs,
         "Equilibrium concentration", "Equilibrium uptake", "Temkin isotherm")
    pairs = [(c,q) for c,q in zip(xs,qs) if c>0 and q>0 and math.isfinite(c) and math.isfinite(q)]
    xx = [math.log(c) for c,q in pairs]
    yy = [q for c,q in pairs]
    draw(arguments.output / (prefix + "_linear.svg"), xx, yy,
         r"$\ln C_e$", r"$q_e$", "Temkin linearised identity")
    draw_sensitivities(arguments.output / (prefix + "_sensitivity.svg"), xs, sk, sb)

if __name__ == "__main__":
    main()
