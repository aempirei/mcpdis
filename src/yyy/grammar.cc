#include <iostream>

#include <yyy.hh>

namespace yyy {


	template <typename T> resultant<closure<T>> grammar<T>::parse(const key_type& k, const function<T>& f) const {

		std::wcout << ANSI_REV << "parse: apply '" << k << "' to " ANSI_CLR << f.str() << std::endl;

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

		if(r.op != OP_ANY and r.op != OP_THIS and r.op != f.op) {
			std::wcout << '\t' << ANSI_REV << "rule op doesn't match function op -- " << r.op << " ~ " << f.op << ANSI_CLR << std::endl;
			return resultant<bindings<T>>();
		}

		// just match each predicate against what remains of the function with iterative comutation of df = df - (predicate ~ df)
		//
		//

		function<T> df = f;

		for(const auto& rule_argument : r.args) {

			if(rule_argument.template contains<predicate<T>>()) {

				auto ra_predicate = rule_argument.template get<predicate<T>>();

				std::wcout << '\t' << ANSI_REV << "testing predicate: " << rule_argument.str() << " -- " << ANSI_CLR;

				auto result_closure = ra_predicate.test(*this, df);

				if(not result_closure.first) {
					std::wcout << ANSI_REV << "test failed" << ANSI_CLR << std::endl;
					return resultant<bindings<T>>();
				}

				result.second.push_back(result_closure.second);

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
