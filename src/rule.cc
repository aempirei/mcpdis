#include <algorithm>
#include <sstream>
#include <iomanip>
#include <iostream>

#include <cstdlib>
#include <cstdint>
#include <climits>

#include <mcpdis.hh>

template<typename T> rule<T>::rule(const rule& r) : op(r.op), type(r.type), predicates(r.predicates) {
}

template<typename T> rule<T>::rule() : rule(OP_LIST, rule_type::unordered) {
}

template<typename T> rule<T>::rule(op_t my_op) : rule(my_op, rule_type::unordered) {
}

template<typename T> rule<T>::rule(op_t my_op, rule_type my_type) : op(my_op), type(my_type) {
}

template<typename T> rule<T>& rule<T>::operator<<(const predicate<T>& x) {
	predicates.push_back(x);
	return *this;
}

template<typename T> rule<T>& rule<T>::operator=(const rule<T>& x) {
	if(this != &x) {
	}
	return *this;
}

template<typename T> std::wstring rule<T>::str() const {

	std::wstringstream ss;

	if(op != OP_LIST)
		ss << '(' << ANSI_HIYELLOW << op << ANSI_CLR << ") ";

	ss << (type == rule_type::ordered ? L'(' : L'{');

	for(const auto& x : predicates)
		ss << L' ' << x.str();

	ss << L' ';
	ss << (type == rule_type::ordered ? L')' : L'}');

	return ss.str();
}

template struct rule<term>;
