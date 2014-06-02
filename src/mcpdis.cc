#include <algorithm>
#include <sstream>
#include <iomanip>
#include <iostream>

#include <cstdlib>
#include <cstdint>
#include <climits>

#include <mcpdis.hh>

/////////////////////
//
//
// struct instruction
//
//
/////////////////////

template<class T> bool instruction::match(const symbol& s, T f) const {
	
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

template<> bool instruction::match(const symbol& s, arguments *p) const {

	p->clear();

	return match(s, [&](int n) { (*p)[pattern[n]].push_back(s[n]); });
}

bool instruction::match(const symbol& s) const {

	return match(s, [](int){});
}

bool instruction::operator<(const instruction& x) const {
	return pattern < x.pattern;
}

/////////////////////////
//
//
// struct instruction_set
//
//
/////////////////////////

instruction_set::value_type instruction_set::find(const symbol& s) const {

	for(const auto& op : *this)
		if(op.match(s))
			return op;

	return instruction_set::value_type();
}

void instruction_set::sort() {
	std::sort(begin(), end());
}

///////////////////
//
//
// struct bitstream
//
//
///////////////////

bitstream::bitstream() : bitstream(stdin) {
}

bitstream::bitstream(FILE *my_f) : f(my_f), buffer_size(0), buffer_pos(0) {
}

std::wstring bitstream::get(int n) {
	
	std::wstring s;

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

///////////////////
//
//
// struct arguments
//
//
///////////////////

literal_t arguments::value(key_type ch) const {

	unsigned long x = 0;

	const mapped_type& s = at(ch);

	for(size_t n = 0; n < s.length(); n++) {

		x <<= 1;

		if(s[n] == L'1')
			x++;
	}

	return x;
}

bool arguments::has_arg(key_type ch) const {
	return find(ch) != end();
}

bool arguments::has_args(const key_type *s) const {

	while(*s)
		if(!has_arg(*s++))
			return false;

	return true;
}

////////////////////
//
//
// struct dictionary
//
//
////////////////////

bool dictionary::has_key(const dictionary::key_type& s) const {
	return find(s) != end();
}

dictionary::mapped_type& dictionary::touch(const dictionary::key_type& s) {
	if(!has_key(s))
		operator[](s) = mapped_type(s);
	return at(s);
}

std::wstring str(const dictionary::value_type& x) {
	std::wstringstream ws;
	ws << std::setw(6) << std::left << std::setfill(L' ') << x.first << L" := " << (std::wstring)x.second;
	return ws.str();
}

///////////////////
//
//
// struct operation
//
//
///////////////////

operation::operation() {
}

operation::operation(const symbol& my_s, literal_t my_address, const instruction_set& cpu) : s(my_s), address(my_address) {
	opcode = cpu.find(s);
	opcode.match(s, &args);
}

void operation::execute(dictionary& d) {

	if(address == 0x000U)
		pic12f::power(d);

	opcode.fn(*this, d);

	if(opcode.pcl_type == instruction::pcl_types::normal)
		pic12f::PC(*this, d);

	pic12f::finalize(d);
}
