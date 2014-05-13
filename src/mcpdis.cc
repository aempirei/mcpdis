#include <mcpdis.hh>
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <cstdlib>
#include <cstdint>
#include <climits>
#include <iostream>

namespace pic12f {

	std::string load_d(operation& o) {
		return dest_string(o.args.value('d'), o.args.value('f'));
	}

	std::string load_f(operation& o) {
		return register_name(o.args.value('f'));
	}

	unsigned long load_k(operation& o) {
		return o.args.value('k');
	}

	uint8_t load_b(operation& o) {
		return 1 << o.args.value('b');
	}

#define FN(a) void a(operation&, dictionary&)
#define GN(a) void a(operation&o, dictionary&c)

#define LOAD_D std::string d = load_d(o)
#define LOAD_F std::string f = load_f(o)
#define LOAD_W const std::string w = "W"
#define LOAD_K unsigned long k = load_k(o)
#define LOAD_B uint8_t b = load_b(o)

	FN(RETURN) { throw std::runtime_error(std::string("RETURN overwrites program counter")); }
	FN(RETFIE) { throw std::runtime_error(std::string("RETFIE overwrites program counter")); }

	FN(SLEEP)  { /* put microcontroller to sleep */ }
	FN(CLRWDT) { /* clear watchdog timer */         } 
	FN(NOP)    { /* no operation */                 }

	GN(CLR)    { LOAD_D;                 c[d] = 0;                                      }
	GN(MOVWF)  { LOAD_W; LOAD_F;         c[f] = c.touch(w);                             }
	GN(MOVF)   { LOAD_D; LOAD_F;         c[d] = c.touch(f);                             }

	GN(IORWF)  { LOAD_W; LOAD_D; LOAD_F; c[d] = expr("|", { c.touch(f) , c.touch(w) }); }
	GN(ANDWF)  { LOAD_W; LOAD_D; LOAD_F; c[d] = expr("&", { c.touch(f) , c.touch(w) }); }
	GN(XORWF)  { LOAD_W; LOAD_D; LOAD_F; c[d] = expr("^", { c.touch(f) , c.touch(w) }); }
	GN(SUBWF)  { LOAD_W; LOAD_D; LOAD_F; c[d] = expr("-", { c.touch(f) , c.touch(w) }); }
	GN(ADDWF)  { LOAD_W; LOAD_D; LOAD_F; c[d] = expr("+", { c.touch(f) , c.touch(w) }); }

	GN(DECF)   { LOAD_D; LOAD_F;         c[d] = expr("-" , { c.touch(f), 1          }); }
	GN(INCF)   { LOAD_D; LOAD_F;         c[d] = expr("+" , { c.touch(f), 1          }); }

	GN(COMF)   { LOAD_D; LOAD_F;         c[d] = expr("~" , { c.touch(f)             }); }
	GN(RRF)    { LOAD_D; LOAD_F;         c[d] = expr("!>", { c.touch(f)             }); }
	GN(RLF)    { LOAD_D; LOAD_F;         c[d] = expr("<!", { c.touch(f)             }); }
	GN(SWAPF)  { LOAD_D; LOAD_F;         c[d] = expr("><", { c.touch(f)             }); }

	GN(BCF)    { LOAD_F; LOAD_B; b = ~b; c[f] = expr("&" , { b         , c.touch(f) }); }
	GN(BSF)    { LOAD_F; LOAD_B;         c[f] = expr("|" , { b         , c.touch(f) }); } 

	FN(DECFSZ) { throw std::runtime_error(std::string("DECFSZ performs conditional program counter modification")); }
	FN(INCFSZ) { throw std::runtime_error(std::string("INCFSZ performs conditional program counter modification")); }
	FN(BTFSC)  { throw std::runtime_error(std::string("BTFSC performs conditional program counter modification" )); }
	FN(BTFSS)  { throw std::runtime_error(std::string("BTFSS performs conditional program counter modification" )); }
	FN(CALL)   { throw std::runtime_error(std::string("CALL overwrites program counter"                         )); }
	FN(GOTO)   { throw std::runtime_error(std::string("GOTO overwrites program counter"                         )); }

	GN(MOVLW)  { LOAD_K; LOAD_W;         c[w] = k;                                      }

	GN(RETLW)  { MOVLW(o, c); throw std::runtime_error("RETLW overwrites program counter"); }

	GN(IORLW)  { LOAD_K; LOAD_W;         c[w] = expr("|", { k          , c.touch(w) }); }
	GN(ANDLW)  { LOAD_K; LOAD_W;         c[w] = expr("&", { k          , c.touch(w) }); }
	GN(XORLW)  { LOAD_K; LOAD_W;         c[w] = expr("^", { k          , c.touch(w) }); }
	GN(SUBLW)  { LOAD_K; LOAD_W;         c[w] = expr("-", { k          , c.touch(w) }); }
	GN(ADDLW)  { LOAD_K; LOAD_W;         c[w] = expr("+", { k          , c.touch(w) }); }

