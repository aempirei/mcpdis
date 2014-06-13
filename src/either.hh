#pragma once

#include <cstddef>

#include <string>

template <typename,typename> struct either;

#include <maybe.hh>

template <typename A,typename B,typename C> struct either<either<A,B>,C>;
// template <typename A,typename B,typename C> struct either<C,either<A,B>>;

// template <typename A,typename B,typename C,typename D> struct either<either<A,B>,either<C,D>>;

// either
//
//

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
//
//

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

// either<C,either<A,B>>
//
//

/*
   template <typename A,typename B,typename C> struct either<C,either<A,B>> {

   using first_type = C;
   using second_type = either<A,B>;

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
 */
