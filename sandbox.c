#include <errno.h>
#include <fcntl.h>
#include <sys/ptrace.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/user.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "sandbox.h"

int start_sandbox_fd(int fd, char** argv, char** envp, struct sandbox_handle *handle)
{
  if(handle == NULL)
  {
    return EINVAL;
  }

  handle->p = -1;
  if(argv != NULL)
  {
    //int s,in[2],out[2],err[2];
    //int status;

    /*
    // Create pipes for stdin, stdout, and sterr
    s = pipe(in);
    if(s)
    {
      perror("Could not execute pipe for stdin\n");
      return s;
    }
    s = pipe(out);
    if(s)
    {
      perror("Could not execute pipe for stdout\n");
      return s;
    }
    s = pipe(err);
    if(s)
    {
      perror("Could not execute pipe for stderr\n");
      return s;
    }
    */
    
    handle->p = fork();
    if(handle->p == -1)
    {
      perror("could not fork");
      //return ret
      return ECHILD;
    }

    if(handle->p == 0) // child process
    {
      int r;
      char *null_envp[] = { NULL }; // null environment variable

      r = ptrace(PTRACE_TRACEME, 0, NULL, NULL);
      if(r == -1)
      {
        perror("could not execute PTRACE_TRACEME");

        // kill it with fire, it might be dangerous!
        exit(r);
      }

      /*
      // close the unused side of the pipes
      close(in[1]);
      close(out[0]);
      close(err[0]);
      */

      /*
      // set the stdin/out/err 
      dup2(in[0],STDIN_FILENO); // read side for stdin
      dup2(out[1],STDOUT_FILENO); // write side for stdout
      dup2(err[1],STDERR_FILENO); // write side for stderr
      */

      // since PTRACE_TRACEME has been issued, this will trap just after the
      // program launches this trap starts before the dynamic loader gets
      // executed, so libraries are loaded as per the normal ELF loading routine
      // but no shared library code is run 
      //
      // the use of the env variable is necessary on linux because an invalid
      // argument error will occur if envp is null in the fexecve call, though
      // to all other exec() calls it is taken from the environ
      r = fexecve(fd, argv, envp ? envp : null_envp);

      perror("fexecve failed");
      exit(r);
    }

    // wait for the process to start, will cause a status change since the
    // process is being traced (can use WIFSTOPPED(status) to determine this)
    //waitpid(handle->p,&status,0);
    waitpid(handle->p,NULL,0);

    /*
    else
    {
      // close the unused sides of the pipes
      close(in[0]);
      close(out[1]);
      close(out[2]);

      handle->stdin_fd = in[1]; // save the write side of the pipe for parent
      handle->stdout_fd = out[0]; // the read side for the parent
      handle->stderr_fd = err[0]; // the read side for the parent
    }
    */
  }
  return 0;
}

int start_sandbox_file(const char* filename, char** argv, char** envp, struct sandbox_handle *handle)
{
  int ret = 0;
  // if there is no argv, then there is no point launching
  if(filename == NULL || argv == NULL || handle == NULL)
  {
    ret = EINVAL;
  }
  else
  {
    int fd = open(filename,O_RDONLY); // make sure the file is real
    if(fd != -1)
    {
      ret = start_sandbox_fd(fd, argv, envp, handle);
      close(fd);
    }
  }
  return ret;
}
