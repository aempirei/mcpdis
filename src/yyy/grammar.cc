#include <iostream>

#include <yyy.hh>

namespace yyy {


	template <typename T> resultant<closure<T>> grammar<T>::parse(const key_type& k, const function<T>& f) const {

		std::wcout << ANSI_REV << "parse: apply '" << k << "' to " ANSI_CLR << f.str() << std::endl;

		for(const rule<T>& r : at(k)) {
			auto result = parse(r,f);
			if(result.first)
				return resultant<closure<T>>(true, closure<T>(predicate<T>(k), result.second));
		}

		return resultant<closure<T>>();
	}

	template <typename T> resultant<closures<T>> grammar<T>::parse(const rule<T>& r, const function<T>& f) const {

		//
		// verifiy that f has the same type of operator as r, keeping in mind the special cases of OP_THIS and OP_ANY
		//
		//

		if(r.op != OP_ANY and r.op != OP_THIS and r.op != f.op) {
			std::wcout << '\t' << ANSI_REV << "rule op doesn't match function op -- " << r.op << " ~ " << f.op << ANSI_CLR << std::endl;
			return resultant<closures<T>>();
		}

		// just match each predicate against what remains of the function with iterative comutation of df = df - (predicate ~ df)
		//
		//

		function<T> df = f;

		for(const auto& rule_argument : r.args) {

			if(rule_argument.template contains<predicate<T>>()) {

				auto ra_predicate = rule_argument.template get<predicate<T>>();

				std::wcout << '\t' << ANSI_REV << "testing predicate: " << rule_argument.str() << " -- " << ANSI_CLR;

				auto rap_test_result = ra_predicate.test(*this, df);

				if(not rap_test_result.first) {
					std::wcout << ANSI_REV << "test failed" << ANSI_CLR << std::endl;
					return resultant<closures<T>>(false,{});
				}

				result.second.push_back(closure<T>(ra_predicate, rap_test_result.second));

			} else {

				std::stringstream ss;
				ss << "rule contains non-predicate arugments: " << r.str().c_str();
				throw std::runtime_error(ss.str());
			}
		}

		std::wcout << '\t' << ANSI_REV << "successful parse!!!" << ANSI_CLR << std::endl;

		return result;
	}

	template struct grammar<term>;
}
