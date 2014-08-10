#pragma once

namespace yyy {

	template <typename T> using transformation_function_type = function<T> (*)(const matching<T>&);

	template <typename T> using grammar_value_type = std::pair<rules<T>,transformation_function_type<T>>;

	template <typename T> using _grammar = std::map<symbol::ref,grammar_value_type<T>>;

	template <typename T> struct grammar : _grammar<T> {

		using _grammar<T>::_grammar;
		using _grammar<T>::at;
		using _grammar<T>::operator[];

		using value_type = typename _grammar<T>::value_type;
		using mapped_type = typename _grammar<T>::mapped_type;
		using key_type = typename _grammar<T>::key_type;

		resultant<matching<T>> parse(const key_type&, const function<T>&) const;
		resultant<matching<T>> parse(const rule<T>&, const function<T>&) const;

		static function<T> identity_transformation(const matching<T>&) {
			return function<T>();
		};

		static function<T> null_transformation(const matching<T>&) {
			return function<T>();
		}
	};
}
