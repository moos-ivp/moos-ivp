.SILENT:

.PHONY : all clean ps

PAPER    = artifactsearch
PAPERPDF = artifactsearch
LDIR     = $(CURDIR)/figures/

EPSFILES = $(patsubst $(LDIR)%.fig, \
	   $(LDIR)%.eps,$(wildcard $(LDIR)*.fig))

all: dvi
	dvips -t letter -o $(PAPER).ps $(PAPER).dvi
	gnome-open $(PAPER).dvi &

view:
	gnome-open $(PAPER).dvi &

installpdf: pdf
	cp -f $(PAPERPDF).pdf ~/

pdf: dvi
	dvipdfm -p letter $(PAPER).dvi
	mv $(PAPER).pdf $(PAPERPDF).pdf
	gnome-open $(PAPERPDF).pdf &

dvi: $(EPSFILES)
	rubber $(PAPER)

ps: dvi
	dvips -t letter -o $(PAPER).ps $(PAPER).dvi

clean:
	rm -f *.dvi *~   *.log $(PAPER).pdf
	rm -f *.bbl *.blg $(PAPER).ps *.aux
	rm -f $(LDIR)*.bak $(LDIR)*~ 
	rm -f $(EPSFILES) $(PAPERPDF).pdf

$(EPSFILES): %.eps: %.fig
	cd $(LDIR); fig2dev -L eps $< $@;

