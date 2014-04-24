#include <mcpdis.hh>
#include <set>
#include <algorithm>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <cstdlib>
#include <cctype>

std::string packed14(bitstream&);
std::string swapped16(bitstream&);

template<class F> void handler(bitstream&, const instruction_set&, F);

int main() {

	bitstream b(stdin);

	pic12f675.sort();

	handler(b, pic12f675, swapped16);

	return 0;
}

std::string swapped16(bitstream& b) {

	std::string hi;
	std::string lo;

	hi = b.get(8);
	lo = b.get(8);

	if(hi.empty() || lo.empty())
		return "";

	return (lo + hi).substr(2);
}

std::string packed14(bitstream& b) {
	return b.get(14);
}

template<class F> void handler(bitstream& b, const instruction_set& cpu, F f) {

	std::set<unsigned long> labels = { 0x00, 0x04 };

	int pc = 0;

	for(;;) {

		std::string str = f(b);

		if(str.empty())
			break;

		std::cout << std::right << std::hex << std::setw(3) << std::setfill('0') << pc << "h: ";

		std::cout << str << ' ';

		instruction op = cpu.find(str);

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

			if(op.property_bits == instruction::property::normal) {

				// do nothing

			} else if(op.property_bits == instruction::property::skip) {

				labels.insert(pc + 1);
				labels.insert(pc + 2);

			} else if(op.property_bits == instruction::property::jump) {

				labels.insert(strtoul(args['k'].c_str(), NULL, 2));

			} else if(op.property_bits == instruction::property::call) {

				labels.insert(strtoul(args['k'].c_str(), NULL, 2));
				labels.insert(pc + 1);
			}
		}

		std::cout << std::endl;

		pc++;
	}

	std::cout << "labels: ";
	
	for(auto n : labels)
		std::cout << ' ' << std::hex << std::setfill('0') << std::setw(2) << n << 'h';

	std::cout << std::endl;

}

