#ifndef FORMAT_H
#define FORMAT_H

#include <math.h>

#define _FORMAT_NIL 'n'
#define _FORMAT_TRUE 'T'
#define _FORMAT_FALSE 'F'
#define _FORMAT_INF 'I'
#define _FORMAT_MINF 'i'
#define _FORMAT_NAN 'N'
#define _FORMAT_NUMBER 'D'
#define _FORMAT_STRING '"'
#define _FORMAT_TABLE_START '{'
#define _FORMAT_TABLE_END '}'
#define _FORMAT_ARRAY_HASH_SEP '|'
#define _FORMAT_TABLE_REF '@'

#define _FORMAT_NUMBER_LEN 17
#define _FORMAT_NUMBER_MAX 24

#define is_int(x) (fmod((x), 1.0) == 0.0)

#endif
