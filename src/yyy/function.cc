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

	template <typename T> function<T>::operator std::wstring () const {
		std::wstringstream ss;
		ss << op << '(';
		for(const auto& arg: args) {
			if(arg.template has<literal_t>()) {
				ss << (int)*arg.template ptr_to<literal_t>();
			} else if(arg.template has<symbol>()) {
				ss << " \"" << *arg.template ptr_to<symbol>() << '"';
			} else if(arg.template has<function<term>>()) {
				ss << ' ' << (std::wstring)*arg.template ptr_to<function<term>>();
			} else {
				ss << " <UNKNOWN>";
			}
		}
		ss << ')';
		return ss.str();
	}

	template struct function<term>;
}
