first: my_lib2.so my_lib1.so my_lib1.o
	gcc dynmain.c -o dynmain -ldl
	gcc statmain.c mylib1.c -lm -o statmain
second: my_lib2.so my_lib1.so my_lib2.o
	gcc dynmain.c -o dynmain -ldl
	gcc statmain.c mylib2.c -lm -o statmain
my_lib2.so: my_lib2.o
	gcc -shared mylib2.o -o my_lib2.so -lm
my_lib1.so:my_lib1.o
	gcc -shared mylib1.o -o my_lib1.so -lm
my_lib1.o:
	gcc -fPIC -c mylib1.c -lm
my_lib2.o:
	gcc -fPIC -c mylib2.c -lm
clean:
	rm *.o *.so dynmain statmain