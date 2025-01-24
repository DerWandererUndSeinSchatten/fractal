LIB=-lm -lSDL2 -lSDL2main
OPT=-std=c11 -O3

ifeq ("$(shell uname)", "Darwin")
	LNK+=-rpath /usr/local/lib
	LIB+=-framework OpenGL -framework GLUT
	SRC+=macos.c
else
	OPT+=-mavx2
	SRC+=linux.c
endif

testing: DBG=-ggdb -Werror -Wall -Wpedantic
testing: program.c program.h ${SRC}
	gcc -o $@ $< ${SRC} ${LIB} ${OPT} ${LNK} ${DBG}

release: program.c program.h ${SRC}
	gcc -o $@ $< ${SRC} ${LIB} ${OPT} ${LNK}
