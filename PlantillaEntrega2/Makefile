texfile = PlantillaEntrega2
bibfile = Referencias.bib

# Regla principal
documento: $(texfile).tex $(bibfile)
	# El guion (-) al inicio hace que make ignore errores y continue
	-pdflatex -interaction=nonstopmode $(texfile).tex
	-bibtex $(texfile)
	-pdflatex -interaction=nonstopmode $(texfile).tex
	pdflatex -interaction=nonstopmode $(texfile).tex

clean:
	rm -f *.aux *.bbl *.blg *.dvi *.log *.out *.spl *.pdf *.toc *.snm *.nav *.vrb