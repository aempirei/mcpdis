#include <algorithm>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <set>
#include <list>

#include <cstdlib>
#include <cctype>
#include <cstring>
#include <clocale>

#include <unistd.h>

#include <mcpdis.hh>

struct configuration;

typedef std::wstring stream_processor_fn(bitstream&);

stream_processor_fn be14;
stream_processor_fn le16;

void handler(const configuration&, bitstream&, const instruction_set&, stream_processor_fn*);

struct configuration {
	bool verbose = false;
	stream_processor_fn *stream_processor = be14;
};

std::wstring arg_string(wchar_t, std::wstring);
std::wstring arg_string(wchar_t, std::wstring, std::wstring);

std::wstring arg_string(wchar_t wc, std::wstring parameter, std::wstring message) {
	std::wstringstream ss;
	ss << L'\t' << L'-' << wc << L' ' << std::setw(7) << std::left << parameter << L' ' << message << std::endl;
	return ss.str();
}

std::wstring arg_string(wchar_t wc, std::wstring message) {
	return arg_string(wc, L"", message);
}

void usage(const char *arg0) {

	std::wcerr << std::endl << L"usage: " << arg0 << L" [-{hv}] [-x {le16|be14}]" << std::endl << std::endl;

	std::wcerr << arg_string(L'h', L"show help");
	std::wcerr << arg_string(L'v', L"verbose");
	std::wcerr << arg_string(L'x', L"format", L"stream type (default: be14)");

	std::wcerr << std::endl;
}

int main(int argc, char **argv) {

	configuration config;
	bitstream b(stdin);
	int opt;

	setlocale(LC_CTYPE, "");

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

	pic12f::pic12f675.sort();

	handler(config, b, pic12f::pic12f675, config.stream_processor);

	return 0;
}

std::wstring le16(bitstream& b) {

	std::wstring hi;
	std::wstring lo;

	hi = b.get(8);
	lo = b.get(8);

	if(hi.empty() || lo.empty())
		return L"";

	return (lo + hi).substr(2);
}

std::wstring be14(bitstream& b) {
	return b.get(14);
}

void print_code(const configuration& config, const sourcecode& code, const std::set<unsigned long>& labels) {

	//
	// print code
	//

	for(auto& op : code) {

		if(config.verbose) {

			if(labels.find(op.address) == labels.end())
				std::wcout << L' ';
			else
				std::wcout << L'*';

			std::wcout << pic12f::address_string(op.address) << L": " << op.s;

			if(!op.opcode.name.empty()) {

				std::wcout << L' ' << op.opcode.pattern << L' ' << op.opcode.name;

				for(auto iter = op.args.begin(); iter != op.args.end(); iter++) {

					std::wcout << L' ' << iter->first << L'=';

					if(isalpha(iter->first)) {

						unsigned long x = wcstoul(iter->second.c_str(), NULL, 2);

						std::wcout << std::dec << x;

					} else {

						std::wcout << iter->second;
					}


				}
			}

			std::wcout << std::endl;

		} else {

			if(labels.find(op.address) != labels.end()) 
				std::wcout << pic12f::address_string(op.address) << L':';
			else
				std::wcout << L"     ";

			if(!op.opcode.name.empty()) {

				std::wcout << L' ' << std::setw(6) << std::setfill(L' ') << std::left << std::wstring(op.opcode.name) << L' ';

				if(op.args.has_args(L"df")) {

					if(op.args.value(L'd') == 0)
						std::wcout << L"W, ";

					std::wcout << pic12f::register_name(op.args.value(L'f'));

				} else if(op.args.has_args(L"bf")) {

					std::wcout << pic12f::register_name(op.args.value(L'f')) << L'<' << op.args.value(L'b') << L'>';

				} else if(op.args.has_arg(L'f')) {

					std::wcout << pic12f::register_name(op.args.value(L'f'));

				} else if(op.args.has_arg(L'k')) {

					std::wcout << pic12f::address_string(op.args.value(L'k'));
				}
			}

			std::wcout << std::endl;
		}
	}
}

void handler(const configuration& config, bitstream& b, const instruction_set& cpu, stream_processor_fn *f) {

	std::set<unsigned long> labels = { 0x00, 0x04 };
	sourcecode code;

	unsigned long pc = 0;

	//
	// parse bit stream
	//

	for(;;) {

		std::wstring str = f(b);

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

			labels.insert(op.args.value(L'k'));

		} else if(op.opcode.pcl_type == instruction::pcl_types::call) {

			labels.insert(op.args.value(L'k'));
			labels.insert(op.address + 1);
		}
	}

	print_code(config, code, labels);

	//
	// term accumulator

	int n = 1;

	for(auto iter = code.begin(); iter != code.end(); iter++) {
		
		if(labels.find(iter->address) != labels.end()) {

			dictionary state;

			std::wcout << L"vector_" << std::setw(2) << std::right << std::setfill(L'0') << n++ << L" ";

			std::wstringstream range_ws;
			std::wstringstream opvector_ws;

			decltype(iter) jter;

			for(jter = iter; jter != code.end() && jter->opcode.pcl_type == instruction::pcl_types::normal; jter++) {

				jter->execute(state);

				opvector_ws << L' ' << jter->opcode.name;
			}

			if(jter != code.end()) {

				opvector_ws << L' ' << jter->opcode.name;

				switch(jter->opcode.pcl_type) {

					case instruction::pcl_types::skip:

						// jter->execute(state);
						// TODO: handle branching
						break;

					case instruction::pcl_types::ret:
					case instruction::pcl_types::jump:
					case instruction::pcl_types::call:

						jter->execute(state);
						break;

					case instruction::pcl_types::normal:

						throw std::runtime_error("impossible");
						break;
				}
			}

			range_ws << pic12f::address_string(iter->address) << L'-' << pic12f::address_string(jter->address);

			std::wcout << range_ws.str() << L" : " << opvector_ws.str() << std::endl << std::endl;

			for(const auto& k : state) {

				if(k.second.is_variable(k.first)) {
						
					// do nothing for identity

				} else {

					std::wcout << L'\t' << str(k) << std::endl;
				}
			}

			std::wcout << std::endl;
		}
	}
}
