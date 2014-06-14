#include <cstdlib>

#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

#include <either.hh>

#include <maybe.template.hh>

// either
//
//

template <typename A,typename B> either<A,B>::either() : either(nullptr) {
}

template <typename A,typename B> either<A,B>::either(std::nullptr_t) : a(nullptr), b(nullptr) {
}

template <typename A,typename B> either<A,B>::either(const either& r) : either(r.a, r.b) {
}

template <typename A,typename B> either<A,B>::either(const maybe<A>& my_a, const maybe<B>& my_b) : a(my_a), b(my_b) {
}

template <typename A,typename B> either<A,B>::either(const maybe<A>& my_a) : a(my_a), b(nullptr) {
}

template <typename A,typename B> either<A,B>::either(const maybe<B>& my_b) : a(nullptr), b(my_b) {
}

template <typename A,typename B> void either<A,B>::clear() {
	a.clear();
	b.clear();
}

template <typename A,typename B> either<A,B>& either<A,B>::operator=(const either& r) {
	a = r.a;
	b = r.b;
	return *this;
}

template <typename A,typename B> either<A,B>& either<A,B>::operator=(const maybe<A>& my_a) {
	a = my_a;
	b.clear();
	return *this;
}

template <typename A,typename B> either<A,B>& either<A,B>::operator=(const maybe<B>& my_b) {
	a.clear();
	b = my_b;
	return *this;
}

template <typename A,typename B> either<A,B>& either<A,B>::operator=(std::nullptr_t) {
	clear();
	return *this;
}

template <typename A,typename B> either<A,B>& either<A,B>::include(const maybe<A>& my_a) {
	a = my_a;
	return *this;
}

template <typename A,typename B> either<A,B>& either<A,B>::include(const maybe<B>& my_b) {
	b = my_b;
	return *this;
}


template <typename A,typename B> either<A,B>::operator std::wstring () const {
	if(a.xptr && b.xptr) {
		return L'(' + (std::wstring)a + L' ' + (std::wstring)b + L')';
	} else {
		return a.xptr ? (std::wstring)a : b.xptr ? (std::wstring)b : L"NULL";
	}
}

// <D> either
//
//

template <typename A,typename B> template <typename D> maybe<D> either<A,B>::find() const {
	return has<D>() ? *ptr_to<D>() : maybe<D>();
}

template <typename A,typename B> template <typename D> D *either<A,B>::ptr_to() const {

	if(a.has<D>())
		return a.ptr_to<D>();

	if(b.has<D>())
		return b.ptr_to<D>();

	return nullptr;
}

template <typename A,typename B> template <typename D> bool either<A,B>::has() const {
	return a.has<D>() | b.has<D>();
}

template <typename A,typename B> template <typename D> bool either<A,B>::has_type() const {
	return a.has_type<D>() | b.has_type<D>();
}

template <typename A,typename B> template <typename D> std::string either<A,B>::str() const {

	std::stringstream ss;

	ss << "( a := " << a.str<D>() << " ) ";
	ss << "( b := " << b.str<D>() << " )";

	return ss.str();
}

// either<either<A,B>,C>
//
//

template <typename A,typename B,typename C> either<either<A,B>,C>::either() : either(nullptr) {
}

template <typename A,typename B,typename C> either<either<A,B>,C>::either(std::nullptr_t) : a(nullptr), b(nullptr) {
}

template <typename A,typename B,typename C> either<either<A,B>,C>::either(const either& r) : a(r.a), b(r.b) {
}

template<typename A,typename B,typename C>
either<either<A,B>,C>::either(const maybe<first_type>& my_a, const maybe<second_type>& my_b) : a(my_a), b(my_b) {
}

template <typename A,typename B,typename C> either<either<A,B>,C>::either(const maybe<A>& my_a) : a(my_a), b(nullptr) {
}

template <typename A,typename B,typename C> either<either<A,B>,C>::either(const maybe<B>& my_a) : a(my_a), b(nullptr) {
}

