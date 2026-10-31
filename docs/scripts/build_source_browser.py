#!/usr/bin/env python3
#==============================================================================
# Name        : build_source_browser.py
# Authors     : Joao Flavio Vasconcellos
# Version     : 2.0.0
# Description : Generate offline C++ source pages with verified symbol links.
#
# Copyright   : Copyright (C) 2026 Joao Flavio Vasconcellos
# Licence     : MIT License (see LICENSE)
# SPDX-License-Identifier: MIT
#==============================================================================

"""Create an HTML source browser for reviewed models and shared core dependencies.

Pages are generated from the actual, unmodified C++ files. Indexed names
link to their declared definitions; unsupported C++ semantics are not guessed.
The original files are copied byte-for-byte alongside the highlighted view.
This is a deliberately limited first prototype, not a complete Clang index.
"""

from __future__ import annotations

#==============================================================================
# Python standard library imports
#==============================================================================

import argparse
import html
import os
import re
import shutil
from pathlib import Path
from urllib.parse import quote

#==============================================================================
# Third-party library imports
#==============================================================================

from pygments import lex
from pygments.lexers import CppLexer
from pygments.token import Comment, Keyword, Literal, Name, Number, Operator, String

ROOT = Path(__file__).resolve().parents[2]
BASE_FILES = (
    "include/isotherm/core/type.hpp",
    "include/isotherm/core/gas_constant.hpp",
    "include/isotherm/core/error.hpp",
    "include/isotherm/core/traits.hpp",
    "include/isotherm/core/concepts.hpp",
    "include/isotherm/core/validation.hpp",
    "include/isotherm/core/evaluation.hpp",
    "include/isotherm/models/OneParameter/Henry.hpp",
    "Examples/OneParameter/Henry/main.cpp",
    "IsothermTest/OneParameter/Henry/test_henry.cpp",
    "Examples/TwoParameters/AntiLangmuir/main.cpp",
    "Examples/TwoParameters/DubininRadushkevich/main.cpp",
    "Examples/TwoParameters/Elovich/main.cpp",
    "Examples/TwoParameters/Temkin/main.cpp",
    "IsothermTest/TwoParameters/Elovich/test_elovich.cpp",
    "IsothermTest/TwoParameters/Temkin/test_temkin.cpp",
    "IsothermTest/TwoParameters/DubininRadushkevich/test_dubinin_radushkevich.cpp",
    "IsothermTest/TwoParameters/AntiLangmuir/test_anti_langmuir.cpp",
    "include/isotherm/isotherm.hpp",
)
# The umbrella header includes the complete published catalogue. Make every
# project header browsable so its #include link always has a local destination.
MODEL_HEADERS = tuple(
    str(path.relative_to(ROOT)).replace(os.sep, "/")
    for path in sorted((ROOT / "include/isotherm/models").rglob("*.hpp"))
)
TWO_PARAMETER_EXAMPLES = tuple(
    str(path.relative_to(ROOT)).replace(os.sep, "/")
    for path in sorted((ROOT / "Examples/TwoParameters").rglob("main.cpp"))
)
TWO_PARAMETER_TESTS = tuple(
    str(path.relative_to(ROOT)).replace(os.sep, "/")
    for path in sorted((ROOT / "IsothermTest/TwoParameters").rglob("test_*.cpp"))
)
THREE_PARAMETER_EXAMPLES = tuple(
    str(path.relative_to(ROOT)).replace(os.sep, "/")
    for path in sorted((ROOT / "Examples/ThreeParameters").rglob("main.cpp"))
)
THREE_PARAMETER_TESTS = tuple(
    str(path.relative_to(ROOT)).replace(os.sep, "/")
    for path in sorted((ROOT / "IsothermTest/ThreeParameters").rglob("test_*.cpp"))
)
FOUR_PARAMETER_EXAMPLES = tuple(
    str(path.relative_to(ROOT)).replace(os.sep, "/")
    for path in sorted((ROOT / "Examples/FourParameters").rglob("main.cpp"))
)
FOUR_PARAMETER_TESTS = tuple(
    str(path.relative_to(ROOT)).replace(os.sep, "/")
    for path in sorted((ROOT / "IsothermTest/FourParameters").rglob("test_*.cpp"))
)
FIVE_PARAMETER_EXAMPLES = tuple(
    str(path.relative_to(ROOT)).replace(os.sep, "/")
    for path in sorted((ROOT / "Examples/FiveParameters").rglob("main.cpp"))
)
FIVE_PARAMETER_TESTS = tuple(
    str(path.relative_to(ROOT)).replace(os.sep, "/")
    for path in sorted((ROOT / "IsothermTest/FiveParameters").rglob("test_*.cpp"))
)
SIX_PARAMETER_EXAMPLES = tuple(
    str(path.relative_to(ROOT)).replace(os.sep, "/")
    for path in sorted((ROOT / "Examples/SixParameters").rglob("main.cpp"))
)
SIX_PARAMETER_TESTS = tuple(
    str(path.relative_to(ROOT)).replace(os.sep, "/")
    for path in sorted((ROOT / "IsothermTest/SixParameters").rglob("test_*.cpp"))
)
FILES = tuple(dict.fromkeys((*BASE_FILES, *MODEL_HEADERS,
                              *TWO_PARAMETER_EXAMPLES, *TWO_PARAMETER_TESTS,
                              *THREE_PARAMETER_EXAMPLES, *THREE_PARAMETER_TESTS,
                              *FOUR_PARAMETER_EXAMPLES, *FOUR_PARAMETER_TESTS,
                              *FIVE_PARAMETER_EXAMPLES, *FIVE_PARAMETER_TESTS,
                              *SIX_PARAMETER_EXAMPLES, *SIX_PARAMETER_TESTS)))
