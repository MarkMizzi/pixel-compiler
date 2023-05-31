SOURCES := $(wildcard *.tex)
TARGETS := report.pdf

LATEX_OPTS := --shell-escape
LATEX_ENGINE := pdflatex

# Master targets

all: $(TARGETS)

## Setting targets

FORCE:

.PHONY:	FORCE all clean

.SUFFIXES: # Delete the default suffixes

.DELETE_ON_ERROR:

# Dependencies

$(TARGETS): $(SOURCES)

# Rules for main compilation

%.bbl: %.aux
	-bibtex $*

%.aux: %.tex
	$(LATEX_ENGINE) $(LATEX_OPTS) $<

%.pdf: %.tex %.aux %.bbl
	$(LATEX_ENGINE) $(LATEX_OPTS) $<
	$(LATEX_ENGINE) $(LATEX_OPTS) $<

clean:
	-/bin/rm -f *.aux *.log *.bbl *.blg *.toc *.fls *.fdb_latexmk *.pdf
