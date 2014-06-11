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
#include <yyy.hh>

namespace yyy {

	template <typename T> function<T>::function(const function<value_type>& r) : function(r.op, r.args) {
	}

	template <typename T> function<T>::function(operator_t op0) : function(op0, {}) {
	}

	template <typename T> function<T>::function(operator_t op0, const arguments<value_type>& args0) : op(op0), args(args0) {
	}

	template <typename T> function<T>& function<T>::operator<<(const argument<value_type>& arg) {
		args.push_back(arg);
		return *this;
	}

	template <typename T> function<T>& function<T>::operator<<(const value_type& arg) {
		return operator<<(argument<value_type>(arg));
	}

	template <typename T> function<T>& function<T>::operator<<(const function<value_type>& arg) {
		return operator<<(argument<value_type>(arg));
	}

	template <typename T> template <typename U> function<T>& function<T>::operator<<(const U& arg) {
		return operator<<(argument<value_type>(U(arg)));
	}

	template <typename T> function<T>::operator std::wstring () const {
		std::wstringstream ss;
		ss << op << '(';
		for(const auto& arg: args) {
			if(arg.template has<literal_t>()) {
				ss << (int)*arg.template ptr_to<literal_t>();
			} else if(arg.template has<symbol>()) {
				ss << " \"" << *arg.template ptr_to<symbol>() << '"';
			} else if(arg.template has<function<term>>()) {
				ss << ' ' << (std::wstring)*arg.template ptr_to<function<term>>();
			} else {
				ss << " <UNKNOWN>";
			}
		}
		ss << ')';
		return ss.str();
	}

	/*
	template <typename X> struct predicate {
		typedef X value_type;
		enum class modifiers { push, pop, lift, drop, bind, reject };
		enum class types { end, any, mem, ref, type, op, value };
		modifiers modifier;
		types type;
		either<argument<X>,symbol> arg;
	};

	template <typename X> using _grammar = std::map<symbol,rules<X>>;

	template <typename X> struct grammar : _grammar<X> {
		using _grammar<X>::_grammar;
	};

	template <typename X> using bound_argument = either<argument<X>,binding<X>>;

	template <typename X> struct binding {
		using value_type = X;
		predicate<X> clause;
		std::list<bound_argument<X>> closure;
		binding(const predicate<X>&);
		binding& operator<<(const bound_argument<X>&);
	};
	*/

	template <typename T> void define_grammar(grammar<T>&) {
	}
}

using namespace yyy;

int main(int, char **) {

	maybe<literal_t> lit;

	function<term> f = function<term>(OP_AND) << 666 << symbol(L"what");
	f << f;
	f << f;

	std::wcout << (std::wstring)f << std::endl;

	grammar<term> g;

	define_grammar(g);
	return 0;
}
