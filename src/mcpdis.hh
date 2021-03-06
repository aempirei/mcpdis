#pragma once

#include <vector>
#include <iostream>

#include <yyy.hh>

namespace dis {

	using namespace yyy;

	struct instruction_set;
	struct instruction;
	struct operation;
	struct bitstream;

	using dictionary = std::map<symbol::var,argument<term>>;
	using operands = std::map<operator_t,symbol::var>;
	using sourcecode = std::list<operation>;

	typedef void accumulation_function(operation&, dictionary&);

	namespace pic12f {

		extern instruction_set pic12f675;

		symbol::var address_string(literal_t);
		symbol::var register_string(literal_t);
		symbol::var register_name(literal_t);
		symbol::var dest_string(bool,literal_t);

		void power(dictionary&);
		void finalize(dictionary&);

		void PC(operation&, dictionary&);
	}

	template <typename A, typename B> B& touch(std::map<A,B>& s, const A& x) {
		if(not contains(s,x))
			s[x] = x;
		return s.at(x);
	}

	//
	// bitstream

	struct bitstream {

		static const int buffer_maxsize = 512;

		FILE *f;

		char buffer[buffer_maxsize];

		int buffer_size;
		int buffer_pos;

		std::wstring left;
		std::wstring get(int);

		bitstream();
		bitstream(FILE *);
	};

	//
	// instruction

	struct instruction {

		// types

		enum file_register : literal_t {

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

		enum flags : literal_t {

			none = 0,

			C    = 1 << 0,
			DC   = 1 << 1,
			Z    = 1 << 2,
			PD   = 1 << 3,
			TO   = 1 << 4,
			RP0  = 1 << 5,
			RP1  = 1 << 6,
			IRP  = 1 << 7,

			PEIE = 1 << 6,
			GIE  = 1 << 7,

			arithmetic = C | DC | Z,
			power = TO | PD

		};

		enum class pcl_types {
			normal,
			skip,
			jump,
			call,
			ret
		};

		symbol::var pattern;
		symbol::var name;

		accumulation_function *fn;

		pcl_types pcl_type;

		literal_t status;

		bool match(const symbol::var&) const;
		template<typename T> bool match(const symbol::var&, T) const;

		bool operator<(const instruction&) const;
	};

	//
	// instruction_set

	using _instruction_set = std::vector<instruction>;

	struct instruction_set : _instruction_set {

		using _instruction_set::_instruction_set;

		value_type find(const symbol::var&) const;

		void sort();
	};

	//
	// operation

	struct operation {

		symbol::var s;

		literal_t address;

		instruction opcode;

		operands args;

		operation();
		operation(const symbol::var&, literal_t, const instruction_set&);

		void execute(dictionary&);

		literal_t argul(const operands::key_type&) const;
	};
}

