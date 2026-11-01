#==============================================================================
# Name        : plot.py
# Authors     : Joao Flavio Vasconcellos
# Version     : 2.0.0
# Description : Plot the CSV files of the FixedBed example (Fig. 2 of the paper).
# Copyright   : Copyright (C) 2026 Joao Flavio Vasconcellos
# Licence     : MIT License (see LICENSE.txt)
# SPDX-License-Identifier: MIT
#==============================================================================

"""Draw Fig. 2 from the output of example_Usage_FixedBed.

Usage:
    example_Usage_FixedBed out/
    python plot.py out/ --output fig2.pdf

No isotherm is evaluated here: every curve is read from the CSV files
written by the compiled C++ example.
"""

import argparse
import csv
from pathlib import Path

import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt

matplotlib.rcParams["svg.hashsalt"] = "isothermpp-fixed-bed"

MODELS = ["Langmuir", "Sips", "Toth", "Redlich-Peterson"]
LABEL = {"Langmuir": "Langmuir", "Sips": "Sips", "Toth": "Tóth",
         "Redlich-Peterson": "Redlich–Peterson"}
COLOR = {"Langmuir": "#4655B0", "Sips": "#D9644C", "Toth": "#0A9396",
         "Redlich-Peterson": "#8A6A0A"}
STYLE = {"Langmuir": "-", "Sips": "--", "Toth": "-.", "Redlich-Peterson": (0, (1, 1.2))}
INK, MUTED, GRID = "#1f2330", "#5b6070", "#e3e5ea"


def read(path):
    with open(path, encoding="utf-8") as handle:
        rows = list(csv.reader(handle))
    header = rows[0]
    columns = {name: [] for name in header}
    for row in rows[1:]:
        for name, value in zip(header, row):
            columns[name].append(float(value))
    return columns


def tag(axis, text):
    axis.text(-0.02, 1.04, text, transform=axis.transAxes, fontsize=9,
              fontweight="bold", color=INK, ha="right", va="bottom")


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("directory", type=Path, help="directory with the CSV files")
    parser.add_argument("--output", type=Path, default=Path("fig2.pdf"))
    args = parser.parse_args()

    data = read(args.directory / "fig2_data.csv")
    isotherms = read(args.directory / "fig2_isotherms.csv")
    slopes = read(args.directory / "fig2_slopes.csv")
    loading = read(args.directory / "fig2_loading.csv")
    pulse = read(args.directory / "fig2_pulse.csv")

    plt.rcParams.update({
        "font.size": 8, "axes.labelsize": 8, "xtick.labelsize": 7, "ytick.labelsize": 7,
        "axes.edgecolor": MUTED, "axes.labelcolor": INK, "xtick.color": MUTED,
        "ytick.color": MUTED, "axes.spines.top": False, "axes.spines.right": False,
        "axes.linewidth": 0.6, "legend.fontsize": 7, "legend.frameon": False,
        "axes.grid": True, "grid.color": GRID, "grid.linewidth": 0.5,
        "lines.linewidth": 1.5,
    })
    cm = 1 / 2.54
    fig, ax = plt.subplots(2, 2, figsize=(17 * cm, 11 * cm), constrained_layout=True)

    axis = ax[0, 0]
    for name in MODELS:
        axis.plot(isotherms["concentration"], isotherms[name], ls=STYLE[name],
                  color=COLOR[name], label=LABEL[name])
    axis.plot(data["concentration"], data["uptake"], "o", ms=4.5, mfc="white",
              mec=INK, mew=0.9, label="data", zorder=5)
    axis.set(xlabel=r"$C$ (mg L$^{-1}$)", ylabel=r"$q$ (mg g$^{-1}$)", xlim=(0, 220))
    axis.set_ylim(bottom=0)
    axis.legend(loc="lower right", handlelength=2.6)

    axis = ax[0, 1]
    for name in MODELS:
        axis.loglog(slopes["concentration"], slopes[name], ls=STYLE[name], color=COLOR[name])
    axis.axvspan(min(data["concentration"]), max(data["concentration"]),
                 color="#E6EAF5", zorder=0, lw=0)
    axis.text(14, 0.03, "data range", ha="center", fontsize=6.5, color=MUTED)
    axis.set(xlabel=r"$C$ (mg L$^{-1}$)", ylabel=r"$\mathrm{d}q/\mathrm{d}C$ (L g$^{-1}$)",
             xlim=(1e-2, 250), ylim=(1e-2, 20))

    axis = ax[1, 0]
    for name in MODELS:
        values = [max(v, 1e-6) for v in loading[name]]
        axis.semilogy(loading["time_h"], values, ls=STYLE[name], color=COLOR[name])
    axis.axvline(100, color=MUTED, lw=0.7, ls=":")
    axis.text(105, 0.3, "feed\nstopped", fontsize=6.5, color=MUTED, va="center")
    axis.set(xlabel=r"$t$ (h)", ylabel=r"$C_{\mathrm{out}}/C_0$", xlim=(0, 400), ylim=(1e-3, 1.5))
    axis.set_title("loading and elution", fontsize=7.5, color=MUTED, loc="left")

    axis = ax[1, 1]
    for name in MODELS:
        axis.plot(pulse["time_h"], pulse[name], ls=STYLE[name], color=COLOR[name])
    axis.set(xlabel=r"$t$ (h)", ylabel=r"$C_{\mathrm{out}}/C_{\mathrm{inj}}$", xlim=(0, 800))
    axis.set_ylim(bottom=0)
    axis.set_title("pulse injection", fontsize=7.5, color=MUTED, loc="left")

    for axis, letter in zip(ax.flat, "abcd"):
        tag(axis, f"({letter})")
    fig.savefig(args.output, metadata={"CreationDate": None} if args.output.suffix == ".pdf" else {"Date": None})
    print(f"Figure written to {args.output}")


if __name__ == "__main__":
    main()
