#include <errno.h>
#include <stdio.h>
#include <sys/ptrace.h>
#include <sys/user.h>
#include <sys/wait.h>
#include <string.h>

#include "sandbox.h"

char *cmd = "hello_world";

int main()
{
  int ret = 42;
  int status;
  int res; 
  int filedes[2];
  char*cmdv[]={cmd,NULL};
  struct sandbox_handle h;

  char buf[1024];

  res = start_sandbox_file(cmdv[0],cmdv,NULL,&h);

  if(res == -1)
  {
    printf("Could not launch command %s\n", cmdv[0]);
  }
  else
  {
    printf("Forked pid %d\n",h.p);
    while(1)
    {
      struct user_regs_struct regs;

      if(ptrace(PTRACE_GETREGS, h.p, NULL, &regs) == -1)
      {
        printf("ptrace failed: %s\n", strerror(errno));
      }

      printf("%%rsp: %p, %%rsp: %p\n",regs.rip,regs.rsp);
      printf("  syscall: %d\n",regs.orig_rax);
      ptrace(PTRACE_SYSCALL,h.p,NULL,NULL);
      waitpid(h.p,&status,0);
      if(WIFEXITED(status))
      {
        break;
      }
    }
  }
  return 0;
}
