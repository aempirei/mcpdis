#include <mcpdis.hh>
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <cstdlib>
#include <cstdint>
#include <climits>
#include <iostream>

namespace pic12f {

#define OARG(X)		o.args.value(X)

#define FN(a)		void a(operation&, dictionary&)
#define GN(a)		void a(operation&o, dictionary&c)
#define HN(a)		void a(operation&, dictionary&c)

#define LOAD_D		const std::string d = load_d(o)
#define LOAD_F		const std::string f = load_f(o)
#define LOAD_W		const std::string W = "W"
#define LOAD_STACK	const std::string STACK = "STACK"
#define LOAD_K		const unsigned long k = OARG('k')
#define LOAD_B		uint8_t b = (1 << OARG('b'))
#define LOAD_R(R)	const std::string R = register_name(instruction::file_register::R)
#define LOAD_PC		const unsigned long pc = o.address

#define CLEAR_BIT(reg, mask) do{ LOAD_R(reg); c.touch(reg); c[reg] = expr("&", { (uint8_t)~(uint8_t)(mask), c[reg] }); }while(0)
#define SET_BIT(reg, mask)   do{ LOAD_R(reg); c.touch(reg); c[reg] = expr("|", {           (uint8_t)(mask), c[reg] }); }while(0)


	std::string load_d(operation& o) {
		return dest_string(OARG('d'), OARG('f'));
	}

	std::string load_f(operation& o) {
		return register_name(OARG('f'));
	}

	HN(RETURN) { LOAD_R(PCL); LOAD_R(PCLATH); c[PCL] = expr("TOS.lo"); c[PCLATH] = expr("TOS.hi"); }
	GN(RETFIE) { RETURN(o,c);  SET_BIT(INTCON, instruction::flags::GIE); }

	FN(SLEEP)  { /* put microcontroller to sleep */ }
	FN(CLRWDT) { /* clear watchdog timer */         } 
	FN(NOP)    { /* no operation */                 }

	GN(CLR)    { LOAD_D;                 c[d] = 0; CLEAR_BIT(STATUS, instruction::flags::Z); }

	GN(MOVWF)  { LOAD_W; LOAD_F;         c[f] = c.touch(W);                             }
	GN(MOVF)   { LOAD_D; LOAD_F;         c[d] = c.touch(f);                             }

	/*
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
	*/
	
	GN(IORWF)  { LOAD_W; LOAD_D; LOAD_F; c[d] = expr("|", { c.touch(f) , c.touch(W) }); }
	GN(ANDWF)  { LOAD_W; LOAD_D; LOAD_F; c[d] = expr("&", { c.touch(f) , c.touch(W) }); }
	GN(XORWF)  { LOAD_W; LOAD_D; LOAD_F; c[d] = expr("^", { c.touch(f) , c.touch(W) }); }
	GN(SUBWF)  { LOAD_W; LOAD_D; LOAD_F; c[d] = expr("-", { c.touch(f) , c.touch(W) }); }
	GN(ADDWF)  { LOAD_W; LOAD_D; LOAD_F; c[d] = expr("+", { c.touch(f) , c.touch(W) }); }

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

#define SET_PC(x) do{ LOAD_R(PCL); LOAD_R(PCLATH); c[PCL] = (uint8_t)(x); c[PCLATH] = (uint8_t)((x) >> 8); }while(0)

	GN(GOTO)   { LOAD_K; SET_PC(k); }

	GN(CALL)   { GOTO(o, c); LOAD_PC; LOAD_STACK; c[STACK] = expr(".", { pc + 1, c.touch(STACK) }); }

	GN(MOVLW)  { LOAD_K; LOAD_W; c[W] = (uint8_t)k; }

	GN(RETLW)  { MOVLW(o, c); RETURN(o, c); }

	GN(IORLW)  { LOAD_K; LOAD_W; c[W] = expr("|", { k, c.touch(W) }); }
	GN(ANDLW)  { LOAD_K; LOAD_W; c[W] = expr("&", { k, c.touch(W) }); }
	GN(XORLW)  { LOAD_K; LOAD_W; c[W] = expr("^", { k, c.touch(W) }); }
	GN(SUBLW)  { LOAD_K; LOAD_W; c[W] = expr("-", { k, c.touch(W) }); }
	GN(ADDLW)  { LOAD_K; LOAD_W; c[W] = expr("+", { k, c.touch(W) }); }

	HN(Z)      {
		LOAD_R(STATUS);
		c.touch(STATUS);
		c[STATUS] = expr("&", { (uint8_t)~instruction::flags::Z, c[STATUS] });
		c[STATUS] = expr("|", { expr("Z"), c[STATUS] });
	}

	FN(C)      { if(false) throw std::runtime_error("STATUS<C> carry flag unimplemented"           ); }
	FN(DC)     { if(false) throw std::runtime_error("STATUS<DC> decimal carry flag unimplemented"  ); }
	FN(PD)     { if(false) throw std::runtime_error("STATUS<PD> power down flag unimplemented"     ); }
	FN(TO)     { if(false) throw std::runtime_error("STATUS<TO> time-out flag unimplemented"       ); }

	GN(PC)     { LOAD_PC; SET_PC(pc); }

	FN(finalize) { /* update any registers and anything that happens not under direct control of program flow, such as GPIO. */ }

