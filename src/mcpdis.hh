#pragma once

#include <cstdint>

#include <string>
#include <vector>
#include <map>

// mcpdis.hh

using parameter_map = std::map<char,std::string>;

struct instruction {

	std::string pattern;
	std::string name;

	std::string symbol;

	enum property : uint8_t {
		commutative = 1 << 0,
		associative = 1 << 1,
		skip        = 1 << 2,
		jump        = 1 << 3
	};

	uint8_t property_bits;

	enum status : uint8_t {

		C   = 1 << 0,
		DC  = 1 << 1,
		Z   = 1 << 2,
		PD  = 1 << 3,
		TO  = 1 << 4,
		RP0 = 1 << 5,
		RP1 = 1 << 6,
		IRP = 1 << 7,

		arithmetic_bits = C | DC | Z,
		power_bits = TO | PD

	};

	uint8_t status_bits;

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

	bool match(const std::string) const;
	template<class T> bool match(const std::string, T) const;

	bool operator<(const instruction&) const;
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

using _instruction_set = std::vector<instruction>;

struct instruction_set : _instruction_set {

	using _instruction_set::_instruction_set;

	instruction find(const std::string) const;

	void sort();
};

extern instruction_set pic12f675;
