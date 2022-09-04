all: test

test: test.o fs.o
	gcc -o $@ $^


test.o: test.c fs.h
	gcc -c test.c

fs.o: fs.c fs.h
	gcc -c fs.c

clean:
	rm -rf test fs_data *.o