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

// primary classes

template<typename> struct maybe;

template<typename,typename> struct either;

// aux classes

template<typename,typename> struct maybe_aux;

template<typename X> struct maybe_aux<X,X>;

template<typename X> struct maybe_aux<X,std::nullptr_t>;

template<typename A, typename B, typename T> struct maybe_aux<either<A,B>,T>;

template<typename A, typename B> struct maybe_aux<either<A,B>,std::nullptr_t>;

// maybe

template<typename X> struct maybe {

	typedef X value_type;

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

// either

template<typename A,typename B> struct either {

	maybe<A> a;
	maybe<B> b;

	either();

	either(std::nullptr_t);

	either(const either&);

	either(const maybe<A>&, const maybe<B>&);

	either(const maybe<A>&);
	either(const maybe<B>&);

	void clear();

	either& operator=(const either&);
	either& operator=(const maybe<A>&);
	either& operator=(const maybe<B>&);
	either& operator=(std::nullptr_t);

	template<typename C> explicit operator maybe<C> () const;

	template<typename C> maybe<C> find() const;

	template<typename C> C *ptr_to() const;
	template<typename C> bool has() const;
	template<typename C> bool has_type() const;

	template<typename C> std::string str() const;

	template<typename C> maybe<C> find(const C*) const;

	template<typename C> C *ptr_to(const C*) const;
	template<typename C> bool has(const C*) const;
	template<typename C> bool has_type(const C*) const;

	template<typename C> std::string str(const C*) const;
};

// maybe_aux <X,T> 

template<typename X, typename T> struct maybe_aux {

	T *ptr_to(const maybe<X>&);
	bool has(const maybe<X>&);
	bool has_type(const maybe<X>&);
	std::string str(const maybe<X>&);
};

template<typename X, typename T> T *maybe_aux<X,T>::ptr_to(const maybe<X>&) {
	return nullptr;
}

template<typename X, typename T> bool maybe_aux<X,T>::has(const maybe<X>&) {
	return false;
}

template<typename X, typename T> bool maybe_aux<X,T>::has_type(const maybe<X>&) {
	return false;
}

template<typename X, typename T> std::string maybe_aux<X,T>::str(const maybe<X>&) {
	return "<X,T> ( x is null )";
}

// maybe_aux <either<A,B>,C> 

template<typename A, typename B, typename T> struct maybe_aux<either<A,B>,T> {

	typedef either<A,B> value_type;

	T *ptr_to(const maybe<value_type>&);
	bool has(const maybe<value_type>&);
	bool has_type(const maybe<value_type>&);
	std::string str(const maybe<value_type>&);
};

template<typename A,typename B,typename T> T *maybe_aux<either<A,B>,T>::ptr_to(const maybe<value_type>& a) {
	return (a.xptr == nullptr) ? nullptr : a.xptr->ptr_to((T *)nullptr);
}

template<typename A,typename B,typename T> bool maybe_aux<either<A,B>,T>::has(const maybe<value_type>& a) {
	return (a.xptr == nullptr) ? false : a.xptr->has((T *)nullptr);
}

template<typename A,typename B,typename T> bool maybe_aux<either<A,B>,T>::has_type(const maybe<value_type>& a) {
	const auto& x  = a.xptr == nullptr ? value_type() : *a.xptr;
	return x.has_type((T *)nullptr);
}

template<typename A,typename B,typename T> std::string maybe_aux<either<A,B>,T>::str(const maybe<value_type>& a) {

	std::stringstream ss;

	ss << "<either<A,B>,T> ( type" << ( has_type(a) ? "" : " not" ) << " found ) ";

	if(has(a))
		ss << "( x := " << *ptr_to(a) << " )";
	else
		ss << "( x is null )";

	return ss.str();
}

// maybe_aux <either<A,B>,std::nullptr_t>

template<typename A, typename B> struct maybe_aux<either<A,B>,std::nullptr_t> {

	typedef either<A,B> value_type;

	static const std::nullptr_t null;

	const std::nullptr_t *ptr_to(const maybe<value_type>&);
	bool has(const maybe<value_type>&);
	bool has_type(const maybe<value_type>&);
	std::string str(const maybe<value_type>&);
};

template<typename A,typename B> const std::nullptr_t maybe_aux<either<A,B>,std::nullptr_t>::null = nullptr;

template<typename A,typename B> const std::nullptr_t *maybe_aux<either<A,B>,std::nullptr_t>::ptr_to(const maybe<value_type>&) {
	return &null;
}

template<typename A,typename B> bool maybe_aux<either<A,B>,std::nullptr_t>::has(const maybe<value_type>&a) {
	return a.xptr == nullptr;
}

template<typename A,typename B> bool maybe_aux<either<A,B>,std::nullptr_t>::has_type(const maybe<value_type>&) {
	return true;
}

template<typename A,typename B> std::string maybe_aux<either<A,B>,std::nullptr_t>::str(const maybe<value_type>& a) {

	std::stringstream ss;


	ss << "<either<A,B>,T> ( type" << ( has_type(a) ? "" : " not" ) << " found ) ";

	ss << "<either<A,B>,std::nullptr_t> ( x is" << ( has(a) ? "" : " not" ) << " null )";

	return ss.str();
}


// maybe_aux <X,X>

template<typename X> struct maybe_aux<X,X> {

	typedef X value_type;

	X *ptr_to(const maybe<value_type>&);
	bool has(const maybe<value_type>&);
	bool has_type(const maybe<value_type>&);
	std::string str(const maybe<value_type>&);
};

template<typename X> X *maybe_aux<X,X>::ptr_to(const maybe<value_type>& a) {
	return a.xptr;
}

template<typename X> bool maybe_aux<X,X>::has(const maybe<value_type>& a) {
	return a.xptr != nullptr;
}

template<typename X> bool maybe_aux<X,X>::has_type(const maybe<value_type>&) {
	return true;
}


template<typename X> std::string maybe_aux<X,X>::str(const maybe<value_type>& a) {

	std::stringstream ss;

	ss << "<X,X> ";

	if(has(a)) {
		ss << "( x := " << *ptr_to(a) << " )";
	} else {
		ss << "( x is null )";
	}

	return ss.str();
}

// maybe_aux <X,std::nullptr_t>

template<typename X> struct maybe_aux<X,std::nullptr_t> {

	typedef X value_type;

	static const std::nullptr_t null;

	const std::nullptr_t *ptr_to(const maybe<value_type>&);
	bool has(const maybe<value_type>&);
	bool has_type(const maybe<value_type>&);
	std::string str(const maybe<value_type>&);
};

template<typename X> const std::nullptr_t maybe_aux<X,std::nullptr_t>::null = nullptr;

template<typename X> const std::nullptr_t *maybe_aux<X,std::nullptr_t>::ptr_to(const maybe<value_type>&) {
	return &null;
}

template<typename X> bool maybe_aux<X,std::nullptr_t>::has(const maybe<value_type>&a) {
	return a.xptr == nullptr;
}

template<typename X> bool maybe_aux<X,std::nullptr_t>::has_type(const maybe<value_type>&) {
	return true;
}

template<typename X> std::string maybe_aux<X,std::nullptr_t>::str(const maybe<value_type>& a) {

	std::stringstream ss;

	ss << "<X,std::nullptr_t> ( x is" << ( has(a) ? "" : " not" ) << " null )";

	return ss.str();
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

// <Y> maybe<X>

template<typename X> template<typename Y> Y *maybe<X>::ptr_to() const {
	return maybe_aux<X,Y>().ptr_to(*this);
}

template<typename X> template<typename Y> bool maybe<X>::has() const {
	return maybe_aux<X,Y>().has(*this);
}

template<typename X> template<typename Y> bool maybe<X>::has_type() const {
	return maybe_aux<X,Y>().has_type(*this);
}

template<typename X> template<typename Y> std::string maybe<X>::str() const {
	return maybe_aux<X,Y>().str(*this);
}

// either

template<typename A, typename B> either<A,B>::either() : either(nullptr) {
}

template<typename A, typename B> either<A,B>::either(std::nullptr_t) : a(nullptr), b(nullptr) {
}

template<typename A, typename B> either<A,B>::either(const either& r) : either(r.a, r.b) {
}

template<typename A, typename B> either<A,B>::either(const maybe<A>& my_a, const maybe<B>& my_b) : a(my_a), b(my_b) {
}

template<typename A, typename B> either<A,B>::either(const maybe<A>& my_a) : a(my_a), b(nullptr) {
}

template<typename A, typename B> either<A,B>::either(const maybe<B>& my_b) : a(nullptr), b(my_b) {
}

template<typename A, typename B> void either<A,B>::clear() {
	a.clear();
	b.clear();
}

template<typename A, typename B> either<A,B>& either<A,B>::operator=(const either& r) {
	a = r.a;
	b = r.b;
	return *this;
}

template<typename A, typename B> either<A,B>& either<A,B>::operator=(const maybe<A>& my_a) {
	a = my_a;
	b.clear();
	return *this;
}

template<typename A, typename B> either<A,B>& either<A,B>::operator=(const maybe<B>& my_b) {
	a.clear();
	b = my_b;
	return *this;
}

template<typename A, typename B> either<A,B>& either<A,B>::operator=(std::nullptr_t) {
	clear();
	return *this;
}

// either<A,B>::(<C>())

template<typename A, typename B> template<typename C> either<A,B>::operator maybe<C> () const {
	return this->find<C>();
}

template<typename A, typename B> template<typename C> maybe<C> either<A,B>::find() const {
	return has<C>() ? *ptr_to<C>() : maybe<C>();
}

template<typename A, typename B> template<typename C> C *either<A,B>::ptr_to() const {

	if(a.has<C>())
		return a.ptr_to<C>();

	if(b.has<C>())
		return b.ptr_to<C>();

	return nullptr;
}

template<typename A, typename B> template<typename C> bool either<A,B>::has() const {
	return a.has<C>() | b.has<C>();
}

template<typename A, typename B> template<typename C> bool either<A,B>::has_type() const {
	return a.has_type<C>() | b.has_type<C>();
}

template<typename A, typename B> template<typename C> std::string either<A,B>::str() const {

	std::stringstream ss;

	ss << "( a := " << a.str<C>() << " ) ";
	ss << "( b := " << b.str<C>() << " )";

	return ss.str();
}

// either<A,B>::(<C>(C*))

template<typename A, typename B> template<typename C> maybe<C> either<A,B>::find(const C*) const {
	return this->find<C>();
}

template<typename A, typename B> template<typename C> C *either<A,B>::ptr_to(const C*) const {
	return this->ptr_to<C>();
}

template<typename A, typename B> template<typename C> bool either<A,B>::has(const C*) const {
	return this->has<C>();
}

template<typename A, typename B> template<typename C> bool either<A,B>::has_type(const C*) const {
	return this->has_type<C>();
}

template<typename A, typename B> template<typename C> std::string either<A,B>::str(const C*) const {
	return this->str<C>();
}


// main

int main(int argc, char **argv) {

	typedef either<long,std::string> long_or_string;

	typedef either<long_or_string,bool> long_or_string_or_bool;

	maybe<long> a;
	long_or_string ab;
	long_or_string_or_bool abc;

	if(argc == 2 && strcmp(argv[1], "-h") == 0) {
		std::cerr << std::endl << "usage: " << argv[0] << " [options]" << std::endl << std::endl;
		return -1;
	}

	if(argc == 2)
		a = atol(argv[1]);

	// maybe<long>

	if(a.xptr)
		std::cout << "x := " << *a.xptr << std::endl;
	else
		std::cout << "x := nullptr" << std::endl;

	std::cout << "maybe<long> -> std::string    :: " << a.str<std::string>()    << std::endl;
	std::cout << "maybe<long> -> long           :: " << a.str<long>()           << std::endl;
	std::cout << "maybe<long> -> bool           :: " << a.str<bool>()           << std::endl;
	std::cout << "maybe<long> -> std::nullptr_t :: " << a.str<std::nullptr_t>() << std::endl;
	std::cout << std::endl;

	std::cout << "( ab )" << std::endl << std::endl;

	// long_or_string

	std::cout << "either<long,std::string> -> long           :: " << ab.str<long>()           << std::endl;
	std::cout << "either<long,std::string> -> std::string    :: " << ab.str<std::string>()    << std::endl;
	std::cout << "either<long,std::string> -> bool           :: " << ab.str<bool>()           << std::endl;
	std::cout << "either<long,std::string> -> std::nullptr_t :: " << ab.str<std::nullptr_t>() << std::endl;
	std::cout << std::endl;
	
	ab = a;

	std::cout << "either<long,std::string> -> long           :: " << ab.str<long>()           << std::endl;
	std::cout << "either<long,std::string> -> std::string    :: " << ab.str<std::string>()    << std::endl;
	std::cout << "either<long,std::string> -> bool           :: " << ab.str<bool>()           << std::endl;
	std::cout << "either<long,std::string> -> std::nullptr_t :: " << ab.str<std::nullptr_t>() << std::endl;
	std::cout << std::endl;

	ab = std::string("dicks");

	std::cout << "either<long,std::string> -> long           :: " << ab.str<long>()           << std::endl;
	std::cout << "either<long,std::string> -> std::string    :: " << ab.str<std::string>()    << std::endl;
	std::cout << "either<long,std::string> -> bool           :: " << ab.str<bool>()           << std::endl;
	std::cout << "either<long,std::string> -> std::nullptr_t :: " << ab.str<std::nullptr_t>() << std::endl;
	std::cout << std::endl;

	// long_or_string_or_bool

	std::cout << "( abc )" << std::endl << std::endl;

	std::cout << "either<either<long,std::string>,bool> -> long           :: " << abc.str<long>()           << std::endl;
	std::cout << "either<either<long,std::string>,bool> -> std::string    :: " << abc.str<std::string>()    << std::endl;
	std::cout << "either<either<long,std::string>,bool> -> bool           :: " << abc.str<bool>()           << std::endl;
	std::cout << "either<either<long,std::string>,bool> -> std::nullptr_t :: " << abc.str<std::nullptr_t>() << std::endl;
	std::cout << std::endl;

	abc = true;

	std::cout << "either<either<long,std::string>,bool> -> long           :: " << abc.str<long>()           << std::endl;
	std::cout << "either<either<long,std::string>,bool> -> std::string    :: " << abc.str<std::string>()    << std::endl;
	std::cout << "either<either<long,std::string>,bool> -> bool           :: " << abc.str<bool>()           << std::endl;
	std::cout << "either<either<long,std::string>,bool> -> std::nullptr_t :: " << abc.str<std::nullptr_t>() << std::endl;
	std::cout << std::endl;

	abc = long_or_string(666);

	std::cout << "either<either<long,std::string>,bool> -> long           :: " << abc.str<long>()           << std::endl;
	std::cout << "either<either<long,std::string>,bool> -> std::string    :: " << abc.str<std::string>()    << std::endl;
	std::cout << "either<either<long,std::string>,bool> -> bool           :: " << abc.str<bool>()           << std::endl;
	std::cout << "either<either<long,std::string>,bool> -> std::nullptr_t :: " << abc.str<std::nullptr_t>() << std::endl;
	std::cout << std::endl;

	abc = long_or_string(std::string("dicks"));

	std::cout << "either<either<long,std::string>,bool> -> long           :: " << abc.str<long>()           << std::endl;
	std::cout << "either<either<long,std::string>,bool> -> std::string    :: " << abc.str<std::string>()    << std::endl;
	std::cout << "either<either<long,std::string>,bool> -> bool           :: " << abc.str<bool>()           << std::endl;
	std::cout << "either<either<long,std::string>,bool> -> std::nullptr_t :: " << abc.str<std::nullptr_t>() << std::endl;
	std::cout << std::endl;

	std::cout << *abc.find<std::string>().xptr << std::endl;

	return 0;
}
