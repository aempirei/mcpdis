#pragma once

namespace yyy {

	template <typename T, typename U> using _expr = std::pair<T,std::list<U>>;

	template <typename T, typename U> struct expr : _expr<T,U> {

		using operator_type = T;
		using argument_type = U;
		using arguments_type = std::list<argument_type>;

		using _expr<T,U>::_expr;

		expr(const operator_type& op) : expr(op, {}) {
		}

		expr& operator<<(const argument_type& arg) {
			this->second.push_back(arg);
		}

		std::wstring str() const {

			std::wstringstream ss;

			ss << type::color<expr>() << type::value_to_str<operator_type>::call(this->first) << '(' << ANSI_CLR;

			for(const auto& arg : this->second)
				ss << ' ' << type::value_to_str<argument_type>::call(arg);

			ss << type::color<expr>() << " )" << ANSI_CLR;

			return ss.str();

		}
	};
}
