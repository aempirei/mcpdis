#include <mcpdis.hh>
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <cstdlib>
#include <iostream>

#define F(a) void a(operation&, dictionary&)
#define G(a) void a(operation&o, dictionary&c)

namespace pic12f {

	void wf_function(std::string name, operation& o, dictionary& c) {

		std::string f = register_string(o.args.value('f'));
		std::string d = dest_string(o.args.value('d'), o.args.value('f'));

		c.touch("W");
		c.touch(f);

		const auto& W = c.at("W");
		const auto& F = c.at(f);

		expression D = { name };

		D.append(F);
		D.append(W);
		D.parens();

		c[d] = D;
	}

	F(RETURN) { throw std::runtime_error(std::string("RETURN overwrites program counter")); }
	F(RETFIE) { throw std::runtime_error(std::string("RETFIE overwrites program counter")); }
	F(SLEEP) { }
	F(CLRWDT) { }
	F(NOP) { /* no operation */ }
	G(CLR) {
		std::string d = dest_string(o.args.value('d'), o.args.value('f'));
		c[d] = { "0" };
	}
	G(MOVWF) {
		std::string f = register_string(o.args.value('f'));
		c.touch("W");
		c[f] = c.at("W");
	}
	G(IORWF) { wf_function("IOR", o, c); }
	G(ANDWF) { wf_function("AND", o, c); }
	G(XORWF) { wf_function("XOR", o, c); }
	G(SUBWF) { wf_function("SUB", o, c); }
	G(ADDWF) { wf_function("ADD", o, c); }
	G(MOVF) {
		std::string d = dest_string(o.args.value('d'), o.args.value('f'));
		std::string f = register_string(o.args.value('f'));
		c.touch(f);
		c[d] = c.at(f);
	}
	F(COMF) {
	}
	F(DECF) {
	}
	F(INCF) {
	}
	F(RRF) {
	}
	F(RLF) {
	}
	F(SWAPF) {
	}
	F(BCF) {
	}
	F(BSF) {
	}
	F(DECFSZ) { throw std::runtime_error(std::string("DECFSZ performs conditional program counter modification")); }
	F(INCFSZ) { throw std::runtime_error(std::string("INCFSZ performs conditional program counter modification")); }
	F(BTFSC) { throw std::runtime_error(std::string("BTFSC performs conditional program counter modification")); }
	F(BTFSS) { throw std::runtime_error(std::string("BTFSS performs conditional program counter modification")); }
	F(CALL) { throw std::runtime_error(std::string("CALL overwrites program counter")); }
	F(GOTO) { throw std::runtime_error(std::string("GOTO overwrites program counter")); }
	F(MOVLW) { }
	F(RETLW) { }
	F(IORLW) { }
	F(ANDLW) { }
	F(XORLW) { }
	F(SUBLW) { }
	F(ADDLW) { }

	F(Z) {}
	F(C) {}
	F(DC) {}

	F(PD) {}
	F(TO) {}

	F(PC) {}

}
#undef F
#undef G

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
	return f ? register_string(x) : "W";
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
// struct expression
//

expression expression::expand(const value_type& s, const dictionary& d) const {

	expression e;

	expression sub = d.at(s);

	for(auto iter = begin(); iter != end(); iter++) {
		if(s == *iter)
			e.insert(e.end(), sub.begin(), sub.end());
		else
			e.push_back(*iter);
	}

	return e;
}

std::string expression::str() const {

	std::stringstream ss;

	for(auto iter = begin(); iter != end(); iter++) {
		ss << *iter;
		if(next(iter) != end())
			ss << ' ';
	}

	return ss.str();

}

void expression::parens() {
	push_front("(");
	push_back(")");
}

void expression::append(const expression& e) {
	insert(end(), e.begin(), e.end());
}

//
// struct dictionary
//

bool dictionary::has_key(const key_type& s) const {
	return (find(s) != end());
}

void dictionary::touch(const key_type& s) {
	if(!has_key(s))
		operator[](s) = { s };
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
