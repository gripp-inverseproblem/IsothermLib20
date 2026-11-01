#!/usr/bin/env python3
#==============================================================================
# Name        : build_print_pdf.py
# Authors     : Joao Flavio Vasconcellos
# Version     : 2.0.0
# Description : Compose the Sphinx HTML pages into an A4 CSS-rendered PDF.
#
# Copyright   : Copyright (C) 2026 Joao Flavio Vasconcellos
# Licence     : MIT License (see LICENSE.txt)
# SPDX-License-Identifier: MIT
#==============================================================================

"""Create a vector-quality A4 PDF using the approved HTML/CSS workflow.

The Sphinx RST corpus is the only editorial source. Sphinx generates HTML;
this script assembles that HTML, replaces MathJax-only formula markup with
locally generated SVGs so WeasyPrint can render mathematics offline, and
recomposes each model page as a compact data sheet: the uptake, derivative
and sensitivity plots are redrawn from the CSV files written by the compiled
examples as one strip of three small multiples.
"""

from __future__ import annotations

#==============================================================================
# Python standard library imports
#==============================================================================

import argparse
import csv
import hashlib
import html
import math
import re
from dataclasses import dataclass
from datetime import date
from pathlib import Path
from urllib.parse import urlparse, unquote

#==============================================================================
# Third-party library imports
#==============================================================================

from bs4 import BeautifulSoup
import matplotlib
matplotlib.use("Agg")
from matplotlib import font_manager
from matplotlib import pyplot as plt
from matplotlib import rcParams
from weasyprint import HTML


STYLE_VERSION = "datasheet-1"
OMIT_PAGES = {"search.html", "genindex.html", "py-modindex.html", "404.html"}
CORE_CHAPTERS = (
    "index.html",
    "scientific_catalogue/index.html",
    "user_guide/index.html",
    "architecture/index.html",
    "developers/index.html",
    "verification/index.html",
)
GROUPS = {
    "OneParameter": (1, "One parameter"),
    "TwoParameters": (2, "Two parameters"),
    "ThreeParameters": (3, "Three parameters"),
    "FourParameters": (4, "Four parameters"),
    "FiveParameters": (5, "Five parameters"),
    "SixParameters": (6, "Six parameters"),
}
MODEL_PAGE = re.compile(r"scientific_catalogue/models/([^/]+)/([^/]+)\.html")

INK = "#1D2347"
INDIGO = "#27316A"
TURQUOISE = "#087F82"
CORAL = "#DF735F"
MUTED = "#6B7390"
RULE = "#D9DDEA"
SERIES = ["#27316A", "#DF735F", "#087F82", "#B5892A", "#7A5BA8", "#5B6B7F"]
STYLES = ["-", "--", "-.", (0, (1, 1.3)), (0, (4, 1.5, 1, 1.5, 1, 1.5)), (0, (6, 2))]


#==============================================================================
# Fonts
#==============================================================================

def register_fonts(static: Path) -> None:
    """Register the bundled IBM Plex fonts with Matplotlib (SIL OFL 1.1)."""
    fonts = static / "fonts"
    for path in sorted(fonts.glob("*.ttf")):
        font_manager.fontManager.addfont(str(path))
    rcParams.update({
        "font.family": "IBM Plex Sans",
        "mathtext.fontset": "custom",
        "mathtext.rm": "IBM Plex Sans",
        "mathtext.it": "IBM Plex Sans:italic",
        "mathtext.bf": "IBM Plex Sans:bold",
        "mathtext.sf": "IBM Plex Sans",
        "mathtext.fallback": "stixsans",
        "svg.fonttype": "path",
    })


#==============================================================================
# Page selection
#==============================================================================

