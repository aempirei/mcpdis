#pragma once

namespace yyy {

	template <typename T> struct binding {

		// types
		//

		using value_type = T;

		using argument_type = closure<T>;

		// variables
		//

		predicate<value_type> clause;
		std::list<argument_type> args;

		// constructors
		//

		binding();
		binding(const binding&);
		binding(const predicate<value_type>&);
		binding(const predicate<value_type>&, const std::list<argument_type>&);

		// operators
		//

		binding& operator=(const binding&);

		// string conversion
		//

		std::wstring str() const;
	};
}
