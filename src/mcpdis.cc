#include <mcpdis.hh>
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <cstdlib>
#include <cstdint>
#include <climits>
#include <iostream>
#include <set>

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

#define CLEAR_BIT(reg, mask) do{ USE_REG(reg); c.touch(reg); c[reg] = function(OP_AND, { (reg_t)~(reg_t)(mask), c[reg] }); }while(0)
#define SET_BIT(reg, mask)   do{ USE_REG(reg); c.touch(reg); c[reg] = function(OP_OR , {         (reg_t)(mask), c[reg] }); }while(0)

#define SET_PC(x) do{ USE_REG(PCL); USE_REG(PCLATH); c[PCL] = (reg_t)(x); c[PCLATH] = (reg_t)((x) >> 8); }while(0)

namespace pic12f {

	std::wstring load_d(operation& o) {
		return dest_string(OARG(L'd'), OARG(L'f'));
	}

	std::wstring load_f(operation& o) {
		return register_name(OARG(L'f'));
	}

	HN(RETURN) {

		USE_REG(PCL);
		USE_REG(PCLATH);

		c[PCL] = term(L"TOS.lo");
		c[PCLATH] = term(L"TOS.hi");
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

		term& reg = c.touch(STATUS);

		reg = function(OP_AND, { (reg_t)~instruction::flags::Z, reg });
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

		term& reg = c.touch(STATUS);

		reg = function(OP_AND, { (reg_t)~instruction::flags::Z, reg });
		reg = function(OP_OR , { (reg_t) instruction::flags::Z, reg });
		// reg = term(function(OP_OR, { term(L"Z"), reg }));

	}

	GN(IORWF)  { USE_W; USE_D; USE_F; c[d] = function(OP_OR   , { c.touch(f) , c.touch(W) }); }
	GN(ANDWF)  { USE_W; USE_D; USE_F; c[d] = function(OP_AND  , { c.touch(f) , c.touch(W) }); }
	GN(XORWF)  { USE_W; USE_D; USE_F; c[d] = function(OP_XOR  , { c.touch(f) , c.touch(W) }); }
	GN(SUBWF)  { USE_W; USE_D; USE_F; c[d] = function(OP_MINUS, { c.touch(f) , c.touch(W) }); }
	GN(ADDWF)  { USE_W; USE_D; USE_F; c[d] = function(OP_PLUS , { c.touch(f) , c.touch(W) }); }

	GN(DECF)   { USE_D; USE_F;         c[d] = function(OP_MINUS, { c.touch(f), 1          }); }
	GN(INCF)   { USE_D; USE_F;         c[d] = function(OP_PLUS , { c.touch(f), 1          }); }

	GN(COMF)   { USE_D; USE_F;         c[d] = function(OP_NOT  , { c.touch(f)             }); }
	GN(RRF)    { USE_D; USE_F;         c[d] = function(OP_ROTR , { c.touch(f)             }); }
	GN(RLF)    { USE_D; USE_F;         c[d] = function(OP_ROTL , { c.touch(f)             }); }
	GN(SWAPF)  { USE_D; USE_F;         c[d] = function(OP_SWAP , { c.touch(f)             }); }

	GN(BCF)    { USE_F; USE_B; b = ~b; c[f] = function(OP_AND  , { b         , c.touch(f) }); }
	GN(BSF)    { USE_F; USE_B;         c[f] = function(OP_OR   , { b         , c.touch(f) }); } 

	FN(DECFSZ) { throw std::runtime_error(std::string("DECFSZ performs conditional program counter modification")); }
	FN(INCFSZ) { throw std::runtime_error(std::string("INCFSZ performs conditional program counter modification")); }
	FN(BTFSC)  { throw std::runtime_error(std::string("BTFSC performs conditional program counter modification" )); }
	FN(BTFSS)  { throw std::runtime_error(std::string("BTFSS performs conditional program counter modification" )); }

	GN(GOTO)   { USE_K; SET_PC(k); }

