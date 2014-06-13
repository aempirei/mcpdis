#pragma once

namespace yyy {

	template <typename T> using _grammar = std::map<symbol,rules<T>>;

	template <typename T> struct grammar : _grammar<T> {

		using _grammar<T>::_grammar;
	};
}
