HDR =		ast.h util.h std.h missing.h parser.h traverse.h symbol.h stack.h pretty.h analyser.h codegen.h n7c.h 

OBJ =		ast.o util.o n7c.o parser.o scanner.o traverse.o symbol.o stack.o analyser.o codegen.o pretty.o

CC = 		gcc -Wall -g

n7c:		$(OBJ)
		$(CC) -o n7c $(OBJ)

parser.c parser.h: parser.y util.h ast.h std.h missing.h
		bison --debug -v -d parser.y -o parser.c

scanner.c:	scanner.l parser.h std.h ast.h
		flex -s -oscanner.c scanner.l

clean:
		/bin/rm $(OBJ) parser.c parser.h parser.output scanner.c 

$(OBJ):		$(HDR)
