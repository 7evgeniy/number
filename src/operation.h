#ifndef OPERATION_H
#define OPERATION_H

#include "configure.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
#define DEFAULT(v) = v
#else
#define DEFAULT(v)
#endif

digit_t* make_string(int capacity);
digit_t* copy_string(digit_t* str);
void normalise(digit_t* str);
void add(digit_t* fst, digit_t* snd, digit_t* res, int shift DEFAULT(0) );
void sub(digit_t* fst, digit_t* snd, digit_t* res, int shift DEFAULT(0) );
int cmp(digit_t* fst, digit_t* snd);
void mul_str(digit_t* from, digit_t dgt, digit_t* to);
int decimal(digit_t* str, dec_t* dec);
digit_t approximate(digit_t* fst, digit_t* snd, int shift);

#ifdef __cplusplus
}
#endif

#endif
