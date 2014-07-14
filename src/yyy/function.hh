#pragma once

namespace yyy {

	// this should actually never accept T that are not container
	// maybe static_assert

	template <typename T> struct function {

		static_assert(type::is_container<T>::value, "function expects container<...> as template argument");

		using value_type = T;

		operator_t op;
		arguments<value_type> args;

		function();

		function(operator_t);
		function(const function&);
		function(operator_t, const arguments<value_type>&);

		function& operator<<(const argument<value_type>&);

		template <typename U> function& operator<<(const U&);

		bool operator==(const function&) const;

		std::wstring str() const;
		explicit operator std::wstring() const;
	};

	template <typename T> template <typename U> function<T>& function<T>::operator<<(const U& value) {
		argument<value_type> arg;
		arg.set(value);
		return operator<<(arg);
	}
}
