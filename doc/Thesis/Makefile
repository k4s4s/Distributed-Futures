all: thesis report

thesis:
	latex thesis
	--bibtex thesis
	latex thesis
	dvipdfm thesis

report:	
	latex report
	--bibtex report
	latex report
	dvipdfm report

clean:
	rm -rf *.aux *.bbl *.blg *.log *.out *.toc *.lof *.lot *.dvi *.pdf

clear:
	rm -rf *.aux *.bbl *.blg *.log *.out *.toc *.lof *.lot
