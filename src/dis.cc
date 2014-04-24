#include <algorithm>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <set>
#include <list>

#include <cstdlib>
#include <cctype>

#include <unistd.h>

#include <mcpdis.hh>

struct configuration;

std::string be14(bitstream&);
std::string le16(bitstream&);

template<class F> void handler(const configuration&, bitstream&, const instruction_set&, F);

struct configuration {
	bool verbose = false;
	bool packed = true;
};

void usage(const char *arg0) {
	std::cerr << std::endl << "usage: " << arg0 << " [-{hvx}]" << std::endl << std::endl;
}

int main(int argc, char **argv) {

	configuration config;
	bitstream b(stdin);
	int opt;

	while ((opt = getopt(argc, argv, "hvx")) != -1) {
		switch (opt) {

			case 'v':

				config.verbose = true;
				break;

			case 'x':

				config.packed = false;
				break;

			case 'h':
			default:

				usage(*argv);
				return -1;
		}
	}

	pic12f675.sort();

	handler(config, b, pic12f675, config.packed ? be14 : le16);

	return 0;
}

std::string le16(bitstream& b) {

	std::string hi;
	std::string lo;

	hi = b.get(8);
	lo = b.get(8);

	if(hi.empty() || lo.empty())
		return "";

	return (lo + hi).substr(2);
}

std::string be14(bitstream& b) {
	return b.get(14);
}

template<class F> void handler(const configuration& config, bitstream& b, const instruction_set& cpu, F f) {

	std::set<unsigned long> labels = { 0x00, 0x04 };
	std::list<operation> code;

	int pc = 0;

	if(config.verbose) {
		std::cout << "verbose" << std::endl;
	}

	for(;;) {

		std::string str = f(b);

		if(str.empty())
			break;

		operation op;

		op.s = str;
		op.address = pc;
		op.opcode = cpu.find(op.s);

		op.opcode.match(op.s, &op.args);

		code.push_back(op);

		if(op.opcode.property_bits == instruction::property::normal) {

			// do nothing

		} else if(op.opcode.property_bits == instruction::property::skip) {

			labels.insert(pc + 1);
			labels.insert(pc + 2);

		} else if(op.opcode.property_bits == instruction::property::jump) {

			labels.insert(strtoul(op.args['k'].c_str(), NULL, 2));

		} else if(op.opcode.property_bits == instruction::property::call) {

			labels.insert(strtoul(op.args['k'].c_str(), NULL, 2));
			labels.insert(pc + 1);
		}

		pc++;
	}

	for(auto op : code) {

		if(labels.find(op.address) == labels.end())
			std::cout << ' ';
		else 
			std::cout << '*';

		std::cout << std::right << std::hex << std::setw(3) << std::setfill('0') << op.address << "h: " << op.s;

		if(!op.opcode.name.empty()) {

			std::cout << ' ' << op.opcode.pattern << ' ' << op.opcode.name;

			for(auto iter = op.args.begin(); iter != op.args.end(); iter++) {

				std::cout << ' ' << iter->first << '=';

				if(isalpha(iter->first)) {

					unsigned long x = strtoul(iter->second.c_str(), NULL, 2);

					if(iter->second.length() <= 3)
						std::cout << std::dec << x;
					else 
						std::cout << std::hex << std::setw(2) << std::setfill('0') << x << 'h';

				} else {

					std::cout << iter->second;
				}


			}
		}

		std::cout << std::endl;
	}
}

