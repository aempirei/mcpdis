#include <yyy.hh>

namespace yyy {

	//
	// function
	//

	template <typename T> function<T>::function() : function(OP_ANY) {
	}

	template <typename T> function<T>::function(const function<value_type>& r) : function(r.op, r.args) {
	}

	template <typename T> function<T>::function(operator_t op0) : function(op0, {}) {
	}

	template <typename T> function<T>::function(operator_t op0, const arguments<value_type>& args0) : op(op0), args(args0) {
	}

	//
	// function.operator<<
	//

	template <typename T> function<T>& function<T>::operator<<(const argument<value_type>& arg) {
		args.push_back(arg);
		return *this;
	}

	template <typename T> function<T>& function<T>::operator<<(const value_type& value) {
		argument<value_type> arg;
		arg.assign(value);
		return operator<<(arg);
	}

	template <typename T> function<T>& function<T>::operator<<(const function<value_type>& fn) {
		argument<value_type> arg;
		arg.assign(fn);
		return operator<<(arg);
	}

	//
	// function.operator std::wstring ()
	//

	template <typename T> std::wstring function<T>::str() const {
		std::wstringstream ss;
		ss << op << L"(";
		for(const auto& arg: args)
			ss << L' ' << arg.str();
		ss << L" )";
		return ss.str();
	}

	template <typename T> function<T>::operator std::wstring () const {
		return str();
	}

	template struct function<term>;
	template struct function<predicate<term>>;
}
