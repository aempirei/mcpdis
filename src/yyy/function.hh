#pragma once

namespace yyy {

	// this should actually never accept T that are not container
	// maybe static_assert

	template <typename...Args> struct function<type::container<Args...>> {

		using value_type = type::container<Args...>;

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

	template <typename...Args> template <typename U> function<type::container<Args...>>& function<type::container<Args...>>::operator<<(const U& value) {
		argument<value_type> arg;
		arg.set(value);
		return operator<<(arg);
	}
}
