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

#define USE_B		reg_t b = (1 << OARG(L'b'))
#define USE_W		const std::wstring W = L"W"
#define USE_D		const std::wstring d = load_d(o)
#define USE_F		const std::wstring f = load_f(o)
#define USE_K		const unsigned long k = OARG(L'k')
#define USE_PC		const unsigned long pc = o.address
#define USE_REG(R)	const std::wstring R = register_name(instruction::file_register::R)
#define USE_STACK	const std::wstring STACK = L"STACK"

#define CLEAR_BIT(reg, mask) do{ USE_REG(reg); c.touch(reg); c[reg] = expression(OP_AND, { (reg_t)~(reg_t)(mask), c[reg] }); }while(0)
#define SET_BIT(reg, mask)   do{ USE_REG(reg); c.touch(reg); c[reg] = expression(OP_OR , {           (reg_t)(mask), c[reg] }); }while(0)

#define SET_PC(x) do{ USE_REG(PCL); USE_REG(PCLATH); c[PCL] = (reg_t)(x); c[PCLATH] = (reg_t)((x) >> 8); }while(0)

	std::wstring load_d(operation& o) {
		return dest_string(OARG(L'd'), OARG(L'f'));
	}

	std::wstring load_f(operation& o) {
		return register_name(OARG(L'f'));
	}

	HN(RETURN) {

		USE_REG(PCL);
		USE_REG(PCLATH);

		c[PCL] = expression(L"TOS.lo");
		c[PCLATH] = expression(L"TOS.hi");
	}

	GN(RETFIE) { RETURN(o,c);  SET_BIT(INTCON, instruction::flags::GIE); }

	FN(SLEEP)  { /* put microcontroller to sleep */ }
	FN(CLRWDT) { /* clear watchdog timer */         } 
	FN(NOP)    { /* no operation */                 }

	GN(MOVWF)  {

		USE_W;
		USE_F; 

		c[f] = c.touch(W);  
	}


	GN(CLR)    {

		USE_D;

		c[d] = 0;

		USE_REG(STATUS);

		expression& reg = c.touch(STATUS);

		reg = expression(OP_AND, { (reg_t)~instruction::flags::Z, reg });
	}

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

	GN(MOVF)   {

		USE_D;
		USE_F;

		c[d] = c.touch(f);

		USE_REG(STATUS);

		expression& reg = c.touch(STATUS);

		reg = expression(OP_AND, { (reg_t)~instruction::flags::Z, reg });
		reg = expression(OP_OR, { expression(L"Z"), reg });

	}

	GN(IORWF)  { USE_W; USE_D; USE_F; c[d] = expression(OP_OR   , { c.touch(f) , c.touch(W) }); }
	GN(ANDWF)  { USE_W; USE_D; USE_F; c[d] = expression(OP_AND  , { c.touch(f) , c.touch(W) }); }
	GN(XORWF)  { USE_W; USE_D; USE_F; c[d] = expression(OP_XOR  , { c.touch(f) , c.touch(W) }); }
	GN(SUBWF)  { USE_W; USE_D; USE_F; c[d] = expression(OP_MINUS, { c.touch(f) , c.touch(W) }); }
	GN(ADDWF)  { USE_W; USE_D; USE_F; c[d] = expression(OP_PLUS , { c.touch(f) , c.touch(W) }); }

	GN(DECF)   { USE_D; USE_F;         c[d] = expression(OP_MINUS, { c.touch(f), 1          }); }
	GN(INCF)   { USE_D; USE_F;         c[d] = expression(OP_PLUS , { c.touch(f), 1          }); }

	GN(COMF)   { USE_D; USE_F;         c[d] = expression(OP_NOT  , { c.touch(f)             }); }
	GN(RRF)    { USE_D; USE_F;         c[d] = expression(OP_ROTR , { c.touch(f)             }); }
	GN(RLF)    { USE_D; USE_F;         c[d] = expression(OP_ROTL , { c.touch(f)             }); }
	GN(SWAPF)  { USE_D; USE_F;         c[d] = expression(OP_SWAP , { c.touch(f)             }); }

	GN(BCF)    { USE_F; USE_B; b = ~b; c[f] = expression(OP_AND  , { b         , c.touch(f) }); }
	GN(BSF)    { USE_F; USE_B;         c[f] = expression(OP_OR   , { b         , c.touch(f) }); } 

	FN(DECFSZ) { throw std::runtime_error(std::string("DECFSZ performs conditional program counter modification")); }
	FN(INCFSZ) { throw std::runtime_error(std::string("INCFSZ performs conditional program counter modification")); }
	FN(BTFSC)  { throw std::runtime_error(std::string("BTFSC performs conditional program counter modification" )); }
	FN(BTFSS)  { throw std::runtime_error(std::string("BTFSS performs conditional program counter modification" )); }

	GN(GOTO)   { USE_K; SET_PC(k); }

	GN(CALL)   { GOTO(o, c); USE_PC; USE_STACK; c[STACK] = expression(OP_COMPOSE, { pc + 1, c.touch(STACK) }); }

	GN(MOVLW)  { USE_K; USE_W; c[W] = (reg_t)k; }

	GN(RETLW)  { MOVLW(o, c); RETURN(o, c); }

	GN(IORLW)  { USE_K; USE_W; c[W] = expression(OP_OR   , { k, c.touch(W) }); }
	GN(ANDLW)  { USE_K; USE_W; c[W] = expression(OP_AND  , { k, c.touch(W) }); }
	GN(XORLW)  { USE_K; USE_W; c[W] = expression(OP_XOR  , { k, c.touch(W) }); }
	GN(SUBLW)  { USE_K; USE_W; c[W] = expression(OP_MINUS, { k, c.touch(W) }); }
	GN(ADDLW)  { USE_K; USE_W; c[W] = expression(OP_PLUS , { k, c.touch(W) }); }

	HN(Z)      {

		USE_REG(STATUS);

		expression& reg = c.touch(STATUS);

		reg = expression(OP_AND, { (reg_t)~instruction::flags::Z, reg });
		reg = expression(OP_OR , { expression(L"Z"), reg });
	}

	FN(C)      { if(false) throw std::runtime_error("STATUS<C> carry flag unimplemented"           ); }
	FN(DC)     { if(false) throw std::runtime_error("STATUS<DC> decimal carry flag unimplemented"  ); }
	FN(PD)     { if(false) throw std::runtime_error("STATUS<PD> power down flag unimplemented"     ); }
	FN(TO)     { if(false) throw std::runtime_error("STATUS<TO> time-out flag unimplemented"       ); }

	GN(PC)     { USE_PC; SET_PC(pc); }

	FN(finalize) { /* update any registers and anything that happens not under direct control of program flow, such as GPIO. */ }

	void power(dictionary& c) {

		/* fix these for actual power on values */
		
		USE_REG(PCL)   ; c[PCL]    = 0;
		USE_REG(STATUS); c[STATUS] = 0;
		USE_REG(PCLATH); c[PCLATH] = 0;
		USE_REG(INTCON); c[INTCON] = 0;
		USE_REG(PIR1)  ; c[PIR1]   = 0;
		USE_REG(T1CON) ; c[T1CON]  = 0;
		USE_REG(CMCON) ; c[CMCON]  = 0;
		USE_REG(ADCON0); c[ADCON0] = 0;
	}

