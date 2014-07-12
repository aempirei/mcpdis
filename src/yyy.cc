#include <yyy.hh>

namespace yyy {

	std::unordered_map<std::type_index, const operator_t> typeoperator = {
		{ std::type_index(typeid(void)), L'\u2205' },
		{ std::type_index(typeid(literal_t)), L'L' },
		{ std::type_index(typeid(function<term>)), L'F' },
		{ std::type_index(typeid(symbol::var)), L'S' },
		{ std::type_index(typeid(symbol::ref)), L'R' },

		{ std::type_index(typeid(double)), L'd' },
		{ std::type_index(typeid(float)), L'f' },

		{ std::type_index(typeid(int)), L'i' },
		{ std::type_index(typeid(long)), L'l' },
		{ std::type_index(typeid(bool)), L'b' },

		{ std::type_index(typeid(void *)), L'@' },

		{ std::type_index(typeid(unsigned long)), L'v' },
		{ std::type_index(typeid(unsigned int)), L'u' },

		{ std::type_index(typeid(operator_t)), OP_LAMBDA },
		{ std::type_index(typeid(char)), L'c' },
		{ std::type_index(typeid(unsigned char)), L'y' },

		{ std::type_index(typeid(std::wstring)), L'w' },
		{ std::type_index(typeid(std::string)), L's' },
	};

	std::unordered_map<std::type_index, const std::wstring> typecolor = {
		{ std::type_index(typeid(void)), ANSI_CLR },
		{ std::type_index(typeid(literal_t)), ANSI_LOCYAN },
		{ std::type_index(typeid(function<term>)), ANSI_LOWHITE },
		{ std::type_index(typeid(rule<term>)), ANSI_HIBLACK },
		{ std::type_index(typeid(symbol::var)), ANSI_LOWHITE },
		{ std::type_index(typeid(symbol::ref)), ANSI_LOGREEN },

		{ std::type_index(typeid(void *)), ANSI_CLR },

		{ std::type_index(typeid(double)), ANSI_CLR },
		{ std::type_index(typeid(float)), ANSI_CLR },

		{ std::type_index(typeid(int)), ANSI_CLR },
		{ std::type_index(typeid(long)), ANSI_CLR },
		{ std::type_index(typeid(bool)), ANSI_CLR },

		{ std::type_index(typeid(unsigned long)), ANSI_CLR },
		{ std::type_index(typeid(unsigned int)), ANSI_CLR },

		{ std::type_index(typeid(operator_t)), ANSI_CLR },
		{ std::type_index(typeid(char)), ANSI_CLR },
		{ std::type_index(typeid(unsigned char)), ANSI_CLR },

		{ std::type_index(typeid(std::wstring)), ANSI_CLR },
		{ std::type_index(typeid(std::string)), ANSI_CLR }

	};
}
