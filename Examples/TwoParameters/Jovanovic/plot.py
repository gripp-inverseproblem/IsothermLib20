#==============================================================================
# Name        : plot.py
# Authors     : Joao Flavio Vasconcellos
# Version     : 2.0.0
# Description : Plot C++20 model outputs without reimplementing any isotherm or sensitivity.
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
import matplotlib.pyplot as plt

matplotlib.rcParams["svg.hashsalt"] = "isothermpp-scientific-figures"


def chart(path, xs, ys, xlab, ylab, title):
    fig, axis = plt.subplots(figsize=(7.6, 4.3), layout="constrained")
    axis.plot(xs, ys, color="#087F82", linewidth=2.4)
    axis.scatter(xs[::10], ys[::10], s=16, color="#DF735F", zorder=3)
    axis.set(xlabel=xlab, ylabel=ylab, title=title)
    axis.grid(True, alpha=0.20)
    axis.spines[["top", "right"]].set_visible(False)
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
    expected = {"concentration", "uptake", "derivative", "sensitivity_1", "sensitivity_2"}
    if len(records) != 81 or set(records[0]) != expected:
        raise ValueError("Unexpected model CSV schema or number of rows")
    d = {key: [float(row[key]) for row in records] for key in expected}
    if not all(math.isfinite(v) for series in d.values() for v in series):
        raise ValueError("Non-finite compiled C++ model data")
    xs = d["concentration"]
    (args.output / "jovanovic_data.csv").write_text(raw, encoding="utf-8")
    chart(args.output / "jovanovic_curve.svg", xs, d["uptake"],
          "Equilibrium concentration, $C_e$", "Equilibrium uptake, $q_e$", "Jovanovic isotherm plot")
    chart(args.output / "jovanovic_derivative.svg", xs, d["derivative"],
          "Equilibrium concentration, $C_e$", r"$\mathrm{d}q_e/\mathrm{d}C_e$", "Jovanovic — concentration derivative")
    fig, axis = plt.subplots(figsize=(7.6, 4.3), layout="constrained")
    axis.plot(xs, d["sensitivity_1"], linewidth=2.3, color="#087F82", label=r"$\lambda_{q_{\mathrm{sat}}}$")
    axis.plot(xs, d["sensitivity_2"], linewidth=2.3, color="#DF735F", label=r"$\lambda_{K}$")
    axis.set(xlabel="Equilibrium concentration, $C_e$", ylabel=r"Scaled sensitivity, $\lambda_i$",
             title="Jovanovic — numerical parameter sensitivities")
    axis.grid(True, alpha=0.20)
    axis.legend(frameon=False)
    axis.spines[["top", "right"]].set_visible(False)
    fig.savefig(args.output / "jovanovic_sensitivity.svg", format="svg",
                transparent=True, metadata={"Date": None})
    plt.close(fig)


if __name__ == "__main__":
    main()
