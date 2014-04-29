all: ps_stack

ps_stack: ps_stack.c
	gcc -static -Wall -o $@ $< -lunwind-ptrace -lunwind-x86_64 -lunwind -ldl

dist: ps_stack
	mkdir bin
	cp ps_stack bin/ps_stack

clean: ps_stack
	rm ps_stack

