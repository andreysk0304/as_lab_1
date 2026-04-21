.PHONY: main test_all build_main run_main clear_main build_tests run_tests clear_tests

main: build_main run_main clear_main
test_all: build_tests run_tests clear_tests

build_main:
	gcc -o main main.c tree.c -lm

run_main:
	./main

clear_main:
	rm -rf main

build_tests:
	gcc -o tests tests.c tree.c -lm

run_tests:
	./tests

clear_tests:
	rm -rf tests