def ordered_pages(root: Path, source_root: Path) -> list[Path]:
    """Select published pages corresponding to authored reStructuredText files.

    PyData and other Sphinx themes may place HTML templates in ``_static``.
    Those files are browser assets rather than scientific manual chapters.
    Deriving the page list from the RST sources avoids treating theme assets,
    search pages or generated auxiliary HTML as editorial material.
    """
    if not root.is_dir():
        raise FileNotFoundError(f"Sphinx HTML directory is missing: {root}")
    if not source_root.is_dir():
        raise FileNotFoundError(f"Sphinx source directory is missing: {source_root}")

    paths: list[Path] = []
    for source in source_root.rglob("*.rst"):
        relative = source.relative_to(source_root)
        # Sphinx's internal and generated directories contain no chapters.
        if any(part.startswith("_") or part.startswith(".") for part in relative.parts):
            continue
        if relative.with_suffix(".html").name in OMIT_PAGES:
            continue
        page = root / relative.with_suffix(".html")
        if not page.is_file():
            raise FileNotFoundError(
                f"Sphinx output missing for documentation source {relative}: {page}"
            )
        paths.append(page)

    if (root / "index.html") not in paths:
        raise FileNotFoundError(f"Sphinx root index.rst/index.html missing: {source_root}")

    # Match the HTML catalogue: parameter groups, followed by model names
    # in alphabetical order (case-insensitive). Never use incidental file order.
    def rank(page: Path) -> tuple[int, int, str]:
        relative = page.relative_to(root).as_posix()
        if relative == "index.html":
            return 0, 0, ""
        if relative == "scientific_catalogue/index.html":
            return 1, 0, ""
        match = MODEL_PAGE.fullmatch(relative)
        if match:
            group, model = match.groups()
            if group not in GROUPS:
                raise ValueError(f"Unexpected model parameter group: {relative}")
            return 2, GROUPS[group][0], model.casefold()
        for index, other in enumerate(CORE_CHAPTERS[2:], start=3):
            if relative == other:
                return index, 0, ""
        return 8, 0, relative.casefold()

    return sorted(paths, key=rank)


#==============================================================================
# Mathematics
#==============================================================================

def normalise_latex(text: str) -> str:
    """Recover the TeX expression from the Sphinx/MathJax wrappers."""
    cleaned = text.strip()
    for prefix, suffix in ((r"\(", r"\)"), (r"\[", r"\]"), ("$$", "$$")):
        if cleaned.startswith(prefix) and cleaned.endswith(suffix):
            cleaned = cleaned[len(prefix):-len(suffix)]
            break
    cleaned = re.sub(r"\\label\{[^}]+\}", "", cleaned)
    # Matplotlib mathtext accepts \leq and \geq, not TeX aliases \le and \ge.
    cleaned = re.sub(r"\\le(?![A-Za-z])", r"\\leq", cleaned)
    cleaned = re.sub(r"\\ge(?![A-Za-z])", r"\\geq", cleaned)
    return " ".join(cleaned.split())


def render_equation(expression: str, destination: Path, display: bool,
                    size: float | None = None) -> Path:
    """Render MathJax TeX as an SVG; fail loudly on unsupported mathematics."""
    key = f"{STYLE_VERSION}|{display}|{size}|{expression}"
    fingerprint = hashlib.sha256(key.encode("utf-8")).hexdigest()[:20]
    path = destination / f"formula_{fingerprint}.svg"
    if path.exists():
        return path

    # Mathtext is a vector renderer; the SVG keeps its intrinsic size, so
    # formulae are typeset at a size matched to the body text.
    fig = plt.figure(figsize=(11, 1.2))
    try:
        fig.patch.set_alpha(0)
        fontsize = size if size is not None else (11.5 if display else 9.2)
        fig.text(0, 0.55, f"${expression}$", fontsize=fontsize,
                 va="center", ha="left", color=INK)
        fig.savefig(path, format="svg", transparent=True,
                    bbox_inches="tight", pad_inches=0.01)
    except Exception as exc:
        if path.exists():
            path.unlink()
        raise ValueError(
            f"Unable to convert formula to vector SVG: {expression!r}; {exc}"
        ) from exc
    finally:
        plt.close(fig)
    return path


def replace_math(article: BeautifulSoup, assets: Path) -> int:
    """Make formulas visible to WeasyPrint, which does not run MathJax JS."""
    nodes = list(article.select(".math"))
    converted = 0
    # Process leaf maths first to avoid replacing an outer wrapper twice.
    for node in reversed(nodes):
        if node.find(class_="math") is not None:
            continue
        # An equation number is a label, not part of the LaTeX formula.
        for number in node.select(".eqno"):
            number.decompose()
        latex = normalise_latex(node.get_text(" ", strip=True))
        if not latex:
            continue
        is_display = node.name in ("div", "section")
        svg = render_equation(latex, assets, is_display)
        factory = BeautifulSoup("", "html.parser")
        if is_display:
            tag = factory.new_tag("div", **{"class": "equation"})
            tag.append(factory.new_tag("img", src=svg.as_uri(), alt=latex,
                                       **{"class": "math-vector-display"}))
        else:
            tag = factory.new_tag("img", src=svg.as_uri(), alt=latex,
                                  **{"class": "math-vector-inline"})
        node.replace_with(tag)
        converted += 1
    return converted


