#include <algorithm>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <set>
#include <list>

#include <cstdlib>
#include <cctype>
#include <cstring>

#include <unistd.h>

#include <mcpdis.hh>

struct configuration;

typedef std::string stream_processor_fn(bitstream&);

stream_processor_fn be14;
stream_processor_fn le16;

template<class F> void handler(const configuration&, bitstream&, const instruction_set&, F);

struct configuration {
	bool verbose = false;
	stream_processor_fn *stream_processor = be14;
};

std::string arg_string(char, std::string);
std::string arg_string(char, std::string, std::string);

std::string arg_string(char ch, std::string parameter, std::string message) {
	std::stringstream ss;
	ss << '\t' << '-' << ch << ' ' << std::setw(7) << std::left << parameter << ' ' << message << std::endl;
	return ss.str();
}

std::string arg_string(char ch, std::string message) {
	return arg_string(ch, "", message);
}

void usage(const char *arg0) {

	std::cerr << std::endl << "usage: " << arg0 << " [-{hv}] [-x {le16|be14}]" << std::endl << std::endl;

	std::cerr << arg_string('h', "show help");
	std::cerr << arg_string('v', "verbose");
	std::cerr << arg_string('x', "format", "stream type (default: be14)");

	std::cerr << std::endl;
}

int main(int argc, char **argv) {

	configuration config;
	bitstream b(stdin);
	int opt;

	while ((opt = getopt(argc, argv, "hvx:")) != -1) {
		switch (opt) {

			case 'v':

				config.verbose = true;
				break;

			case 'x':

				if(strcmp(optarg, "le16") == 0) {

					config.stream_processor = le16;

				} else if(strcmp(optarg, "be14") == 0) {

					config.stream_processor = be14;

				} else {

					throw std::runtime_error(std::string("unknown stream type ") + optarg);
				}

				break;

			case 'h':
			default:

				usage(*argv);
				return -1;
		}
	}

	pic12f675.sort();

	handler(config, b, pic12f675, config.stream_processor);

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

	unsigned long pc = 0;

	//
	// parse bit stream
	//

	for(;;) {

		std::string str = f(b);

		if(str.empty())
			break;

		code.push_back(operation(str, pc++, cpu));

		operation& op = code.back();

		if(op.opcode.pcl_type == instruction::pcl_types::normal) {

			// do nothing

		} else if(op.opcode.pcl_type == instruction::pcl_types::skip) {

			labels.insert(op.address + 1);
			labels.insert(op.address + 2);

		} else if(op.opcode.pcl_type == instruction::pcl_types::jump) {

			labels.insert(op.args.value('k'));

		} else if(op.opcode.pcl_type == instruction::pcl_types::call) {

			labels.insert(op.args.value('k'));
			labels.insert(op.address + 1);
		}
	}

	//
	// print code
	//

	for(auto& op : code) {

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

