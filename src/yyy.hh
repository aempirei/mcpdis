#pragma once

#include <cstdint>
#include <clocale>
#include <cstdlib>
#include <cwchar>

#include <string>
#include <list>
#include <map>

#include <operators.hh>

#include <either.template.hh>
#include <maybe.template.hh>

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

}

#include "yyy/predicate.hh"
#include "yyy/function.hh"
#include "yyy/binding.hh"
#include "yyy/grammar.hh"

namespace yyy {

	// extern template struct predicate<term>;
	extern template struct function<term>;
	// extern template struct binding<term>;
	// extern template struct grammar<term>;
}
