
TEX=McNTag1_Perm0Top1_el.pdf McNTag1_Perm0Top1_mu.pdf McNTag1_Perm0Top2_el.pdf \
	McNTag1_Perm0Top2_mu.pdf McNTag1_Perm0Top1.pdf  McNTag1_Perm0Top2.pdf

LATEX=latex

target: ${TEX}

# Build all latex Files to dvi and pdf
./%.pdf: ./%.tex
	@echo " "
	@echo "--------->>>>>> Compiling" $<
	-$(LATEX) $<
	-dvipdf $(<:.tex=.dvi)
	@echo "----->>> Done."

clean:
	rm -f *.aux *.log *.ps *.eps *.out *.dvi  *~
