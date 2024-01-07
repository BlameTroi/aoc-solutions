# shell or sample makefile for fpc
# see https://bytes.usc.edu/cs104/wiki/makefile/ for a quick reference

solution: solutionone.pas solutiontwo.pas
	fpc -glw3 -Ciort -ap -Sgix -Sh- -vewnlb -Fi../../common/ solutionone
	fpc -glw3 -Ciort -ap -Sgix -Sh- -vewnlb -Fi../../common/ solutiontwo

run:
	./solutionone ../../../datasets/2023/07/dataset.txt >output.txt
	./solutiontwo ../../../datasets/2023/07/dataset.txt >>output.txt

test:
	./solutionone ../../../datasets/2023/07/testone.txt >testing.txt
	./solutiontwo ../../../datasets/2023/07/testone.txt >>testing.txt

clean:
	rm -f solutionone solutiontwo *.o output.txt testing.txt
