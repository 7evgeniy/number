#ifndef NUMBER_H
#define NUMBER_H

#include <string>
#include "configure.h"

class Number {
public:
	Number(const Number& o);
	Number(Number&& o);
	Number& operator=(const Number& o);
	Number& operator=(Number&& o);
	~Number();
public:
	Number(unsigned int number = 0, bool negative = false);
	Number(const std::string& str);
	static Number invalidNumber();
	std::string toString() const;
	digit_t toDigit() const;
	int toSign() const;
	bool toBool() const;
	bool valid() const;
	void invalidate();
	void negate();
	friend Number operator+(const Number& fst, const Number& snd);
	friend Number operator-(const Number& fst, const Number& snd);
	friend Number operator*(const Number& fst, const Number& snd);
	friend Number operator/(const Number& fst, const Number& snd);
	friend Number operator%(const Number& fst, const Number& snd);
	friend bool operator<(const Number& fst, const Number& snd);
	friend bool operator>(const Number& fst, const Number& snd);
	friend bool operator<=(const Number& fst, const Number& snd);
	friend bool operator>=(const Number& fst, const Number& snd);
	Number& operator+= (const Number& other);
	Number& operator-= (const Number& other);
	Number& operator*= (const Number& other);
	Number& operator/= (const Number& other);
	Number& operator%= (const Number& other);
	Number subdiv(const Number& other);    // заместить число остатком, вернуть частное
	Number operator- () const;
private:
	typedef int8_t sign_t;
	Number(digit_t *array, sign_t sign);
	static Number addition(digit_t *fst, digit_t *snd, sign_t s1, sign_t s2);
	static void local_addition(digit_t *fst, digit_t *snd, sign_t &s1, sign_t s2);
	static digit_t* div_fragment(digit_t* fst, digit_t* snd);
private:
	digit_t *_array;
	sign_t _sign;
};

#endif
