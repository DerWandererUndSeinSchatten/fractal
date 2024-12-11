LIB=-lm
OPT=-std=c11 -O3

testing: DBG=-ggdb -Werror -Wall -Wpedantic
testing: program.c program.h
	gcc -o $@ $< ${LIB} ${OPT} ${DBG}

release: program.c program.h
	gcc -o $@ $< ${LIB} ${OPT}
