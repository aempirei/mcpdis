#include <iostream>

#include <yyy.hh>

namespace yyy {


	template <typename T> std::pair<bool,closures<T>> grammar<T>::parse(const key_type& k, const function<T>& f) const {

		std::wcout << "parse: apply " << k << " to " << f.str() << std::endl;

		for(const rule<T>& r : at(k)) {
			auto result = parse(r,f);
			if(result.first)
				return result;
		}

		return resultant<closures<T>>(false,{});
	}

	template <typename T> std::pair<bool,closures<T>> grammar<T>::parse(const rule<T>& r, const function<T>& f) const {

		closures<T> ast;

		// verifiy that f has the same type of operator as r, keeping in mind the special cases of OP_THIS and OP_ANY

		if(r.op != OP_ANY and r.op != OP_THIS and r.op != f.op) {
			std::wcout << '\t' << "rule op doesn't match function op -- " << r.op << " ~ " << f.op << std::endl;
			return resultant<closures<T>>(false,{});
		}

		for(const auto& rule_argument : r.args) {

			if(rule_argument.template contains<predicate<T>>()) {

				auto rule_argument_predicate = rule_argument.template get<predicate<T>>();

				std::wcout << '\t' << "testing predicate: " << rule_argument.str() << std::endl;

				auto predicate_test_result = rule_argument_predicate.test(*this, f);

				if(not predicate_test_result.first) {
					std::wcout << '\t' << "test failed" << std::endl;
					return resultant<closures<T>>(false,{});
				}

				ast.push_back(closure<T>(rule_argument_predicate, predicate_test_result.second));

			} else {

				std::stringstream ss;
				ss << "rule contains non-predicate arugments: " << r.str().c_str();
				throw std::runtime_error(ss.str());
			}
		}

		std::wcout << '\t' << "successful parse!!!" << std::endl;

		return resultant<closures<T>>(true, ast);
	}

	template struct grammar<term>;
}
