#pragma once

#include <cstdint>
#include <string>

// mcpdis.hh

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
};