def convert_local_images(article: BeautifulSoup, page: Path) -> None:
    """Resolve Sphinx's relative image paths before merging HTML documents."""
    for img in article.find_all("img"):
        value = img.get("src", "")
        if not value or urlparse(value).scheme or value.startswith("data:"):
            continue
        absolute = (page.parent / unquote(value.split("#", 1)[0])).resolve()
        if not absolute.is_file():
            raise FileNotFoundError(f"HTML image asset missing: {absolute}")
        img["src"] = absolute.as_uri()


#==============================================================================
# Model data sheets
#==============================================================================

@dataclass
class ModelTraits:
    """Subset of ``isotherm::model_traits`` used by the printed data sheet."""

    names: list[str]
    variable: str
    implicit: bool
    thermal: bool


def read_traits(header: Path) -> ModelTraits:
    """Read the traits specialisation of a model header (single source of truth)."""
    text = header.read_text(encoding="utf-8")
    names = re.search(r"parameter_names\s*\{([^}]*)\}", text)
    variable = re.search(r"independent_variable::(\w+)", text)
    if names is None or variable is None:
        raise ValueError(f"Cannot read model_traits from {header}")
    return ModelTraits(
        names=re.findall(r'"([^"]*)"', names.group(1)),
        variable=variable.group(1),
        implicit="equation_type::implicit_equation" in text,
        thermal=re.search(r"temperature_dependent\s*=\s*true", text) is not None,
    )


VARIABLE_LABEL = {
    "concentration": ("Equilibrium concentration", r"C_e"),
    "pressure": ("Pressure", r"p"),
    "relative_pressure": ("Relative pressure", r"p/p_0"),
    "temperature": ("Temperature", r"T"),
}


def math_name(name: str) -> str:
    """Typeset a parameter name such as ``q_max`` as mathtext."""
    if "_" in name:
        base, sub = name.split("_", 1)
        return f"${base}_{{\\mathrm{{{sub}}}}}$" if len(sub) > 1 else f"${base}_{{{sub}}}$"
    greek = {"alpha", "beta", "gamma", "delta", "epsilon", "theta", "lambda", "mu", "nu"}
    return f"$\\{name}$" if name.lower() in greek else f"${name}$"


def read_series(path: Path) -> tuple[list[str], list[list[float]]]:
    """Read the CSV written by the compiled example of a model."""
    with path.open(encoding="utf-8") as handle:
        rows = list(csv.reader(handle))
    header = rows[0]
    columns: list[list[float]] = [[] for _ in header]
    for row in rows[1:]:
        if len(row) != len(header):
            continue
        for index, value in enumerate(row):
            try:
                number = float(value)
            except ValueError:
                number = math.nan
            columns[index].append(number if math.isfinite(number) else math.nan)
    return header, columns


