#include "InputReader.h"
#include "../config.h"
extern InputReader input_reader;
#undef YY_INPUT
#define YY_INPUT( buf, result, max_size ) { \
    char c = input_reader.input();	    \
if (c == 0) {	        		    \
result = YY_NULL;			    \
}					    \
else {					    \
buf[0] = c;				    \
result = 1;				    \
}					    \
} 
