
/*******************************************************************/
/*                                                                 */
/*  FILE     basic.C                                               */
/*  MODULE   basic                                                 */
/*  PROGRAM  SFST                                                  */
/*  AUTHOR   Helmut Schmid, IMS, University of Stuttgart           */
/*                                                                 */
/*  PURPOSE                                                        */
/*                                                                 */
/*******************************************************************/

#include <stdlib.h>
#include <string.h>

#include "basic.h"

using std::ios;

bool Switch_Bytes=false;

/*
  Read a block of data ( of size amount ) into the place p from the buffer. 
*/
void * BinaryReader::read( void * p, size_t amount ) {
  
  //if ( in.eof() )
  //  return NULL;

  if (in.eof())
    return NULL;
  if (in.bad()) {
    fprintf(stderr, "An error occurred when reading input.\n\n");
    exit(1);
  }
  char *pp = (char*)(p);
  in.tellg();
  in.read(pp, amount);
  return pp;
};


/*
  Read a string, of size maximally max_size, from the buffer into the char * str 
*/
void * BinaryReader::read_string(char * str, size_t max_size ) {

  if (in.eof())
    return NULL;
  if (in.bad()) {
    fprintf(stderr, "An error occurred when reading input.\n\n");
    exit(1);
  }
  in.tellg();
  in.getline(str, max_size, '\0');
  return str;
};

/* Return the next character from the stream. */

char BinaryReader::get_one_character(void) {
  if (in.eof()) {
    fprintf(stderr, "The input ended unexpectedly.\n\n");
    exit(1);
  }
  if (in.bad()) {
    fprintf(stderr, "An error occurred when reading input.\n\n");
    exit(1);
  }
  //return in.get();
  in.tellg();
  return (char)(in.get());

};

/*******************************************************************/
/*                                                                 */
/*  fst_strdup                                                     */
/*                                                                 */
/*******************************************************************/

char* fst_strdup(const char* pString)

{
  char* pStringCopy = (char*)malloc(strlen(pString) + 1);
  if (pStringCopy == NULL) {
    fprintf(stderr, "\nError: out of memory (malloc failed)\naborted.\n");
    exit(1);
  }
  strcpy(pStringCopy, pString);
  return pStringCopy;
}


/*******************************************************************/
/*                                                                 */
/*  read_string                                                    */
/*                                                                 */
/*******************************************************************/

int read_string( char *buffer, int size, FILE *file )

{
  for( int i=0; i<size; i++ ) {
    int c=fgetc(file);
    if (c == EOF || c == 0) {
      buffer[i] = 0;
      return (c==0);
    }
    buffer[i] = (char)c;
  }
  buffer[size-1] = 0;
  return 0;
}

// Added by Miikka Silfverberg
int read_string( char *buffer, int size, istream &file )

{
  for( int i=0; i<size; i++ ) {
    int c=file.get();
    if (c == EOF || c == 0) {
      buffer[i] = 0;
      return (c==0);
    }
    buffer[i] = (char)c;
  }
  buffer[size-1] = 0;
  return 0;
}


/*******************************************************************/
/*                                                                 */
/*  read_num                                                       */
/*                                                                 */
/*******************************************************************/

size_t read_num( void *p, size_t n, FILE *file )

{
  char *pp=(char*)p;
  size_t result=fread( pp, 1, n, file );
  if (Switch_Bytes) {
    size_t e=n/2;
    for( size_t i=0; i<e; i++ ) {
      char tmp=pp[i];
      pp[i] = pp[--n];
      pp[n] = tmp;
    }
  }
  return result;
}

// Added by Miikka Silfverberg
size_t read_num( void *p, size_t n, istream &file )

{
  char *pp=(char*)p;
  file.get( pp, n );
  if (Switch_Bytes) {
    size_t e=n/2;
    for( size_t i=0; i<e; i++ ) {
      char tmp=pp[i];
      pp[i] = pp[--n];
      pp[n] = tmp;
    }
  }
  return 1;
}

