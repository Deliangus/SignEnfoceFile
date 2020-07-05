main:main.o
	gcc -o main main.o
	del *.o
	-main.exe Cases/Correct/23.code
	-del *.exe
	-del main

main.o: main.c Scanner.c
	gcc -c main.c
	
clean:
	del *.exe
	del *.o
	del *.out