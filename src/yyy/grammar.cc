#include <yyy.hh>

namespace yyy {

	template <typename T> bool grammar<T>::parse(bindings<T>&ast, const key_type& k, const T& x) {

		if(!contains(*this, k))
			throw std::runtime_error("grammar does not contain expected rule");

		for(const rule<T> r : at(k))
			if(parse(ast, r, x))
				return true;

		return false;
	}

	template <typename T> bool grammar<T>::parse(bindings<T>& , const rule<T>&, const T&) {

		//for(const auto& p : r.args) {
		//}

		return false;
	}

	template struct grammar<term>;
}
