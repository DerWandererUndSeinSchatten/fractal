LIB=-lm -lSDL2 -lSDL2main
OPT=-std=c11 -O3

ifeq ("$(shell uname)", "Darwin")
	LNK+=-rpath /usr/local/lib
	LIB+=-framework OpenGL -framework GLUT
endif

testing: DBG=-ggdb -Werror -Wall -Wpedantic
testing: program.c program.h
	gcc -o $@ $< ${LIB} ${OPT} ${LNK} ${DBG}

release: program.c program.h
	gcc -o $@ $< ${LIB} ${OPT} ${LNK}
