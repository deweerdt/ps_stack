/*
   Copyright (c) Frederik Deweerdt
   All rights reserved.

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are met:

   1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.
   2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
   ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
   DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
   ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
   (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
   ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

   The views and conclusions contained in the software and documentation are those
   of the authors and should not be interpreted as representing official policies,
   either expressed or implied, of the FreeBSD Project.
*/
#define _GNU_SOURCE

#include <dlfcn.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/reg.h>
#include <sys/user.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/ptrace.h>

#include <libunwind.h>
#include <libunwind-ptrace.h>

void unwind_bt(int pid)
{
	unw_cursor_t c;
	unw_addr_space_t as;
	int ret;
	void *ui;

	ret = ptrace(PTRACE_ATTACH, pid, 0, 0);
	if (ret < 0 && errno) {
		perror("peek user ATTACH");
		return;
	}
	waitpid(pid, NULL, WCONTINUED);

	as = unw_create_addr_space(&_UPT_accessors, 0);
	if (!as) {
		fprintf(stderr, "create addr space: %d\n", ret);
		return;
	}
	ui = _UPT_create(pid);
	if (!ui) {
		fprintf(stderr, "upt_create: %d\n", ret);
		return;
	}
	ret = unw_init_remote(&c, as, ui);
	if (ret) {
		fprintf(stderr, "remote: %d\n", ret);
		return;
	}

	while (unw_step(&c) > 0) {
		unsigned long long rip;

		unw_get_reg(&c, UNW_REG_IP, (void *)&rip);

		fprintf(stdout, "0x%016llx\n", rip);
	}
	ptrace(PTRACE_DETACH, pid, NULL, NULL);

	unw_destroy_addr_space(as);
	_UPT_destroy(ui);
}

void usage(char *progname)
{
	fprintf(stderr, "Usage: %s <pid>\n", progname);
}
int main(int argc, char **argv)
{
	if (argc != 2) {
		usage(argv[0]);
		exit(EXIT_FAILURE);
	}
	unwind_bt(atoi(argv[1]));
	return 0;
}
