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

using namespace dis;

using namespace yyy::quick;

struct configuration;

typedef std::wstring stream_processor_fn(bitstream&);

stream_processor_fn be14;
stream_processor_fn le16;

void handler(const configuration&, bitstream&, const instruction_set&, stream_processor_fn*);

struct configuration {
	bool verbose = false;
	bool print_model = false;
	stream_processor_fn *stream_processor = be14;
	G g;
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

#define Pr(x) P(S::ref(x))

void initialize_grammar(std::list<S::ref>& s, std::list<S::ref>& z, std::list<S::ref>& a, G& g) {

	 const auto pL = P(M() << L()).by_type();
	 const auto pF = P(M() << F()).by_type();
	 const auto pS = P(M() << S::var()).by_type();
	 const auto pLF = P(M() << L() << F()).by_type();
	 const auto pLS = P(M() << L() << S::var()).by_type();
	 const auto pFS = P(M() << F() << S::var()).by_type();
	 const auto pLFS = P(M() << L() << F() << S::var()).by_type();

	// init

	g.clear();
	s.clear();
	z.clear();
	a.clear();

	// root rules
	//
	//

	G dgs = {

		{ L"AND"    , { R(OP_AND    ) << Pr(L"n-ary") } },
		{ L"OR"     , { R(OP_OR     ) << Pr(L"n-ary") } },
		{ L"XOR"    , { R(OP_XOR    ) << Pr(L"n-ary") } },
		{ L"PLUS"   , { R(OP_PLUS   ) << Pr(L"n-ary") } },
		{ L"MINUS"  , { R(OP_MINUS  ) << Pr(L"n-ary") } },
		{ L"COMPOSE", { R(OP_COMPOSE) << Pr(L"n-ary") } },
		{ L"LIST"   , { R(OP_LIST   ) << Pr(L"n-ary") } },

		{ L"SWAP"   , { R(OP_SWAP) << Pr(L"unary") } },
		{ L"NOT"    , { R(OP_NOT ) << Pr(L"unary") } },
		{ L"ROTL"   , { R(OP_ROTL) << Pr(L"unary") } },
		{ L"ROTR"   , { R(OP_ROTR) << Pr(L"unary") } },

	};

	for(const auto& r : dgs)
		s.push_back(r.first);

	g.insert(dgs.begin(), dgs.end());

	// optimization rules
	//
	//

	G dgz = {

		{ L"lift",
			{
				R(OP_AND    ) << Pr(L"AND"),
				R(OP_OR     ) << Pr(L"OR"),
				R(OP_XOR    ) << Pr(L"XOR"),
				R(OP_PLUS   ) << Pr(L"PLUS"),
				R(OP_COMPOSE) << Pr(L"COMPOSE"),
				R(OP_LIST   ) << Pr(L"LIST")
			} },

		{ L"aggregate",
			{
				R(OP_AND ) << Pr(L"literals"),
				R(OP_OR  ) << Pr(L"literals"),
				R(OP_XOR ) << Pr(L"literals"),
				R(OP_PLUS) << Pr(L"literals")
			} },

		{ L"compute",
			{
				R(OP_SWAP) << pL,
				R(OP_NOT ) << pL,
				R(OP_ROTL) << pL,
				R(OP_ROTR) << pL,
			} },

		{ L"distribute",
			{
				R(OP_AND) << P() << Pr(L"OR"),
				R(OP_OR) << P() << Pr(L"AND")
			} },

		{ L"idempotent",
			{
				R(OP_AND) << Pr(L"matchpair"),
				R(OP_OR ) << Pr(L"matchpair")
			} },

		{ L"nilpotent",
			{
				R(OP_XOR) << Pr(L"matchpair")
			} },

		{ L"involution",
			{
				R(OP_NOT ) << Pr(L"NOT"),
				R(OP_SWAP) << Pr(L"SWAP")
			} },

		{ L"inverse",
			{
				R(OP_ROTL) << Pr(L"ROTR"),
				R(OP_ROTR) << Pr(L"ROTL")
			} },

		{ L"nop",
			{
				R(OP_AND    ) << Pr(L"unary"),
				R(OP_OR     ) << Pr(L"unary"),
				R(OP_XOR    ) << Pr(L"unary"),
				R(OP_PLUS   ) << Pr(L"unary"),
				R(OP_MINUS  ) << Pr(L"unary"),
				R(OP_COMPOSE) << Pr(L"unary"),
				R(OP_LIST   ) << Pr(L"unary")
			} }
	};

	for(const auto& r : dgz)
		z.push_back(r.first);

	g.insert(dgz.begin(), dgz.end());

	// auxiliary rules
	//
	//

	P Pfn( F(OP_AND) << L(1) );

	grammar<term> dga = {

		{ L"nonsense" , { R(OP_THIS) << Pfn.by_op() << Pfn.by_type() << P() << Pfn << P() << P(L(7)) << P(S::var(L"r27")) << P().end() } },

		{ L"unary"    , { R(OP_THIS) << P() << P().end()	} },

		{ L"n-ary"    , { R(OP_THIS) << +P() << P().end()	} },

		{ L"matchpair", { R(OP_THIS) << P() << P().mem()	} },

		{ L"literals" , { R(OP_THIS) << pL.min(2)		} }
	};

	for(const auto& r : dga)
		a.push_back(r.first);

	g.insert(dga.begin(), dga.end());
}

void print_rules(const configuration& config, const std::wstring& name, const std::list<S::ref>& refs, G& g) {

	std::wcout << L"[:" << name << L":]" << std::endl << std::endl;

	for(const auto& ref : refs) {

		const auto& rs = g[ref];
		auto iter = rs.begin();

		if(config.verbose) {

			if(iter != rs.end())
				std::wcout << type::color<symbol::ref>() << ref << ANSI_CLR << L" := " << iter->str() << std::endl;

			while(++iter != rs.end())
				std::wcout << std::setw(ref.length()) << L"" << L" := " << iter->str() << std::endl;

			std::wcout << std::endl;

		} else {

			if(iter != rs.end())
				std::wcout << std::setw(22) << std::left << type::color<symbol::ref>() << ref << ANSI_CLR << L" := " << iter->str();

			while(++iter != rs.end())
				std::wcout << ANSI_HIRED << L" / " << ANSI_CLR << iter->str();

			std::wcout << std::endl;

		}
	}

	std::wcout << std::endl;
}

int main(int argc, char **argv) {

	configuration config;
	bitstream b(stdin);
	int opt;

	srand(time(NULL));

	setlocale(LC_CTYPE, "");

	std::list<symbol::ref> s;
	std::list<symbol::ref> z;
	std::list<symbol::ref> a;

	initialize_grammar(s, z, a, config.g);

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

		print_rules(config, L"root", s, config.g);
		print_rules(config, L"optimization", z, config.g);
		print_rules(config, L"auxiliary", a, config.g);

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

			if(!yyy::contains(labels,op.address))
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

			if(yyy::contains(labels, op.address))
				std::wcout << pic12f::address_string(op.address) << L':';
			else
				std::wcout << L"     ";

			if(!op.opcode.name.empty()) {

				std::wcout << L' ' << std::setw(6) << std::setfill(L' ') << std::left << op.opcode.name << L' ';

				if(contains(op.args, L'd', L'f')) {

					if(op.argul(L'd') == 0)
						std::wcout << ANSI_LOGREEN << L'W' << ANSI_CLR << ", ";

					std::wcout << ANSI_LOGREEN << pic12f::register_name(op.argul(L'f')) << ANSI_CLR;

				} else if(contains(op.args, L'b', L'f')) {

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

		if(yyy::contains(labels,iter->address)) {

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

				const auto name = type::container<decltype(k.first)>(k.first).str();

				if(k.second.contains<symbol::var>()) {

					if(not k.second.contains(k.first))
						std::wcout << std::setw(20) << std::setfill(L' ') << name << L" $= " << k.second.str() << std::endl;

				} else if(k.second.contains<literal_t>()) {

					std::wcout << std::setw(20) << std::setfill(L' ') << name << L" #= " << k.second.str() << std::endl;

				} else if(k.second.contains<function<term>>()) {

					std::wcout << std::setw(20) << std::setfill(L' ') << name << L" := " << k.second.str() << std::endl;

					//
					// test the parsing on every formula
					//

					config.g.parse(L"OR", k.second.get<function<term>>());
				}
			}

			std::wcout << std::endl;
		}
	}
}
