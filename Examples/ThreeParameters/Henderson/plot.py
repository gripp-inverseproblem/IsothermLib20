#==============================================================================
# Name        : plot.py
# Authors     : Joao Flavio Vasconcellos
# Version     : 2.0.0
# Description : Plot Henderson model C++20 temperature-response CSV and parameter sensitivities.
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
import matplotlib.pyplot as plt


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("executable", type=Path)
    parser.add_argument("--output", type=Path, required=True)
    args = parser.parse_args()
    args.output.mkdir(parents=True, exist_ok=True)
    raw = subprocess.run([str(args.executable.resolve())], check=True,
                         capture_output=True, text=True).stdout
    rows = list(csv.DictReader(io.StringIO(raw)))
    expected = ["temperature_k", "uptake", "derivative",
                "sensitivity_1", "sensitivity_2", "sensitivity_3"]
    if len(rows) != 81 or list(rows[0]) != expected:
        raise ValueError("Unexpected Henderson C++ output schema")
    data = {key: [float(row[key]) for row in rows] for key in expected}
    if not all(math.isfinite(v) for series in data.values() for v in series):
        raise ValueError("Non-finite C++ model output")
    args.output.joinpath("henderson_data.csv").write_text(raw, encoding="utf-8")
    x = data["temperature_k"]
    for name, y, ylab, title in (
        ("curve", data["uptake"], r"Equilibrium moisture, $q_e$", "Henderson isotherm plot"),
        ("derivative", data["derivative"], r"$\mathrm{d}q_e/\mathrm{d}T$", "Henderson — temperature derivative"),
    ):
        fig, ax = plt.subplots(figsize=(7.6, 4.3), layout="constrained")
        ax.plot(x, y, linewidth=2.4)
        ax.set(xlabel="Absolute temperature, $T$ (K)", ylabel=ylab, title=title)
        ax.grid(alpha=0.2)
        fig.savefig(args.output / f"henderson_{name}.svg", metadata={"Date": None})
        plt.close(fig)
    fig, ax = plt.subplots(figsize=(7.6, 4.3), layout="constrained")
    for index, label in enumerate((r"$\lambda_A$", r"$\lambda_B$", r"$\lambda_{a_w}$"), 1):
        ax.plot(x, data[f"sensitivity_{index}"], label=label, linewidth=2.2)
    ax.set(xlabel="Absolute temperature, $T$ (K)", ylabel=r"Scaled sensitivity, $\lambda_i$",
           title="Henderson — numerical parameter sensitivities")
    ax.legend(frameon=False)
    ax.grid(alpha=0.2)
    fig.savefig(args.output / "henderson_sensitivity.svg", metadata={"Date": None})
    plt.close(fig)


if __name__ == "__main__":
    main()
