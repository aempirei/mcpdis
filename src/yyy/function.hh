#pragma once

#define TCA type::container<Args...>

namespace yyy {

	template <typename...Args> struct function<TCA> {

		// types

		using value_type = type::container<Args...>;

		// variables

		operator_t op;
		arguments<value_type> args;

		// constructors

		function();

		function(operator_t);
		function(const function&);
		function(operator_t, const arguments<value_type>&);

		// operators

		function& operator<<(const argument<value_type>&);

		bool operator==(const function&) const;

		operator const wchar_t * () const;

		template <typename U> function& operator<<(const U&);

		function& operator=(const function&);

		// functions

		std::wstring str() const;
	};

	template <typename...Args> template <typename U> function<TCA>& function<TCA>::operator<<(const U& value) {
		argument<value_type> arg;
		arg.set(value);
		return operator<<(arg);
	}
}

#undef TCA
