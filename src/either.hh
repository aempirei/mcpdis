#pragma once

#include <climits>
#include <cstdint>

#include <sstream>
#include <string>

// primary classes

template <typename> struct maybe;

template <typename,typename> struct either;

template <typename A,typename B,typename T> struct either<either<A,B>,T>;
// template <typename A,typename B,typename T> struct either<T,either<A,B>>;
// template <typename A,typename B,typename C,typename D> struct either<either<A,B>,either<C,D>>;

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

	using first_type = A;
	using second_type = B;

	maybe<first_type> a;
	maybe<second_type> b;

	either();

	either(std::nullptr_t);

	either(const either&);

	either(const maybe<first_type>&, const maybe<second_type>&);

	either(const maybe<first_type>&);
	either(const maybe<second_type>&);

	void clear();

	either& operator=(const either&);

	either& operator=(const maybe<first_type>&);
	either& operator=(const maybe<second_type>&);

	either& operator=(std::nullptr_t);

	template <typename D> maybe<D> find() const;

	template <typename D> D *ptr_to() const;

	template <typename> bool has() const;
	template <typename> bool has_type() const;

	template <typename> std::string str() const;
};

// either<either<A,B>,C>

template <typename A,typename B,typename C> struct either<either<A,B>,C> {

	using first_type = either<A,B>;
	using second_type = C;

	maybe<first_type> a;
	maybe<second_type> b;

	either();

	either(std::nullptr_t);

	either(const either&);

	either(const maybe<first_type>&, const maybe<second_type>&);

	either(const maybe<A>&);
	either(const maybe<B>&);

	either(const maybe<first_type>&);
	either(const maybe<second_type>&);

	void clear();

	either& operator=(const either&);

	either& operator=(const maybe<A>&);
	either& operator=(const maybe<B>&);

	either& operator=(const maybe<first_type>&);
	either& operator=(const maybe<second_type>&);

	either& operator=(std::nullptr_t);

	template <typename D> maybe<D> find() const;

	template <typename D> D *ptr_to() const;

	template <typename> bool has() const;
	template <typename> bool has_type() const;

	template <typename> std::string str() const;
};


// maybe_aux<T,U> 

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

// maybe_aux<either<A,B>,U> 

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
