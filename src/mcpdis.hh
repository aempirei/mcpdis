#pragma once

#include <cstdint>

#include <string>
#include <vector>
#include <map>

// mcpdis.hh

struct instruction;
struct operation;
struct bitstream;

using _arguments = std::map<char,std::string>;

struct arguments : _arguments {
	using _arguments::_arguments;
	unsigned long value(char);
	bool has_arg(char);
	bool has_args(const char *);
};

struct bitstream {

	static const int buffer_maxsize = 512;

	FILE *f;

	char buffer[buffer_maxsize];

	int buffer_size;
	int buffer_pos;

	std::string left;

	std::string get(int);

	bitstream();
	bitstream(FILE *);
};

struct instruction {

	enum file_register : uint8_t {

		INDF   = 0x00,
		TMR0   = 0x01,
		PCL    = 0x02,
		STATUS = 0x03,
		FSR    = 0x04,
		GPIO   = 0x05,
		PCLATH = 0x0a,
		INTCON = 0x0b,
		PIR1   = 0x0c,
		TMR1L  = 0x0e,
		TMR1H  = 0x0f,
		T1CON  = 0x10,
		CMCON  = 0x19,
		ADRESH = 0x1e,
		ADCON0 = 0x1f,

	};

	enum flags : uint8_t {

		C   = 1 << 0,
		DC  = 1 << 1,
		Z   = 1 << 2,
		PD  = 1 << 3,
		TO  = 1 << 4,
		RP0 = 1 << 5,
		RP1 = 1 << 6,
		IRP = 1 << 7,

		arithmetic_flags = C | DC | Z,
		power_flags = TO | PD

	};

	enum class pcl_types {
		normal, skip, jump, call
	};

	std::string pattern;
	std::string name;

	std::string symbol;

	pcl_types pcl_type;

	uint8_t status;

	bool match(const std::string) const;
	template<class T> bool match(const std::string, T) const;

	bool operator<(const instruction&) const;
};

using _instruction_set = std::vector<instruction>;

struct instruction_set : _instruction_set {

	using _instruction_set::_instruction_set;

	instruction find(const std::string) const;

	void sort();
};

struct operation {

	std::string s;

	unsigned long address;

	instruction opcode;

	arguments args;

	operation();
	operation(std::string, unsigned long, const instruction_set&);
};

extern instruction_set pic12f675;

std::string address_string(unsigned long);
std::string register_string(unsigned long);
std::string register_name(uint8_t);
