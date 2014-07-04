#pragma once

namespace yyy {

	template <typename T> using _grammar = std::map<symbol::ref,rules<T>>;

	template <typename T> struct grammar : _grammar<T> {

		using _grammar<T>::_grammar;
		using _grammar<T>::at;
		using _grammar<T>::operator[];

		using value_type = typename _grammar<T>::value_type;
		using mapped_type = typename _grammar<T>::mapped_type;
		using key_type = typename _grammar<T>::key_type;

		bool parse(bindings<T>&, const key_type&, const T&);
		bool parse(bindings<T>&, const rule<T>&, const T&);
	};
}
