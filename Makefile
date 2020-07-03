HDR =		ast.h std.h missing.h parser.h pretty.h n7c.h 

OBJ =		n7c.o parser.o scanner.o pretty.o 

CC = 		gcc -Wall

n7c:		$(OBJ)
		$(CC) -o n7c $(OBJ)

parser.c parser.h: parser.y ast.h std.h missing.h
		bison --debug -v -d parser.y -o parser.c

scanner.c:	scanner.l parser.h std.h ast.h
		flex -s -oscanner.c scanner.l

clean:
		/bin/rm $(OBJ) parser.c parser.h parser.output scanner.c

$(OBJ):		$(HDR)
