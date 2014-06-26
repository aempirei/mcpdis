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
	bool print_model = false;
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

	std::wcerr << std::endl << L"usage: " << arg0 << L" [-{mvh}] [-x {le16|be14}]" << std::endl << std::endl;

	std::wcerr << arg_string(L'x', L"format", L"stream type (default: be14)");
	std::wcerr << arg_string(L'm', L"show model");
	std::wcerr << arg_string(L'v', L"verbose");
	std::wcerr << arg_string(L'h', L"show help");

	std::wcerr << std::endl;
}

/*
void initialize_grammar(std::list<symbol::var>& s, std::list<symbol::var>& z, std::list<symbol::var>& a, grammar<term>& g) {

	typedef predicate<term> PT;
	typedef const PT CPT;
	typedef term::term_type TT;
	typedef PT::filter_type FT;
	typedef rule<term> RT;

	CPT L(FT { TT::literal });
	CPT S(FT { TT::symbol });
	CPT F(FT { TT::function });
	CPT LS(FT { TT::literal, TT::symbol });

	CPT DOT = PT();
	CPT TAIL = PT().star();
	CPT END = PT::predicate_type::end;
	CPT MEM = PT::predicate_type::mem;

	// init

	g.clear();
	s.clear();
	z.clear();
	
	// root rules
	//
	//

	grammar<term> dgs = {

		{ L"AND"    , { RT(OP_AND    ) << L"n-ary" } },
		{ L"OR"     , { RT(OP_OR     ) << L"n-ary" } },
		{ L"XOR"    , { RT(OP_XOR    ) << L"n-ary" } },
		{ L"PLUS"   , { RT(OP_PLUS   ) << L"n-ary" } },
		{ L"MINUS"  , { RT(OP_MINUS  ) << L"n-ary" } },
		{ L"COMPOSE", { RT(OP_COMPOSE) << L"n-ary" } },
		{ L"LIST"   , { RT(OP_LIST   ) << L"n-ary" } },

		{ L"SWAP"   , { RT(OP_SWAP) << L"unary" } },
		{ L"NOT"    , { RT(OP_NOT ) << L"unary" } },
		{ L"ROTL"   , { RT(OP_ROTL) << L"unary" } },
		{ L"ROTR"   , { RT(OP_ROTR) << L"unary" } },

	};

	for(const auto& r : dgs)
		s.push_back(r.first);

	g.insert(dgs.begin(), dgs.end());

	// optimization rules
	//
	//

	grammar<term> dgz = {

		{ L"lift",
			{
				RT(OP_AND    ) << L"AND",
				RT(OP_OR     ) << L"OR",
				RT(OP_XOR    ) << L"XOR",
				RT(OP_PLUS   ) << L"PLUS",
				RT(OP_COMPOSE) << L"COMPOSE",
				RT(OP_LIST   ) << L"LIST"
			} },

		{ L"aggregate",
			{
				RT(OP_AND ) << L"literals",
				RT(OP_OR  ) << L"literals",
				RT(OP_XOR ) << L"literals",
				RT(OP_PLUS) << L"literals"
			} },

		{ L"compute",
			{
				RT(OP_SWAP) << L,
				RT(OP_NOT ) << L,
				RT(OP_ROTL) << L,
				RT(OP_ROTR) << L
			} },

		{ L"distribute",
			{
				RT(OP_AND) << DOT << L"OR",
				RT(OP_OR) << DOT << L"AND"
			} },

		{ L"idempotent",
			{
				RT(OP_AND) << L"matchpair",
				RT(OP_OR ) << L"matchpair"
			} },

		{ L"nilpotent",
			{
				RT(OP_XOR) << L"matchpair"
			} },

		{ L"involution",
			{
				RT(OP_NOT ) << L"NOT",
				RT(OP_SWAP) << L"SWAP"
			} },

		{ L"inverse",
			{
				RT(OP_ROTL) << L"ROTR",
				RT(OP_ROTR) << L"ROTL"
			} },

		{ L"nop",
			{
				RT(OP_AND    ) << L"unary",
				RT(OP_OR     ) << L"unary",
				RT(OP_XOR    ) << L"unary",
				RT(OP_PLUS   ) << L"unary",
				RT(OP_MINUS  ) << L"unary",
				RT(OP_COMPOSE) << L"unary",
				RT(OP_LIST   ) << L"unary"
			} }
	};

	for(const auto& r : dgz)
		z.push_back(r.first);

	g.insert(dgz.begin(), dgz.end());

	// auxiliary rules
	//
	// 

	grammar<term> dga = {

		{ L"unary"    , { RT() << DOT << END        } },

		{ L"n-ary"    , { RT() << DOT.plus() << END } },

		{ L"matchpair", { RT() << DOT << MEM        } },

		{ L"literals" , { RT() << L.ge(2)           } }
	};

	for(const auto& r : dga)
		a.push_back(r.first);

	g.insert(dga.begin(), dga.end());
}

void print_rules(const configuration& config, const std::wstring& name, const std::list<symbol::var> ks, grammar<term>& g) {

	std::wcout << L"[:" << name << L":]" << std::endl << std::endl;

	for(const auto& k : ks) {

		const auto& rs = g[k];
		auto iter = rs.begin();

		if(config.verbose) {

			if(iter != rs.end())
				std::wcout << k << L" := " << (std::wstring)*iter << std::endl;

			while(++iter != rs.end())
				std::wcout << std::setw(k.length()) << L"" << L" := " << (std::wstring)*iter << std::endl;

			std::wcout << std::endl;

		} else {

			if(iter != rs.end())
				std::wcout << std::setw(10) << std::left << k << ANSI_HIRED << L" := " << ANSI_CLR << (std::wstring)*iter;

			while(++iter != rs.end())
				std::wcout << ANSI_HIRED << L" / " << ANSI_CLR << (std::wstring)*iter;

			std::wcout << std::endl;

		}

	}

	std::wcout << std::endl;
}
*/