	GN(CALL)   { GOTO(o, c); USE_PC; USE_STACK; c[STACK] = function(OP_LIST, { pc + 1, c.touch(STACK) }); }

	GN(MOVLW)  { USE_K; USE_W; c[W] = (reg_t)k; }

	GN(RETLW)  { MOVLW(o, c); RETURN(o, c); }

	GN(IORLW)  { USE_K; USE_W; c[W] = function(OP_OR   , { k, c.touch(W) }); }
	GN(ANDLW)  { USE_K; USE_W; c[W] = function(OP_AND  , { k, c.touch(W) }); }
	GN(XORLW)  { USE_K; USE_W; c[W] = function(OP_XOR  , { k, c.touch(W) }); }
	GN(SUBLW)  { USE_K; USE_W; c[W] = function(OP_MINUS, { k, c.touch(W) }); }
	GN(ADDLW)  { USE_K; USE_W; c[W] = function(OP_PLUS , { k, c.touch(W) }); }

	GN(PC)     {
		USE_PC;
		SET_PC(pc);
	}

	void finalize() {
		/* update any registers and anything that happens not under direct control of program flow, such as GPIO. */
	}

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

/////////////////////
//
//
// struct instruction
//
//
/////////////////////

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

/////////////////////////
//
//
// struct instruction_set
//
//
/////////////////////////

instruction_set::value_type instruction_set::find(const std::wstring& s) const {

	for(const auto& op : *this)
		if(op.match(s))
			return op;

	return instruction_set::value_type();
}

void instruction_set::sort() {
	std::sort(begin(), end());
}

///////////////////
//
//
// struct bitstream
//
//
///////////////////

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

///////////////////
//
//
// struct arguments
//
//
///////////////////

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
	return find(ch) != end();
}

bool arguments::has_args(const key_type *s) const {

	while(*s)
		if(!has_arg(*s++))
			return false;

	return true;
}

///////////////////////
//
//
// transformation rules
//
//
///////////////////////

template<class F> static void aggregate_literals(term& e, reg_t x0, F f) {

	reg_t x = x0;

	if(!e.is_function())
		return;

	auto iter = e.f.args.begin();

	while(iter != e.f.args.end()) {

		auto jter = next(iter);

		if(iter->is_literal()) {
			x = f(x, iter->l);
			e.f.args.erase(iter);
		}

		iter = jter;
	}

	if(x != x0)
		e.f.args.push_front(x);
}

static void association_rule(term& e, op_t op, const arglist& args) {

	for(const auto& arg : args) {

		if(arg.is_function(op))
			e.f.args.insert(e.f.args.end(), arg.f.args.begin(), arg.f.args.end());
		else
			e.f.args.push_back(arg);
	}
}

static void distribution_rule(term& e, op_t op1, op_t op2) {

	// find term to distribute
	// in order of highest priority first
	// : literals, variables, functions ("OP_AND" only)

	if(!e.is_function())
		return;

	auto jter = e.f.args.begin();

	while(jter != e.f.args.end() && !jter->is_literal())
		jter++;

	if(jter == e.f.args.end()) {
		jter = e.f.args.begin();
		while(jter != e.f.args.end() && jter->is_variable())
			jter++;
	}

	if(jter == e.f.args.end()) {
		jter = e.f.args.begin();
		while(jter != e.f.args.end() && !jter->is_function(op2))
			jter++;
	}

	if(jter == e.f.args.end())
		return;

	// find term to distribute over
	// specifically ("OP_AND") functions only

	auto kter = e.f.args.begin();

	while(kter != e.f.args.end() && (kter == jter || !kter->is_function(op2)))
		kter++;

	if(kter == e.f.args.end())
		return;

	// distribute *jter over kter->f.args

	const auto& e1 = *jter;

	for(auto& arg : kter->f.args) {
		const auto& e2 = arg;
		arg = function(op1, { e1, e2 });
	}

	e.f.args.erase(jter);
}

////////////////////////////////
//
//
// function_expression templates
//
//
////////////////////////////////

template<wchar_t> static void function_expression(term&, const arglist&);

