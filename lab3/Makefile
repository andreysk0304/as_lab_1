.PHONY: main test_all build_main run_main clear_main build_tests run_tests clear_tests

main: build_main run_main clear_main
test_all: build_tests run_tests clear_tests

build_main:
	gcc -o main main.c parse.c stack.c structures.c tree.c -lm

clear_main:
	rm -rf main

run_main:
	./main

build_tests:
	gcc -o tests tests.c parse.c stack.c structures.c tree.c -lm

clear_tests:
	rm -rf tests

run_tests:
	./tests
