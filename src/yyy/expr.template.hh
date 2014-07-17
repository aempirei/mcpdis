#pragma once

namespace yyy {

	template <typename T, typename U> using _expr = std::pair<T,std::list<U>>;

	template <typename T, typename U> struct expr : _expr<T,U> {

		using operator_type = T;
		using argument_type = U;
		using arguments_type = std::list<argument_type>;

		using _expr::_expr;

		expr(const operator_type& op) : expr(op, {}) {
		}

		expr& operator<<(const argument_type& arg) {
			second.push_back(arg);
		}

		std::wstring str() const {

			std::wstringstream ss;

			ss << type::color<expr>() << type::value_to_str(first) << '(' << ANSI_CLR;

			/* FIXME: for(const auto& arg : second) */
			for(auto& arg : second)
				ss << ' ' << type::value_to_str(arg);

			ss << type::color<expr>() << " )" << ANSI_CLR;

			return ss.str();

		}
	};
}
