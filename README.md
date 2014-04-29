ps_stack
========

Display the stack of a running program.

`ps_stack` attaches itself to a given PID, and prints the addresses in
the stack at the moment it attaches using libunwind, it then detaches
immediately.
This is meant for programs that have debugging symbols, that is mostly
programs you wrote.

There's a statically compiled binary under bin/ for convenience

Sample usage
============

	$ cat test.c
	#include <unistd.h>

	void g(void)
	{
		pause();
	}
	void f(void)
	{
		g();
	}

	int main(void)
	{
		f();
		return 0;
	}
	$ gcc -g -o test test.c
	$ test &
	[1] 12345
	$ pid=12345; for i in `./ps_stack $pid`; do addr2line -p -e /proc/$pid/exe $i; done
	g at ps_stack/test.c:6
	f at ps_stack/test.c:10
	main at ps_stack/test.c:15
	??
	??:0
	_start at ??:0

