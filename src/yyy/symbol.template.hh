#pragma once

#define DEFINE_SYMBOL_TYPE(X) using X = basic_symbol<wchar_t,type::X>

namespace yyy {

	//
	// basic_symbol, symbol, symbol_type
	//

	enum struct symbol_type { ref, var, name, key, id, word };

	template <typename,symbol_type> struct basic_symbol;

	struct symbol {
		symbol() = delete;
		using type = symbol_type;
		DEFINE_SYMBOL_TYPE ( ref  );
		DEFINE_SYMBOL_TYPE ( var  );
		DEFINE_SYMBOL_TYPE ( name );
		DEFINE_SYMBOL_TYPE ( key  );
		DEFINE_SYMBOL_TYPE ( id   );
		DEFINE_SYMBOL_TYPE ( word );
	};

	template <typename T, symbol_type S> struct basic_symbol : std::basic_string<T> {
		static constexpr decltype(S) type_enum = S;
		using unit_type = T;
		using base_type = std::basic_string<unit_type>;
		using base_type::base_type;
		basic_symbol(const base_type&s) : base_type(s.begin(), s.end()) {
		}
		basic_symbol() : base_type() {
		}
	};
}

#undef DEFINE_SYMBOL_TYPE
