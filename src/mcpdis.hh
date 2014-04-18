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

	uint8_t status;

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

extern std::vector<instruction> pic12f675;
