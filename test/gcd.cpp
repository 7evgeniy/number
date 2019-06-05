#include <number/number.h>
#include <iostream>

Number gcd(Number a, Number b) {
	if (!a.valid() || !b.valid())
		return Number::invalidNumber();
	if (a.toSign() <= 0 || b.toSign() <= 0)
		return Number::invalidNumber();
	if (a < b)
		std::swap(a, b);
	while (b.toBool()) {
		a %= b;
		std::swap(a, b);
	}
	return a;
}

int main(int argc, char **argv) {
	if (argc != 3) {
		std::cerr << "Неверное количество параметров\n";
		return 1;
	}
	Number m = gcd(Number(argv[1]), Number(argv[2]));
	if (!m.valid()) {
		std::cerr << "Неверные заданы параметры\n";
		return 1;
	}
	std::cout << m.toString() << '\n';
	return 0;
}
