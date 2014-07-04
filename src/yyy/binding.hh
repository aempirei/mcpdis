#pragma once

namespace yyy {

	template <typename T> struct binding {

		using value_type = T;

		using argument_type = typename choice<argument<value_type>,binding<value_type>>::type;

		predicate<value_type> clause;
		std::list<argument_type> args;

		binding& operator<<(const argument_type&);
	};
}
