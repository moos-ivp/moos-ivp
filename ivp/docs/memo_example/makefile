.SILENT:

.PHONY : all clean

LATEX     = pdflatex
PAPER     = ihuxley
LDIR      = $(CURDIR)/figures/

PNGFILES = $(patsubst $(LDIR)%.fig, 				\
	   $(LDIR)%.png,$(wildcard $(LDIR)*.fig))		\

main: $(PNGFILES)
	$(LATEX) $(PAPER)
	$(LATEX) $(PAPER)

main-quick: $(PNGFILES)
	$(LATEX) $(PAPER)

pdfv:
	open $(PAPER).pdf

png: $(PNGFILES)
	echo Done creating PNG files.

pdfo: main
	open $(PAPER).pdf

pdfoq: main-quick
	open $(PAPER).pdf &

clean:
	rm -f $(PAPER).dvi *~  $(PAPER).log $(PAPER).pdf  $(PAPER).ind
	rm -f $(PAPER).bbl $(PAPER).blg $(PAPER).ps $(PAPER).aux
	rm -f $(EPSFILES) $(PAPER).toc $(PAPER).idx $(PAPER).ilg
	rm -f $(LDIR)*.bak $(LDIR)*~ $(PAPER).and $(PAPER).adx $(PAPER).out

$(PNGFILES): %.png: %.fig
	echo $@
	fig2dev -L png $< $@;


