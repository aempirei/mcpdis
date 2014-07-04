#include <yyy.hh>

namespace yyy {

	template <typename T> binding<T>::binding() {
	}

	template <typename T> binding<T>::binding(const binding<T>& r) : binding(r.clause, r.args) {
	}

	template <typename T> binding<T>::binding(const predicate<T>& my_clause, const std::list<argument_type>& my_args) : clause(my_clause), args(my_args) {
	}

	template <typename T> binding<T>::binding(const predicate<T>& my_clause) : binding(my_clause, {}) {
	}

	template <typename T> binding<T>& binding<T>::operator=(const binding<T>& r) {
		if(this != &r) {
			clause = r.clause;
			args = r.args;
		}
		return *this;
	}

	template struct binding<term>;
}
