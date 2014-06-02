#include <algorithm>
#include <sstream>
#include <iomanip>

#include <cstdlib>
#include <cstdint>
#include <climits>

#include <mcpdis.hh>

term::term() : term(fn<term>(OP_LIST)) {
}

term::term(const term& r) {
	*this = r;
}

term::term(literal_t my_l) : type(term_type::literal), l(my_l) {
}

term::term(const symbol& my_s) : type(term_type::symbol), s(my_s) {
}

term::term(const fn<term>& my_f) : type(term_type::function), f(my_f) {
}

term& term::operator=(const term& r) {

	if(this != &r) {

		type = r.type;

		switch(type) {

			case term_type::literal:

				l = r.l;
				s.clear();
				f.clear();
				break;

			case term_type::symbol:

				l = 0;
				s = r.s;
				f.clear();
				break;

			case term_type::function:

				l = 0;
				s.clear();
				f = r.f;
				break;
		}
	}

	return *this;
}

bool term::operator==(const term& r) const {

	if(type != r.type)
		return false;

	switch(type) {
		case term_type::literal : return l == r.l;
		case term_type::symbol  : return s == r.s;
		case term_type::function: return f == r.f;
	}

	return false;
}

bool term::operator<(const term& r) const {

	if(type != r.type)
		return type < r.type;

	switch(type) {
		case term_type::literal : return l < r.l;
		case term_type::symbol  : return s < r.s;
		case term_type::function: return f < r.f;
	}

	return false;
}

term::operator std::wstring () const {

	std::wstringstream ws;

	switch(type) {

		case term_type::symbol:

			ws << ANSI_LOGREEN << s << ANSI_CLR;

			break;

		case term_type::function:

			ws << ANSI_HIBLACK << L'(' << ANSI_HIYELLOW << f.op << ANSI_CLR;

			for(const auto& arg : f.args)
				ws << L' ' << (std::wstring)arg;

			ws << ANSI_HIBLACK << L')' << ANSI_CLR;


			break;

		case term_type::literal:

			ws << ANSI_LORED << std::showbase << (l < 10 ? std::dec : std::hex) << l << ANSI_CLR;

			break;
	}

	return ws.str();
}

bool term::is_function(op_t my_op) const {
	return type == term_type::function && f.op == my_op;
}

bool term::is_symbol(const symbol& my_s) const {
	return type == term_type::symbol && s == my_s;
}

bool term::is_literal(literal_t my_l) const {
	return type == term_type::literal && l == my_l;
}

bool term::is_function() const {
	return type == term_type::function;
}

bool term::is_symbol() const {
	return type == term_type::symbol;
}

bool term::is_literal() const {
	return type == term_type::literal;
}

bool term::is_nullary() const {
	return is_arity(0);
}

bool term::is_unary() const {
	return is_arity(1);
}

bool term::is_binary() const {
	return is_arity(2);
}

bool term::is_arity(size_t n) const {
	return is_function() && f.arity() == n;
}
