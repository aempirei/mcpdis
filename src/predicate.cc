#include <algorithm>
#include <sstream>
#include <iomanip>
#include <iostream>

#include <cstdlib>
#include <cstdint>
#include <climits>

#include <mcpdis.hh>

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

template<typename T> predicate<T>& predicate<T>::operator=(const predicate& x) {
	if(this != &x) {
	}
	return *this;
}

template<typename T> bool predicate<T>::operator==(const predicate& r) const {
	if(type != r.type)
		return false;
	return true;
	// FIXME: fix predicate comparison, although im not sure why
}

template<typename T> bool predicate<T>::operator<(const predicate& r) const {
	if(type != r.type)
		return type < r.type;
	return false;
	// FIXME: fix predicate comparison, although im not sure why
}

template<typename T> predicate<T>::operator std::wstring () const {

	std::wstringstream ss;

	switch(type) {
		case predicate_type::type:

				ss << ANSI_HIMAGENTA << L'[' << ANSI_LOMAGENTA;

			for(const auto& t : filter)
				ss << (wchar_t)t;

				ss << ANSI_HIMAGENTA << L']' << ANSI_CLR;

			break;
		case predicate_type::value:
			ss << ANSI_LOMAGENTA << (wchar_t)value.type << ANSI_CLR << L':'  << (std::wstring)value;
			break;
		case predicate_type::ref:
			ss << ANSI_LOGREEN << ref << ANSI_CLR;
			break;
		case predicate_type::mem:
			ss << ANSI_HIBLUE << L'#' << ANSI_CLR;
			break;
		case predicate_type::any:
			ss << ANSI_HIBLACK << L'.' << ANSI_CLR;
			break;
		case predicate_type::end:
			ss << ANSI_HIBLACK << L'$' << ANSI_CLR;
			break;
	}

	ss << ANSI_HIWHITE;

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

	ss << ANSI_CLR;

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
