#include <mcpdis.hh>
#include <set>
#include <algorithm>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <cstdlib>
#include <cctype>

void packed14();
void swapped16();

int main() {

	pic12f675.sort();

	swapped16();

	return 0;
}

void swapped16() {

	bitstream b(stdin);

	std::set<unsigned long> labels = { 0x00, 0x04 };

	int pc = 0;

	for(;;) {

		std::string hi;
		std::string lo;
		std::string str;

		hi = b.get(8);
		lo = b.get(8);

		if(hi.empty() || lo.empty())
			break;

		str = (lo + hi).substr(2);

		std::cout << std::right << std::hex << std::setw(3) << std::setfill('0') << pc << "h: ";

		std::cout << str << ' ';

		instruction op = pic12f675.find(str);

		if(!op.name.empty()) {

			parameter_map args;

			op.match(str, &args);

			std::cout << op.pattern;

			for(auto iter = args.begin(); iter != args.end(); iter++) {

				std::cout << ' ' << iter->first << '=';

				if(!isalpha(iter->first)) {
					std::cout << iter->second;
				} else {

					unsigned long x = strtoul(iter->second.c_str(), NULL, 2);

					if(iter->second.length() <= 3) {
						std::cout << std::dec << x;
					} else {
						std::cout << std::hex << std::setw(2) << std::setfill('0') << x << 'h';
					}
				}
			}

			if(args['$'] == "GOTO")
				labels.insert(strtoul(args['k'].c_str(), NULL, 2));
		}

		std::cout << std::endl;

		pc++;
	}

	std::cout << "labels: ";
	
	for(auto n : labels)
		std::cout << ' ' << std::hex << std::setfill('0') << std::setw(2) << n << 'h';

	std::cout << std::endl;
}

void packed14() {

	bitstream b(stdin);

	std::set<unsigned long> labels = { 0x00, 0x04 };

	unsigned long pc = 0;

	for(;;) {

		std::string str = b.get(14);

		if(str.empty())
			break;

		std::cout << std::right << std::hex << std::setw(3) << std::setfill('0') << pc << "h: ";

		std::cout << str << ' ';

		instruction op = pic12f675.find(str);

		if(!op.name.empty()) {

			parameter_map args;

			op.match(str, &args);

			std::cout << op.pattern;

			for(auto iter = args.begin(); iter != args.end(); iter++) {

				std::cout << ' ' << iter->first << '=';

				if(!isalpha(iter->first)) {
					std::cout << iter->second;
				} else {

					unsigned long x = strtoul(iter->second.c_str(), NULL, 2);

					if(iter->second.length() <= 3) {
						std::cout << std::dec << x;
					} else {
						std::cout << std::hex << std::setw(2) << std::setfill('0') << x << 'h';
					}
				}
			}

			if(args['$'] == "GOTO")
				labels.insert(strtoul(args['k'].c_str(), NULL, 2));
		}

		std::cout << std::endl;

		pc++;
	}

	std::cout << "labels: ";
	
	for(auto n : labels)
		std::cout << ' ' << std::hex << std::setfill('0') << std::setw(2) << n << 'h';

	std::cout << std::endl;
}