template<> void function_expression<OP_SWAP>(term& e, const arglist& args) {

	if(args.size() != 1) {
		std::stringstream ss;
		ss << "unexpected arity of " << args.size() << " for unary swap (\"OP_SWAP\") operation." << std::endl;
		throw std::runtime_error(ss.str());
	}

	if(args.front().is_function(OP_SWAP)) {

		e = args.front().f.args.front();

	} else if(args.front().is_literal()) {

		uint8_t reg = args.front().l;

		reg = (reg << 4) | (reg >> 4);
		e = reg;

	} else {
		e.f.args = args;
	}
}

template<> void function_expression<OP_NOT>(term& e, const arglist& args) {

	if(args.size() != 1) {
		std::stringstream ss;
		ss << "unexpected arity of " << args.size() << " for unary not (\"OP_NOT\") operation." << std::endl;
		throw std::runtime_error(ss.str());
	}

	if(args.front().is_function(OP_NOT)) {

		e = args.front().f.args.front();

	} else if(args.front().is_literal()) {

		reg_t reg = args.front().l;

		reg = ~reg;
		e = reg;

	} else {
		e.f.args = args;
	}
}

template<> void function_expression<OP_OR>(term& e, const arglist& args) {

	association_rule(e, OP_OR, args);

	aggregate_literals(e, (reg_t)0, [](reg_t a, reg_t x) -> reg_t { return a | x; });

	distribution_rule(e, OP_OR, OP_AND);
	
	if(e.is_unary() && OP_IS_UNARY_NOP(e.f.op)) e = e.f.args.front();
}

template<> void function_expression<OP_AND>(term& e, const arglist& args) {

	association_rule(e, OP_AND, args);

	aggregate_literals(e, (reg_t)~0, [](reg_t a, reg_t x) -> reg_t { return a & x; });

	distribution_rule(e, OP_AND, OP_OR);

	if(e.is_unary() && OP_IS_UNARY_NOP(e.f.op)) e = e.f.args.front();
}

template<> void function_expression<OP_PLUS>(term& e, const arglist& args) {

	association_rule(e, OP_PLUS, args);

	aggregate_literals(e, 0, [](reg_t a, reg_t x) -> reg_t { return a + x; });
}

template<wchar_t> static void function_expression(term& e, const arglist& args) {
	e.f.args = args;
}

//////////////////
//
//
// struct function
//
//
//////////////////

function::function() : function(OP_LIST, {}) {
}

function::function(op_t my_op) : function(my_op, {}) {
}

function::function(op_t my_op, const arglist& my_args) : op(my_op), args(my_args) {
}

function::function(const function&r) : function(r.op, r.args) {
}

function& function::operator=(const function& r) {

	if(this != &r) {
		op = r.op;
		args = arglist(r.args);
	}

	return *this;
}

bool function::operator==(const function& r) const {
	return op == r.op && args == r.args;
}

size_t function::arity() const {
	return args.size();
}

//////////////
//
//
// struct term
//
//
//////////////

term& term::operator=(const term& r) {

	if(this != &r) {

		if(is_function())
			f.args.clear();

		type = r.type;

		switch(type) {
			case term_type::literal  : l = r.l; break;
			case term_type::variable : v = r.v; break;
			case term_type::function : f = r.f; break;
		}
	}

	return *this;
}

term::~term() {
	if(type == term_type::function) {
		f.args.~list();
	}
}

term::term() : term(function(OP_LIST)) {
}

term::term(const term& r) {
	*this = r;
}

term::term(literal_t my_l) : type(term_type::literal), l(my_l) {
}

term::term(const variable& my_v) : type(term_type::variable), v(my_v) {
}

#define OPERATION_HANDLER(__OP__) case __OP__: function_expression<__OP__>(*this, my_args); break

