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

	enum algebraic_property : uint8_t {
		commutative = 1,
		associative = 2
	};

	uint8_t algebra;

	enum status_bit : uint8_t {

		C   =   1,
		DC  =   2,
		Z   =   4,
		PD  =   8,
		TO  =  16,
		RP0 =  32,
		RP1 =  64,
		IRP = 128,

		arithmetic_bits = C | DC | Z,
		power_bits = TO | PD

	};

	uint8_t status;

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