	FN(Z)      { throw std::runtime_error("STATUS<Z> zero flag unimplemented"            ); }
	FN(C)      { throw std::runtime_error("STATUS<C> carry flag unimplemented"           ); }
	FN(DC)     { throw std::runtime_error("STATUS<DC> decimal carry flag unimplemented"  ); }
	FN(PD)     { throw std::runtime_error("STATUS<PD> power down flag unimplemented"     ); }
	FN(TO)     { throw std::runtime_error("STATUS<TO> time-out flag unimplemented"       ); }
	FN(PC)     { throw std::runtime_error("PCL/PCLATH (PC) program counter unimplemented"); }

#undef LOAD_D
#undef LOAD_F
#undef LOAD_B
#undef LOAD_K

#undef FN
#undef GN
}

instruction_set pic12f675 = {

	{ "00000000001000", "RETURN", pic12f::RETURN, instruction::pcl_types::ret   , instruction::flags::none       },
	{ "00000000001001", "RETFIE", pic12f::RETFIE, instruction::pcl_types::ret   , instruction::flags::none       },
	{ "0000000xx00000", "NOP"   , pic12f::NOP   , instruction::pcl_types::normal, instruction::flags::none       },
	{ "00000001100011", "SLEEP" , pic12f::SLEEP , instruction::pcl_types::normal, instruction::flags::power      },
	{ "00000001100100", "CLRWDT", pic12f::CLRWDT, instruction::pcl_types::normal, instruction::flags::power      },
	{ "0000001fffffff", "MOVWF" , pic12f::MOVWF , instruction::pcl_types::normal, instruction::flags::none       },
	{ "000001dfffffff", "CLR"   , pic12f::CLR   , instruction::pcl_types::normal, instruction::flags::Z          },
	{ "000100dfffffff", "IORWF" , pic12f::IORWF , instruction::pcl_types::normal, instruction::flags::Z          },
	{ "000101dfffffff", "ANDWF" , pic12f::ANDWF , instruction::pcl_types::normal, instruction::flags::Z          },
	{ "000110dfffffff", "XORWF" , pic12f::XORWF , instruction::pcl_types::normal, instruction::flags::Z          },
	{ "000010dfffffff", "SUBWF" , pic12f::SUBWF , instruction::pcl_types::normal, instruction::flags::arithmetic },
	{ "000111dfffffff", "ADDWF" , pic12f::ADDWF , instruction::pcl_types::normal, instruction::flags::arithmetic },
	{ "001000dfffffff", "MOVF"  , pic12f::MOVF  , instruction::pcl_types::normal, instruction::flags::Z          },
	{ "001001dfffffff", "COMF"  , pic12f::COMF  , instruction::pcl_types::normal, instruction::flags::Z          },
	{ "000011dfffffff", "DECF"  , pic12f::DECF  , instruction::pcl_types::normal, instruction::flags::Z          },
	{ "001010dfffffff", "INCF"  , pic12f::INCF  , instruction::pcl_types::normal, instruction::flags::Z          },
	{ "001100dfffffff", "RRF"   , pic12f::RRF   , instruction::pcl_types::normal, instruction::flags::C          },
	{ "001101dfffffff", "RLF"   , pic12f::RLF   , instruction::pcl_types::normal, instruction::flags::C          },
	{ "001110dfffffff", "SWAPF" , pic12f::SWAPF , instruction::pcl_types::normal, instruction::flags::none       },
	{ "0100bbbfffffff", "BCF"   , pic12f::BCF   , instruction::pcl_types::normal, instruction::flags::none       },
	{ "0101bbbfffffff", "BSF"   , pic12f::BSF   , instruction::pcl_types::normal, instruction::flags::none       },
	{ "001011dfffffff", "DECFSZ", pic12f::DECFSZ, instruction::pcl_types::skip  , instruction::flags::none       },
	{ "001111dfffffff", "INCFSZ", pic12f::INCFSZ, instruction::pcl_types::skip  , instruction::flags::none       },
	{ "0110bbbfffffff", "BTFSC" , pic12f::BTFSC , instruction::pcl_types::skip  , instruction::flags::none       },
	{ "0111bbbfffffff", "BTFSS" , pic12f::BTFSS , instruction::pcl_types::skip  , instruction::flags::none       },
	{ "100kkkkkkkkkkk", "CALL"  , pic12f::CALL  , instruction::pcl_types::call  , instruction::flags::none       },
	{ "101kkkkkkkkkkk", "GOTO"  , pic12f::GOTO  , instruction::pcl_types::jump  , instruction::flags::none       },
	{ "1101xxkkkkkkkk", "RETLW" , pic12f::RETLW , instruction::pcl_types::ret   , instruction::flags::none       },
	{ "1100xxkkkkkkkk", "MOVLW" , pic12f::MOVLW , instruction::pcl_types::normal, instruction::flags::none       },
	{ "111000kkkkkkkk", "IORLW" , pic12f::IORLW , instruction::pcl_types::normal, instruction::flags::Z          },
	{ "111001kkkkkkkk", "ANDLW" , pic12f::ANDLW , instruction::pcl_types::normal, instruction::flags::Z          },
	{ "111010kkkkkkkk", "XORLW" , pic12f::XORLW , instruction::pcl_types::normal, instruction::flags::Z          },
	{ "11110xkkkkkkkk", "SUBLW" , pic12f::SUBLW , instruction::pcl_types::normal, instruction::flags::arithmetic },
	{ "11111xkkkkkkkk", "ADDLW" , pic12f::ADDLW , instruction::pcl_types::normal, instruction::flags::arithmetic }
};

