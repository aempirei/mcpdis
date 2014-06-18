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

#include <operators.hh>

#include <either.template.hh>
#include <maybe.template.hh>

namespace universal {

	using literal_t = uint16_t;
	using register_t = uint8_t;
	using operator_t = wchar_t;

	using range = std::pair<size_t,size_t>;
}

namespace yyy {

	using namespace universal;

	using symbol = std::wstring;

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

	extern template struct predicate<term>;
	extern template struct function<term>;
	extern template struct function<predicate<term>>;
	// extern template struct binding<term>;
	extern template struct grammar<term>;
}

namespace zzz {

	using namespace universal;

	enum struct modifier { push, pop, lift, drop, bind, reject };
	enum struct symbol_type { reference, variable, name, key, id, word };

	//
	// either -- all 16 possible 4-parameter templates over either<either<A,B>,either<C,D>> are covered by
	// the default template either<a,b>, and the two specializations of maybe=either<a,nullptr_t> and nothing=either<nullptr_t,nullptr_t>
	// the specialization of either<nullptr_t,b>!=nothing should generate a static assert or compiler error of some sort
	//

	using null = std::nullptr_t;

	template <typename,typename> struct either;

	//template <typename A> struct either<A, null>;
	template <typename A> using maybe = either<A, null>;

	//template <> struct either<null, null>;
	using nothing = maybe<null>;

	template <typename A, typename B> struct either {
		using a_type = A;
		using b_type = B;
		a_type *a;
		b_type *b;
		either();
		either(const either&);
		~either();
		void clear();
	};

	template <typename A, typename B> either<A,B>::either() : a(nullptr), b(nullptr) {
	}

	template <typename A, typename B> either<A,B>::either(const either&r) :
		a(r.a ? new a_type(*r.a) : nullptr),
		b(r.b ? new b_type(*r.b) : nullptr)
	{
	}

	template <typename A, typename B> either<A,B>::~either() {
		clear();
	}

	template <typename A, typename B> void either<A,B>::clear() {
		if(a) {
			delete a;
			a = nullptr;
		}
		if(b) {
			delete b;
			b = nullptr;
		}
	}

	/*
	template <typename A> struct either<A,null> {
		using a_type = A;
		using b_type = null;
		a_type *a;
		either();
		either(const either&);
		~either();
	};
	*/

	/*
	template <> struct either<null,null> {
		using a_type = null;
		using b_type = null;
	};
	*/

	//
	//
	//

	template <typename> struct argument;

	template <typename,symbol_type> struct basic_symbol;

#define DEFINE_SYMBOL_TYPE(X) using X = basic_symbol<wchar_t,type::X>

	struct symbol {
		using type = symbol_type;
		DEFINE_SYMBOL_TYPE ( reference );
		DEFINE_SYMBOL_TYPE ( variable  );
		DEFINE_SYMBOL_TYPE ( name      );
		DEFINE_SYMBOL_TYPE ( key       );
		DEFINE_SYMBOL_TYPE ( id        );
		DEFINE_SYMBOL_TYPE ( word      );
	};

#undef DEFINE_SYMBOL_TYPE

	using modifiers = std::set<modifier>;
	using adjectives = std::pair<modifiers,range>;
	using reference = symbol::reference;
	using term = either<literal_t,symbol::variable>;

	template <typename T> using arguments = std::list<argument<T>>;
	template <typename T> using function = std::pair<operator_t,arguments<T>>;
	template <typename T> using meta = either<reference,T>;
	template <typename T> using predicate = std::pair<adjectives,meta<argument<T>>>;
	template <typename T> using rule = function<predicate<T>>;
	template <typename T> using rules = std::list<rule<T>>;
	template <typename T> using grammar = std::map<symbol::key,rules<T>>;
	template <typename T> using entry = typename grammar<T>::value_type;
	template <typename T> using binding = std::pair<predicate<T>,arguments<T>>;

	template <typename T, symbol_type S> struct basic_symbol : std::basic_string<T> {
		static constexpr decltype(S) type_enum = S;
		using unit_type = T;
		using base_type = std::basic_string<unit_type>;
		using base_type::base_type;
	};

	template <typename T> struct argument : either<T,function<T>> {
		using unit_type = T;
		using base_type = either<T,function<T>>;
		using base_type::base_type;
	};
}
