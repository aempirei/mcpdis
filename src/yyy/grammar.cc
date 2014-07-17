#include <iostream>

#include <yyy.hh>

namespace yyy {


	template <typename T> typename grammar<T>::parse_return_type grammar<T>::parse(const key_type& k, const function<T>& f) const {

		std::wcout << "parse: apply " << k << " to " << f.str() << std::endl;

		for(const rule<T>& r : at(k)) {
			auto result = parse(r,f);
			if(result.first)
				return result;
		}

		return parse_return_type(false,{});
	}

	template <typename T> typename grammar<T>::parse_return_type grammar<T>::parse(const rule<T>& r, const function<T>& f) const {

		bindings<T> ast;

		for(const auto& arg : r.args) {

			if(arg.template contains<predicate<T>>()) {

				auto p = arg.template get<predicate<T>>();

				std::wcout << '\t' << "testing predicate: " << arg.str() << std::endl;

				auto result = p.test(*this, f);

				if(!result.first) {
					std::wcout << '\t' << "test failed" << std::endl;
					return parse_return_type(false,{});
				}

				ast.push_back(result.second);

			} else {
				std::stringstream ss;
				ss << "rule contains non-predicate arugments: " << r.str().c_str();
				throw std::runtime_error(ss.str());
			}
		}

		std::wcout << '\t' << "successful parse" << std::endl;

		return parse_return_type(true,ast);
	}

	template struct grammar<term>;
}