//
// struct instruction
//

template<class T> bool instruction::match(const std::string& s, T f) const {
	
	if(s.length() != pattern.length())
		return false;

	for(int n = 0; n < (int)s.length(); n++) {

		if(pattern[n] == '0' || pattern[n] == '1') {
			if(s[n] != pattern[n])
				return false;
		} else {
			f(n);
		}
	}

	return true;
}

template<> bool instruction::match(const std::string& s, arguments *p) const {

	p->clear();

	return match(s, [&](int n) { p->operator[](pattern[n]).push_back(s[n]); });
}

bool instruction::match(const std::string& s) const {

	return match(s, [](int){});
}

bool instruction::operator<(const instruction& x) const {
	return pattern < x.pattern;
}

//
// struct instruction_set
//

instruction_set::value_type instruction_set::find(const std::string& s) const {

	for(const auto& op : *this)
		if(op.match(s))
			return op;

	return instruction_set::value_type();
}

void instruction_set::sort() {
	std::sort(begin(), end());
}

//
// struct bitstream
//

bitstream::bitstream() : bitstream(stdin) {
}

bitstream::bitstream(FILE *my_f) : f(my_f), buffer_size(0), buffer_pos(0) {
}

std::string bitstream::get(int n) {
	
	std::string s;

	while((int)s.length() < n) {

		if(left.empty()) {

			if(buffer_pos >= buffer_size) {
				buffer_pos = 0;
				buffer_size = fread(buffer, 1, buffer_maxsize, f);
			}

			if(buffer_size == 0)
				return s;

			char ch = buffer[buffer_pos++];

			for(int i = 0; i < 8; i++, ch >>= 1)
				left.push_back('0' + (ch & 1));
		}

		s.push_back(left.back());
		left.pop_back();
	}

	return s;
}

//
// global
//

std::string register_string(unsigned long x) {
	std::stringstream ss;
	ss << 'r' << std::uppercase << std::right << std::hex << std::setw(2) << std::setfill('0') << x;
	return ss.str();
}

std::string address_string(unsigned long x) {
	std::stringstream ss;
	ss << std::uppercase << std::right << std::hex << std::setw(3) << std::setfill('0') << x << 'h';
	return ss.str();
}

std::string dest_string(bool f,unsigned long x) {
	return f ? register_name(x) : "W";
}

std::string register_name(uint8_t x) {

	switch(x) {
		case 0x00: return "INDF";
		case 0x01: return "TMR0";
		case 0x02: return "PCL";
		case 0x03: return "STATUS";
		case 0x04: return "FSR";
		case 0x05: return "GPIO";
		case 0x0a: return "PCLATH";
		case 0x0b: return "INTCON";
		case 0x0c: return "PIR1";
		case 0x0e: return "TMR1L";
		case 0x0f: return "TMR1H";
		case 0x10: return "T1CON";
		case 0x19: return "CMCON";
		case 0x1e: return "ADRESH";
		case 0x1f: return "ADCON0";
	}
	return register_string(x);
}

//
// struct arguments
//

unsigned long arguments::value(key_type ch) const {
	return strtoul(at(ch).c_str(), NULL, 2);
}

bool arguments::has_arg(key_type ch) const {
	return (find(ch) != end());
}

bool arguments::has_args(const key_type *s) const {

	while(*s)
		if(!has_arg(*s++))
			return false;

	return true;
}

//
// struct expr
//

expr::expr() : expr("", {}) {
}

expr::expr(unsigned long my_value) : value(my_value), type(expr_type::literal) {
}

