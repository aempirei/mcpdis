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
#include <unordered_map>
#include <set>

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
	template <typename> struct parser;

	template <typename T> using meta = typename choice<symbol::ref,T>::type;
	template <typename T> using argument = typename choice<function<T>,T>::type;
	template <typename T> using rule = function<predicate<T>>;

	using _hetero_datatype = std::unordered_map<std::type_index,void*>;
	struct hetero_datatype : _hetero_datatype {
		using _hetero_datatype::_hetero_datatype;
		void unassign(const std::type_index&);
		template <typename T> void unassign();
		template <typename T> void assign(const T&);
	};
	template <typename...Args> struct multi_datatype : hetero_datatype {
		using hetero_datatype::hetero_datatype;
		using hetero_datatype::unassign;
		using hetero_datatype::assign;
	};

	pluralize(argument);	// arguments
	pluralize(rule);	// rules
	pluralize(binding);	// bindings
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
		using M = meta<argument<quick_type>>;
		using B = binding<quick_type>;
		using G = grammar<quick_type>;
		using L = literal_t;
		using S = symbol;

		constexpr L L8  (L x) { return (uint8_t )x; }
		constexpr L L16 (L x) { return (uint16_t)x; }

		constexpr L L8H (L x) { return (uint8_t )(x >> 8); }
		constexpr L L8L (L x) { return (uint8_t )(x >> 0); }
	}

	extern template struct predicate <quick::quick_type>;
	extern template struct function <quick::quick_type>;
	extern template struct function <predicate <quick::quick_type> >;
	extern template struct binding <quick::quick_type>;
	extern template struct grammar <quick::quick_type>;

	// inline function templates
	template <typename S, typename X> bool contains(const S& s, const X& x) {
		return (s.find(x) not_eq s.end());
	}

	template <typename S, typename X, typename...Xs> bool contains(const S& s, const X& x, Xs...xs) {
		return contains(s,x) and contains(s,xs...);
	}

	template <typename T> std::type_index typekey(const T&) {
		return std::type_index( contains(typecolor,  std::type_index(typeid(T))) ? typeid(T) : typeid(void) );
	}

	template <typename T> std::wstring colorize(const T& t) {
		std::wstringstream ss;
		ss << typecolor[typekey(t)] << t << ANSI_CLR;
		return ss.str();
	}


}
