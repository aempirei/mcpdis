#include <yyy.hh>

namespace yyy {
#if SATANIC
	template <typename T> using _grammar = std::map<symbol,rules<T>>;

	template <typename T> struct grammar : _grammar<T> {
		using _grammar<T>::_grammar;
	};
#endif
}
