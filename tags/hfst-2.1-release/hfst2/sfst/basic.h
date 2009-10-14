
/*******************************************************************/
/*                                                                 */
/*  FILE     basic.h                                               */
/*  MODULE   basic                                                 */
/*  PROGRAM  SFST                                                  */
/*  AUTHOR   Helmut Schmid, IMS, University of Stuttgart           */
/*                                                                 */
/*  PURPOSE                                                        */
/*                                                                 */
/*******************************************************************/

#ifndef _BASIC_H_
#define _BASIC_H_

#include <stdio.h>
#include <iostream>
using std::istream;
using std::ostream;

extern bool Switch_Bytes;

class BinaryReader {
  
 private:
  istream &in;

 public:
  BinaryReader( istream &file ) :
    in(file) {};
  
  void * read(void * p, size_t amount);
  void * read_string(char * str, size_t max_size);

  char get_one_character( void );

};

char* fst_strdup(const char* pString);
int read_string( char *buffer, int size, FILE *file );
size_t read_num( void *p, size_t size, FILE *file );
size_t read_num( void *p, size_t size, istream &file );
int read_string( char *buffer, int size, istream &file );

#endif
