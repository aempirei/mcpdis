#include <climits>
#include <cstdint>
#include <cstdlib>
#include <cstring>

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

#include <list>
#include <map>
#include <set>

template<typename> struct maybe;

template<typename,typename> struct maybe_aux;

template<typename X> struct maybe_aux<X,std::nullptr_t>;
template<typename X> struct maybe_aux<X,X>;

template<typename,typename> struct either;

template<typename,typename,typename> struct either_aux;

// maybe

template<typename X> struct maybe {

	X *xptr;

	maybe();
	maybe(const X&);
	maybe(const maybe&);
	maybe(std::nullptr_t);

	~maybe();

	void clear();

	maybe& operator=(const maybe&);

	template<typename Y> Y *ptr_to() const;
	template<typename Y> bool has() const;
	template<typename Y> bool has_type() const;

	template<typename Y> std::string str() const;
};

// maybe_aux <X,T> 

template<typename X, typename T> struct maybe_aux {
	static T *ptr_to(const maybe<X>&);
	static bool has(const maybe<X>&);
	static bool has_type(const maybe<X>&);
};

template<typename X, typename T> T *maybe_aux<X,T>::ptr_to(const maybe<X>&) {
	return nullptr;
}

template<typename X, typename T> bool maybe_aux<X,T>:: has(const maybe<X>& a) {
	return ptr_to(a) != nullptr;
	// return false;
}

template<typename X, typename T> bool maybe_aux<X,T>:: has_type(const maybe<X>&) {
	return false;
}

// maybe_aux <X,X>

template<typename X> struct maybe_aux<X,X> {
	static X *ptr_to(const maybe<X>&);
	static bool has(const maybe<X>&);
	static bool has_type(const maybe<X>&);
};

template<typename X> X *maybe_aux<X,X>::ptr_to(const maybe<X>& a) {
	return a.xptr;
}

template<typename X> bool maybe_aux<X,X>::has(const maybe<X>& a) {
	return ptr_to(a) != nullptr;
}

template<typename X> bool maybe_aux<X,X>::has_type(const maybe<X>&) {
	return true;
}

// maybe_aux <X,std::nullptr_t>

template<typename X> struct maybe_aux<X,std::nullptr_t> {
	static std::nullptr_t ptr_to(const maybe<X>&);
	static bool has(const maybe<X>&);
	static bool has_type(const maybe<X>&);
};

template<typename X> std::nullptr_t maybe_aux<X,std::nullptr_t>::ptr_to(const maybe<X>&) {
	return nullptr;
}

template<typename X> bool maybe_aux<X,std::nullptr_t>::has(const maybe<X>&a) {
	return a.xptr == nullptr;
}

template<typename X> bool maybe_aux<X,std::nullptr_t>::has_type(const maybe<X>&) {
	return true;
}

// maybe

template<typename X> maybe<X>::maybe() : maybe(nullptr) {
}

template<typename X> maybe<X>::maybe(std::nullptr_t) : xptr(nullptr) {
}

template<typename X> maybe<X>::maybe(const X& x0) : xptr(new X(x0)) {
}

template<typename X> maybe<X>::maybe(const maybe& r) : xptr(r.xptr ? new X(*r.xptr) : nullptr) {
}

template<typename X> void maybe<X>::clear() {
	if(xptr) {
		delete xptr;
		xptr = nullptr;
	}
}	

template<typename X> maybe<X>::~maybe() {
	clear();
}

template<typename X> maybe<X>& maybe<X>::operator=(const maybe&r) {

	clear();

	if(r.xptr)
		xptr = new X(*r.xptr);

	return *this;
}

// <Y>

template<typename X> template<typename Y> Y *maybe<X>::ptr_to() const {
	return maybe_aux<X,Y>::ptr_to(*this);
}

template<typename X> template<typename Y> bool maybe<X>::has() const {
	return maybe_aux<X,Y>::has(*this);
}

template<typename X> template<typename Y> bool maybe<X>::has_type() const {
	return maybe_aux<X,Y>::has_type(*this);
}

// str()

template<typename X> template<typename Y> std::string maybe<X>::str() const {

	std::stringstream ss;

	ss << "ptr_to, has, has_type := ";
	ss << ptr_to<Y>() << ' ';
	ss << ( has<Y>() ? 'T' : 'F' ) << ' ';
	ss << ( has_type<Y>() ? 'T' : 'F' );

	return ss.str();
}

// either_aux

template<typename A, typename B, typename C> struct either_aux {
};

// either

template<typename A,typename B> struct either {

	maybe<A> a;
	maybe<B> b;

	either();

	either(std::nullptr_t);

	either(const either&);

	either(const maybe<A>&);
	either(const maybe<B>&);

	either(const A&);
	either(const B&);

	void clear();

	either& operator=(const either&);

	template<typename C> C *ptr_to() const;
	template<typename C> bool has() const;
	template<typename C> bool has_type() const;

	template<typename C> std::string str() const;

};

// main

int main(int argc, char **argv) {

	maybe<long> a;

	if(argc == 2 && strcmp(argv[1], "-h") == 0) {
		std::cerr << std::endl << "usage: " << argv[0] << " [options]" << std::endl << std::endl;
		return -1;
	}

	if(argc == 2)
		a = atol(argv[1]);

	if(a.xptr)
		std::cout << "x := " << *a.xptr << std::endl;
	else
		std::cout << "x := nullptr" << std::endl;

	std::cout << "maybe<long> -> std::string    :: " << a.str<std::string>()    << std::endl;
	std::cout << "maybe<long> -> long           :: " << a.str<long>()           << std::endl;
	std::cout << "maybe<long> -> std::nullptr_t :: " << a.str<std::nullptr_t>() << std::endl;

	return 0;
}
