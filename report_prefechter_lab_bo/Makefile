.PHONY: just_pdf
just_pdf: report_bop.pdf
	latexmk -c
	rm *.bbl

report_bop.pdf:
	latexmk -pdf -pdflatex="pdflatex" report_bop.tex

.PHONY: clean
clean:
	latexmk -CA
	rm *.bbl
