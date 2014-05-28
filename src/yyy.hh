#pragma once

#include <cstdint>
#include <cstdlib>
#include <cwchar>

#include <string>
#include <vector>
#include <list>
#include <map>
#include <set>
#include <initializer_list>

#include <operators.hh>
#include <ansicolor.hh>

namespace yyy {

	using literal_t = uint16_t;
	using register_t = uint8_t;
	using operator_t = wchar_t;

	using symbol = std::wstring;
	using range = std::pair<size_t,size_t>;

	template<typename,typename> struct either;

	template<typename> struct predicate;
	template<typename> struct function;
	template<typename> struct binding;
	template<typename> struct grammar;
	template<typename> struct maybe;

	template<typename X> using argument = either<X,function<X>>;
	template<typename X> using rule = function<predicate<X>>;

	template<typename X> using arguments = std::list<argument<X>>;
	template<typename X> using rules = std::list<rule<X>>;

	using term = either<symbol,literal_t>;

	template<typename A, typename B> struct either {

		maybe<A> a;
		maybe<B> b;

		either();
		either(const either&);

		either(const maybe<A>&);
		either(const maybe<B>&);

		either& operator=(const maybe<A>&);
		either& operator=(const maybe<B>&);

		bool operator==(const either&);
		bool operator<(const either&);

		operator const maybe<A>& () const;
		operator maybe<A>& ();

		operator const maybe<B>& () const;
		operator maybe<B>& ();
	};

	template<typename X> struct maybe {

		typedef X value_type;

		X *x = NULL;

		maybe();
		maybe(const maybe&);
		maybe(const X&);

		~maybe();

		maybe& operator=(const X&);
		maybe& operator=(const maybe&);

		bool operator==(const maybe&);
		bool operator<(const maybe&);

		bool is_set() const;
		bool is_null() const;

		void clear();

		operator const X& () const;
		operator X ();
	};

	template<typename X> struct function {

		typedef X value_type;

		operator_t op;
		arguments<X> args;
	};

	template<typename X> struct predicate {

		typedef X value_type;

		enum class modifier { push, pop, lift, drop, bind, reject };
		enum class types { end, any, mem, ref, type, op, value };

		either<argument<X>,symbol> arg;
	};

	template<typename X> using _grammar = std::map<symbol,rules<X>>;

	template<typename X> struct grammar : _grammar<X> {

		using _grammar<X>::_grammar;
	};

	template<typename X> struct binding {

		typedef X value_type;

		predicate<X> clause;
		std::list<either<argument<X>,binding<X>>> closure;
	};

	extern template struct maybe<symbol>;
	extern template struct maybe<literal_t>;

	extern template struct either<symbol,literal_t>;

	extern template struct predicate<term>;
	extern template struct function<term>;
	extern template struct binding<term>;
	extern template struct grammar<term>;

}
