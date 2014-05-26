#include <algorithm>
#include <sstream>
#include <iomanip>
#include <iostream>

#include <cstdlib>
#include <cstdint>
#include <climits>

#include <mcpdis.hh>

const range range::star(0,UINT_MAX);
const range range::plus(1,UINT_MAX);
const range range::qm(0,1);
const range range::one(1,1);
const range range::zero(0,0);

///////
//
//
// rule
//
//
///////

template<typename T> rule<T>::rule(const rule& r) : op(r.op), type(r.type), predicates(r.predicates) {
}

template<typename T> rule<T>::rule() : rule(L'\0', rule_type::unordered) {
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

	if(op != L'\0')
		ss << '(' << op << ") ";

	ss << (type == rule_type::ordered ? L'(' : L'{');

	for(const auto& x : predicates)
		ss << L' ' << x.str();

	ss << L' ';
	ss << (type == rule_type::ordered ? L')' : L'}');

	return ss.str();
}

template struct rule<term>;

////////////
//
//
// predicate
//
//
////////////

template<typename T> predicate<T>::predicate() : predicate(predicate_type::any, range::one) {
}

template<typename T> predicate<T>::predicate(const predicate& r) : type(r.type), q(r.q) {
	switch(type) {
		case predicate_type::type:
			filter = r.filter;
			break;
		case predicate_type::value:
			value = r.value;
			break;
		case predicate_type::ref:
			ref = r.ref;
			break;
		case predicate_type::mem:
		case predicate_type::any:
		case predicate_type::end:
		default:
			break;
	}
}

template<typename T> predicate<T>::predicate(predicate_type my_type) : predicate(my_type, range::one) {
}

template<typename T> predicate<T>::predicate(predicate_type my_type, const range& my_q) : type(my_type), q(my_q) {
}

template<typename T> predicate<T>::predicate(const wchar_t* my_ref) : type(predicate_type::ref), ref(my_ref), q(range::one) {
}

template<typename T> predicate<T>::predicate(const symbol& my_ref) : type(predicate_type::ref), ref(my_ref), q(range::one) {
}

template<typename T> predicate<T>::predicate(const value_type& my_value) : type(predicate_type::value), value(my_value), q(range::one) {
}

template<typename T> predicate<T>::predicate(const filter_type& my_filter) : type(predicate_type::type), filter(my_filter), q(range::one) {
}

template<typename T> predicate<T>& predicate<T>::operator=(const predicate<T>& x) {
	if(this != &x) {
	}
	return *this;
}

template<typename T> std::wstring predicate<T>::str() const {

	std::wstringstream ss;

	switch(type) {
		case predicate_type::type:
			ss << L'[';
			for(auto t : filter)
				ss << (int)t;
			ss << L']';
			break;
		case predicate_type::value:
			ss << (int)value.type;
			ss << L':' << value.str();
			break;
		case predicate_type::ref:
			ss << ref;
			break;
		case predicate_type::mem:
			ss << L'#';
			break;
		case predicate_type::any:
			ss << L'.';
			break;
		case predicate_type::end:
			ss << L'$';
			break;
	}

	if(q == range::star) {
		ss << L'*';
	} else if(q == range::plus) {
		ss << L'+';
	} else if(q == range::qm) {
		ss << L'?';
	} else if(q == range::one) {
		// nothing
	} else if(q.first == q.second) {
		ss << L'{' << q.first << L'}';
	} else if(q.first == 0) {
		ss <<  L"{," << q.second << L'}';
	} else if(q.second == UINT_MAX) {
		ss <<  L'{' << q.first << L",}";
	}

	return ss.str();
}

template<typename T> predicate<T> predicate<T>::star() const {
	predicate p(*this);
	p.q = range::star;
	return p;
}

template<typename T> predicate<T> predicate<T>::plus() const {
	predicate p(*this);
	p.q = range::plus;
	return p;
}

template<typename T> predicate<T> predicate<T>::qm() const {
	predicate p(*this);
	p.q = range::qm;
	return p;
}

template<typename T> predicate<T> predicate<T>::ge(unsigned int n) const {
	predicate p(*this);
	p.q = range(n, UINT_MAX);
	return p;
}

template<typename T> predicate<T> predicate<T>::le(unsigned int n) const {
	predicate p(*this);
	p.q = range(0, n);
	return p;
}

template<typename T> predicate<T> predicate<T>::eq(unsigned int n) const {
	predicate p(*this);
	p.q = range(n, n);
	return p;
}

template struct predicate<term>;
