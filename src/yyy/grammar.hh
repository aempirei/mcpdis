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

		resultant<closures<T>> parse(const key_type&, const function<T>&) const;
		resultant<closures<T>> parse(const rule<T>&, const function<T>&) const;
	};
}
