#include <iostream>

#include <yyy.hh>

namespace yyy {


	template <typename T> resultant<matching<T>> grammar<T>::parse(const key_type& k, const function<T>& f) const {

		for(const rule<T>& r : at(k)) {
			auto result = parse(r,f);
			if(result.first) {
				result.second.first.first = (predicate<T>(k)); // FIXME: remove key_type() cast
				return result;
			}
		}

		return resultant<matching<T>>();
	}

	template <typename T> resultant<matching<T>> grammar<T>::parse(const rule<T>& r, const function<T>& f) const {

		// verifiy that f has the same type of operator as r, keeping in mind the special cases of OP_THIS and OP_ANY
		//
		//

		if(r.op != OP_ANY and r.op != OP_THIS and r.op != f.op)
			return resultant<matching<T>>();

		// just match each predicate against what remains of the function with iterative comutation of df = df - (predicate ~ df)
		//
		//

		resultant<matching<T>> result;

		result.first = true;
		result.second.second = f;

		for(const auto& rule_argument : r.args) {

			if(rule_argument.template contains<predicate<T>>()) {

				auto& ra_predicate = rule_argument.template get<predicate<T>>(); // FIXME: make const

				auto result_match = ra_predicate.test(*this, result.second.second);

				if(not result_match.first)
					return resultant<matching<T>>();

				result.second.second = result_match.second.second;
				result.second.first.second.push_back(result_match.second.first);

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