# (File, defining text): the line is resolved from source, not hard-coded.
DEFINITIONS = {
    "Real": ("include/isotherm/core/type.hpp", r"\busing Real ="),
    "gas_constant_unit": ("include/isotherm/core/gas_constant.hpp", r"\benum class gas_constant_unit\b"),
    "molar_gas_constant": ("include/isotherm/core/gas_constant.hpp", r"\bReal molar_gas_constant\s*\("),
    "isotherm_error": ("include/isotherm/core/error.hpp", r"\bclass isotherm_error\b"),
    "error_code": ("include/isotherm/core/error.hpp", r"\benum class error_code\b"),
    "model_traits": ("include/isotherm/core/traits.hpp", r"\bstruct model_traits\b"),
    "scalar_isotherm": ("include/isotherm/core/concepts.hpp", r"\bconcept scalar_isotherm\b"),
    "thermal_isotherm": ("include/isotherm/core/concepts.hpp", r"\bconcept thermal_isotherm\b"),
    "differentiable_scalar_isotherm": ("include/isotherm/core/concepts.hpp", r"\bconcept differentiable_scalar_isotherm\b"),
    "require_finite": ("include/isotherm/core/validation.hpp", r"\bvoid require_finite\s*\("),
    "require_positive": ("include/isotherm/core/validation.hpp", r"\bvoid require_positive\s*\("),
    "require_nonnegative": ("include/isotherm/core/validation.hpp", r"\bvoid require_nonnegative\s*\("),
    "evaluate": ("include/isotherm/core/evaluation.hpp", r"\bReal evaluate\s*\("),
    "derivative": ("include/isotherm/core/evaluation.hpp", r"\bReal derivative\s*\("),
    "evaluate_into": ("include/isotherm/core/evaluation.hpp", r"\bvoid evaluate_into\s*\("),
    "Henry": ("include/isotherm/models/OneParameter/Henry.hpp", r"\bclass Henry\b"),
    "AntiLangmuir": ("include/isotherm/models/TwoParameters/AntiLangmuir.hpp", r"\bclass AntiLangmuir\b"),
    "DubininRadushkevich": ("include/isotherm/models/TwoParameters/DubininRadushkevich.hpp", r"\bclass DubininRadushkevich\b"),
    "Elovich": ("include/isotherm/models/TwoParameters/Elovich.hpp", r"\bclass Elovich\b"),
    "Temkin": ("include/isotherm/models/TwoParameters/Temkin.hpp", r"\bclass Temkin\b"),
    "numerical_scaled_sensitivity": (
        "Examples/OneParameter/Henry/main.cpp", r"\bReal numerical_scaled_sensitivity\s*\("
    ),
    "main": ("Examples/OneParameter/Henry/main.cpp", r"\bint main\s*\("),
}
# Code and layout are intentionally self-contained: the browser also works
# when viewed as file:// from a Sphinx build copied to another machine.
CSS = """
:root { --indigo:#27316a; --teal:#087f82; --ink:#27334e; --bg:#f4f6fd; }
* { box-sizing:border-box; }
body { font:15px/1.5 system-ui, sans-serif; margin:0; background:var(--bg); color:var(--ink); }
header { color:white; background:var(--indigo); padding:18px 28px; }
header h1 { font-size:21px; margin:0 0 8px; }
header a { color:#c8f5f1; margin-right:18px; }
.main { max-width:1320px; margin:auto; padding:24px; }
.path { color:#62708e; font-size:13px; overflow-wrap:anywhere; margin:0 0 12px; }
.card { background:white; border:1px solid #dfe5f3; border-radius:19px; overflow:hidden; box-shadow:0 5px 22px #27316a11; }
.browse { display:flex; gap:20px; flex-wrap:wrap; padding:15px 22px; background:#e9f7f5; }
a { color:var(--teal); } a:hover { text-decoration:underline; }
.code-table { border-collapse:collapse; width:100%; font:13px/1.7 'DejaVu Sans Mono', Consolas, monospace; }
.code-table tr:hover { background:#e9f9f6; }
.line-no { user-select:none; white-space:nowrap; text-align:right; vertical-align:top;
  padding:0 14px; width:65px; background:#f3f5fc; border-right:1px solid #dce2ee; }
.line-no a { color:#6b7794; text-decoration:none; }
.code-text { white-space:pre; padding:0 16px; }
.code-table tr:target { background:#ffefd9; }
.code-text a { text-decoration:none; border-bottom:1px dotted #087f82; font-weight:600; }
.code-text a:hover { background:#d4f0ec; }
.t-comment { color:#697b81; } .t-string { color:#9f582f; } .t-number { color:#9a4485; }
.t-keyword { color:#27316a; font-weight:650; } .t-name { color:#204e81; }
.t-operator { color:#4c5771; }
.cards { display:grid; grid-template-columns:repeat(auto-fit,minmax(240px,1fr)); gap:16px; }
.cards section { border:1px solid #dfe5f3; background:white; padding:16px; border-radius:14px; }
.cards h2 { font-size:15px; margin-top:0; }
.small { color:#63718d; font-size:13px; }
@media(max-width:650px) { .main{padding:12px}header{padding:16px}.code-table{font-size:11px} }
"""


