#include <iostream>

#include <yyy.hh>

namespace yyy {


	template <typename T> resultant<matching<T>> grammar<T>::parse(const key_type& k, const function<T>& f) const {

		for(const rule<T>& r : at(k)) {
			auto result = parse(r,f);
			if(result.first) {
				result.second.first.op = predicate<T>(k);
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

		resultant<matching<T>> result = { true, { closure<T>(), f } };

		matching<T>& m = result.second;
		closure<T>& c = m.first;
		function<T>& df = m.second;

		for(const auto& rule_argument : r.args) {

			if(rule_argument.template contains<predicate<T>>()) {

				const auto& ra_predicate = rule_argument.template get<predicate<T>>();

				auto result_match = ra_predicate.test(*this, df);

				matching<T>& sub_m = result_match.second;
				closure<T>& sub_c = sub_m.first;
				function<T>& sub_f = sub_m.second;

				if(not result_match.first)
					return resultant<matching<T>>();

				c << sub_c;

				df = result_match.second.second;

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
