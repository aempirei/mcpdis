#pragma once

namespace yyy {

	template <typename T> using _grammar = std::map<symbol::ref,rules<T>>;

	template <typename T> struct grammar : _grammar<T> {

		using _grammar<T>::_grammar;
		using value_type = typename _grammar<T>::value_type;
	};
}
