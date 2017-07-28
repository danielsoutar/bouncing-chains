all: clean sudoku clear

sudoku: sudoku.h io.h
	clang -Wall -Wextra -o sudoku sudoku.c io.c program.c

clean:
	rm sudoku

clear:
	clear
