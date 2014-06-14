#include <cstdlib>

#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

#include <maybe.hh>

#include <either.template.hh>

// maybe_aux
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

// maybe_aux <either<A,B>,C> 
//
//

template <typename A,typename B,typename C> maybe_aux<either<A,B>,C>::maybe_aux(const value_type& a) : xptr(a.xptr) {
}

template <typename A,typename B,typename C> C *maybe_aux<either<A,B>,C>::ptr_to() const {
	return xptr ? xptr->template ptr_to<cod>() : nullptr;
}

template <typename A,typename B,typename C> bool maybe_aux<either<A,B>,C>::has() const {
	return (bool)ptr_to();
}

template <typename A,typename B,typename C> bool maybe_aux<either<A,B>,C>::has_type() const {
	return ( xptr ? *xptr : dom() ).template has_type<cod>();
}

template <typename A,typename B,typename C> std::string maybe_aux<either<A,B>,C>::str() const {

	std::stringstream ss;

	ss << "A or B => C : ";

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

// maybe_aux <T,T>
//
//

template <typename T> maybe_aux<T,T>::maybe_aux(const value_type& a) : xptr(a.xptr) {
}

template <typename T> T *maybe_aux<T,T>::ptr_to() const {
	return xptr;
}

template <typename T> bool maybe_aux<T,T>::has() const {
	return (bool)xptr;
}

template <typename T> bool maybe_aux<T,T>::has_type() const {
	return true;
}

template <typename T> std::string maybe_aux<T,T>::str() const {

	std::stringstream ss;

	ss << "T => T : [X] ( ";

	if(has())
		ss << "x := " << *ptr_to();
	else
		ss << "x == null";

	ss << " )";

	return ss.str();
}

// maybe_aux <T,std::nullptr_t>
//
//

template <typename T> std::nullptr_t maybe_aux<T,std::nullptr_t>::y = nullptr;

template <typename T> maybe_aux<T,std::nullptr_t>::maybe_aux(const value_type& a) : xptr(a.xptr) {
}

template <typename T> std::nullptr_t *maybe_aux<T,std::nullptr_t>::ptr_to() const {
	return &y;
}

template <typename T> bool maybe_aux<T,std::nullptr_t>::has() const {
	return !(bool)xptr;
}

template <typename T> bool maybe_aux<T,std::nullptr_t>::has_type() const {
	return true;
}

template <typename T> std::string maybe_aux<T,std::nullptr_t>::str() const {

	return has()
		? "T => _ : [X] ( x == null )" 
		: "T => _ : [X] ( x != null )";
}

// maybe
//
//

template <typename T> maybe<T>::maybe() : maybe(nullptr) {
}

template <typename T> maybe<T>::maybe(std::nullptr_t) : xptr(nullptr) {
}

template <typename T> maybe<T>::maybe(const value_type& x0) : xptr(new value_type(x0)) {
}

template <typename T> maybe<T>::maybe(const maybe& r) : xptr(r.xptr ? new value_type(*r.xptr) : nullptr) {
}

template <typename T> void maybe<T>::clear() {
	if(xptr) {
		delete xptr;
		xptr = nullptr;
	}
}	

template <typename T> maybe<T>::~maybe() {
	clear();
}

template <typename T> maybe<T>& maybe<T>::operator=(const maybe&r) {

	clear();

	if(r.xptr)
		xptr = new value_type(*r.xptr);

	return *this;
}

template <typename T> maybe<T>::operator std::wstring () const {
	return xptr ? std::to_wstring(*xptr) : std::wstring();
}

// <Y> maybe
//
//

template <typename T> template <typename Y> Y *maybe<T>::ptr_to() const {
	return aux<Y>(*this).ptr_to();
}

template <typename T> template <typename Y> bool maybe<T>::has() const {
	return aux<Y>(*this).has();
}

template <typename T> template <typename Y> bool maybe<T>::has_type() const {
	return aux<Y>(*this).has_type();
}

template <typename T> template <typename Y> std::string maybe<T>::str() const {
	return aux<Y>(*this).str();
}

// maybe<V<T>>
//
//

template <template <typename> class V, typename T> maybe<V<T>>::maybe() : maybe(nullptr) {
}

template <template <typename> class V, typename T> maybe<V<T>>::maybe(std::nullptr_t) : xptr(nullptr) {
}

template <template <typename> class V, typename T> maybe<V<T>>::maybe(const value_type& x0) : xptr(new value_type(x0)) {
}

template <template <typename> class V, typename T> maybe<V<T>>::maybe(const maybe& r) : xptr(r.xptr ? new value_type(*r.xptr) : nullptr) {
}

template <template <typename> class V, typename T> void maybe<V<T>>::clear() {
	if(xptr) {
		delete xptr;
		xptr = nullptr;
	}
}	

template <template <typename> class V, typename T> maybe<V<T>>::~maybe() {
	clear();
}

template <template <typename> class V, typename T> maybe<V<T>>& maybe<V<T>>::operator=(const maybe&r) {

	clear();

	if(r.xptr)
		xptr = new value_type(*r.xptr);

	return *this;
}

template <template <typename> class V, typename T> maybe<V<T>>::operator std::wstring () const {
	return xptr ? (std::wstring)*xptr : std::wstring();
}

// <Y> maybe<V<T>>
//
//

template <template <typename> class V, typename T> template <typename Y> Y* maybe<V<T>>::ptr_to() const {
	return aux<Y>(*this).ptr_to();
}

template <template <typename> class V, typename T> template <typename Y> bool maybe<V<T>>::has() const {
	return aux<Y>(*this).has();
}

template <template <typename> class V, typename T> template <typename Y> bool maybe<V<T>>::has_type() const {
	return aux<Y>(*this).has_type();
}

template <template <typename> class V, typename T> template <typename Y> std::string maybe<V<T>>::str() const {
	return aux<Y>(*this).str();
}

// maybe<std::basic_string<T>>
//
//

template <typename T> maybe<std::basic_string<T>>::maybe() : maybe(nullptr) {
}

template <typename T> maybe<std::basic_string<T>>::maybe(std::nullptr_t) : xptr(nullptr) {
}

template <typename T> maybe<std::basic_string<T>>::maybe(const value_type& x0) : xptr(new value_type(x0)) {
}

template <typename T> maybe<std::basic_string<T>>::maybe(const char_type *str) : xptr(new value_type(str)) {
}

template <typename T> maybe<std::basic_string<T>>::maybe(const maybe& r) : xptr(r.xptr ? new value_type(*r.xptr) : nullptr) {
}

template <typename T> void maybe<std::basic_string<T>>::clear() {
	if(xptr) {
		delete xptr;
		xptr = nullptr;
	}
}	

template <typename T> maybe<std::basic_string<T>>::~maybe() {
	clear();
}

template <typename T> maybe<std::basic_string<T>>& maybe<std::basic_string<T>>::operator=(const maybe&r) {

	clear();

	if(r.xptr)
		xptr = new value_type(*r.xptr);

	return *this;
}

template <typename T> maybe<std::basic_string<T>>::operator std::wstring () const {
	return xptr ? ( L'"' + std::wstring(xptr->begin(), xptr->end()) + L'"' ) : std::wstring();
}

// <Y> maybe<std::basic_string<T>>
//
//

template <typename T> template <typename Y> Y *maybe<std::basic_string<T>>::ptr_to() const {
	return aux<Y>(*this).ptr_to();
}

template <typename T> template <typename Y> bool maybe<std::basic_string<T>>::has() const {
	return aux<Y>(*this).has();
}

template <typename T> template <typename Y> bool maybe<std::basic_string<T>>::has_type() const {
	return aux<Y>(*this).has_type();
}

template <typename T> template <typename Y> std::string maybe<std::basic_string<T>>::str() const {
	return aux<Y>(*this).str();
}

// maybe<either<A,B>>
//
//

template <typename A,typename B> maybe<either<A,B>>::maybe() : maybe(nullptr) {
}

template <typename A,typename B> maybe<either<A,B>>::maybe(std::nullptr_t) : xptr(nullptr) {
}

template <typename A,typename B> maybe<either<A,B>>::maybe(const value_type& x0) : xptr(new value_type(x0)) {
}

template <typename A,typename B> maybe<either<A,B>>::maybe(const maybe& r) : xptr(r.xptr ? new value_type(*r.xptr) : nullptr) {
}

template <typename A,typename B> void maybe<either<A,B>>::clear() {
	if(xptr) {
		delete xptr;
		xptr = nullptr;
	}
}	

template <typename A,typename B> maybe<either<A,B>>::~maybe() {
	clear();
}

template <typename A,typename B> maybe<either<A,B>>& maybe<either<A,B>>::operator=(const maybe&r) {

	clear();

	if(r.xptr)
		xptr = new value_type(*r.xptr);

	return *this;
}

template <typename A,typename B> maybe<either<A,B>>::operator std::wstring () const {
	return xptr ? (std::wstring)*xptr : std::wstring();
}

// <Y> maybe<either<A,B>>
//
//

template <typename A,typename B> template <typename Y> Y *maybe<either<A,B>>::ptr_to() const {
	return aux<Y>(*this).ptr_to();
}

template <typename A,typename B> template <typename Y> bool maybe<either<A,B>>::has() const {
	return aux<Y>(*this).has();
}

template <typename A,typename B> template <typename Y> bool maybe<either<A,B>>::has_type() const {
	return aux<Y>(*this).has_type();
}

template <typename A,typename B> template <typename Y> std::string maybe<either<A,B>>::str() const {
	return aux<Y>(*this).str();
}


