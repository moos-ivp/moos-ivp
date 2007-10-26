.SILENT:

.PHONY : all clean ps

PAPER    = artifactsearch
PAPERPDF = artifactsearch
LDIR     = $(CURDIR)/figures/

PNGFILES = $(patsubst $(LDIR)%.png, \
	   $(LDIR)%.eps,$(wildcard $(LDIR)*.png))

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

dvi: $(PNGFILES)
	rubber $(PAPER)

force: $(PNGFILES)
	rubber -f $(PAPER)

ps: $(PNGFILES)
	rubber --ps $(PAPER)

clean:
	rm -f *.dvi *~   *.log $(PAPER).pdf
	rm -f *.bbl *.blg $(PAPER).ps *.aux
	rm -f $(LDIR)*.bak $(LDIR)*~ 
	rm -f $(PNGFILES) $(PAPERPDF).pdf

$(PNGFILES): %.eps: %.png
	cd $(LDIR); convert $< $@;