def render_strip(csv_path: Path, traits: ModelTraits, destination: Path) -> Path:
    """Draw uptake, derivative and scaled sensitivities as three small multiples."""
    key = f"{STYLE_VERSION}|{csv_path.read_bytes()!r}|{traits}"
    fingerprint = hashlib.sha256(key.encode("utf-8")).hexdigest()[:20]
    path = destination / f"strip_{fingerprint}.svg"
    if path.exists():
        return path

    header, columns = read_series(csv_path)
    x = columns[0]
    if header[0] == "activity":
        label, symbol = "Water activity", r"a_w"
    elif header[0].startswith("temperature"):
        label, symbol = "Temperature (K)", r"T"
    else:
        label, symbol = VARIABLE_LABEL.get(traits.variable, ("Independent variable", "x"))
        if header[0] == "concentration" and traits.variable == "concentration":
            label, symbol = "Concentration", r"C_e"

    sensitivity = [(h, c) for h, c in zip(header, columns) if h.startswith("sensitivity")]

    def legend_name(column: str, index: int) -> str:
        suffix = column.split("_", 1)[1] if "_" in column else str(index + 1)
        if suffix.isdigit():
            position = int(suffix) - 1
            if 0 <= position < len(traits.names):
                return math_name(traits.names[position])
            return f"$k_{{{suffix}}}$"
        for name in traits.names:
            if name.replace("_", "").lower() == suffix.replace("_", "").lower():
                return math_name(name)
        return math_name(suffix)

    plt.rcParams.update({
        "font.size": 7, "axes.labelsize": 7, "xtick.labelsize": 6.3,
        "ytick.labelsize": 6.3, "axes.edgecolor": MUTED, "axes.labelcolor": INK,
        "xtick.color": MUTED, "ytick.color": MUTED, "axes.linewidth": 0.5,
        "xtick.major.width": 0.5, "ytick.major.width": 0.5,
        "xtick.major.size": 2.2, "ytick.major.size": 2.2,
        "axes.spines.top": False, "axes.spines.right": False,
        "axes.grid": True, "grid.color": "#E7E9F1", "grid.linewidth": 0.45,
        "legend.fontsize": 6.4, "legend.frameon": False, "lines.linewidth": 1.25,
    })
    fig, axes = plt.subplots(1, 3, figsize=(18.2 / 2.54, 5.0 / 2.54))
    panels = (
        (columns[1], r"$q$", "Uptake"),
        (columns[2], rf"$\mathrm{{d}}q/\mathrm{{d}}{symbol.split('/')[0]}$", "Derivative"),
    )
    for axis, (values, ylabel, title) in zip(axes[:2], panels):
        axis.plot(x, values, color=SERIES[0])
        axis.set_ylabel(ylabel, labelpad=1.5)
        axis.set_title(title, loc="left", fontsize=7.2, color=INK, fontweight="semibold", pad=3)
    axis = axes[2]
    for index, (column, values) in enumerate(sensitivity):
        axis.plot(x, values, color=SERIES[index % len(SERIES)],
                  linestyle=STYLES[index % len(STYLES)], label=legend_name(column, index))
    axis.set_ylabel(r"$k_i\,\partial q/\partial k_i$", labelpad=1.5)
    axis.set_title("Scaled sensitivity", loc="left", fontsize=7.2, color=INK,
                   fontweight="semibold", pad=3)
    if sensitivity:
        axis.legend(loc="best", handlelength=2.2, borderaxespad=0.2, labelspacing=0.25)
    for axis in axes:
        axis.set_xlabel(f"{label}, ${symbol}$" if "(K)" not in label else f"${symbol}$ (K)",
                        labelpad=1.5)
        axis.margins(x=0.02)
    fig.tight_layout(pad=0.3, w_pad=1.6)
    fig.savefig(path, format="svg", transparent=True)
    plt.close(fig)
    return path


