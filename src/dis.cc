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

void initialize_grammar(grammar<term> &g) {

	
	const predicate<term> L(predicate<term>::filter_type { term::term_type::literal });
	const predicate<term> S(predicate<term>::filter_type { term::term_type::symbol });
	const predicate<term> F(predicate<term>::filter_type { term::term_type::function });
	const predicate<term> LS(predicate<term>::filter_type { term::term_type::literal, term::term_type::symbol });

	const predicate<term> DOT = predicate<term>();
	const predicate<term> TAIL = predicate<term>().star();
	const predicate<term> END = predicate<term>::predicate_type::end;
	const predicate<term> MEM = predicate<term>::predicate_type::mem;

	auto& AND = g[L"AND"];
	auto& OR = g[L"OR"];
	auto& XOR = g[L"XOR"];
	auto& PLUS = g[L"PLUS"];
	auto& MINUS = g[L"MINUS"];
	auto& COMPOSE = g[L"COMPOSE"];
	auto& LIST = g[L"LIST"];

	auto& SWAP = g[L"SWAP"];
	auto& NOT = g[L"NOT"];
	auto& ROTL = g[L"ROTL"];
	auto& ROTR = g[L"ROTR"];

	AND.push_back( rule<term>(OP_AND) << DOT.plus() << END );
	OR.push_back( rule<term>(OP_OR) << DOT.plus() << END );
	XOR.push_back( rule<term>(OP_XOR) << DOT.plus() << END );
	PLUS.push_back( rule<term>(OP_PLUS) << DOT.plus() << END );
	MINUS.push_back( rule<term>(OP_MINUS) << DOT.plus() << END );
	COMPOSE.push_back( rule<term>(OP_COMPOSE) << DOT.plus() << END );
	LIST.push_back( rule<term>(OP_LIST) << DOT.plus() << END );

	SWAP.push_back( rule<term>(OP_SWAP) << DOT << END );
	NOT.push_back( rule<term>(OP_NOT) << DOT << END );
	ROTL.push_back( rule<term>(OP_ROTL) << DOT << END );
	ROTR.push_back( rule<term>(OP_ROTR) << DOT << END );

	auto& lift_rule = g[L"lift"];

	lift_rule.push_back( rule<term>(OP_AND) << L"AND" );
	lift_rule.push_back( rule<term>(OP_OR) << L"OR" );
	lift_rule.push_back( rule<term>(OP_XOR) << L"XOR" );
	lift_rule.push_back( rule<term>(OP_PLUS) << L"PLUS" );
	lift_rule.push_back( rule<term>(OP_COMPOSE) << L"COMPOSE" );
	lift_rule.push_back( rule<term>(OP_LIST) << L"LIST" );

	auto& literals_rule = g[L"literals"];

	literals_rule.push_back( rule<term>() << L.ge(2) );

	auto& distribute_rule = g[L"distribute"];

	distribute_rule.push_back( rule<term>(OP_AND) << DOT << L"OR" );
	distribute_rule.push_back( rule<term>(OP_OR) << DOT << L"AND" );

	auto& idempotent_rule = g[L"idempotent"];

	idempotent_rule.push_back( rule<term>(OP_AND) << DOT << MEM );
	idempotent_rule.push_back( rule<term>(OP_OR) << DOT << MEM );

	auto& nilpotent_rule = g[L"nilpotent"];

	nilpotent_rule.push_back( rule<term>(OP_XOR) << DOT << MEM );
	nilpotent_rule.push_back( rule<term>(OP_MINUS) << DOT << MEM );

	auto& involution_rule = g[L"involution"];

	involution_rule.push_back( rule<term>(OP_NOT) << L"NOT" << END);
	involution_rule.push_back( rule<term>(OP_SWAP) << L"SWAP" << END );

	auto& inverse_rule = g[L"inverse"];

	inverse_rule.push_back( rule<term>(OP_ROTL) << L"ROTR" << END );
	inverse_rule.push_back( rule<term>(OP_ROTR) << L"ROTL" << END );

	auto& nop_rule = g[L"nop"];

	nop_rule.push_back( rule<term>(OP_AND) << DOT << END );
	nop_rule.push_back( rule<term>(OP_OR) << DOT << END );
	nop_rule.push_back( rule<term>(OP_XOR) << DOT << END );
	nop_rule.push_back( rule<term>(OP_PLUS) << DOT << END );
	nop_rule.push_back( rule<term>(OP_MINUS) << DOT << END );
	nop_rule.push_back( rule<term>(OP_COMPOSE) << DOT << END );
	nop_rule.push_back( rule<term>(OP_LIST) << DOT << END );
}

int main(int argc, char **argv) {

	grammar<term> g;

	configuration config;
	bitstream b(stdin);
	int opt;

	setlocale(LC_CTYPE, "");

	initialize_grammar(g);

	for(const auto& x : g) {
		for(const auto& y : x.second) {
			std::wcout << x.first << L" := " << y.str() << std::endl;
		}
		// std::wcout << std::endl;
	}

	return 0;

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

						literal_t x = wcstoul(iter->second.c_str(), NULL, 2);

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

				std::wcout << L' ' << std::setw(6) << std::setfill(L' ') << std::left << op.opcode.name << L' ';

				if(op.args.has_args(L"df")) {

					if(op.args.value(L'd') == 0)
						std::wcout << ANSI_LOGREEN << L'W' << ANSI_CLR << ", ";

					std::wcout << ANSI_LOGREEN << pic12f::register_name(op.args.value(L'f')) << ANSI_CLR;

				} else if(op.args.has_args(L"bf")) {

					std::wcout << ANSI_LOGREEN << pic12f::register_name(op.args.value(L'f')) << ANSI_CLR;
					std::wcout << L'<' << ANSI_LORED << op.args.value(L'b') << ANSI_CLR << L'>';

				} else if(op.args.has_arg(L'f')) {

					std::wcout << ANSI_LOGREEN << pic12f::register_name(op.args.value(L'f')) << ANSI_CLR;

				} else if(op.args.has_arg(L'k')) {

					std::wcout << ANSI_LORED << pic12f::address_string(op.args.value(L'k')) << ANSI_CLR;
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

				if(k.second.is_symbol(k.first)) {
						
					// do nothing for identity

				} else {

					std::wcout << L'\t' << str(k) << std::endl;
				}
			}

			std::wcout << std::endl;
		}
	}
}
