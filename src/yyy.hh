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

#include <either.template.hh>
#include <maybe.template.hh>

#define DISTINCT(CLASS,BASE)			\
	using _##CLASS = BASE;			\
	struct CLASS : _##CLASS {		\
		using _##CLASS::_##CLASS;	\
	}

#define DISTINCT_TEMPLATE(CLASS,BASE)		\
	using _##CLASS = BASE;			\
	struct CLASS : _##CLASS {		\
		using _##CLASS::_##CLASS;	\
	}

namespace yyy {

	using literal_t = uint16_t;
	using register_t = uint8_t;
	using operator_t = wchar_t;

	//DISTINCT(symbol, std::wstring);
	//DISTINCT(reference, std::wstring);

	using symbol = std::wstring;
	using reference = std::wstring;
	using range = std::pair<size_t,size_t>;

	template <typename> struct predicate;
	template <typename> struct function;
	template <typename> struct binding;
	template <typename> struct grammar;

	template <typename T> using argument = either<T,function<T>>;
	template <typename T> using rule = function<predicate<T>>;

	template <typename T> using arguments = std::list<argument<T>>;
	template <typename T> using rules = std::list<rule<T>>;

	using term = either<symbol,literal_t>;

	template <typename T> struct function {

		using value_type = T;

		operator_t op;
		arguments<value_type> args;

		function();

		function(operator_t);
		function(const function&);
		function(operator_t, const arguments<value_type>&);

		function& operator<<(const argument<value_type>&);
		function& operator<<(const function<value_type>&);
		function& operator<<(const value_type&);

		template <typename U> function& operator<<(const U&);

		explicit operator std::wstring() const;
	};

	template <typename T> struct predicate {

		typedef T value_type;

		enum class modifiers { push, pop, lift, drop, bind, reject };
		enum class types { end, any, mem, ref, type, op, value };

		modifiers modifier;
		types type;
		either<argument<value_type>,symbol> arg;
	};

	template <typename T> using _grammar = std::map<symbol,rules<T>>;

	template <typename T> struct grammar : _grammar<T> {

		using _grammar<T>::_grammar;
	};

	template <typename T> using meta_argument = either<argument<T>,binding<T>>;

	template <typename T> struct binding {

		using value_type = T;

		predicate<value_type> clause;
		std::list<meta_argument<value_type>> closure;

		binding(const predicate<value_type>&);
		binding& operator<<(const meta_argument<value_type>&);
	};

	// extern template struct predicate<term>;
	// extern template struct function<term>;
	// extern template struct binding<term>;
	// extern template struct grammar<term>;
}