int main(int argc, char **argv) {

	configuration config;
	bitstream b(stdin);
	int opt;

	setlocale(LC_CTYPE, "");

	std::list<symbol> s;
	std::list<symbol> z;
	std::list<symbol> a;

	// grammar<term> g;

	// initialize_grammar(s, z, a, g);

	while ((opt = getopt(argc, argv, "hvx:m")) != -1) {
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

			case 'm':

				config.print_model = true;
				break;

			case 'h':
			default:

				usage(*argv);
				return -1;
		}
	}

	if(config.print_model) {

		/*
		print_rules(config, L"root", s, g);
		print_rules(config, L"optimization", z, g);
		print_rules(config, L"auxiliary", a, g);
		*/

	} else {

		pic12f::pic12f675.sort();
		handler(config, b, pic12f::pic12f675, config.stream_processor);
	}

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

				if(contains(op.args, L'd') and contains(op.args, L'f')) {

					if(op.argul(L'd') == 0)
						std::wcout << ANSI_LOGREEN << L'W' << ANSI_CLR << ", ";

					std::wcout << ANSI_LOGREEN << pic12f::register_name(op.argul(L'f')) << ANSI_CLR;

				} else if(contains(op.args, L'b') and contains(op.args, L'f')) {

					std::wcout << ANSI_LOGREEN << pic12f::register_name(op.argul(L'f')) << ANSI_CLR;
					std::wcout << L'<' << ANSI_LORED << op.argul(L'b') << ANSI_CLR << L'>';

				} else if(contains(op.args, L'f')) {

					std::wcout << ANSI_LOGREEN << pic12f::register_name(op.argul(L'f')) << ANSI_CLR;

				} else if(contains(op.args, L'k')) {

					std::wcout << ANSI_LORED << pic12f::address_string(op.argul(L'k')) << ANSI_CLR;
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

			labels.insert(op.argul(L'k'));

		} else if(op.opcode.pcl_type == instruction::pcl_types::call) {

			labels.insert(op.argul(L'k'));
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

				std::wcout << std::setw(14) << std::setfill(L' ') << k.first << L" := " << k.second.str() << std::endl;
			}
			std::wcout << std::endl;
		}
	}
}