def restructure_model(article: BeautifulSoup, page: Path, repo: Path,
                      static: Path, assets: Path, number: int) -> dict[str, str]:
    """Recompose a model page as a data sheet and return its rail metadata."""
    match = MODEL_PAGE.search(page.as_posix())
    group, model = match.groups()
    header = repo / "src" / "isotherm" / "models" / group / f"{model}.hpp"
    if not header.is_file():
        header = repo / "include" / "isotherm" / "models" / group / f"{model}.hpp"
    traits = read_traits(header)

    figures = article.find_all("figure")
    stem = None
    captions: list[str] = []
    for figure in figures:
        image = figure.find("img")
        source = image.get("src", "") if image else ""
        found = re.search(r"([a-z0-9_]+?)_(curve|derivative|sensitivity)[a-z0-9_]*\.svg$", source)
        if found and stem is None:
            stem = found.group(1)
        caption = figure.select_one(".caption-text")
        if caption is not None:
            for img in caption.select("img.math-vector-inline"):
                small = render_equation(img.get("alt", ""), assets, False, size=7.2)
                img["src"] = small.as_uri()
            captions.append(caption.decode_contents().strip().rstrip("."))
    data = static / "figures" / f"{stem}_data.csv" if stem else None

    strip_html = ""
    if data is not None and data.is_file():
        svg = render_strip(data, traits, assets)
        letters = "abc"
        detail = "; ".join(f"({letters[i]}) {text}" for i, text in enumerate(captions[:3]))
        strip_html = (
            f'<figure class="strip"><img src="{svg.as_uri()}" alt="{html.escape(model)} plots">'
            f'<figcaption><b>Figure {number}.</b> {detail}.</figcaption></figure>'
        )
        for figure in figures:
            section = figure.find_parent("section")
            figure.decompose()
            # Drop sections that only carried a plot heading.
            if section is not None and not section.find(["p", "div", "img", "pre", "table", "ul"]):
                section.decompose()

    title = article.find("h1")
    name = title.get_text(" ", strip=True) if title else model
    count, group_name = GROUPS[group]
    tags = ["implicit" if traits.implicit else "explicit"]
    if traits.thermal:
        tags.append("temperature")
    params = ", ".join(html.escape(n) for n in traits.names)
    rail = BeautifulSoup(f'<aside class="sheet-meta"><div class="sheet-number">{number:02d}</div></aside>',
                         "html.parser")
    spec = BeautifulSoup(
        f'<div class="sheet-spec">'
        f'<span class="sheet-dots">{"".join("<i></i>" for _ in range(count))}</span>'
        f'<span class="sheet-label">{group_name}</span>'
        f'<span class="sheet-params">{params}</span>'
        f'{"".join(f"<span class=sheet-tag>{t}</span>" for t in tags)}'
        f'</div>', "html.parser")
    if title is not None:
        title.insert_before(rail)
        title.insert_after(spec)

    if strip_html:
        formulation = article.find("section", id="mathematical-formulation")
        anchor = formulation if formulation is not None else article.select_one(".sheet-spec")
        anchor.insert_after(BeautifulSoup(strip_html, "html.parser"))
    return {"name": name, "group": group, "number": f"{number:02d}"}


def extract_article(page: Path, assets: Path) -> tuple[str, BeautifulSoup, int]:
    """Extract published article content, ignoring the PyData navigation UI."""
    soup = BeautifulSoup(page.read_text(encoding="utf-8"), "html.parser")
    body = soup.select_one("article.bd-article") or soup.select_one("article")
    if body is None:
        raise ValueError(f"Cannot find the Sphinx article in {page}")
    for element in body.select(
        "script, noscript, .toctree-wrapper, .headerlink, .sd-tab-label, "
        ".sphinx-tabs-tablist, .related, .edit-this-page, "
        ".isotherm-source-tools, section#source-code-navigation-html, button"
    ):
        element.decompose()
    # Render the formulae before adjusting image URIs.
    math_count = replace_math(body, assets)
    convert_local_images(body, page)
    title_tag = body.find("h1")
    title = title_tag.get_text(" ", strip=True) if title_tag else "Scientific documentation"
    return title, body, math_count


#==============================================================================
# Cover and document
#==============================================================================

def cover_field() -> str:
    """Generative cover art: a family of Langmuir and Freundlich curves."""
    width, height = 210.0, 150.0
    lines = []
    for index in range(46):
        k = 10 ** (-1.6 + index * 0.075)
        points = []
        for step in range(161):
            c = step / 160 * 10.0
            q = k * c / (1 + k * c)
            points.append(f"{c / 10 * width:.2f},{height - q * height * 0.92:.2f}")
        hue = TURQUOISE if index % 5 == 0 else ("#AEB6D6" if index % 2 else "#C9D0E6")
        weight = 0.55 if index % 5 == 0 else 0.3
        lines.append(f'<polyline points="{" ".join(points)}" fill="none" '
                     f'stroke="{hue}" stroke-width="{weight}"/>')
    accent = []
    for step in range(161):
        c = step / 160 * 10.0
        q = (c / 10) ** 0.42
        accent.append(f"{c / 10 * width:.2f},{height - q * height * 0.92:.2f}")
    lines.append(f'<polyline points="{" ".join(accent)}" fill="none" stroke="{CORAL}" '
                 f'stroke-width="1.1"/>')
    return (f'<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 {width} {height}" '
            f'preserveAspectRatio="none">{"".join(lines)}</svg>')


