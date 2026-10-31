#==============================================================================
# Name        : conf.py
# Authors     : Joao Flavio Vasconcellos
# Version     : 2.0.0
# Description : Shared Sphinx configuration for an HTML/CSS scientific manual.
#
# Copyright   : Copyright (C) 2026 Joao Flavio Vasconcellos
# Licence     : MIT License (see LICENSE)
# SPDX-License-Identifier: MIT
#==============================================================================

"""Scientific documentation, written in Australian English."""

from __future__ import annotations

project = "Isotherm++"
version = "2.0"
release = "2.0.0-dev"
author = "Joao Flavio Vasconcellos"
copyright = "2026, Joao Flavio Vasconcellos"
language = "en"
root_doc = "index"

extensions = [
    "sphinx.ext.autosectionlabel",
    "sphinx.ext.mathjax",
]
autosectionlabel_prefix_document = True
numfig = True
math_numfig = True
smartquotes = True

exclude_patterns = ["_build", ".DS_Store", "Thumbs.db", "design/*", "scripts/*"]

# The officially approved palette and the truly transparent light-on-indigo
# logo remain identical to the editorial prototype.
html_theme = "pydata_sphinx_theme"
html_title = "Isotherm++ 2.0 | Scientific manual"
html_logo = "_static/logo_sidebar_transparent.png"
html_favicon = "_static/mark_transparent.png"
html_static_path = ["_static"]
html_css_files = ["isotherm.css"]
# Open only external hyperlinks in a new tab, in every Sphinx HTML page.
html_js_files = ["external_links.js"]
html_theme_options = {
    "logo": {
        "text": "",
        "image_light": "_static/logo_sidebar_transparent.png",
        "image_dark": "_static/logo_sidebar_transparent.png",
    },
    "show_toc_level": 2,
    "navigation_with_keys": True,
    "use_edit_page_button": False,
    "header_links_before_dropdown": 4,
    "navbar_end": ["navbar-icon-links"],
    "footer_start": ["copyright"],
}
html_context = {}
html_show_sourcelink = False
html_copy_source = False
html_last_updated_fmt = None

# Hide the Sphinx permalink glyph (#) after section and figure titles.
# Section IDs remain available for links from other pages.
html_permalinks = False

# The PDF is printed from Sphinx HTML using WeasyPrint. Do not add a
# separate LaTeX PDF theme: it would diverge from the approved CSS identity.
# Keep MathJax for the interactive HTML. The offline print renderer converts
# the corresponding formula text to vector SVG with Matplotlib mathtext.
