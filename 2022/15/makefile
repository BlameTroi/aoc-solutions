# shell or sample makefile for fpc
# see https://bytes.usc.edu/cs104/wiki/makefile/ for a quick reference

solution: solution.pas
	fpc -glw3 -Ciort -ap -Sgix -Sh- -vewnlb -Fi../../common/ solution

run:
	./solution ../../../datasets/2022/15/dataset.txt >output.txt

test:
	./solution ../../../datasets/2022/15/testone.txt >testing.txt

clean:
	rm -f solution *.o output.txt testing.txt
