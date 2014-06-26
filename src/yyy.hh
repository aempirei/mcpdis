#pragma once

#include <cstdint>
#include <climits>
#include <clocale>
#include <cstdlib>
#include <cwchar>

#include <string>
#include <iomanip>
#include <sstream>
#include <list>
#include <map>
#include <set>

#include <typeinfo>

#include <ansicolor.hh>

#include "yyy/symbol.template.hh"
#include "yyy/choice.template.hh"

#define pluralize(noun) template <typename T> using noun##s = std::list<noun<T>>

namespace yyy {

	//
	// basic types
	//

	using literal_t = uint16_t;
	using operator_t = wchar_t;

	using range = std::pair<size_t,size_t>;

}

#include "yyy/operators.hh"

#include "yyy/symbol.template.hh"
#include "yyy/choice.template.hh"

#define pluralize(noun) template <typename T> using noun##s = std::list<noun<T>>

namespace yyy {

	//
	// forward declarations
	//

	template <typename> struct predicate;
	template <typename> struct function;
	template <typename> struct binding;
	template <typename> struct grammar;

	template <typename T> using meta = typename choice<symbol::ref,T>::type;
	template <typename T> using argument = typename choice<function<T>,T>::type;
	template <typename T> using rule = function<predicate<T>>;

	pluralize(argument);	// arguments
	pluralize(rule);	// rules
}

#undef pluralize

#include "yyy/predicate.hh"
#include "yyy/function.hh"
#include "yyy/binding.hh"
#include "yyy/grammar.hh"

namespace yyy {

	using term = choice<symbol::var,literal_t>::type;

	namespace quick {

		using namespace yyy;

		using quick_type = term;

		using F = function<quick_type>;
		using P = predicate<quick_type>;
		using R = rule<quick_type>;
		using A = argument<quick_type>;
		using M = meta<quick_type>;
		using B = binding<quick_type>;
		using G = grammar<quick_type>;

	}

	extern template struct predicate<quick::quick_type>;
	extern template struct function<quick::quick_type>;
	extern template struct function<predicate<quick::quick_type>>;
	extern template struct binding<quick::quick_type>;
	extern template struct grammar<quick::quick_type>;
}
