#pragma once

#include <cstddef>

#include <string>

template <typename> struct maybe;
template <typename T> struct maybe<std::basic_string<T>>;

#include <either.hh>

template <typename,typename> struct maybe_aux;
template <typename T> struct maybe_aux<T,T>;
template <typename T> struct maybe_aux<T,std::nullptr_t>;
template <typename A,typename B,typename C> struct maybe_aux<either<A,B>,C>;
template <typename A,typename B> struct maybe_aux<either<A,B>,std::nullptr_t>;

// maybe
//
//

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

	template <typename U> using aux = maybe_aux<value_type,U>;

	template <typename U> U *ptr_to() const;

	template <typename> bool has() const;
	template <typename> bool has_type() const;

	template <typename> std::string str() const;
};

// maybe<std::basic_string<T>>
//
//

template <typename T> struct maybe<std::basic_string<T>> {

	using char_type = T;
	using value_type = std::basic_string<char_type>;

	value_type *xptr;

	maybe();
	maybe(std::nullptr_t);

	maybe(const maybe&);
	maybe(const value_type&);
	maybe(const char_type *);

	~maybe();

	void clear();

	maybe& operator=(const maybe&);

	// aux

	template <typename U> using aux = maybe_aux<value_type,U>;

	template <typename U> U *ptr_to() const;

	template <typename> bool has() const;
	template <typename> bool has_type() const;

	template <typename> std::string str() const;
};


// maybe_aux <T,U> 
//
//

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

// maybe_aux <either<A,B>,C> 
//
//

template <typename A,typename B,typename C> struct maybe_aux<either<A,B>,C> {

	using dom = either<A,B>;
	using cod = C;

	using value_type = maybe<dom>;

	dom *xptr;

	maybe_aux(const value_type&);

	cod *ptr_to() const;
	bool has() const;
	bool has_type() const;
	std::string str() const;
};

// maybe_aux <either<A,B>,std::nullptr_t>
//
//

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

// maybe_aux <T,T>
//
//

template <typename T> struct maybe_aux<T,T> {

	using dom = T;
	using cod = T;

	using value_type = maybe<dom>;

	dom *xptr;

	maybe_aux(const value_type&);

	cod *ptr_to() const;
	bool has() const;
	bool has_type() const;
	std::string str() const;
};

// maybe_aux <T,std::nullptr_t>
//
//

template <typename T> struct maybe_aux<T,std::nullptr_t> {

	using dom = T;
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
