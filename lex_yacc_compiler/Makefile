all: scc 

scc: simple.l simple.y
	lex simple.l
	yacc -d simple.y
	gcc -g -o scc lex.yy.c y.tab.c
	echo "To run tests type \"cd tests; ./testall\"" 

clean:
	rm -f scc lex.yy.c y.tab.c y.tab.h *.out *.output *.s *.scc *.gcc
	rm -f tests/*.s tests/*.scc tests/*.gcc tests/*.gcc.out tests/*.out.scc tests/*.out.gcc tests/total.txt
