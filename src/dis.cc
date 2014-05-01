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

	//
	// parse bit stream
	//

	for(;;) {

		std::string str = f(b);

		if(str.empty())
			break;

		operation op;

		op.s = str;
		op.address = pc++;
		op.opcode = cpu.find(op.s);

		op.opcode.match(op.s, &op.args);

		code.push_back(op);

		if(op.opcode.property_bits == instruction::property::normal) {

			// do nothing

		} else if(op.opcode.property_bits == instruction::property::skip) {

			labels.insert(op.address + 1);
			labels.insert(op.address + 2);

		} else if(op.opcode.property_bits == instruction::property::jump) {

			labels.insert(strtoul(op.args['k'].c_str(), NULL, 2));

		} else if(op.opcode.property_bits == instruction::property::call) {

			labels.insert(strtoul(op.args['k'].c_str(), NULL, 2));
			labels.insert(op.address + 1);
		}
	}

	//
	// print code
	//

	for(auto op : code) {

		if(config.verbose) {

			if(labels.find(op.address) == labels.end())
				std::cout << ' ';
			else 
				std::cout << '*';

			std::cout << address_string(op.address) << ": " << op.s;

			if(!op.opcode.name.empty()) {

				std::cout << ' ' << op.opcode.pattern << ' ' << op.opcode.name;

				for(auto iter = op.args.begin(); iter != op.args.end(); iter++) {

					std::cout << ' ' << iter->first << '=';

					if(isalpha(iter->first)) {

						unsigned long x = strtoul(iter->second.c_str(), NULL, 2);

						std::cout << std::dec << x;

					} else {

						std::cout << iter->second;
					}


				}
			}

			std::cout << std::endl;

		} else {

#define FIND(a,b) ((a).find(b) != (a).end())
#define BINARY(a) strtoul(a.c_str(), NULL, 2)

			if(FIND(labels, op.address))
				std::cout << address_string(op.address) << ':';
			else
				std::cout << "     ";

			if(!op.opcode.name.empty()) {

				std::cout << ' ' << std::setw(6) << std::setfill(' ') << std::left << op.opcode.name << ' ';

				if(op.args.has_args("df")) {

					if(op.args.value('d') == 0)
						std::cout << "W, ";

					std::cout << register_name(op.args.value('f'));

				} else if(op.args.has_args("bf")) {

					std::cout << register_name(op.args.value('f')) << '<' << op.args.value('b') << '>';

				} else if(op.args.has_arg('f')) {

					std::cout << register_name(op.args.value('f'));

				} else if(op.args.has_arg('k')) {

					std::cout << address_string(op.args.value('k'));
				}
			}

			std::cout << std::endl;

		}


	}
}

