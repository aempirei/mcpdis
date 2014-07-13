#include <yyy.hh>

namespace yyy {

	//
	// function
	//

	template <typename T> function<T>::function() : op(OP_ANY) {
	}
	template <typename T> function<T>::function(operator_t my_op) : op(my_op) {
	}
	template <typename T> function<T>::function(operator_t my_op, const arguments<value_type>& my_args) : op(my_op), args(my_args) {
	}
	template <typename T> function<T>::function(const function<value_type>& r) : function(r.op, r.args) {
	}

	//
	// function.operator<<
	//

	template <typename T> function<T>& function<T>::operator<<(const argument<value_type>& arg) {
		args.push_back(arg);
		return *this;
	}

	//
	// function.operator std::wstring ()
	//

	template <typename T> std::wstring function<T>::str() const {
		std::wstringstream ss;
		ss << (operator_t)op << '(';
		for(auto arg : args)
			ss << ' ' << arg.str();
		ss << " )";
		return ss.str();
	}

	template <typename T> function<T>::operator std::wstring () const {
		return str();
	}

	template <typename T> bool function<T>::operator==(const function<T>& r) const {
		return op == r.op and args == r.args;
	}

	template struct function<term>;
	template struct function<type::container<predicate<term>>>;
}
