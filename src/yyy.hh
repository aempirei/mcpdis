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

	template<typename> struct predicate;
	template<typename> struct function;
	template<typename> struct binding;
	template<typename> struct grammar;
	template<typename> struct maybe;

	template<typename T, typename U> using either = std::pair<maybe<T>,maybe<U>>;

	template<typename T> using argument = either<T,function<T>>;
	template<typename T> using rule = function<predicate<T>>;

	template<typename T> using arguments = std::list<argument<T>>;
	template<typename T> using rules = std::list<rule<T>>;

	using term = either<symbol,literal_t>;

	template<typename T> struct maybe {

		typedef T value_type;

		T *x = NULL;

		maybe();
		maybe(const maybe&);
		maybe(const T&);

		~maybe();

		maybe& operator=(const T&);
		maybe& operator=(const maybe&);

		bool operator==(const maybe&);
		bool operator<(const maybe&);

		bool is_set() const;
		bool is_null() const;

		void clear();

		operator const T& () const;
		operator T& ();
	};

	template<typename T> struct function {

		typedef T value_type;

		operator_t op;
		arguments<T> args;
	};

	template<typename T> struct predicate {

		typedef T value_type;

		enum class modifier { push, pop, lift, drop, bind, reject };
		enum class types { end, any, mem, ref, type, op, value };

		either<argument<T>,symbol> arg;
	};

	template<typename T> using _grammar = std::map<symbol,rules<T>>;

	template<typename T> struct grammar : _grammar<T> {

		using _grammar<T>::_grammar;
	};

	template<typename T> struct binding {

		typedef T value_type;

		predicate<T> clause;
		std::list<either<argument<T>,binding<T>>> closure;
	};

	extern template struct predicate<term>;
	extern template struct function<term>;
	extern template struct binding<term>;
	extern template struct grammar<term>;

	extern template struct maybe<symbol>;
	extern template struct maybe<literal_t>;
}
