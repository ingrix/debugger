#ifndef __SANDBOX_H__
#define __SANDBOX_H__

#include <unistd.h>

struct sandbox_handle
{
  pid_t p; // process id of the forked process
  /*
  int stdin_fd; 
  int stdout_fd;
  int stderr_fd;
  */
};

int start_sandbox_file(const char* filename, char** argv, char** envp, struct sandbox_handle *handle);
int start_sandbox_fd(int fd, char** argv, char** envp, struct sandbox_handle *handle);

#endif