#undef USE_B
#undef USE_W
#undef USE_D
#undef USE_F
#undef USE_K
#undef USE_PC
#undef USE_REG
#undef USE_STACK

#undef FN
#undef GN
}

instruction_set pic12f675 = {

	{ L"00000000001000", L"RETURN", pic12f::RETURN, instruction::pcl_types::ret   , instruction::flags::none       },
	{ L"00000000001001", L"RETFIE", pic12f::RETFIE, instruction::pcl_types::ret   , instruction::flags::none       },
	{ L"0000000xx00000", L"NOP"   , pic12f::NOP   , instruction::pcl_types::normal, instruction::flags::none       },
	{ L"00000001100011", L"SLEEP" , pic12f::SLEEP , instruction::pcl_types::normal, instruction::flags::power      },
	{ L"00000001100100", L"CLRWDT", pic12f::CLRWDT, instruction::pcl_types::normal, instruction::flags::power      },
	{ L"0000001fffffff", L"MOVWF" , pic12f::MOVWF , instruction::pcl_types::normal, instruction::flags::none       },
	{ L"000001dfffffff", L"CLR"   , pic12f::CLR   , instruction::pcl_types::normal, instruction::flags::Z          },
	{ L"000100dfffffff", L"IORWF" , pic12f::IORWF , instruction::pcl_types::normal, instruction::flags::Z          },
	{ L"000101dfffffff", L"ANDWF" , pic12f::ANDWF , instruction::pcl_types::normal, instruction::flags::Z          },
	{ L"000110dfffffff", L"XORWF" , pic12f::XORWF , instruction::pcl_types::normal, instruction::flags::Z          },
	{ L"000010dfffffff", L"SUBWF" , pic12f::SUBWF , instruction::pcl_types::normal, instruction::flags::arithmetic },
	{ L"000111dfffffff", L"ADDWF" , pic12f::ADDWF , instruction::pcl_types::normal, instruction::flags::arithmetic },
	{ L"001000dfffffff", L"MOVF"  , pic12f::MOVF  , instruction::pcl_types::normal, instruction::flags::Z          },
	{ L"001001dfffffff", L"COMF"  , pic12f::COMF  , instruction::pcl_types::normal, instruction::flags::Z          },
	{ L"000011dfffffff", L"DECF"  , pic12f::DECF  , instruction::pcl_types::normal, instruction::flags::Z          },
	{ L"001010dfffffff", L"INCF"  , pic12f::INCF  , instruction::pcl_types::normal, instruction::flags::Z          },
	{ L"001100dfffffff", L"RRF"   , pic12f::RRF   , instruction::pcl_types::normal, instruction::flags::C          },
	{ L"001101dfffffff", L"RLF"   , pic12f::RLF   , instruction::pcl_types::normal, instruction::flags::C          },
	{ L"001110dfffffff", L"SWAPF" , pic12f::SWAPF , instruction::pcl_types::normal, instruction::flags::none       },
	{ L"0100bbbfffffff", L"BCF"   , pic12f::BCF   , instruction::pcl_types::normal, instruction::flags::none       },
	{ L"0101bbbfffffff", L"BSF"   , pic12f::BSF   , instruction::pcl_types::normal, instruction::flags::none       },
	{ L"001011dfffffff", L"DECFSZ", pic12f::DECFSZ, instruction::pcl_types::skip  , instruction::flags::none       },
	{ L"001111dfffffff", L"INCFSZ", pic12f::INCFSZ, instruction::pcl_types::skip  , instruction::flags::none       },
	{ L"0110bbbfffffff", L"BTFSC" , pic12f::BTFSC , instruction::pcl_types::skip  , instruction::flags::none       },
	{ L"0111bbbfffffff", L"BTFSS" , pic12f::BTFSS , instruction::pcl_types::skip  , instruction::flags::none       },
	{ L"100kkkkkkkkkkk", L"CALL"  , pic12f::CALL  , instruction::pcl_types::call  , instruction::flags::none       },
	{ L"101kkkkkkkkkkk", L"GOTO"  , pic12f::GOTO  , instruction::pcl_types::jump  , instruction::flags::none       },
	{ L"1101xxkkkkkkkk", L"RETLW" , pic12f::RETLW , instruction::pcl_types::ret   , instruction::flags::none       },
	{ L"1100xxkkkkkkkk", L"MOVLW" , pic12f::MOVLW , instruction::pcl_types::normal, instruction::flags::none       },
	{ L"111000kkkkkkkk", L"IORLW" , pic12f::IORLW , instruction::pcl_types::normal, instruction::flags::Z          },
	{ L"111001kkkkkkkk", L"ANDLW" , pic12f::ANDLW , instruction::pcl_types::normal, instruction::flags::Z          },
	{ L"111010kkkkkkkk", L"XORLW" , pic12f::XORLW , instruction::pcl_types::normal, instruction::flags::Z          },
	{ L"11110xkkkkkkkk", L"SUBLW" , pic12f::SUBLW , instruction::pcl_types::normal, instruction::flags::arithmetic },
	{ L"11111xkkkkkkkk", L"ADDLW" , pic12f::ADDLW , instruction::pcl_types::normal, instruction::flags::arithmetic }
};