term::term(const function& my_f) : type(term_type::function), f(my_f) {
	/*
	switch(op) {

		OPERATION_HANDLER(OP_AND);
		OPERATION_HANDLER(OP_OR);
		OPERATION_HANDLER(OP_XOR);
		OPERATION_HANDLER(OP_PLUS);
		OPERATION_HANDLER(OP_MINUS);
		OPERATION_HANDLER(OP_SWAP);
		OPERATION_HANDLER(OP_NOT);
		OPERATION_HANDLER(OP_ROTL);
		OPERATION_HANDLER(OP_ROTR);
		OPERATION_HANDLER(OP_COMPOSE);
		OPERATION_HANDLER(OP_LIST);

		default:
		throw std::runtime_error("unhandled operation");
		break;
	}
	*/
}

#undef OPERATION_HANDLER

std::wstring term::wstr() const {

	std::wstringstream ws;

	switch(type) {

		case term_type::variable:

			ws << v;

			break;

		case term_type::function:

			ws << L'(' << (op_t)f.op;

			for(const auto& sub : f.args)
				ws << L' ' << sub.wstr();

			ws << L')';


			break;

		case term_type::literal:

			ws << std::showbase << (l < 10 ? std::dec : std::hex) << l;

			break;
	}

	return ws.str();
}

bool term::is_function(op_t my_op) const {
	return type == term_type::function && f.op == my_op;
}

bool term::is_variable(const variable& my_v) const {
	return type == term_type::variable && v == my_v;
}

bool term::is_literal(literal_t my_l) const {
	return type == term_type::literal && l == my_l;
}

bool term::is_function() const {
	return type == term_type::function;
}

bool term::is_variable() const {
	return type == term_type::variable;
}

bool term::is_literal() const {
	return type == term_type::literal;
}

bool term::is_nullary() const {
	return is_arity(0);
}

bool term::is_unary() const {
	return is_arity(1);
}

bool term::is_binary() const {
	return is_arity(2);
}

bool term::is_arity(size_t n) const {
	return is_function() && f.arity() == n;
}

bool term::operator==(const term& r) const {

	if(type != r.type)
		return false;

	switch(type) {
		case term_type::literal  : return l == r.l;
		case term_type::variable : return v == r.v;
		case term_type::function : return f == r.f;
	}

	return false;
}

/*
bool term::operator<(const term& r) const {

	if(type != r.type)
		return type < r.type;

	switch(type) {

		case term_type::literal:

			return value < r.value;

		case term_type::variable:

			return v < r.v;

		case term_type::function: 

			// lexicographic comparison

			auto iter = args.cbegin();
			auto jter = r.args.cbegin();

			while(iter != args.end() && jter != r.args.end()) {

				if(*iter < *jter) {

					return true;

				} else if(*iter == *jter) {

					iter++;
					jter++;

				} else {
					return false;
				}
			}
	}

	return false;
}
*/

////////////////////
//
//
// struct dictionary
//
//
////////////////////

bool dictionary::has_key(const dictionary::key_type& s) const {
	return find(s) != end();
}

dictionary::mapped_type& dictionary::touch(const dictionary::key_type& s) {
	if(!has_key(s))
		operator[](s) = dictionary::mapped_type(s);
	return at(s);
}

std::wstring str(const dictionary::value_type& x) {
	std::wstringstream ws;
	ws << std::setw(6) << std::left << std::setfill(L' ') << x.first << L" := " << x.second.wstr();
	return ws.str();
}

///////////////////
//
//
// struct operation
//
//
///////////////////

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

	if(opcode.pcl_type == instruction::pcl_types::normal)
		pic12f::PC(*this, d);

	pic12f::finalize();
}

/////////
//
//
// global
//
//
/////////

std::wstring address_string(literal_t x) {
	std::wstringstream ts;
	ts << std::uppercase << std::right << std::hex << std::setw(3) << std::setfill(L'0') << x << L'h';
	return ts.str();
}

std::wstring register_string(reg_t x) {
	std::wstringstream ts;
	ts << L'r' << std::uppercase << std::right << std::hex << std::setw(2) << std::setfill(L'0') << x;
	return ts.str();
}

std::wstring dest_string(bool f, reg_t x) {
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


