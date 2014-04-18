#include <mcpdis.hh>
#include <algorithm>
#include <iostream>
#include <sstream>
#include <iomanip>

int main() {

	std::sort(pic12f675.begin(), pic12f675.end());

	for(auto& x : pic12f675) {
		std::cout << x.pattern << ' ';
		std::cout << std::setfill(' ') << std::setw(6) << std::left << x.name << ' ';
		std::cout << std::setfill('0') << std::setw(2) << std::right << std::hex << (int)x.status << 'h' << std::endl;
	}

	return 0;
}
