#include <iostream>

#include <yyy.hh>

namespace yyy {


	template <typename T> resultant<closure<T>> grammar<T>::parse(const key_type& k, const function<T>& f) const {

		for(const rule<T>& r : at(k)) {
			auto result = parse(r,f);
			if(result.first)
				return resultant<closure<T>> { true, { predicate<T>(key_type(k)), result.second } };
		}

		return resultant<closure<T>>();
	}

	template <typename T> resultant<bindings<T>> grammar<T>::parse(const rule<T>& r, const function<T>& f) const {

		resultant<bindings<T>> result(true, {});

		//
		// verifiy that f has the same type of operator as r, keeping in mind the special cases of OP_THIS and OP_ANY
		//
		//

		if(r.op != OP_ANY and r.op != OP_THIS and r.op != f.op)
			return resultant<bindings<T>>();

		// just match each predicate against what remains of the function with iterative comutation of df = df - (predicate ~ df)
		//
		//

		function<T> df = f;

		for(const auto& rule_argument : r.args) {

			if(rule_argument.template contains<predicate<T>>()) {

				auto ra_predicate = rule_argument.template get<predicate<T>>();

				auto result_closure = ra_predicate.test(*this, df);

				if(not result_closure.first)
					return resultant<bindings<T>>();

				result.second.push_back(result_closure.second);

			} else {

				std::stringstream ss;
				ss << "rule contains non-predicate arugments: " << r.str().c_str();
				throw std::runtime_error(ss.str());
			}
		}

		return result;
	}

	template struct grammar<term>;
}
