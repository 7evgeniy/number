#include "number/number.h"
#include <iostream>
#include <deque>

int main() {
	Number n1("286739756007480238956829");
	Number n2("40104783");
	n2 = n1.subdiv(n2);
	std::cout << n2.toString() << std::endl << n1.toString() << std::endl;
	return 0;  // должно получаться 7149764555701005 и 20549914
}
