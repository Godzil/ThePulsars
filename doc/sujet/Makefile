DVIPS = dvips
PS    = sujet.ps
LATEX = latex
RM    = rm -f

all: ps

ps: $(PS)

clean:
	$(RM) *~ *.aux *.log *.toc *.bbl *.blg

.PHONY: all ps clean

%.ps: %.dvi
	$(DVIPS) $< -o $@

%.dvi: %.tex
	$(LATEX) $<
	$(LATEX) $<

