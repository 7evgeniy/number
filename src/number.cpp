#include "number.h"
#include "operation.h"
#include <climits>
#include <cctype>
#include <cstdlib>
#include <algorithm>

#define CONV(array) ((digit_t*)(array))
#define CAP(array) (CONV(array)[0])
#define SIZ(array) (CONV(array)[1])
#define DIG(array, i) (CONV(array)[(i)+2])

Number::Number(unsigned int number, bool negative) {
	if (number) {
		int size = 0;
		const int cap = 1 + (sizeof(unsigned int)-1)/sizeof(digit_t);
		_array = make_string(cap);
		if (cap > 1) {
			int shift = sizeof(digit_t)*CHAR_BIT;
			while (number) {
				DIG(_array, size) = number;
				number >>= shift;
				++ size;
			}
		}
		else {
			DIG(_array, 0) = number;
			++ size;
		} 
		SIZ(_array) = size;
		_sign = negative ? -1 : 1;
	}
	else {
		_array = make_string(0);
		_sign = 0;
	}
}

Number::Number(const std::string &str) : _sign(0) {
	if (str.empty()) {
		invalidate();
		return;
	}
	bool negative = false;
	int i = 0;
	if (str[i] == '-') {
		negative = true;
		++ i;
	}
	if (str[i] == '+')
		++ i;
	_array = make_string((str.size()/STR_FACTOR) + 2);
	digit_t digit[3] = {1, 1, 1};
	for (; i < str.size(); ++ i) {
		if (!std::isdigit(str[i])) {
			invalidate();
			return;
		}
		if (str[i] != '0')
			_sign = negative ? -1 : 1;
		DIG(digit, 0) = str[i]-'0';
		mul_str(CONV(_array), 10, CONV(_array));
		add(CONV(_array), digit, CONV(_array));
	}
}

std::string Number::toString() const {
	if (!valid())
		return "ОШИБКА";
	if (!toBool())
		return "0";
	std::string str;
	int length = SIZ(_array)*(STR_FACTOR+1)+2;
	str.reserve(length);
	if (_sign < 0)
		str.push_back('-');
	dec_t* dec = new dec_t[length];
	digit_t* copy = copy_string(CONV(_array));
	length = decimal(copy, dec);
	free(copy);
	for (int i = length-1; i >= 0; -- i)
		str.push_back(dec[i]+'0');
	delete[] dec;
	return str;
}

Number Number::invalidNumber() {return Number(nullptr, 0);}
void Number::invalidate() {std::free(_array); _array = nullptr;}
void Number::negate() {_sign = -_sign;}
bool Number::valid() const {return _array;}
digit_t Number::toDigit() const {return (_array && SIZ(_array)) ? DIG(_array, 0) : 0;}
int Number::toSign() const {return _sign;}
bool Number::toBool() const {return _sign && valid();}

Number operator+(const Number& fst, const Number& snd) {
	if (!fst.valid() || !snd.valid())
		return Number::invalidNumber();
	return Number::addition(CONV(fst._array), CONV(snd._array), fst._sign, snd._sign);
}

Number operator-(const Number& fst, const Number& snd) {
	if (!fst.valid() || !snd.valid())
		return Number::invalidNumber();
	return Number::addition(CONV(fst._array), CONV(snd._array), fst._sign, - snd._sign);
}

Number operator*(const Number& n1, const Number& n2) {
	if (!n1.valid() || !n2.valid())
		return Number::invalidNumber();
	int size1 = SIZ(n1._array), size2 = SIZ(n2._array);
	digit_t *sum = make_string(size1+size2);
	digit_t *str = make_string(size1+1);
	for (int i = 0; i < size2; ++ i) {
		mul_str(CONV(n1._array), DIG(n2._array, i), str);
		add(sum, str, sum, i);
	}
	free(str);
	return Number(sum, n1._sign * n2._sign);
}

Number operator/(const Number& n1, const Number& n2) {Number n = n1; return n.subdiv(n2);}
Number operator%(const Number& n1, const Number& n2) {Number n = n1; n.subdiv(n2); return n;}

Number Number::subdiv(const Number& o) {
	if (!valid() || !o.toBool())
		return invalidNumber();
	if (!toBool())
		return Number();
	digit_t *quo = div_fragment(CONV(_array), CONV(o._array));
	sign_t s = _sign;
	if (!SIZ(_array))
		_sign = 0;
	return Number(quo, s * o._sign);
}

