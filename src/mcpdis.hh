#pragma once

#include <cstdint>
#include <cwchar>

#include <string>
#include <vector>
#include <list>
#include <map>

#include <operators.hh>

// mcpdis.hh

struct instruction_set;
struct instruction;
struct operation;
struct arguments;
struct bitstream;
struct dictionary;
struct expression;

using accumulation_function = void (operation&, dictionary&);

using sourcecode = std::list<operation>;

using reg_t = uint8_t;

//
// dictionary

using _dictionary = std::map<std::wstring,expression>;

struct dictionary : _dictionary {
	using _dictionary::_dictionary;
	bool has_key(const key_type&) const;
	dictionary::mapped_type& touch(const key_type&);
};

std::wstring str(const dictionary::value_type&);

//
// expression

struct expression {

	std::wstring name;

	unsigned long value;

	wchar_t op;

	enum class expression_type { literal, variable, function };

	expression_type type;

	using args_type = std::list<expression>;

	args_type args;

	expression();

	expression(const expression&);

	expression(unsigned long);
	expression(const std::wstring&);
	expression(wchar_t, const args_type&);

	std::wstring wstr() const;

	bool is_function(wchar_t) const;
	bool is_variable(const std::wstring&) const;
	bool is_literal(unsigned long) const;

	bool is_function() const;
	bool is_variable() const;
	bool is_literal() const;

	bool is_nullary() const;
	bool is_unary() const;
	bool is_binary() const;

	bool is_arity(int) const;

	expression& operator=(const expression&);

	bool operator==(const expression&) const;


};

//
// arguments

using _arguments = std::map<wchar_t,std::wstring>;

struct arguments : _arguments {
	using _arguments::_arguments;
	unsigned long value(key_type) const;
	bool has_arg(key_type) const;
	bool has_args(const key_type *) const;
};

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

	enum file_register : reg_t {

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

	enum flags : reg_t {

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
		normal, skip, jump, call, ret
	};

	// variables

	std::wstring pattern;
	std::wstring name;

	accumulation_function *fn;

	pcl_types pcl_type;

	reg_t status;


	// methods

	bool match(const std::wstring&) const;
	template<class T> bool match(const std::wstring&, T) const;

	bool operator<(const instruction&) const;
};

//
// instruction_set

using _instruction_set = std::vector<instruction>;

struct instruction_set : _instruction_set {

	using _instruction_set::_instruction_set;

	value_type find(const std::wstring&) const;

	void sort();
};

//
// operation

struct operation {

	std::wstring s;

	unsigned long address;

	instruction opcode;

	arguments args;

	operation();
	operation(const std::wstring&, unsigned long, const instruction_set&);

	void execute(dictionary&);
};

extern instruction_set pic12f675;

std::wstring address_string(unsigned long);
std::wstring register_string(unsigned long);
std::wstring register_name(reg_t);
std::wstring dest_string(bool,unsigned long);
