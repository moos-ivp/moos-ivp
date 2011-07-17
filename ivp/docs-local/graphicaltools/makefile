.SILENT:

.PHONY : all clean ps

PAPER    = graphicaltools
PAPERPDF = graphicaltools
LDIR     = $(CURDIR)/figures/

EPSFILES = $(patsubst $(LDIR)%.fig, \
	   $(LDIR)%.eps,$(wildcard $(LDIR)*.fig))

all: dvi

view:
	gnome-open $(PAPER).dvi &

viewpdf:
	gnome-open $(PAPER).pdf &

installpdf: pdf
	cp -f $(PAPER).pdf ~/$(PAPERPDF).pdf

pdf:
	rubber --ps --pdf $(PAPER)
	gnome-open $(PAPERPDF).pdf &

dvi: $(EPSFILES)
	rubber $(PAPER)

force: $(EPSFILES)
	rubber -f $(PAPER)

ps: $(EPSFILES)
	rubber --ps $(PAPER)

clean:
	rm -f *.dvi *~   *.log $(PAPER).pdf
	rm -f *.bbl *.blg $(PAPER).ps *.aux
	rm -f $(LDIR)*.bak $(LDIR)*~ 
	rm -f $(EPSFILES) $(PAPERPDF).pdf

$(EPSFILES): %.eps: %.fig
	cd $(LDIR); fig2dev -L eps $< $@;

