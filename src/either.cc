#include <climits>
#include <cstdint>
#include <cstdlib>
#include <cstring>

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

#include <either.hh>

// maybe_aux <T,U> 
//
//

template <typename T,typename U> maybe_aux<T,U>::maybe_aux(const value_type&) {
}

template <typename T,typename U> U *maybe_aux<T,U>::ptr_to() const {
	return nullptr;
}

template <typename T,typename U> bool maybe_aux<T,U>::has() const {
	return false;
}

template <typename T,typename U> bool maybe_aux<T,U>::has_type() const {
	return false;
}

template <typename T,typename U> std::string maybe_aux<T,U>::str() const {
	return "T => U : [ ] ( x == null )";
}

// maybe_aux <either<A,B>,U> 
//
//

template <typename A,typename B,typename U> maybe_aux<either<A,B>,U>::maybe_aux(const value_type& a) : xptr(a.xptr) {
}

template <typename A,typename B,typename U> U *maybe_aux<either<A,B>,U>::ptr_to() const {
	return xptr ? xptr->template ptr_to<cod>() : nullptr;
}

template <typename A,typename B,typename U> bool maybe_aux<either<A,B>,U>::has() const {
	return (bool)ptr_to();
}

template <typename A,typename B,typename U> bool maybe_aux<either<A,B>,U>::has_type() const {
	return ( xptr ? *xptr : dom() ).template has_type<cod>();
}

template <typename A,typename B,typename U> std::string maybe_aux<either<A,B>,U>::str() const {

	std::stringstream ss;

	ss << "A or B => U : ";

	ss << (has_type() ? "[X]" : "[ ]");

	if(has())
		ss << " ( x := " << *ptr_to();
	else
		ss << " ( x == null";

	ss << " )";

	return ss.str();
}

// maybe_aux <either<A,B>,std::nullptr_t>
//
//

template <typename A,typename B> std::nullptr_t maybe_aux<either<A,B>,std::nullptr_t>::y = nullptr;

template <typename A,typename B> maybe_aux<either<A,B>,std::nullptr_t>::maybe_aux(const value_type& a) : xptr(a.xptr) {
}

template <typename A,typename B> std::nullptr_t *maybe_aux<either<A,B>,std::nullptr_t>::ptr_to() const {
	return &y;
}

template <typename A,typename B> bool maybe_aux<either<A,B>,std::nullptr_t>::has() const {
	return !(bool)xptr;
}

template <typename A,typename B> bool maybe_aux<either<A,B>,std::nullptr_t>::has_type() const {
	return true;
}

template <typename A,typename B> std::string maybe_aux<either<A,B>,std::nullptr_t>::str() const {

	return has()
		? "A or B => _ : [X] ( x == null )" 
		: "A or B => _ : [X] ( x != null )";
}

// maybe_aux <X,X>
//
//

template <typename X> maybe_aux<X,X>::maybe_aux(const value_type& a) : xptr(a.xptr) {
}

template <typename X> X *maybe_aux<X,X>::ptr_to() const {
	return xptr;
}

template <typename X> bool maybe_aux<X,X>::has() const {
	return (bool)xptr;
}

template <typename X> bool maybe_aux<X,X>::has_type() const {
	return true;
}

template <typename X> std::string maybe_aux<X,X>::str() const {

	std::stringstream ss;

	ss << "X => X : [X] ( ";

	if(has())
		ss << "x := " << *ptr_to();
	else
		ss << "x == null";

	ss << " )";

	return ss.str();
}

// maybe_aux <X,std::nullptr_t>
//
//

template <typename X> std::nullptr_t maybe_aux<X,std::nullptr_t>::y = nullptr;

template <typename X> maybe_aux<X,std::nullptr_t>::maybe_aux(const value_type& a) : xptr(a.xptr) {
}

template <typename X> std::nullptr_t *maybe_aux<X,std::nullptr_t>::ptr_to() const {
	return &y;
}

template <typename X> bool maybe_aux<X,std::nullptr_t>::has() const {
	return !(bool)xptr;
}

template <typename X> bool maybe_aux<X,std::nullptr_t>::has_type() const {
	return true;
}

template <typename X> std::string maybe_aux<X,std::nullptr_t>::str() const {

	return has()
		? "X => _ : [X] ( x == null )" 
		: "X => _ : [X] ( x != null )";
}

// maybe
//
//

template <typename X> maybe<X>::maybe() : maybe(nullptr) {
}

template <typename X> maybe<X>::maybe(std::nullptr_t) : xptr(nullptr) {
}

template <typename X> maybe<X>::maybe(const X& x0) : xptr(new X(x0)) {
}

template <typename X> maybe<X>::maybe(const maybe& r) : xptr(r.xptr ? new X(*r.xptr) : nullptr) {
}

template <typename X> void maybe<X>::clear() {
	if(xptr != nullptr) {
		delete xptr;
		xptr = nullptr;
	}
}	

template <typename X> maybe<X>::~maybe() {
	clear();
}

template <typename X> maybe<X>& maybe<X>::operator=(const maybe&r) {

	clear();

	if(r.xptr)
		xptr = new X(*r.xptr);

	return *this;
}

// <Y> maybe<X>
//
//

template <typename X> template <typename Y> Y *maybe<X>::ptr_to() const {
	return aux<Y>(*this).ptr_to();
}

template <typename X> template <typename Y> bool maybe<X>::has() const {
	return aux<Y>(*this).has();
}

template <typename X> template <typename Y> bool maybe<X>::has_type() const {
	return aux<Y>(*this).has_type();
}

template <typename X> template <typename Y> std::string maybe<X>::str() const {
	return aux<Y>(*this).str();
}

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
	if(this != &r) {
		a = r.a;
		b = r.b;
	}
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

// <D> either<A,B>
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
