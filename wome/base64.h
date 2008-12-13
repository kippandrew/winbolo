#ifndef _BASE_64_H
#define _BASE_64_H

#include <ctype.h> 
#include <stdio.h>

#include <stdlib.h>
#include <string.h> 

#include "global.h"

long base64_encode(BYTE *src, long srclength, BYTE *target, long targsize);

/* skips all whitespace anywhere.
   converts characters, four at a time, starting at (or after)
   src from base - 64 numbers into three 8 bit bytes in the target area.
   it returns the number of data bytes stored at the target, or -1 on error.
 */

long base64_decode(BYTE *src, BYTE *target, long targsize); 

#endif /* _BASE_64_H */

