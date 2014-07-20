#pragma once

namespace yyy {

	template <typename T, typename U> struct expr {

		using head_type = T;
		using argument_type = typename U::template append<expr>;
		using tail_type = std::list<argument_type>;

		head_type op;
		tail_type args;

		expr() : op(), args() {
		}

		expr(const head_type& my_op) : op(my_op), args({}) {
		}

		expr(const head_type& my_op, const tail_type& my_args) : op(my_op), args(my_args) {
		}

		expr(const expr& r) : op(r.op), args(r.args) {
		}

		expr& operator=(const expr& r) {
			if(this != &r) {
				op = r.op;
				args = r.args;
			}
			return *this;
		}

		expr& operator<<(const argument_type& arg) {
			args.push_back(arg);
			return *this;
		}

		expr& operator<<(const expr& x) {
			args.push_back(argument_type(x));
			return *this;
		}

		template <typename X> expr& operator<<(const X& x) {
			args.push_back(argument_type(x));
			return *this;
		}

		bool operator==(const expr& r) const {
			return op == r.op and args == r.args;
		}

		bool operator!=(const expr& r) const {
			return not operator==(r);
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
