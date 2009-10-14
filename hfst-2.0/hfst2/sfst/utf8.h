
/*******************************************************************/
/*                                                                 */
/*     File: utf8.h                                                */
/*   Author: Helmut Schmid                                         */
/*  Purpose:                                                       */
/*  Created: Mon Sep  5 17:49:16 2005                              */
/* Modified: Thu Apr 27 10:13:09 2006 (schmid)                     */
/*                                                                 */
/*******************************************************************/

#include <vector>
using std::vector;

unsigned int utf8_to_int( char *s );
unsigned int utf8_to_int( char **s );
char *int_to_utf8( unsigned int );

unsigned int utf8toint( char *s );
unsigned int utf8toint( char **s );
char *int2utf8( unsigned int );
