#include <mcpdis.hh>
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <cstdlib>

instruction_set pic12f675 = {

	{ "00000000001000", "RETURN", ""  , instruction::pcl_types::normal, instruction::flags::none },
	{ "00000000001001", "RETFIE", ""  , instruction::pcl_types::normal, instruction::flags::none },

	{ "00000001100011", "SLEEP" , ""  , instruction::pcl_types::normal, instruction::flags::power },
	{ "00000001100100", "CLRWDT", ""  , instruction::pcl_types::normal, instruction::flags::power },

	{ "0000000xx00000", "NOP"   , ""  , instruction::pcl_types::normal, instruction::flags::none },

	{ "000001dfffffff", "CLR"   , ""  , instruction::pcl_types::normal, instruction::flags::Z },

	{ "0000001fffffff", "MOVWF" , "=" , instruction::pcl_types::normal, instruction::flags::none },

	{ "000100dfffffff", "IORWF" , "|" , instruction::pcl_types::normal, instruction::flags::Z },
	{ "000101dfffffff", "ANDWF" , "&" , instruction::pcl_types::normal, instruction::flags::Z },
	{ "000110dfffffff", "XORWF" , "^" , instruction::pcl_types::normal, instruction::flags::Z },

	{ "000010dfffffff", "SUBWF" , "-" , instruction::pcl_types::normal, instruction::flags::arithmetic },
	{ "000111dfffffff", "ADDWF" , "+" , instruction::pcl_types::normal, instruction::flags::arithmetic },

	{ "001000dfffffff", "MOVF"  , "=" , instruction::pcl_types::normal, instruction::flags::Z },

	{ "001001dfffffff", "COMF"  , "~" , instruction::pcl_types::normal, instruction::flags::Z },

	{ "000011dfffffff", "DECF"  , "--", instruction::pcl_types::normal, instruction::flags::Z },
	{ "001010dfffffff", "INCF"  , "++", instruction::pcl_types::normal, instruction::flags::Z },

	{ "001100dfffffff", "RRF"   , "#>", instruction::pcl_types::normal, instruction::flags::C },
	{ "001101dfffffff", "RLF"   , "<#", instruction::pcl_types::normal, instruction::flags::C },

	{ "001110dfffffff", "SWAPF" , "><", instruction::pcl_types::normal, instruction::flags::none },

	{ "0100bbbfffffff", "BCF"   , ""  , instruction::pcl_types::normal, instruction::flags::none },
	{ "0101bbbfffffff", "BSF"   , ""  , instruction::pcl_types::normal, instruction::flags::none },

	{ "001011dfffffff", "DECFSZ", ""  , instruction::pcl_types::skip, instruction::flags::none },
	{ "001111dfffffff", "INCFSZ", ""  , instruction::pcl_types::skip, instruction::flags::none },

	{ "0110bbbfffffff", "BTFSC" , ""  , instruction::pcl_types::skip, instruction::flags::none },
	{ "0111bbbfffffff", "BTFSS" , ""  , instruction::pcl_types::skip, instruction::flags::none },

	{ "100kkkkkkkkkkk", "CALL"  , ""  , instruction::pcl_types::call, instruction::flags::none },
	{ "101kkkkkkkkkkk", "GOTO"  , ""  , instruction::pcl_types::jump, instruction::flags::none },

	{ "1100xxkkkkkkkk", "MOVLW" , "=" , instruction::pcl_types::normal, instruction::flags::none },
	{ "1101xxkkkkkkkk", "RETLW" , ""  , instruction::pcl_types::normal, instruction::flags::none },

	{ "111000kkkkkkkk", "IORLW" , "|=", instruction::pcl_types::normal, instruction::flags::Z },
	{ "111001kkkkkkkk", "ANDLW" , "&=", instruction::pcl_types::normal, instruction::flags::Z },
	{ "111010kkkkkkkk", "XORLW" , "^=", instruction::pcl_types::normal, instruction::flags::Z },

	{ "11110xkkkkkkkk", "SUBLW" , "-" , instruction::pcl_types::normal, instruction::flags::arithmetic },
	{ "11111xkkkkkkkk", "ADDLW" , "+" , instruction::pcl_types::normal, instruction::flags::arithmetic }
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

