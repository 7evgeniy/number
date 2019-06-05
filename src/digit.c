#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "operation.h"

#define IND(i) ((i)+2)
#define STR(s) (((s)+2)*sizeof(digit_t))

digit_t* make_string(int capacity) {
	digit_t *str = malloc(STR(capacity));
	str[0] = capacity;
	for (int i = 1; i < capacity+2; ++ i)
		str[i] = 0;
	return str;
}

digit_t* copy_string(digit_t *o) {
	int sz = (STR(o[0]));
	digit_t *str = malloc(sz);
	return memcpy(str, o, sz);
}

void normalise(digit_t* str) {
	int size = str[0];
	for (int i = size+1; !str[i] && i >= 2; -- i)
		-- size;
	str[1] = size;
}

void add(digit_t* fst, digit_t* snd, digit_t* res, int shift) {
	proc_t carry = 0;
	for (int i = 0; i < res[0]; ++ i) {
		proc_t d1 = 0, d2 = 0;
		if (i < fst[1])
			d1 = fst[IND(i)];
		if (i >= shift && i < snd[1] + shift)
			d2 = snd[IND(i-shift)];
		carry += (d1+d2);
		res[IND(i)] = carry & DGT_LOW;
		carry >>= DGT_LEN;
	}
	normalise(res);
}

void sub(digit_t* fst, digit_t* snd, digit_t* res, int shift) {
	proc_t borrow = 0;
	for (int i = 0; i < fst[1]; ++ i) {
		proc_t d1 = fst[IND(i)], d2 = 0;
		if (i >= shift && i < snd[1] + shift)
			d2 = snd[IND(i-shift)];
		borrow = d1-d2-borrow;
		res[IND(i)] = borrow & DGT_LOW;
		borrow >>= (DGT_LEN*2 - 1);
	}
	normalise(res);
}

int cmp(digit_t* fst, digit_t* snd) {
	if (fst[1] > snd[1])
		return 1;
	if (fst[1] < snd[1])
		return -1;
	for (int i = fst[1]-1; i >= 0; -- i) {
		if (fst[IND(i)] > snd[IND(i)])
			return 1;
		if (fst[IND(i)] < snd[IND(i)])
			return -1;
	}
	return 0;
}

void mul_str(digit_t* from, digit_t dgt, digit_t* to) {
	int i;
	proc_t carry = 0, prev = 0;
	for (i = 0; i < from[1]; ++ i) {
		proc_t mul = from[IND(i)] * (proc_t)dgt;
		carry += (prev + (mul & DGT_LOW));
		to[IND(i)] = (carry & DGT_LOW);
		carry >>= DGT_LEN;
		prev = (mul & DGT_HIGH) >> DGT_LEN;
	}
	to[IND(i)] = prev + carry;
	normalise(to);
}

int decimal(digit_t *str, dec_t *dec) {
	int count = 0;
	int size = str[1];
	while (size > 0) {
		int i = size - 1;
		proc_t div = str[IND(i)];
		digit_t res = div / 10;
		proc_t prev = div % 10;
		if (res == 0)
			-- size;
		str[IND(i)] = res;
		-- i;
		while (i >= 0) {
			div = (prev << DGT_LEN) | str[IND(i)];
			res = div / 10;
			prev = div % 10;
			str[IND(i)] = res;
			-- i;
		}
		dec[count++] = prev;
	}
	return count;
}

digit_t approximate(digit_t *fst, digit_t *snd, int shift) {
	int last = snd[1]-1;
	proc_t div = snd[IND(last)]+1;
	proc_t low = fst[IND(last+shift)];
	proc_t high = 0;
	if (fst[1] > last+shift+1)
		high = (proc_t)(fst[IND(last+shift+1)]) << DGT_LEN;
	digit_t res = (high | low) / div;
	return res ? res : 1;
}
