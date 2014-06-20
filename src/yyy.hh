#pragma once

#include <cstdint>
#include <climits>
#include <clocale>
#include <cstdlib>
#include <cwchar>

#include <string>
#include <list>
#include <map>
#include <set>

#include <typeinfo>

#include <operators.hh>

namespace yyy {

	//
	// basic types
	//

	using literal_t = uint16_t;
	using register_t = uint8_t;
	using operator_t = wchar_t;

	using range = std::pair<size_t,size_t>;

}

#include "yyy/choice.template.hh"
#include "yyy/symbol.template.hh"

namespace yyy {

	//
	// forward declarations
	//

	template <typename> struct predicate;
	template <typename> struct function;
	template <typename> struct binding;
	template <typename> struct grammar;

	template <typename T> using argument = choice<T,function<T>>::type;
	template <typename T> using rule = function<predicate<T>>;

	template <typename T> using arguments = std::list<argument<T>>;
	template <typename T> using rules = std::list<rule<T>>;

	using term = choice<symbol::generic,literal_t>::type;
}

#include "yyy/predicate.hh"
#include "yyy/function.hh"
#include "yyy/binding.hh"
#include "yyy/grammar.hh"

namespace yyy {

	extern template struct predicate<term>;
	extern template struct function<term>;
	extern template struct function<predicate<term>>;
	// extern template struct binding<term>;
	extern template struct grammar<term>;

}

namespace zzz {
	/*
	   template <typename> struct argument;

	   using modifiers = std::set<modifier>;
	   using adjectives = std::pair<modifiers,range>;
	   using reference = symbol::reference;
	   using term = either<maybe<literal_t>,maybe<symbol::variable>>;

	   template <typename T> using arguments = std::list<argument<T>>;
	   template <typename T> using function = std::pair<operator_t,arguments<T>>;
	   template <typename T> using meta = either<maybe<reference>,T>;
	   template <typename T> using predicate = std::pair<adjectives,meta<argument<T>>>;
	   template <typename T> using rule = function<predicate<T>>;
	   template <typename T> using rules = std::list<rule<T>>;
	   template <typename T> using grammar = std::map<symbol::key,rules<T>>;
	   template <typename T> using entry = typename grammar<T>::value_type;
	   template <typename T> using binding = std::pair<predicate<T>,arguments<T>>;

	   template <typename T> struct argument : either<T,maybe<function<T>>> {
	   using unit_type = T;
	   using base_type = either<T,maybe<function<T>>>;
	   using base_type::base_type;
	   };
	 */
}