def build_document(chapters: list[dict], logo: Path, stylesheet: Path) -> str:
    """Build a single print-ready document with the data-sheet layout."""
    if not stylesheet.is_file():
        raise FileNotFoundError(f"Print stylesheet not found: {stylesheet}")
    sections: list[str] = []
    front: list[str] = []
    back: list[str] = []
    groups: dict[str, list[str]] = {}
    for index, chapter in enumerate(chapters, start=1):
        anchor = f"chapter-{index}"
        title = html.escape(chapter["title"])
        entry = f'<li><a href="#{anchor}"><span>{title}</span></a></li>'
        if chapter.get("group"):
            groups.setdefault(chapter["group"], []).append(
                f'<li><a href="#{anchor}"><em>{chapter["number"]}</em>'
                f'<span>{title}</span></a></li>')
            kind = "model-sheet"
        else:
            (front if not groups else back).append(entry)
            kind = "chapter"
        sections.append(f'<section class="print-chapter {kind}" id="{anchor}">'
                        f'{chapter["body"]}</section>')
    contents = [f'<div class="toc-block"><h2>Introduction</h2><ol>{"".join(front)}</ol></div>']
    for group, items in groups.items():
        count, label = GROUPS[group]
        contents.append(f'<div class="toc-block"><h2><b>{count}</b> {label}</h2>'
                        f'<ol>{"".join(items)}</ol></div>')
    contents.append(f'<div class="toc-block"><h2>Software</h2><ol>{"".join(back)}</ol></div>')
    edition = date.today().strftime("%B %Y")
    return f'''<!doctype html>
<html lang="en-AU"><head><meta charset="utf-8">
<title>Isotherm++ 2.0 | Scientific manual</title>
<link rel="stylesheet" href="{stylesheet.as_uri()}"></head>
<body>
<section class="print-cover">
  <div class="cover-top"><img src="{logo.as_uri()}" alt=""><span>Scientific manual</span><span>Version 2.0</span></div>
  <h1>Isotherm<span class="plus">++</span></h1>
  <p class="cover-lead">Adsorption isotherm models for mass transport simulation: equations, exact derivatives, sensitivities and original sources.</p>
  <div class="cover-field">{cover_field()}</div>
  <div class="cover-foot"><span>Header-only C++20 library</span><span>MIT licence</span><span>{edition}</span></div>
</section>
<section class="print-contents"><h1>Contents</h1><div class="toc">{"".join(contents)}</div></section>
{"".join(sections)}
</body></html>'''


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--html-dir", type=Path, required=True)
    parser.add_argument("--source-dir", type=Path,
                        help="Sphinx RST root; defaults to the parent of --static-dir")
    parser.add_argument("--static-dir", type=Path, required=True)
    parser.add_argument("--output", type=Path, required=True)
    args = parser.parse_args()
    html_root = args.html_dir.resolve()
    static = args.static_dir.resolve()
    source_root = (args.source_dir.resolve() if args.source_dir
                   else static.parent)
    repo = source_root.parent
    output = args.output.resolve()
    assets = output.parent / "_print_assets"
    assets.mkdir(parents=True, exist_ok=True)
    register_fonts(static)

    chapters = []
    equations = 0
    number = 0
    pages = ordered_pages(html_root, source_root)
    for page in pages:
        title, body, count = extract_article(page, assets)
        chapter = {"title": title}
        if MODEL_PAGE.search(page.relative_to(html_root).as_posix()):
            number += 1
            chapter.update(restructure_model(body, page, repo, static, assets, number))
        chapter["body"] = str(body)
        chapters.append(chapter)
        equations += count
    logo = static / "mark_transparent.png"
    stylesheet = static / "isotherm-print.css"
    document = build_document(chapters, logo, stylesheet)
    html_file = output.parent / "print-preview.html"
    html_file.write_text(document, encoding="utf-8")
    HTML(string=document, base_url=html_root.as_uri() + "/").write_pdf(output)
    if not output.is_file() or output.stat().st_size < 4096:
        raise RuntimeError("PDF output missing or unexpectedly small")
    print(f"WeasyPrint PDF: {output}")
    print(f"Source chapters: {len(pages)}; models: {number}; vector equations: {equations}")
    print(f"Print HTML preview: {html_file}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
