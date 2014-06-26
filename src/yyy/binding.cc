#include <yyy.hh>

namespace yyy {
#if SATANIC
	template <typename T> struct binding {

		using value_type = T;

		using argument_type = choice<argument<T>,binding<T>>::type;

		predicate<value_type> clause;
		std::list<argument_type> args;

		binding(const predicate<value_type>&);
		binding& operator<<(const argument_type&);
	};
#endif
		template <typename T> binding<T>::binding(const predicate<value_type>&my_clause) : clause(my_clause) {
		}

		template <typename T> binding<T>& binding<T>::operator<<(const argument_type&arg) {
			args.push_back(arg);
			return *this;
		}

}