//
// struct instruction
//

template<class T> bool instruction::match(const std::wstring& s, T f) const {
	
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

template<> bool instruction::match(const std::wstring& s, arguments *p) const {

	p->clear();

	return match(s, [&](int n) { p->operator[](pattern[n]).push_back(s[n]); });
}

bool instruction::match(const std::wstring& s) const {

	return match(s, [](int){});
}

bool instruction::operator<(const instruction& x) const {
	return pattern < x.pattern;
}

//
// struct instruction_set
//

instruction_set::value_type instruction_set::find(const std::wstring& s) const {

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

std::wstring bitstream::get(int n) {
	
	std::wstring s;

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

std::wstring register_string(unsigned long x) {
	std::wstringstream ts;
	ts << L'r' << std::uppercase << std::right << std::hex << std::setw(2) << std::setfill(L'0') << x;
	return ts.str();
}

std::wstring address_string(unsigned long x) {
	std::wstringstream ts;
	ts << std::uppercase << std::right << std::hex << std::setw(3) << std::setfill(L'0') << x << L'h';
	return ts.str();
}

std::wstring dest_string(bool f,unsigned long x) {
	return f ? register_name(x) : L"W";
}

std::wstring register_name(reg_t x) {

	switch(x) {
		case 0x00: return L"INDF";
		case 0x01: return L"TMR0";
		case 0x02: return L"PCL";
		case 0x03: return L"STATUS";
		case 0x04: return L"FSR";
		case 0x05: return L"GPIO";
		case 0x0a: return L"PCLATH";
		case 0x0b: return L"INTCON";
		case 0x0c: return L"PIR1";
		case 0x0e: return L"TMR1L";
		case 0x0f: return L"TMR1H";
		case 0x10: return L"T1CON";
		case 0x19: return L"CMCON";
		case 0x1e: return L"ADRESH";
		case 0x1f: return L"ADCON0";
	}
	return register_string(x);
}

//
// struct arguments
//

unsigned long arguments::value(key_type ch) const {

	unsigned long x = 0;

	const std::wstring& s = at(ch);

	for(unsigned int n = 0; n < s.length(); n++) {

		x <<= 1;

		if(s[n] == L'1')
			x++;
	}

	return x;
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
// struct expression
//

expression::expression() : expression(OP_COMPOSE, {}) {
}

expression::expression(const expression& r) : name(r.name), value(r.value), op(r.op), type(r.type), args(r.args) {
}

expression::expression(unsigned long my_value) : value(my_value), type(expr_type::literal) {
}

expression::expression(const std::wstring& my_name) : name(my_name), type(expr_type::variable) {
}

expression::expression(wchar_t my_op, std::initializer_list<expression> my_args) : op(my_op), type(expr_type::function), args(my_args) {
}

std::wstring expression::wstr() const {

	std::wstringstream ws;

	switch(type) {

		case expr_type::variable:

			ws << name;

			break;

		case expr_type::function:

			ws << L'(' << (wchar_t)op;

			for(const auto& sub : args)
				ws << L' ' << sub.wstr();

			ws << L')';


			break;

		case expr_type::literal:

			ws << std::showbase << std::hex << value;

			break;
	}

	return ws.str();
}

bool expression::is_function(wchar_t my_op) const {
	return type == expr_type::function && op == my_op;
}

/*
expression expression::expand(const dictionary::key_type&,const dictionary&) const {

	if(type != expr_type::function)
		return *this;

	return *this;
}

expression expression::flatten() const {

	if(type != expr_type::function)
		return *this;

	expression e(op);

	for(const auto& arg : args) {

		if(arg.is_function(op)) {

			expression sub_expr = arg.flatten();

			for(const auto& sub_arg : sub_expr.args)
				e.args.push_back(sub_arg);

		} else {
			e.args.push_back(arg);
		}
	}

	return e;
}

expression expression::optimize() const {

	if(type != expr_type::function)
		return *this;

	if(args.empty())
		return *this;

	expression::args_type dargs;

	if(op == OP_AND || op == OP_OR || op == OP_XOR || op == OP_PLUS)
		for(const auto& arg : flatten().args)
			dargs.push_back(arg.optimize());
	else
		for(const auto& arg : args)
			dargs.push_back(arg.optimize());

	const std::list<wchar_t> unary = { OP_SWAP, OP_NOT, OP_ROTL, OP_ROTR };
	const std::list<wchar_t> binary = { OP_MINUS };

	unsigned int arity = UINT_MAX;

	for(const auto& my_op : unary) {
		if(op == my_op) {
			arity = dargs.size();
			if(arity != 1) {
				std::wstringstream ss;
				ss << "unary operator \"" << op << "\" has unexpected argument count of " << dargs.size();
				throw ss.str();
			}
		}
	}

	for(const auto& my_op : binary) {
		if(op == my_op) {
			arity = dargs.size();
			if(arity != 2) {
				std::wstringstream ss;
				ss << "binary operator \"" << op << "\" has unexpected argument count of " << dargs.size();
				throw ss.str();
			}
		}
	}

	if(is_function(OP_COMPOSE))
		return flatten();

	if(arity == 1) {

		const expression& arg0 = dargs.front();

		if(
				(arg0.type == expr_type::function)

				&&

				(
				 (is_function(OP_SWAP) && arg0.is_function(OP_SWAP)) ||
				 (is_function(OP_NOT ) && arg0.is_function(OP_NOT )) ||
				 (is_function(OP_ROTL) && arg0.is_function(OP_ROTR)) ||
				 (is_function(OP_ROTR) && arg0.is_function(OP_ROTL))
				)
		  )
		{
			return arg0.args.front().optimize();
		}

	} else if(arity == 2) {

		// nothing

	} else if(arity == UINT_MAX) {

		args_type ddargs;

		reg_t acc = (op == OP_AND) ? ~0 : 0;
		int acc_count = 0;

		for(const auto& arg : dargs) {

			if(arg.type == expr_type::literal) {

				acc_count++;

				if(op == OP_PLUS) {

					acc += arg.value;

				} else if(op == OP_XOR) {

					acc ^= arg.value;

				} else if(op == OP_OR) {

					acc |= arg.value;

				} else if(op == OP_AND) {

					acc &= arg.value;

				} else {

					throw std::runtime_error("unknown n-ary operator");
				}

			} else {
				ddargs.push_back(arg);
			}
		}

		if(acc_count == 0) {

			return expression(op, ddargs).flatten();

		} else {
			if(ddargs.size() == 0) {

				return acc;

			} else if(ddargs.size() == 1) {

				if(is_function(OP_OR) || is_function(OP_AND)) {

					expression rarg = ddargs.front().optimize();

					if(!rarg.args.empty() && (rarg.op == OP_OR || rarg.op == OP_AND)) {

						expression re(rarg.op);

						for(const auto& sub_rarg : rarg.args) {

							expression re_sub(op, { (unsigned long)acc, sub_rarg });

							re_sub = re_sub.optimize();

							if(re_sub.type == expr_type::literal && re_sub.value == 0 && rarg.op == OP_OR) {
								// nothing
							} else if(re_sub.type == expr_type::literal && re_sub.value == 255 && rarg.op == OP_AND) {
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
			return expression(op, ddargs);

		}
	}

	return expression(op, dargs);
}
*/

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

operation::operation(const std::wstring& my_s, unsigned long my_address, const instruction_set& cpu) : s(my_s), address(my_address) {
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
