.PHONY : all clean

PAPER    = artifactsearch
PAPERPDF = artifactsearch
LDIR     = $(CURDIR)/figures/

# Looking at all the png files in the LDIR (wildcard command),
# find the phrase .png and replace it with .eps
# This creates a list of target eps files to make from pngs.
PNGFILES = $(patsubst $(LDIR)%.png,$(LDIR)%.eps,$(wildcard $(LDIR)*.png))

all: dvi

view:
	gnome-open $(PAPER).dvi &

viewpdf: pdf
	gnome-open $(PAPER).pdf &

installpdf: pdf
	cp -f $(PAPER).pdf ~/$(PAPERPDF).pdf

pdf: $(PNGFILES)
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
	convert $< $@;