def definition_lines():
    """Resolve each explicitly indexed symbol and reject unresolvable links."""
    output = {}
    for name, (filename, pattern) in DEFINITIONS.items():
        matches = [i for i, line in enumerate((ROOT / filename).read_text(encoding="utf-8").splitlines(), 1)
                   if re.search(pattern, line)]
        expected = 2 if name in {"evaluate", "derivative", "evaluate_into"} else 1
        if len(matches) != expected:
            raise RuntimeError(f"Symbol {name} has {len(matches)} definitions, expected {expected}")
        output[name] = (filename, matches[0])
    return output


def css_token(ttype):
    if ttype in Comment: return "t-comment"
    if ttype in String or ttype in Literal.String: return "t-string"
    if ttype in Number: return "t-number"
    if ttype in Keyword: return "t-keyword"
    if ttype in Name: return "t-name"
    if ttype in Operator: return "t-operator"
    return ""


def highlighted_line(ttype, value, current: str, lines: dict) -> str:
    """Only link code identifier tokens; never link comments or literals."""
    style = css_token(ttype)
    can_link = ttype in Name or ttype in Keyword
    parts = re.split(r"(\b[A-Za-z_][A-Za-z_0-9]*\b)", value) if can_link else [value]
    result = []
    for part in parts:
        encoded = html.escape(part)
        if can_link and part in lines:
            target, number = lines[part]
            relative = Path(target + ".html")
            # All pages reside under one common output root.
            href = Path(os.path.relpath(relative, Path(current).parent)).as_posix() + f"#L{number}"
            if target == current:
                href = f"#L{number}"
            encoded = f'<a href="{html.escape(quote(href, safe="/#."), quote=True)}" title="Definition of {part}">{encoded}</a>'
        result.append(encoded)
    text = "".join(result)
    return f'<span class="{style}">{text}</span>' if style else text


def linked_local_include(source_line: str, current: str, available: set[str]) -> str | None:
    """Render each project-local #include as a link to its exact header.

    Standard-library and external headers have no local source page and keep
    their normal Pygments formatting. The link target is never guessed.
    """
    match = re.fullmatch(
        r'(?P<indent>\s*)(?P<directive>\#\s*include)(?P<space>\s*)'
        r'(?P<open>[<"])(?P<header>isotherm/[^">]+)(?P<close>[>"])'
        r'(?P<rest>.*)', source_line
    )
    if match is None:
        return None
    target = "include/" + match.group("header")
    if target not in available:
        raise RuntimeError(f"Unindexed project header in {current}: {target}")
    relative = Path(os.path.relpath(Path(target + ".html"), Path(current).parent)).as_posix()
    href = html.escape(quote(relative, safe="/."), quote=True)
    displayed = html.escape(match.group("open") + match.group("header") + match.group("close"))
    label = html.escape(match.group("header"), quote=True)
    return (html.escape(match.group("indent"))
            + '<span class="t-keyword">' + html.escape(match.group("directive")) + '</span>'
            + html.escape(match.group("space"))
            + f'<a href="{href}" title="Browse include {label}"><span class="t-string">{displayed}</span></a>'
            + html.escape(match.group("rest")))


