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

template <typename> struct maybe;

template <typename,typename> struct either;

//template <typename A,typename B,typename T> struct either<either<A,B>,T>;
//template <typename A,typename B,typename T> struct either<T,either<A,B>>;
//template <typename A,typename B,typename C,typename D> struct either<either<A,B>,either<C,D>>;

// aux classes

template <typename,typename> struct maybe_aux;

template <typename X> struct maybe_aux<X,X>;

template <typename X> struct maybe_aux<X,std::nullptr_t>;

template <typename A,typename B,typename T> struct maybe_aux<either<A,B>,T>;

template <typename A,typename B> struct maybe_aux<either<A,B>,std::nullptr_t>;

// maybe

template <typename T> struct maybe {

	using value_type = T;

	value_type *xptr;

	maybe();
	maybe(std::nullptr_t);

	maybe(const maybe&);
	maybe(const value_type&);

	~maybe();

	void clear();

	maybe& operator=(const maybe&);

	// aux

	template <typename U> using aux = maybe_aux<T,U>;

	template <typename U> U *ptr_to() const;

	template <typename> bool has() const;
	template <typename> bool has_type() const;

	template <typename> std::string str() const;
};

// either

template <typename A,typename B> struct either {

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

	template <typename C> maybe<C> find() const;

	template <typename C> C *ptr_to() const;

	template <typename> bool has() const;
	template <typename> bool has_type() const;

	template <typename> std::string str() const;
};

// maybe_aux <T,U> 

template <typename T,typename U> struct maybe_aux {

	using dom = T;
	using cod = U;

	using value_type = maybe<dom>;

	maybe_aux(const value_type&);

	cod *ptr_to() const;
	bool has() const;
	bool has_type() const;
	std::string str() const;
};

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

template <typename A,typename B,typename U> struct maybe_aux<either<A,B>,U> {

	using dom = either<A,B>;
	using cod = U;

	using value_type = maybe<dom>;

	dom *xptr;

	maybe_aux(const value_type&);

	cod *ptr_to() const;
	bool has() const;
	bool has_type() const;
	std::string str() const;
};

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

template <typename A,typename B> struct maybe_aux<either<A,B>,std::nullptr_t> {

	using dom = either<A,B>;
	using cod = std::nullptr_t;

	using value_type = maybe<dom>;

	static cod y;

	dom *xptr;

	maybe_aux(const value_type&);

	cod *ptr_to() const;
	bool has() const;
	bool has_type() const;
	std::string str() const;
};

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

template <typename X> struct maybe_aux<X,X> {

	using dom = X;
	using cod = X;

	using value_type = maybe<dom>;

	dom *xptr;

	maybe_aux(const value_type&);

	cod *ptr_to() const;
	bool has() const;
	bool has_type() const;
	std::string str() const;
};

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

template <typename X> struct maybe_aux<X,std::nullptr_t> {

	using dom = X;
	using cod = std::nullptr_t;

	using value_type = maybe<dom>;

	static cod y;

	dom *xptr;

	maybe_aux(const value_type&);

	cod *ptr_to() const;
	bool has() const;
	bool has_type() const;
	std::string str() const;
};

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

// either<A,B>::(<C>())

template <typename A,typename B> template <typename C> maybe<C> either<A,B>::find() const {
	return has<C>() ? *ptr_to<C>() : maybe<C>();
}

template <typename A,typename B> template <typename C> C *either<A,B>::ptr_to() const {

	if(a.has<C>())
		return a.ptr_to<C>();

	if(b.has<C>())
		return b.ptr_to<C>();

	return nullptr;
}

template <typename A,typename B> template <typename C> bool either<A,B>::has() const {
	return a.has<C>() | b.has<C>();
}

template <typename A,typename B> template <typename C> bool either<A,B>::has_type() const {
	return a.has_type<C>() | b.has_type<C>();
}

template <typename A,typename B> template <typename C> std::string either<A,B>::str() const {

	std::stringstream ss;

	ss << "( a := " << a.str<C>() << " ) ";
	ss << "( b := " << b.str<C>() << " )";

	return ss.str();
}

// main

typedef either<long,std::string> long_or_string;
typedef either<long_or_string,bool> long_or_string_or_bool;

void do_abc(long_or_string_or_bool& abc) {

#define XXX(a,b,c,d) ((a).b<c>() ? d:' ')
#define YYY(a,c) XXX(a,has_type,c,'*') << XXX(a,has,c,'#') << "either<either<long,std::string>,bool> -> " << std::setw(15) << #c << ":: " << abc.str<c>()

	std::cout << YYY(abc,bool) << std::endl;
	std::cout << YYY(abc,char) << std::endl;
	std::cout << YYY(abc,long) << std::endl;
	std::cout << YYY(abc,std::string) << std::endl;
	std::cout << YYY(abc,std::nullptr_t) << std::endl;

	std::cout << std::endl;
}


int main(int argc, char **argv) {

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

	ab = std::string("DICK");

	std::cout << "either<long,std::string> -> long           :: " << ab.str<long>()           << std::endl;
	std::cout << "either<long,std::string> -> std::string    :: " << ab.str<std::string>()    << std::endl;
	std::cout << "either<long,std::string> -> bool           :: " << ab.str<bool>()           << std::endl;
	std::cout << "either<long,std::string> -> std::nullptr_t :: " << ab.str<std::nullptr_t>() << std::endl;
	std::cout << std::endl;

	// long_or_string_or_bool

	std::cout << "( abc )" << std::endl << std::endl;

	do_abc(abc);

	abc = true;

	do_abc(abc);

	// abc = std::string("214;//long_or_string(a);");
	abc = long_or_string(a);

	do_abc(abc);

	abc = long_or_string(std::string("DICK"));

	do_abc(abc);

	std::cout << *abc.find<std::string>().xptr << std::endl;

	return 0;
}
