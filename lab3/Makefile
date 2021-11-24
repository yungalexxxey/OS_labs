all:
	gcc parent.c -o prog.out -lrt -pthread -Wall -Wextra
	gcc child.c -o child.out -lrt -pthread -Wall -Wextra

clean:
	touch res res2
	rm prog.out child.out res res2
	touch res res2