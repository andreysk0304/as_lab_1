clear_data:
	rm -rf main tests

build_files:
	gcc -o main main.c tree.c
	gcc -o tests tests.c tree.c

run:
	./main

test:
	./tests

main: build_files run clear_data

test_all: build_files test clear_data