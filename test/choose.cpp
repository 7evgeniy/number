#include <number/number.h>
#include <iostream>
#include <cstring>
#include <algorithm>

Number choose_fac(int M, int N) {
	if (M < 0 || M > N)
		return 0;
	int Q = N - M;
	Number n = 1, nm = 1, nq = 1, nn = 1;
	for (int i = 1; i <= N; ++ i) {
		n *= i;
		if (i == M) nm = n;
		if (i == Q) nq = n;
		if (i == N) nn = n;
	}
	return nn/(nm*nq);
}

int index (int m, int base) {
	return m-base;
}

Number choose_pas(int M, int N) {
	int size = std::min(M, N-M)+2;
	Number *former = new Number[size], *latter = new Number[size];
	for (int low = 0, n = 0; n < N; ++ n) {
		int next = std::max((M-N)+(n+1), 0);
		int high = std::min(n, M);
		if (low == 0)
			former[0] = 1;
		former[index(high+1, low)] = 0;
		for (int m = low; m <= high; ++ m)
			latter[index(m+1, next)] = former[index(m, low)] + former[index(m+1, low)];
		low = next; std::swap(former, latter);
	}
	Number answer = M > 0 ? former[0] : former[1];
	delete[] former; delete[] latter;
	return answer;
}

class CommandLine {
public:
	enum Mode {NoMode, Factorial, Pascal};
	enum Error {OK, Quantity, Format};
	
	CommandLine(int argc, char **argv) {
		int num_count = 0;
		_mode = NoMode;
		_error = OK;
		for (int i = 1; i < argc; ++ i) {
			int n = number(argv[i]);
			if (n >= 0) {
				if (num_count < 2)
					_num[num_count++] = n;
				else {
					_error = Quantity;
					return;
				}
			}
			else {
				if (_mode != NoMode) {
					_error = Format;
					return;
				}
				if (std::strcmp(argv[i], "-f") == 0)
					_mode = Factorial;
				else if (std::strcmp(argv[i], "-p") == 0)
					_mode = Pascal;
				else {
					_error = Format;
					return;
				}
			}
		}
		if (num_count != 2)
			_error = Quantity;
		if (_mode == NoMode)
			_mode = Factorial;
	}
	
	int fst_number() const {return _num[0];}
	int snd_number() const {return _num[1];}
	Mode mode() const {return _mode;}
	Error error() const {return _error;}

private:
	int _num[2];
	Mode _mode;
	Error _error;
	
	static int number (char *str) {
		int num = 0;
		for (int i = 0; str[i]; ++ i) {
			if (!isdigit(str[i]))
				return -1;
			num *= 10;
			num += (str[i] - '0');
		}
		return num;
	}
};

int main (int argc, char **argv) {
	CommandLine cl(argc, argv);
	if (cl.error() != CommandLine::OK) {
		std::cerr << "Расчёт числа выборок из N по M.\n";
		std::cerr << "Параметры программы: 1) два натуральных числа; 2) опционально — флаг:\n";
		std::cerr << "\t-f — расчёт через факториалы;\n";
		std::cerr << "\t-p — расчёт через треугольник Паскаля.\n";
		std::cerr << "Значение флага по умолчанию — это -f.\n";
		std::cerr << "Параметры можно задавать в любом порядке.\n";
		std::cerr << "Если числа различны, то M — меньшее число, N — большее.\n";
		return 1;
	}
	int M = cl.fst_number(), N = cl.snd_number();
	if (M > N)
		std::swap(M, N);
	switch (cl.mode()) {
	case CommandLine::Factorial:
		std::cout << choose_fac(M, N).toString() << std::endl;
		return 0;
	case CommandLine::Pascal:
		std::cout << choose_pas(M, N).toString() << std::endl;
		return 0;
	default: return 2;
	}
}
