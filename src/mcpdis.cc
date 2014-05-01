#include <mcpdis.hh>
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <cstdlib>

instruction_set pic12f675 = {

	{ "00000000001000", "RETURN", ""  , 0, 0 },
	{ "00000000001001", "RETFIE", ""  , 0, 0 },

	{ "00000001100011", "SLEEP" , ""  , 0, instruction::status::power_bits },
	{ "00000001100100", "CLRWDT", ""  , 0, instruction::status::power_bits },

	{ "0000000xx00000", "NOP"   , ""  , 0, 0 },

	{ "000001dfffffff", "CLR"   , ""  , 0, instruction::status::Z },

	{ "0000001fffffff", "MOVWF" , "=" , 0, 0 },

	{ "000100dfffffff", "IORWF" , "|" , 0, instruction::status::Z },
	{ "000101dfffffff", "ANDWF" , "&" , 0, instruction::status::Z },
	{ "000110dfffffff", "XORWF" , "^" , 0, instruction::status::Z },

	{ "000010dfffffff", "SUBWF" , "-" , 0, instruction::status::arithmetic_bits },
	{ "000111dfffffff", "ADDWF" , "+" , 0, instruction::status::arithmetic_bits },

	{ "001000dfffffff", "MOVF"  , "=" , 0, instruction::status::Z },

	{ "001001dfffffff", "COMF"  , "~" , 0, instruction::status::Z },

	{ "000011dfffffff", "DECF"  , "--", 0, instruction::status::Z },
	{ "001010dfffffff", "INCF"  , "++", 0, instruction::status::Z },

	{ "001100dfffffff", "RRF"   , "#>", 0, instruction::status::C },
	{ "001101dfffffff", "RLF"   , "<#", 0, instruction::status::C },

	{ "001110dfffffff", "SWAPF" , "><", 0, 0 },

	{ "0100bbbfffffff", "BCF"   , ""  , 0, 0 },
	{ "0101bbbfffffff", "BSF"   , ""  , 0, 0 },

	{ "001011dfffffff", "DECFSZ", ""  , instruction::property::skip, 0 },
	{ "001111dfffffff", "INCFSZ", ""  , instruction::property::skip, 0 },

	{ "0110bbbfffffff", "BTFSC" , ""  , instruction::property::skip, 0 },
	{ "0111bbbfffffff", "BTFSS" , ""  , instruction::property::skip, 0 },

	{ "100kkkkkkkkkkk", "CALL"  , ""  , instruction::property::call, 0 },
	{ "101kkkkkkkkkkk", "GOTO"  , ""  , instruction::property::jump, 0 },

	{ "1100xxkkkkkkkk", "MOVLW" , "=" , 0, 0 },
	{ "1101xxkkkkkkkk", "RETLW" , ""  , 0, 0 },

	{ "111000kkkkkkkk", "IORLW" , "|=", 0, instruction::status::Z },
	{ "111001kkkkkkkk", "ANDLW" , "&=", 0, instruction::status::Z },
	{ "111010kkkkkkkk", "XORLW" , "^=", 0, instruction::status::Z },

	{ "11110xkkkkkkkk", "SUBLW" , "-" , 0, instruction::status::arithmetic_bits },
	{ "11111xkkkkkkkk", "ADDLW" , "+" , 0, instruction::status::arithmetic_bits }
};

//
// struct instruction
//

template<class T> bool instruction::match(const std::string s, T f) const {
	
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

template<> bool instruction::match(const std::string s, arguments *p) const {

	p->clear();

	return match(s, [&](int n) { p->operator[](pattern[n]).push_back(s[n]); });
}

bool instruction::match(const std::string s) const {

	return match(s, [](int){});
}

bool instruction::operator<(const instruction& x) const {
	return pattern < x.pattern;
}

//
// struct instruction_set
//

instruction instruction_set::find(const std::string s) const {

	for(const auto& op : *this)
		if(op.match(s))
			return op;

	return instruction();
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

unsigned long arguments::value(char ch) {
	return strtoul(at(ch).c_str(), NULL, 2);
}

bool arguments::has_arg(char ch) {
	return (find(ch) != end());
}

bool arguments::has_args(const char *s) {

	while(*s)
		if(!has_arg(*s++))
			return false;

	return true;
}

//
// struct operation
//

operation::operation() {
}

operation::operation(std::string my_s, unsigned long my_address, const instruction_set& cpu) : s(my_s), address(my_address) {
	opcode = cpu.find(s);
	opcode.match(s, &args);
}

