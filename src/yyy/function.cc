#include <yyy.hh>

namespace yyy {

	// function
	//
	//

	template <typename T> function<T>::function(const function<value_type>& r) : function(r.op, r.args) {
	}

	template <typename T> function<T>::function(operator_t op0) : function(op0, {}) {
	}

	template <typename T> function<T>::function(operator_t op0, const arguments<value_type>& args0) : op(op0), args(args0) {
	}

	template <typename T> function<T>& function<T>::operator<<(const argument<value_type>& arg) {
		args.push_back(arg);
		return *this;
	}

	// function.operator<<
	//
	//

	template <typename T> function<T>& function<T>::operator<<(const value_type& arg) {
		return operator<<(argument<value_type>(arg));
	}

	template <typename T> function<T>& function<T>::operator<<(const function<value_type>& arg) {
		return operator<<(argument<value_type>(arg));
	}

	// function.operator std::wstring ()
	//
	//

	template <typename T> function<T>::operator std::wstring () const {
		std::wstringstream ss;
		ss << op << L"(";
		for(const auto& arg: args)
			ss << L' ' << (std::wstring)arg;
		ss << L" )";
		return ss.str();
	}

	template struct function<term>;
	template struct function<predicate<term>>;
}
