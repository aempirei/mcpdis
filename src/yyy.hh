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

	using symbol = std::wstring;
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

		// typedefs

		using value_type = T;

		enum class types { end, any, mem, by_ref, by_type, by_op, by_value };
		enum class modifiers { push, pop, lift, drop, bind, reject };

		// properties

		types type;
		argument<value_type> arg;
		symbol reference;
		modifiers modifier;
		range quantifier;

		// constructors

		predicate();

		predicate(const predicate&);

		predicate(const symbol&);
		predicate(const argument<value_type>&);

		// type

		predicate end() const;
		predicate any() const;
		predicate mem() const;

		predicate by_ref() const;

		predicate by_op() const;
		predicate by_type() const;
		predicate by_value() const;

		// modifier

		predicate pop() const;
		predicate push() const;
		predicate lift() const;
		predicate drop() const;
		predicate bind() const;
		predicate reject() const;

		// quantifier

		predicate q(const range&) const;

		predicate max(size_t) const;
		predicate min(size_t) const;

		predicate star() const;
		predicate plus() const;
		predicate qm() const;
	};

	template <typename T> using _grammar = std::map<symbol,rules<T>>;

	template <typename T> struct grammar : _grammar<T> {

		using _grammar<T>::_grammar;
	};

	template <typename T> struct binding {

		using value_type = T;

		using argument_type = either<argument<T>,binding<T>>;

		predicate<value_type> clause;
		std::list<argument_type> args;

		binding(const predicate<value_type>&);
		binding& operator<<(const argument_type&);
	};

	// extern template struct predicate<term>;
	// extern template struct function<term>;
	// extern template struct binding<term>;
	// extern template struct grammar<term>;
}
