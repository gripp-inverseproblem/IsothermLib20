#==============================================================================
# Name        : plot.py
# Authors     : Joao Flavio Vasconcellos
# Version     : 2.0.0
# Description : Plot C++20 results for four-parameter extended Oswin.
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
import matplotlib.pyplot as plt

matplotlib.rcParams["svg.hashsalt"] = "isothermpp-scientific-figures"


def plot_single(path, x, y, xlabel, ylabel):
    fig, axis = plt.subplots(figsize=(7.6, 4.3), layout="constrained")
    axis.plot(x, y, linewidth=2.4)
    axis.set(xlabel=xlabel, ylabel=ylabel)
    axis.grid(True, alpha=0.20)
    fig.savefig(path, format="svg", metadata={"Date": None})
    plt.close(fig)


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("executable", type=Path)
    parser.add_argument("--output", type=Path, required=True)
    args = parser.parse_args()
    args.output.mkdir(parents=True, exist_ok=True)
    raw = subprocess.run([str(args.executable.resolve())], check=True,
                         capture_output=True, text=True).stdout
    rows = list(csv.DictReader(io.StringIO(raw)))
    keys = ["activity", "uptake", "derivative", "sensitivity_1",
            "sensitivity_2", "sensitivity_3", "sensitivity_4"]
    if len(rows) != 81 or list(rows[0]) != keys:
        raise ValueError("Unexpected four-parameter Oswin C++ CSV format")
    data = {key: [float(row[key]) for row in rows] for key in keys}
    if not all(math.isfinite(value) for series in data.values() for value in series):
        raise ValueError("Non-finite C++ model data")
    (args.output / "oswin_data.csv").write_text(raw, encoding="utf-8")
    x = data["activity"]
    xlabel = r"Water activity, $a_w$ (dimensionless)"
    plot_single(args.output / "oswin_curve.svg", x, data["uptake"],
                xlabel, r"Equilibrium moisture, $q_e$")
    plot_single(args.output / "oswin_derivative.svg", x, data["derivative"],
                xlabel, r"$\mathrm{d}q_e/\mathrm{d}a_w$")
    fig, axis = plt.subplots(figsize=(7.6, 4.3), layout="constrained")
    for number, label in enumerate((r"$\lambda_A$", r"$\lambda_B$",
                                    r"$\lambda_C$", r"$\lambda_D$"), 1):
        axis.plot(x, data[f"sensitivity_{number}"], label=label, linewidth=2.2)
    axis.set(xlabel=xlabel, ylabel=r"Scaled sensitivity, $\lambda_i$")
    axis.legend(frameon=False)
    axis.grid(True, alpha=0.20)
    fig.savefig(args.output / "oswin_sensitivity.svg", format="svg",
                metadata={"Date": None})
    plt.close(fig)


if __name__ == "__main__":
    main()
