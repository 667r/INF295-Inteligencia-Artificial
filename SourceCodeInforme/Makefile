
texfile = InformeEntrega1
bibfile = Referencias.bib

documento: $(texfile).tex $(bibfile)
	pdflatex $(texfile).tex
	bibtex $(texfile).aux
	pdflatex $(texfile).tex
	pdflatex $(texfile).tex

clean:
	rm -f *~
	rm -f $(texfile).aux
	rm -f $(texfile).bbl
	rm -f $(texfile).blg
	rm -f $(texfile).dvi
	rm -f $(texfile).log
	rm -f $(texfile).out
	rm -f $(texfile).spl
	rm -f $(texfile).pdf
	rm -f $(texfile).tex.backup
	rm -f $(bibfile).backup
