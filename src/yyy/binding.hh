#pragma once

namespace yyy {

	template <typename T> struct binding {

		using value_type = T;

		using argument_type = type::append<binding<value_type>,argument<value_type>>;

		predicate<value_type> clause;
		std::list<argument_type> args;

		binding();
		binding(const binding&);
		binding(const predicate<value_type>&);
		binding(const predicate<value_type>&, const std::list<argument_type>&);

		binding& operator=(const binding&);

		std::wstring str() const;
	};
}