bool operator<(const Number& fst, const Number& snd) {
	if (fst._sign == snd._sign) {
		int c = cmp(CONV(fst._array), CONV(snd._array));
		return fst._sign < 0 ? c > 0 : c < 0;
	}
	return fst._sign < snd._sign;
}

bool operator>(const Number& fst, const Number& snd) {
	if (fst._sign == snd._sign) {
		int c = cmp(CONV(fst._array), CONV(snd._array));
		return fst._sign < 0 ? c < 0 : c > 0;
	}
	return fst._sign > snd._sign;
}

bool operator<=(const Number& fst, const Number& snd) {
	if (fst._sign == snd._sign) {
		int c = cmp(CONV(fst._array), CONV(snd._array));
		return fst._sign < 0 ? c >= 0 : c <= 0;
	}
	return fst._sign < snd._sign;
}

bool operator>=(const Number& fst, const Number& snd) {
	if (fst._sign == snd._sign) {
		int c = cmp(CONV(fst._array), CONV(snd._array));
		return fst._sign < 0 ? c <= 0 : c >= 0;
	}
	return fst._sign > snd._sign;
}

Number& Number::operator+= (const Number& o) {
	if (!valid() || !o.valid()) {
		invalidate();
		return *this;
	}
	local_addition(CONV(_array), CONV(o._array), _sign, o._sign);
	return *this;
}

Number& Number::operator-= (const Number& o) {
	if (!valid() || !o.valid()) {
		invalidate();
		return *this;
	}
	local_addition(CONV(_array), CONV(o._array), _sign, - o._sign);
	return *this;
}

Number& Number::operator*= (const Number& other) {
	*this = *this * other;
	return *this;
}

Number& Number::operator/= (const Number& other) {
	*this = *this / other;
	return *this;
}

Number& Number::operator%= (const Number& other) {
	*this = *this % other;
	return *this;
}

Number Number::operator- () const {
	if (!valid())
		return *this;
	return Number(copy_string(CONV(_array)), -_sign);
}

Number Number::addition(digit_t *fst, digit_t *snd, sign_t s1, sign_t s2) {
	if (s1 == s2) {
		digit_t* res = make_string(std::max(SIZ(fst), SIZ(snd))+1);
		add(fst, snd, res);
		return Number(res, s1);
	}
	int c = cmp(fst, snd);
	if (c >= 0) {
		digit_t* res = make_string(SIZ(fst));
		sub(fst, snd, res);
		return Number(res, s1);
	}
	else {
		digit_t* res = make_string(SIZ(snd));
		sub(snd, fst, res);
		return Number(res, s2);
	}
}

void Number::local_addition(digit_t *fst, digit_t *snd, sign_t &s1, sign_t s2) {
	if (s1 == s2) {
		add(fst, snd, fst);
		return;
	}
	int c = cmp(fst, snd);
	if (c >= 0)
		sub(fst, snd, fst);
	else {
		sub(snd, fst, fst);
		s1 = s2;
	}
	if (SIZ(fst) == 0)
		s1 = 0;
}

digit_t* Number::div_fragment(digit_t* fst, digit_t* snd) {
	digit_t *quo = make_string(SIZ(fst)-SIZ(snd)+1);
	digit_t *mul = make_string(SIZ(fst)+1);
	digit_t dgt;
	while (cmp(fst, snd) >= 0) {
		int pos = SIZ(fst) - SIZ(snd);
		if (DIG(fst, SIZ(fst)-1) < DIG(snd, SIZ(snd)-1))
			-- pos;
		dgt = approximate(fst, snd, pos);
		mul_str(snd, dgt, mul);
		sub(fst, mul, fst, pos);
		DIG(quo, pos) += dgt;
	}
	normalise(quo);
	free(mul);
	return quo;
}

Number::Number(const Number& o) {
	_array = copy_string(CONV(o._array));
	_sign = o._sign;
}

Number::Number(Number&& o) {
	_array = o._array;
	_sign = o._sign;
	o._array = nullptr;
}

Number& Number::operator=(const Number& o) {
	if (this != &o) {
		std::free(_array);
		_array = copy_string(CONV(o._array));
		_sign = o._sign;
	}
	return *this;
}

Number& Number::operator=(Number&& o) {
	std::swap(_array, o._array);
	_sign = o._sign;
	return *this;
}

Number::Number(digit_t *a, sign_t s) : _array(a), _sign((a && SIZ(a)) ? s : 0) {}
Number::~Number() {std::free(_array);}