template <typename A,typename B,typename C> either<either<A,B>,C>::either(const maybe<first_type>& my_a) : a(my_a), b(nullptr) {
}

template <typename A,typename B,typename C> either<either<A,B>,C>::either(const maybe<second_type>& my_b) : a(nullptr), b(my_b) {
}

template <typename A,typename B,typename C> void either<either<A,B>,C>::clear() {
	a.clear();
	b.clear();
}

template <typename A,typename B,typename C> either<either<A,B>,C>& either<either<A,B>,C>::operator=(const either& r) {
	a = r.a;
	b = r.b;
	return *this;
}

template <typename A,typename B,typename C> either<either<A,B>,C>& either<either<A,B>,C>::operator=(const maybe<A>& my_a) {
	a = either<A,B>(my_a);
	b.clear();
	return *this;
}

template <typename A,typename B,typename C> either<either<A,B>,C>& either<either<A,B>,C>::operator=(const maybe<B>& my_a) {
	a = either<A,B>(my_a);
	b.clear();
	return *this;
}

template <typename A,typename B,typename C> either<either<A,B>,C>& either<either<A,B>,C>::operator=(const maybe<first_type>& my_a) {
	a = my_a;
	b.clear();
	return *this;
}

template <typename A,typename B,typename C> either<either<A,B>,C>& either<either<A,B>,C>::operator=(const maybe<second_type>& my_b) {
	a.clear();
	b = my_b;
	return *this;
}

template <typename A,typename B,typename C> either<either<A,B>,C>& either<either<A,B>,C>::operator=(std::nullptr_t) {
	clear();
	return *this;
}

template <typename A,typename B,typename C> either<either<A,B>,C>& either<either<A,B>,C>::include(const maybe<A>& my_a) {
	if(a.xptr)
		a.xptr->include(my_a);
	else
		a = either<A,B>(my_a);
	return *this;
}

template <typename A,typename B,typename C> either<either<A,B>,C>& either<either<A,B>,C>::include(const maybe<B>& my_a) {
	if(a.xptr)
		a.xptr->include(my_a);
	else
		a = either<A,B>(my_a);
	return *this;
}

template <typename A,typename B,typename C> either<either<A,B>,C>& either<either<A,B>,C>::include(const maybe<first_type>& my_a) {
	a = my_a;
	return *this;
}

template <typename A,typename B,typename C> either<either<A,B>,C>& either<either<A,B>,C>::include(const maybe<second_type>& my_b) {
	b = my_b;
	return *this;
}

template <typename A,typename B,typename C> template <typename D> maybe<D> either<either<A,B>,C>::find() const {
	return has<D>() ? *ptr_to<D>() : maybe<D>();
}

template <typename A,typename B,typename C> template <typename D> D *either<either<A,B>,C>::ptr_to() const {
	if(a.has<D>())
		return a.ptr_to<D>();
	if(b.has<D>())
		return b.ptr_to<D>();
	return nullptr;
}

template <typename A,typename B,typename C> either<either<A,B>,C>::operator std::wstring () const {
	if(a.xptr && b.xptr) {
		return L'(' + (std::wstring)a + L' ' + (std::wstring)b + L')';
	} else {
		return a.xptr ? (std::wstring)a : b.xptr ? (std::wstring)b : L"NULL";
	}

}

// <D> either<either<A,B>,C>
//
//

template <typename A,typename B,typename C> template <typename D> bool either<either<A,B>,C>::has() const {
	return a.has<D>() | b.has<D>();
}

template <typename A,typename B,typename C> template <typename D> bool either<either<A,B>,C>::has_type() const {
	return a.has_type<D>() | b.has_type<D>();
}

template <typename A,typename B,typename C> template <typename D> std::string either<either<A,B>,C>::str() const {
	std::stringstream ss;
	ss << "( a := " << a.str<D>() << " ) ";
	ss << "( b := " << b.str<D>() << " )";
	return ss.str();
}

