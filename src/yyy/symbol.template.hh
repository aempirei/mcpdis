#pragma once

namespace yyy {

	//
	// basic_symbol, symbol, symbol_type
	//

	enum struct symbol_type { ref, var, name, key, id, word };

	template <typename,symbol_type> struct basic_symbol;

	template <typename T> struct symbol_typedefs {

		symbol_typedefs() = delete;

		using type = symbol_type;

		using ref  = basic_symbol<wchar_t,type::ref>;
		using var  = basic_symbol<wchar_t,type::var>;
		using name = basic_symbol<wchar_t,type::name>;
		using key  = basic_symbol<wchar_t,type::key>;
		using id   = basic_symbol<wchar_t,type::id>;
		using word = basic_symbol<wchar_t,type::word>;
	};

	// using symbol = symbol_typedefs<char>;
	using symbol  = symbol_typedefs<wchar_t>;
	using wsymbol = symbol_typedefs<wchar_t>;

	template <typename T, symbol_type S> struct basic_symbol : std::basic_string<T> {

		static constexpr decltype(S) type_enum = S;

		using unit_type = T;
		using base_type = std::basic_string<unit_type>;
		using base_type::base_type;
	};
}