	void power(dictionary& c) {

		/* fix these for actual power on values */
		
		LOAD_R(PCL)   ; c[PCL]    = 0;
		LOAD_R(STATUS); c[STATUS] = 0;
		LOAD_R(PCLATH); c[PCLATH] = 0;
		LOAD_R(INTCON); c[INTCON] = 0;
		LOAD_R(PIR1)  ; c[PIR1]   = 0;
		LOAD_R(T1CON) ; c[T1CON]  = 0;
		LOAD_R(CMCON) ; c[CMCON]  = 0;
		LOAD_R(ADCON0); c[ADCON0] = 0;
	}

#undef LOAD_D
#undef LOAD_F
#undef LOAD_W
#undef LOAD_K
#undef LOAD_B
#undef LOAD_R
#undef LOAD_PC

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

expr::expr(const std::string& my_prefix, const args_type& my_args) : prefix(my_prefix), type(expr_type::symbolic), args(my_args) {
}

std::string expr::str() const {

	std::stringstream ss;

	switch(type) {

		case expr_type::symbolic:

			if(!args.empty())
				ss << '(';

			ss << prefix;

			for(const auto& sub : args)
				ss << ' ' << sub.str();

			if(!args.empty())
				ss << ')';


			break;

		case expr_type::literal:

			ss << std::showbase << std::hex << value;

			break;
	}

	return ss.str();
}

expr expr::expand(const dictionary::key_type&,const dictionary&) const {

	if(type != expr_type::symbolic)
		return *this;

	return *this;
}

bool expr::is_function(const std::string& s) const {
	return type == expr_type::symbolic && prefix == s;
}

expr expr::flatten() const {

	if(type != expr_type::symbolic)
		return *this;

	expr e(prefix);

	for(const auto& arg : args) {

		if(arg.is_function(prefix)) {

			expr sub_expr = arg.flatten();

			for(const auto& sub_arg : sub_expr.args)
				e.args.push_back(sub_arg);

		} else {
			e.args.push_back(arg);
		}
	}

	return e;
}

expr expr::optimize() const {

	if(type != expr_type::symbolic)
		return *this;

	if(args.empty())
		return *this;

	expr::args_type dargs;

	if(prefix == "&" || prefix == "|" || prefix == "^" || prefix == "+")
		for(const auto& arg : flatten().args)
			dargs.push_back(arg.optimize());
	else
		for(const auto& arg : args)
			dargs.push_back(arg.optimize());

	const std::list<std::string> unary = { "><", "~", "!>", "<!" };
	const std::list<std::string> binary = { "-" };

	unsigned int arity = UINT_MAX;

	for(const auto& op : unary) {
		if(prefix == op) {
			arity = dargs.size();
			if(arity != 1) {
				std::stringstream ss;
				ss << "unary operator \"" << prefix << "\" has unexpected argument count of " << dargs.size();
				throw std::runtime_error(ss.str());
			}
		}
	}

	for(const auto& op : binary) {
		if(prefix == op) {
			arity = dargs.size();
			if(arity != 2) {
				std::stringstream ss;
				ss << "binary operator \"" << prefix << "\" has unexpected argument count of " << dargs.size();
				throw std::runtime_error(ss.str());
			}
		}
	}

	if(is_function(".")) {
		return flatten();
	}

	if(arity == 1) {

		const expr& arg0 = dargs.front();

		if(
				(arg0.type == expr_type::symbolic)

				&&

				(
				 (is_function("><") && arg0.is_function("><")) ||
				 (is_function("~" ) && arg0.is_function("~" )) ||
				 (is_function("<!") && arg0.is_function("!>")) ||
				 (is_function("!>") && arg0.is_function("<!"))
				)
		  )
		{
			return arg0.args.front().optimize();
		}

	} else if(arity == 2) {

		// nothing

	} else if(arity == UINT_MAX) {

		args_type ddargs;

		uint8_t acc = (prefix == "&") ? ~0 : 0;
		int acc_count = 0;

		for(const auto& arg : dargs) {

			if(arg.type == expr_type::literal) {

				acc_count++;

				if(prefix == "+") {

					acc += arg.value;

				} else if(prefix == "^") {

					acc ^= arg.value;

				} else if(prefix == "|") {

					acc |= arg.value;

				} else if(prefix == "&") {

					acc &= arg.value;

				} else {

					throw std::runtime_error("unknown n-ary operator");
				}

			} else {
				ddargs.push_back(arg);
			}
		}

		if(acc_count == 0) {

			return expr(prefix, ddargs).flatten();

		} else {
			if(ddargs.size() == 0) {

				return expr(acc);

			} else if(ddargs.size() == 1) {

				if(is_function("|") || is_function("&")) {

					expr rarg = ddargs.front().optimize();

					if(!rarg.args.empty() && (rarg.prefix == "|" || rarg.prefix == "&")) {

						expr re(rarg.prefix);

						for(const auto& sub_rarg : rarg.args) {

							expr re_sub(prefix, { acc, sub_rarg });

							re_sub = re_sub.optimize();

							if(re_sub.type == expr_type::literal && re_sub.value == 0 && rarg.prefix == "|") {
								// nothing
							} else if(re_sub.type == expr_type::literal && re_sub.value == 255 && rarg.prefix == "&") {
								// nothing
							} else {
								re.args.push_back( re_sub );
							}
						}

						if(re.args.size() == 1)
							return re.args.front();

						return re.flatten();
					}

				}
			}

			ddargs.push_front(acc);
			return expr(prefix, ddargs);

		}
	}

	return expr(prefix, dargs);
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

	if(address == 0x000U)
		pic12f::power(d);

	opcode.fn(*this, d);

	if(opcode.status & instruction::flags::Z)
		pic12f::Z(*this, d);

	if(opcode.status & instruction::flags::C)
		pic12f::C(*this, d);

	if(opcode.status & instruction::flags::DC)
		pic12f::DC(*this, d);

	if(opcode.status & instruction::flags::TO)
		pic12f::TO(*this, d);

	if(opcode.status & instruction::flags::PD)
		pic12f::PD(*this, d);

	if(opcode.pcl_type == instruction::pcl_types::normal)
		pic12f::PC(*this, d);

	pic12f::finalize(*this, d);
}
