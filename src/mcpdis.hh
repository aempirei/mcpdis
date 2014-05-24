#pragma once

#include <cstdint>
#include <cwchar>

#include <string>
#include <vector>
#include <list>
#include <map>
#include <initializer_list>

#include <operators.hh>
#include <ansicolor.hh>

struct instruction_set;
struct instruction;
struct operation;
struct arguments;
struct bitstream;
struct dictionary;
struct term;
struct function;

typedef std::list<operation> sourcecode;
typedef std::list<term> arglist;
typedef std::wstring variable;

typedef unsigned long literal_t;
typedef uint8_t reg_t;
typedef wchar_t op_t;

using accumulation_function = void (operation&, dictionary&);

//
// globals


namespace pic12f {

	extern instruction_set pic12f675;

	std::wstring address_string(literal_t);
	std::wstring register_string(reg_t);
	std::wstring register_name(reg_t);
	std::wstring dest_string(bool,reg_t);

	void power(dictionary&);
	void finalize(dictionary&);

	void PC(operation&, dictionary&);
}

//
// dictionary

using _dictionary = std::map<std::wstring,term>;

struct dictionary : _dictionary {
	using _dictionary::_dictionary;
	bool has_key(const key_type&) const;
	mapped_type& touch(const key_type&);
};

std::wstring str(const dictionary::value_type&);

//
// function

struct function {

	op_t op;
	arglist args;

	function();
	function(op_t);
	function(op_t, const arglist&);
	function(const function&);

	function& operator=(const function&);

	bool operator==(const function&) const;
	bool operator<(const function&) const;

	function& operator<<(const term&);

	size_t arity() const;

	void clear();

	void concat(const arglist&);
};

//
// term

struct term {

	enum class term_type {
		literal,
		variable,
		function
	};

	term_type type;

	literal_t l;
	variable v;
	function f;

	term();
	term(literal_t);
	term(const variable&);
	term(const function&);
	term(const term&);

	std::wstring wstr() const;

	bool is_function(op_t) const;
	bool is_literal(literal_t) const;
	bool is_variable(const variable&) const;

	bool is_function() const;
	bool is_variable() const;
	bool is_literal() const;

	bool is_nullary() const;
	bool is_unary() const;
	bool is_binary() const;

	bool is_arity(size_t) const;

	bool operator==(const term&) const;
	bool operator<(const term&) const;

	term& operator=(const term&);
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

