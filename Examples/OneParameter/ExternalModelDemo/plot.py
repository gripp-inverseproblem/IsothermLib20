#==============================================================================
# Name        : plot.py
# Authors     : Joao Flavio Vasconcellos
# Version     : 2.0.0
# Description : Generate the example CSV and SVG graph.
#
# Copyright   : Copyright (C) 2026 Joao Flavio Vasconcellos
# Licence     : MIT License (see LICENSE)
# SPDX-License-Identifier: MIT
#==============================================================================

"""Plot the external C++ model contract demonstration (not a catalogue model)."""
from __future__ import annotations
import argparse
import csv
import io
import subprocess
from pathlib import Path
import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("executable", type=Path)
    parser.add_argument("--output", type=Path, default=Path("output"))
    args = parser.parse_args()
    result = subprocess.run([str(args.executable.resolve())], capture_output=True, text=True, check=True)
    args.output.mkdir(parents=True, exist_ok=True)
    (args.output / "contract_demo.csv").write_text(result.stdout, encoding="utf-8")
    data = list(csv.DictReader(io.StringIO(result.stdout)))
    fig, ax = plt.subplots(figsize=(6.6, 4.2), layout="constrained")
    ax.plot([float(r["concentration"]) for r in data],
            [float(r["uptake"]) for r in data], linewidth=2.2)
    ax.set(xlabel="Concentration (illustrative units)",
           ylabel="Uptake (illustrative units)",
           title="External model contract demonstration")
    ax.grid(alpha=0.25)
    fig.savefig(args.output / "contract_demo.svg")
    plt.close(fig)

if __name__ == "__main__":
    main()
