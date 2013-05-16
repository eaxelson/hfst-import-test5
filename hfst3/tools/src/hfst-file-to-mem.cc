#include <stdio.h>
#include <cstring>
#include "hfst-commandline.h"
#include <cstdlib>

#include <stdbool.h>
#include "../../back-ends/foma/fomalib.h" // todo: find the exact header where xxmalloc is defined

// Based on a function in foma written by Mans Hulden.                                       
// Read the file 'filename' to memory and return a pointer to it.                            
// Filename "<stdout>" uses stdout for reading.                                              
// Returns NULL if file cannot be opened or read or memory cannot be allocated.              

char * hfst_file_to_mem(const char *filename) {

  FILE   *infile;
  size_t  numbytes;
  char   *buffer;
  infile = (strcmp(filename, "<stdout>") == 0)? stdout : fopen(filename, "r");
  if(infile == NULL)
    {
      error(EXIT_FAILURE, 0, "Error opening file '%s'\n", filename);
      return NULL;
    }
  fseek(infile, 0L, SEEK_END);
  numbytes = ftell(infile);
  fseek(infile, 0L, SEEK_SET);
  // FIX: use malloc instead                 
  buffer = (char*)xxmalloc((numbytes+1) * sizeof(char));
  if(buffer == NULL)
    {
      error(EXIT_FAILURE, 0, "Error allocating memory to read file '%s'\n", filename);
      return NULL;
    }
  if (fread(buffer, sizeof(char), numbytes, infile) != numbytes)
    {
      error(EXIT_FAILURE, 0, "Error reading file '%s' to memory\n", filename);
      return NULL;
    }
  if (strcmp(filename, "<stdout>") != 0)
    {
      fclose(infile);
    }
  *(buffer+numbytes)='\0';
  return(buffer);
}
