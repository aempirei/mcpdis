#pragma once

namespace yyy {

	template <typename T> struct function {

		using value_type = T;

		operator_t op;
		arguments<value_type> args;

		function();

		function(operator_t);
		function(const function&);
		function(operator_t, const arguments<value_type>&);

		function& operator<<(const argument<value_type>&);
		function& operator<<(const function<value_type>&);
		function& operator<<(const value_type&);

		template <typename U> function& operator<<(const U&);

		explicit operator std::wstring() const;
	};

	template <typename T> template <typename U> function<T>& function<T>::operator<<(const U& arg) {
		return operator<<(argument<value_type>(U(arg)));
	}
}