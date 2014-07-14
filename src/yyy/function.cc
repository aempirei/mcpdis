#include <yyy.hh>

namespace yyy {

	//
	// function
	//

	template <typename...Args> function<type::container<Args...>>::function() : op(OP_ANY) {
	}
	template <typename...Args> function<type::container<Args...>>::function(operator_t my_op) : op(my_op) {
	}
	template <typename...Args> function<type::container<Args...>>::function(operator_t my_op, const arguments<value_type>& my_args) : op(my_op), args(my_args) {
	}
	template <typename...Args> function<type::container<Args...>>::function(const function<value_type>& r) : function(r.op, r.args) {
	}

	//
	// function.operator<<
	//

	template <typename...Args> function<type::container<Args...>>& function<type::container<Args...>>::operator<<(const argument<value_type>& arg) {
		args.push_back(arg);
		return *this;
	}

	//
	// function.operator std::wstring ()
	//

	template <typename...Args> std::wstring function<type::container<Args...>>::str() const {
		std::wstringstream ss;
		ss << (operator_t)op << '(';
		for(auto arg : args)
			ss << ' ' << arg.str();
		ss << " )";
		return ss.str();
	}

	template <typename...Args> function<type::container<Args...>>::operator std::wstring () const {
		return str();
	}

	template <typename...Args> bool function<type::container<Args...>>::operator==(const function<type::container<Args...>>& r) const {
		return op == r.op and args == r.args;
	}

	template struct function<term>;
	template struct function<type::container<predicate<term>>>;
}
