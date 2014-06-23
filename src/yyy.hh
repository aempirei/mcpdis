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

#include <operators.hh>

#include "yyy/symbol.template.hh"
#include "yyy/choice.template.hh"

#define pluralize(noun) template <typename T> using noun##s = std::list<noun<T>>

namespace yyy {

	//
	// basic types
	//

	using literal_t = uint16_t;
	using register_t = uint8_t;
	using operator_t = wchar_t;

	using range = std::pair<size_t,size_t>;

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
	template <typename T> using entry = typename grammar<T>::value_type;

	pluralize(argument);
	pluralize(rule);

}

#undef pluralize

#include "yyy/predicate.hh"
#include "yyy/function.hh"
#include "yyy/binding.hh"
#include "yyy/grammar.hh"

namespace yyy {

	using term = choice<symbol::var,literal_t>::type;

	extern template struct predicate<term>;
	extern template struct function<term>;
	extern template struct function<predicate<term>>;
	extern template struct grammar<term>;
	// extern template struct binding<term>;
}