def code_rows(source: str, current: str, lines: dict, available: set[str]) -> str:
    """Use Pygments for lexical classification and retain original line layout."""
    assembled = [""]
    for ttype, value in lex(source, CppLexer()):
        chunks = value.split("\n")
        for j, chunk in enumerate(chunks):
            if j:
                assembled.append("")
            if chunk:
                assembled[-1] += highlighted_line(ttype, chunk, current, lines)
    if source.endswith("\n") and assembled[-1] == "":
        assembled.pop()
    source_lines = source.splitlines()
    if len(assembled) != len(source_lines):
        raise RuntimeError(f"Source line alignment failed for {current}")
    for index, original in enumerate(source_lines):
        include = linked_local_include(original, current, available)
        if include is not None:
            assembled[index] = include
    return "\n".join(
        f'<tr id="L{i}"><td class="line-no"><a href="#L{i}">{i}</a></td>'
        f'<td class="code-text">{part or "&#8203;"}</td></tr>'
        for i, part in enumerate(assembled, 1)
    )


def page(title: str, body: str) -> str:
    return ("<!doctype html>\n<html lang=\"en-AU\"><head><meta charset=\"utf-8\">"
            '<meta name="viewport" content="width=device-width, initial-scale=1">'
            f"<title>{html.escape(title)} · Isotherm++</title>"
            f"<style>{CSS}</style></head><body><header><h1>{html.escape(title)}</h1>"
            "</header>" + body + "</body></html>\n")


def generate(output: Path) -> None:
    symbols = definition_lines()
    output.mkdir(parents=True, exist_ok=True)
    index_parts = []
    available = set(FILES)
    for filename in FILES:
        source = ROOT / filename
        if not source.is_file():
            raise FileNotFoundError(source)
        raw_path = output / filename
        raw_path.parent.mkdir(parents=True, exist_ok=True)
        shutil.copyfile(source, raw_path)
        code = source.read_text(encoding="utf-8")
        page_symbols = dict(symbols)
        if filename in ("Examples/TwoParameters/AntiLangmuir/main.cpp",
                        "Examples/TwoParameters/DubininRadushkevich/main.cpp"):
            main_lines = [number for number, line in enumerate(code.splitlines(), 1)
                          if re.search(r"\bint main\s*\(", line)]
            if len(main_lines) != 1:
                raise RuntimeError(f"Example {filename} must have exactly one main")
            page_symbols["main"] = (filename, main_lines[0])
        rows = code_rows(code, filename, page_symbols, available)
        depth = len(Path(filename).parts) - 1
        prefix = "../" * depth
        # Associate each implementation, test and example with its own
        # scientific manual. Shared/core files have no model-specific manual.
        parts = Path(filename).parts
        model_location = None
        if len(parts) == 5 and parts[:3] == ("include", "isotherm", "models"):
            model_location = (parts[3], Path(parts[4]).stem)
        elif len(parts) >= 4 and parts[0] in ("Examples", "IsothermTest"):
            model_location = (parts[1], parts[2])
        manual_link = ""
        if model_location is not None:
            group, model_name = model_location
            manual = ROOT / "docs/scientific_catalogue/models" / group / f"{model_name}.rst"
            if manual.is_file():
                display_names = {
                    "AntiLangmuir": "Anti-Langmuir",
                    "DubininRadushkevich": "Dubinin–Radushkevich",
                    "HarkinJura": "Harkins–Jura",
                }
                model_label = display_names.get(model_name, model_name)
                model_section = f"{group}/{model_name}"
                manual_link = (f'<a href="{prefix}../scientific_catalogue/models/'
                               f'{model_section}.html">{html.escape(model_label)} manual</a>')
        nav = (f'<a href="{prefix}index.html">Source index</a>'
               + manual_link
               + f'<a href="{html.escape(Path(filename).name)}">Original C++ file</a>')
        browse = (f'<p class="path">{html.escape(filename)}</p><div class="card">'
                  f'<div class="browse">{nav}</div>'
                  f'<div style="overflow-x:auto"><table class="code-table">{rows}</table></div></div>')
        destination = output / (filename + ".html")
        destination.write_text(page(f"C++ source: {Path(filename).name}",
                                     '<main class="main">' + browse + '</main>'), encoding="utf-8")
        index_parts.append(f'<section><h2>{html.escape(Path(filename).name)}</h2>'
                           f'<p class="small">{html.escape(filename)}</p>'
                           f'<a href="{quote(filename + ".html", safe="/.")}">Browse code →</a></section>')

    index_html = ('<main class="main"><p>Actual C++20 source with line numbers, '
                  'indexed symbol references and a link to the original file. '
                  'Project-local includes open the corresponding C++ header.</p><div class="cards">'
                  + "\n".join(index_parts) + '</div></main>')
    (output / "index.html").write_text(page("Isotherm++ · C++ source browser", index_html),
                                        encoding="utf-8")


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--output", type=Path, required=True)
    args = parser.parse_args()
    generate(args.output.resolve())
    print(f"Source browser OK: {len(FILES)} original files, {len(DEFINITIONS)} indexed symbols")


if __name__ == "__main__":
    main()