expr::expr(const expr& r) : prefix(r.prefix), value(r.value), type(r.type), args(r.args) {
}

expr::expr(const std::string& my_prefix) : expr(my_prefix, {}) {
}

expr::expr(const std::string& my_prefix, std::initializer_list<expr> my_args) : prefix(my_prefix), type(expr_type::symbol), args(my_args) {
}

std::string expr::str() const {

	std::stringstream ss;

	if(type == expr_type::symbol) {

		if(args.empty()) {

			ss << prefix;

		} else {

			ss << '(' << prefix;

			for(const auto& sub : args)
				ss << ' ' << sub.str();

			ss << ')';
		}

	} else if(type == expr_type::literal) {
		ss << std::dec << value;
	}

	return ss.str();
}

bool expr::is_nullary() const {
	return type == expr_type::symbol && args.empty();
}

bool expr::is_nonterminal() const {
	return type == expr_type::symbol && !args.empty();
}

bool expr::is_terminal() const {
	return type == expr_type::literal || is_nullary();
}

expr expr::expand(const dictionary::key_type& s,const dictionary& d) const {

	if(type == expr_type::literal)
		return *this;

	if(is_nullary())
		return (prefix == s) ? d.at(s) : *this;

	expr e(prefix);

	for(const auto& arg : args)
		e.args.push_back(arg.expand(s,d));

	return e;
}

expr expr::optimize() const {

	if(is_terminal())
		return *this;

	const std::list<std::string> unary = { "><", "~", "!>", "<!" };
	const std::list<std::string> binary = { "-" };

	unsigned int arity = UINT_MAX;

	for(const auto& op : unary) {
		if(prefix == op) {
			arity = args.size();
			if(arity != 1) {
				std::stringstream ss;
				ss << "unary operator \"" << prefix << "\" has unexpected argument count of " << args.size();
				throw std::runtime_error(ss.str());
			}
		}
	}

	for(const auto& op : binary) {
		if(prefix == op) {
			arity = args.size();
			if(arity != 2) {
				std::stringstream ss;
				ss << "binary operator \"" << prefix << "\" has unexpected argument count of " << args.size();
				throw std::runtime_error(ss.str());
			}
		}
	}

	if(arity == 1) {

		if(args.front().is_nonterminal()) {
			if(
					(prefix == "><" && args.front().prefix == prefix) ||
					(prefix == "~"  && args.front().prefix == prefix) ||
					(prefix == "<!" && args.front().prefix == "!>"  ) ||
					(prefix == "!>" && args.front().prefix == "<!"  )
			  )
			{
				return args.front().args.front().optimize();
			}
		}
		
		// fall-thru

	} else if(arity == 2) {

		// do nothing

	} else if(arity == UINT_MAX) {

		expr e(prefix);

		for(const auto& arg : args) {

			expr sub = arg.optimize();

			if(sub.is_nonterminal() && sub.prefix == prefix) {

				for(const auto& subarg : sub.args)
					e.args.push_back(subarg.optimize());

			} else {
				e.args.push_back(sub);
			}
		}

		// literals

		expr ee(prefix);

		unsigned long aggregated_literal = 0;

		bool literal_set = false;

		for(const auto& arg : e.args) {

			if(arg.type == expr_type::literal) {

				if(literal_set) {
					if(prefix == "+") {
						aggregated_literal += arg.value;
					} else if(prefix  == "^") {
						aggregated_literal ^= arg.value;
					} else if(prefix  == "|") {
						aggregated_literal |= arg.value;
					} else if(prefix  == "&") {
						aggregated_literal &= arg.value;
					} else {
						throw std::runtime_error("unknown n-ary operator");
					}
				} else {
					literal_set = true;
					aggregated_literal = arg.value;
				} 

			} else {
				ee.args.push_back(arg);
			}
		}

		if(literal_set)
			ee.args.push_front(aggregated_literal);

		// if the n-ary operator has only 1 parameter, then drop the operator

		return (ee.args.size() == 1) ? ee.args.front() : ee;
	}

	expr e(prefix);

	for(const auto& arg : args)
		e.args.push_back(arg.optimize());

	return e;
}

//
// struct dictionary
//

bool dictionary::has_key(const dictionary::key_type& s) const {
	return (find(s) != end());
}

dictionary::mapped_type& dictionary::touch(const dictionary::key_type& s) {
	if(!has_key(s))
		operator[](s) = dictionary::mapped_type(s);
	return at(s);
}

//
// struct operation
//

operation::operation() {
}

operation::operation(const std::string& my_s, unsigned long my_address, const instruction_set& cpu) : s(my_s), address(my_address) {
	opcode = cpu.find(s);
	opcode.match(s, &args);
}

void operation::execute(dictionary& d) {
	opcode.fn(*this, d);
}
