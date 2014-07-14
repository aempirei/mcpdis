#include <yyy.hh>

#define TCA type::container<Args...>

namespace yyy {

	// function<type::container<...>>
	//

	template <typename...Args> function<TCA>::function() : op(OP_ANY) {
	}
	template <typename...Args> function<TCA>::function(operator_t my_op) : op(my_op) {
	}
	template <typename...Args> function<TCA>::function(operator_t my_op, const arguments<value_type>& my_args) : op(my_op), args(my_args) {
	}
	template <typename...Args> function<TCA>::function(const function<value_type>& r) : function(r.op, r.args) {
	}

	// function<type::container<...>>.operator<<
	//

	template <typename...Args> function<TCA>& function<TCA>::operator<<(const argument<value_type>& arg) {
		args.push_back(arg);
		return *this;
	}

	// function<type::container<...>>.str()
	//

	template <typename...Args> std::wstring function<TCA>::str() const {
		std::wstringstream ss;
		ss << type::color<function<TCA>>() << (operator_t)op << '(' << ANSI_CLR;
		for(auto arg : args)
			ss << ' ' << arg.str();
		ss << type::color<function<TCA>>() << " )" << ANSI_CLR;
		return ss.str();
	}

	// function<type::container<...>>.operator==
	//

	template <typename...Args> bool function<TCA>::operator==(const function<TCA>& r) const {
		return op == r.op and args == r.args;
	}

	template <typename...Args> function<TCA>& function<TCA>::operator=(const function<TCA>& r) {
		if(&r != this) {
			op = r.op;
			args = r.args;
		}
		return *this;
	}

	// build these specifically
	//

	template struct function<term>;
	template struct function<type::container<predicate<term>>>;
}
