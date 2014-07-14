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

	template <typename T> binding<T>::operator const wchar_t * () const {
		return str().c_str();
	}

	template <typename T> std::wstring binding<T>::str() const {

		std::wstringstream ss;

		ss << clause.str() << L" =>";
		for(const auto& arg : args)
			ss << L' ' << arg.str();

		return ss.str();
	}

	template struct binding<term>;
}
