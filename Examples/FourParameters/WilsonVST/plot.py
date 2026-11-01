#==============================================================================
# Name        : plot.py
# Authors     : Joao Flavio Vasconcellos
# Version     : 2.0.0
# Description : Scientific SVG figures from compiled WilsonVST CSV output.
#
# Copyright   : Copyright (C) 2026 Joao Flavio Vasconcellos
# Licence     : MIT License (see LICENSE.txt)
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


def render_chart(path, xx, yy, ylabel, title):
    """Plot independently compiled output; never re-evaluate the C++ formula."""
    fig, ax = plt.subplots(figsize=(7.6, 4.3), layout="constrained")
    ax.plot(xx, yy, color="#087F82", linewidth=2.3)
    ax.scatter(xx[::10], yy[::10], color="#DF735F", s=17, zorder=3)
    ax.set(xlabel="Equilibrium concentration", ylabel=ylabel, title=title)
    ax.grid(True, alpha=.20)
    ax.spines[["top", "right"]].set_visible(False)
    fig.savefig(path, format="svg", transparent=True)
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
    if len(records) != 81:
        raise ValueError("Expected exactly 81 compiled model observations")
    xx = [float(row["concentration"]) for row in records]
    yy = [float(row["uptake"]) for row in records]
    dy = [float(row["derivative"]) for row in records]
    scaled = [[float(row[f"sensitivity_{j}"]) for row in records] for j in range(1, 4 + 1)]
    if not all(math.isfinite(z) for z in xx + yy + dy + sum(scaled, [])):
        raise ValueError("Non-finite compiled figure data")
    prefix = "wilson_v_s_t"
    (args.output / f"{prefix}_data.csv").write_text(raw, encoding="utf-8")
    render_chart(args.output / f"{prefix}_curve.svg", xx, yy,
                 "Equilibrium uptake", "WilsonVST isotherm")
    render_chart(args.output / f"{prefix}_derivative.svg", xx, dy,
                 "dq/dc", "WilsonVST: concentration derivative")

    fig, ax = plt.subplots(figsize=(7.6, 4.6), layout="constrained")
    for values, label in zip(scaled, ['q_max', 'K_H', 'L1v', 'Lv1']):
        ax.plot(xx, values, linewidth=2.0, label=r"$\lambda_{" + (label.split("_", 1)[0] + "_{" + label.split("_", 1)[1] + "}" if "_" in label else label) + "}$")
    ax.set(xlabel="Equilibrium concentration", ylabel=r"Scaled sensitivity, $\lambda_i$",
           title="WilsonVST numerical parameter sensitivities")
    ax.legend(frameon=False, ncol=2, fontsize=9)
    ax.grid(True, alpha=.20)
    ax.spines[["top", "right"]].set_visible(False)
    fig.savefig(args.output / f"{prefix}_sensitivity.svg", format="svg", transparent=True)
    plt.close(fig)


if __name__ == "__main__":
    main()
