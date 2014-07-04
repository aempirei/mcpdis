#include <yyy.hh>

namespace yyy {

	template <typename T> typename grammar<T>::parse_return_type grammar<T>::parse(const key_type& k, const function<T>& x) {

		for(const rule<T> r : at(k)) {
			auto result = parse(r,x);
			if(result.first)
				return result;
		}

		return parse_return_type(false,{});
	}

	template <typename T> typename grammar<T>::parse_return_type grammar<T>::parse(const rule<T>& r, const function<T>& x) {

		bindings<T> ast;

		for(const auto& argp : r.args) {

			if(argp.template contains_type<predicate<T>>()) {

				auto p = argp.template get<predicate<T>>();
				auto result = p.test(*this, x);

				if(!result.first)
					return parse_return_type(false,{});

				ast.push_back(result.second);

			} else {
				throw std::runtime_error("rule contains nested predicates");
			}
		}

		return parse_return_type(true,ast);
	}

	template struct grammar<term>;
}
