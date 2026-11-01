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
this script assembles that HTML and replaces MathJax-only formula markup
with locally generated SVGs so WeasyPrint can render mathematics offline.
"""

from __future__ import annotations

#==============================================================================
# Python standard library imports
#==============================================================================

import argparse
import hashlib
import re
from pathlib import Path
from urllib.parse import urlparse, unquote

#==============================================================================
# Third-party library imports
#==============================================================================

from bs4 import BeautifulSoup
from matplotlib import pyplot as plt
from matplotlib import rcParams
from weasyprint import HTML


OMIT_PAGES = {"search.html", "genindex.html", "py-modindex.html", "404.html"}
CORE_CHAPTERS = (
    "index.html",
    "scientific_catalogue/index.html",
    "user_guide/index.html",
    "architecture/index.html",
    "developers/index.html",
    "verification/index.html",
)


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
    group_rank = {
        "OneParameter": 1, "TwoParameters": 2, "ThreeParameters": 3,
        "FourParameters": 4, "FiveParameters": 5, "SixParameters": 6,
    }

    def rank(page: Path) -> tuple[int, int, str]:
        relative = page.relative_to(root).as_posix()
        if relative == "index.html":
            return 0, 0, ""
        if relative == "scientific_catalogue/index.html":
            return 1, 0, ""
        match = re.fullmatch(r"scientific_catalogue/models/([^/]+)/([^/]+)\.html", relative)
        if match:
            group, model = match.groups()
            if group not in group_rank:
                raise ValueError(f"Unexpected model parameter group: {relative}")
            return 2, group_rank[group], model.casefold()
        for index, other in enumerate(CORE_CHAPTERS[2:], start=3):
            if relative == other:
                return index, 0, ""
        return 8, 0, relative.casefold()

    return sorted(paths, key=rank)


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


def render_equation(expression: str, destination: Path, display: bool) -> Path:
    """Render MathJax TeX as an SVG; fail loudly on unsupported mathematics."""
    fingerprint = hashlib.sha256((str(display) + expression).encode("utf-8")).hexdigest()[:20]
    path = destination / f"formula_{fingerprint}.svg"
    if path.exists():
        return path

    # Mathtext is a vector renderer. The generated SVG remains crisp in PDF.
    # It is intentionally not a replacement for a complete TeX parser.
    previous = rcParams["svg.fonttype"]
    rcParams["svg.fonttype"] = "path"
    fig = plt.figure(figsize=(11, 1.2))
    try:
        fig.patch.set_alpha(0)
        fig.text(0, 0.55, f"${expression}$", fontsize=17 if display else 12,
                 va="center", ha="left", color="#27316A")
        fig.savefig(path, format="svg", transparent=True,
                    bbox_inches="tight", pad_inches=0.02)
    except Exception as exc:
        if path.exists():
            path.unlink()
        raise ValueError(
            f"Unable to convert formula to vector SVG: {expression!r}; {exc}"
        ) from exc
    finally:
        plt.close(fig)
        rcParams["svg.fonttype"] = previous
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
        tag = factory.new_tag("img", src=svg.as_uri(), alt=latex)
        tag["class"] = "math-vector-display" if is_display else "math-vector-inline"
        if is_display:
            tag = factory.new_tag("div", **{"class": "equation-card"})
            tag.append(factory.new_tag("img", src=svg.as_uri(), alt=latex,
                                       **{"class": "math-vector-display"}))
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


def extract_article(page: Path, assets: Path) -> tuple[str, str, int]:
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
    return title, str(body), math_count


def build_document(chapters: list[tuple[str, str]], logo: Path, stylesheet: Path) -> str:
    """Build a single print-ready document with the same visual colour system."""
    import html
    if not stylesheet.is_file():
        raise FileNotFoundError(f"Print stylesheet not found: {stylesheet}")
    sections = []
    summary = []
    for index, (title, body) in enumerate(chapters, start=1):
        anchor = f"chapter-{index}"
        summary.append(f'<li><a href="#{anchor}">{html.escape(title)}</a></li>')
        sections.append(
            f'<section class="print-chapter" id="{anchor}">'
            f'<div class="chapter-eyebrow">SCIENTIFIC MANUAL / {index:02d}</div>'
            f'{body}</section>'
        )
    return f'''<!doctype html>
<html lang="en-AU"><head><meta charset="utf-8">
<title>Isotherm++ 2.0 | Scientific manual</title>
<link rel="stylesheet" href="{stylesheet.as_uri()}"></head>
<body>
<section class="print-cover">
  <div class="cover-stripe">ISOTHERM++ 2.0 <span>SCIENTIFIC MANUAL</span></div>
  <div class="cover-mark"><img src="{logo.as_uri()}" alt="Isotherm++ circular logo"></div>
  <div class="cover-eyebrow">ADSORPTION ISOTHERM MODELS</div>
  <h1>Isotherm++ <span>2.0</span></h1>
  <p class="cover-lead">Scientific reference, mathematical models and modern C++20 architecture.</p>
  <div class="cover-alert">Development documentation: model migration and validation are not yet complete.</div>
  <div class="cover-bottom">SCIENTIFIC DOCUMENTATION &nbsp; / &nbsp; AUSTRALIAN ENGLISH</div>
</section>
<section class="print-contents"><h1>Contents</h1><ol>{''.join(summary)}</ol></section>
{''.join(sections)}
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
    output = args.output.resolve()
    assets = output.parent / "_print_assets"
    assets.mkdir(parents=True, exist_ok=True)
    chapters = []
    equations = 0
    pages = ordered_pages(html_root, source_root)
    for page in pages:
        title, body, count = extract_article(page, assets)
        chapters.append((title, body))
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
    print(f"Source chapters: {len(pages)}; vector equations: {equations}")
    print(f"Print HTML preview: {html_file}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
