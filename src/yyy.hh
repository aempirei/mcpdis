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
#include <unordered_map>
#include <unordered_set>
#include <algorithm>

#include <typeinfo>
#include <typeindex>

#include <ansicolor.hh>

#define pluralize(noun) template <typename T> using noun##s = std::list<noun<T>>

namespace yyy {

	//
	// basic types
	//

	using literal_t = uint16_t;
	using operator_t = wchar_t;

	using range = std::pair<size_t,size_t>;

	extern std::unordered_map<std::type_index, const operator_t> typeoperator;
	extern std::unordered_map<std::type_index, const std::wstring> typecolor;

	//
	// forward declarations
	//

	template <typename,typename> struct expr;
	template <typename> struct predicate;
	template <typename> struct grammar;

	template <typename T> using function = expr<operator_t,T>;
	pluralize(function); // functions

	namespace type {
		template <typename...> struct container;
	}

	template <typename T> using argument = typename T::template append<function<T>>;
	pluralize(argument); // arguments

	template <typename T> using closure = expr<predicate<T>,type::container<argument<T>>>;
	pluralize(closure); // closures
}

#include "yyy/operators.hh"

#include "yyy/symbol.template.hh"
#include "yyy/type.template.hh"
#include "yyy/expr.template.hh"

namespace yyy {

	template <typename T> using resultant = std::pair<bool,T>;

	template <typename T> using meta = typename T::template append<symbol::ref>;

	template <typename T> using rule = function<type::container<predicate<T>>>;
	pluralize(rule); // rules

	template <typename T> using matching = std::pair<closure<T>,function<T>>;
}

#undef pluralize

#include "yyy/predicate.hh"
#include "yyy/grammar.hh"

namespace yyy {

	using term = type::container<literal_t,symbol::var>;

	namespace quick {

		using namespace yyy;

		using quick_type = term;

		using F = function<quick_type>;
		using P = predicate<quick_type>;
		using R = rule<quick_type>;
		using A = argument<quick_type>;
		using M = meta<argument<quick_type>>;
		using G = grammar<quick_type>;
		using L = literal_t;
		using S = symbol;
		using O = operator_t;

		constexpr L L8  (L x) { return (uint8_t )x; }
		constexpr L L16 (L x) { return (uint16_t)x; }

		constexpr L L8H (L x) { return (uint8_t )(x >> 8); }
		constexpr L L8L (L x) { return (uint8_t )(x >> 0); }
	}

	extern template struct predicate<quick::quick_type>;
	extern template struct grammar<quick::quick_type>;

	// inline function templates

	template <typename S, typename X> bool contains(const S& s, const X& x) {
		return (s.find(x) not_eq s.end());
	}

	template <typename S, typename X, typename...Xs> bool contains(const S& s, const X& x, Xs...xs) {
		return contains(s,x) and contains(s,xs...);
	}
}
