#include <mcpdis.hh>
#include <algorithm>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <cstdlib>

int main() {

	pic12f675.sort();

	bitstream b(stdin);

	for(;;) {
		std::string str = b.get(14);

		if(str.empty())
			break;

		std::cout << str << ' ';

		instruction op = pic12f675.find(str);

		if(!op.name.empty()) {

			parameter_map args;

			op.match(str, &args);

			std::cout << op.pattern << ' ' << std::setw(6) << std::left << std::setfill(' ') << op.name;

			for(auto iter = args.begin(); iter != args.end(); iter++) {
				unsigned long x = strtoul(iter->second.c_str(), NULL, 2);
				std::cout << ' ' << iter->first << '=';
				if(iter->second.length() <= 3) {
					std::cout << std::dec << x;
				} else {
					std::cout << std::hex << std::setw(2) << std::setfill('0') << x << 'h';
				}
			}

			std::cout << std::endl;
		}
	}

	return 0;
}
