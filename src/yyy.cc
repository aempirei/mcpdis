#include <cstdint>
#include <cstdlib>
#include <cwchar>

#include <string>
#include <vector>
#include <list>
#include <map>
#include <set>
#include <initializer_list>

#include <yyy.hh>

namespace yyy {

	/*
	template<typename X> struct function {

		typedef X value_type;

		operator_t op;
		arguments<X> args;
	};
	*/

	/*
	template<typename X> struct predicate {

		typedef X value_type;

		enum class modifier { push, pop, lift, drop, bind, reject };
		enum class types { end, any, mem, ref, type, op, value };

		either<argument<X>,symbol> arg;
	};
	*/

	/*
	template<typename X> struct grammar : _grammar<X> {

		using _grammar<X>::_grammar;
	};
	*/

	/*
	template<typename X> struct binding {

		typedef X value_type;

		predicate<X> clause;
		std::list<either<argument<X>,binding<X>>> closure;
	};
	*/
}

int main(int, char **) {
	return 0;
}
