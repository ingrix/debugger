#ifndef __ELF_H__
#define __ELF_H__

#include <elf.h>
#include <stdio.h>

// return a FILE pointer to an opened ELF file
// this function checks that the file is actually an ELF executable
FILE* open_elf(char *const filename)
{
  FILE* f = fopen(filename,"r");
  if(!f)
  {
    perror("Could not open ELF executable file");
    return f;
  }
}

int get_elf_bitness(FILE* f)
{
}

#endif // __ELF_H__
