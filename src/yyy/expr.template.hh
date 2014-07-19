#pragma once

namespace yyy {

	template <typename T, typename U> using _expr = std::pair<T,std::list<U>>;

	template <typename T, typename U> struct expr : _expr<T,U> {

		using head_type = T;
		using argument_type = U;
		using tail_type = std::list<U>;

		using _expr<T,U>::_expr;

		expr(const head_type& head) : expr(head, {}) {
		}

		expr& operator<<(const argument_type& arg) {
			this->second.push_back(arg);
			return *this;
		}

		std::wstring str() const {

			std::wstringstream ss;

			ss << "( " << type::value_to_str<head_type>::call(this->first) << " ~ ";

			for(const auto& arg : this->second)
				ss << type::value_to_str<argument_type>::call(arg) << ' ';

			ss << ')';

			return ss.str();

		}
	};
}
