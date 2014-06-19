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

	template <typename,typename> struct either;

	template <typename A> struct either<A,void>;
	template <typename A> using maybe = either<A,void>;

	struct either<void,void>;
	using nothing = maybe<void>;

	//
	// nothing
	//

	template <> struct either<void,void> {
		void clear() const {
		}
		std::wstring str() const {
			return L"()";
		}
		template <typename> constexpr bool contains_type() const {
			return false;
		}
		template <typename> constexpr bool allows_type() const {
			return false;
		}
		template <typename T> constexpr bool contains_value(const T&) const {
			return false;
		}
	};

	//
	//  native type or nothing
	//

	template <typename A,typename T> struct either_typing {
		using either_type = either<A,void>;
		using value_type = A;
		using test_type = T;
		static const bool allows_type = false;
		static bool contains_type(const either_type&) {
			return false;
		}
		static bool contains_value(const either_type&, const test_type&) {
			return false;
		}
	};

	template <typename A> struct either_typing<A,A> {
		using either_type = either<A,void>;
		using value_type = A;
		using test_type = A;
		static const bool allows_type = true;
		static bool contains_type(const either_type& x) {
			return x.a_ptr not_eq nullptr;
		}
		static bool contains_value(const either_type& x, const test_type& a) {
			return contains_type(x) and ( a == *x.a_ptr );
		}

	};

	template <typename A> struct either<A,void> {

		using a_type = A;

		a_type *a_ptr;

		constexpr either() : a_ptr(nullptr) {
		}

		either(const either& r) : a_ptr(r.a_ptr ? new a_type(*r.a_ptr) : nullptr) {
		}

		either(const a_type& a) : a_ptr(new a_type(a)) {
		}

		void clear() {
			if(a_ptr) {
				delete a_ptr;
				a_ptr = nullptr;
			}
		}

		void insert(const a_type& a) {
			if(a_ptr == nullptr)
				a_ptr = new a_type();
			*a_ptr = a;
		}

		template <typename T> void insert(const T&) {
			std::stringstream ss;
			ss << "insert of " << typeid(T).name() << " to " << typeid(*this).name() << " not possible.";
			throw std::runtime_error(ss.str());
		}

		template <typename T> void assign(const T& t) {
			clear();
			insert(t);
		}

		~either() {
			clear();
		} 

		std::wstring str() const {
			if(a_ptr) {
				std::wstringstream ss;
				ss << *a_ptr;
				return ss.str();
			} else {
				return nothing().str();
			}
		}

		template <typename T> using typing = either_typing<A,T>;

		template <typename T> constexpr bool contains_type() const {
			return typing<T>::contains_type(*this);
		}
		template <typename T> constexpr bool allows_type() const {
			return typing<T>::allows_type;
		}
		template <typename T> constexpr bool contains_value(const T& x) const {
			return typing<T>::contains_value(*this,x);
		}
	};
	//
	// recursive either<either,either>
	//

	template <typename A,typename B,typename C,typename D> struct either<either<A,B>,either<C,D>> {

		using a_type = either<A,B>;
		using b_type = either<C,D>;

		a_type *a_ptr = nullptr;
		b_type *b_ptr = nullptr;

		either() {
		}

		either(const either& r) {
			if(r.a_ptr)
				a_ptr = new a_type(*r.a_ptr);
			if(r.b_ptr)
				b_ptr = new b_type(*r.b_ptr);
		}

		void clear() {
			if(a_ptr) {
				delete a_ptr;
				a_ptr = nullptr;
			}
			if(b_ptr) {
				delete b_ptr;
				b_ptr = nullptr;
			}
		}

		std::wstring str() const {
			if(a_ptr && b_ptr) {
				return a_ptr->str() + L',' + b_ptr->str();
			} else if(a_ptr) {
				return a_ptr->str();
			} else if(b_ptr) {
				return b_ptr->str();
			} else {
				return nothing().str();
			}
		}

		~either() {
			clear();
		}

		template <typename T> void insert(const T& t) {

			if( a_type().template allows_type<T>() ) {

				if(a_ptr == nullptr)
					a_ptr = new a_type();
				a_ptr->insert(t);

			} else if( b_type().template allows_type<T>() ) {

				if(b_ptr == nullptr)
					b_ptr = new b_type();

				b_ptr->insert(t);

			} else {
				std::stringstream ss;
				ss << "insert of " << typeid(T).name() << " to " << typeid(*this).name() << " not possible.";
				throw std::runtime_error(ss.str());
			}

		}

		template <typename T> void assign(const T& t) {
			clear();
			insert(t);
		}

		template <typename T> constexpr bool contains_type() const {
			return ( a_ptr && a_ptr->template contains_type<T>() )
				or ( b_ptr && b_ptr->template contains_type<T>() );
		}

		template <typename T> constexpr bool allows_type() const {
			return ( a_type().template allows_type<T>() )
				or ( b_type().template allows_type<T>() );
		}

		template <typename T> constexpr bool contains_value(const T& t) const {
			return ( a_ptr && a_ptr->template contains_value<T>(t) )
				or ( b_ptr && b_ptr->template contains_value<T>(t) ); 

		}
	};

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
