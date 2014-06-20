#pragma once

namespace yyy {

	template <typename T> struct binding {

		using value_type = T;

		using argument_type = choice<argument<T>,binding<T>>::type;

		predicate<value_type> clause;
		std::list<argument_type> args;

		binding(const predicate<value_type>&);
		binding& operator<<(const argument_type&);
	};
}
