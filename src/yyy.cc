#include <yyy.hh>

namespace yyy {

	std::unordered_map<std::type_index, const operator_t> typeoperator = {

		{ std::type_index(typeid(void)), L'\u2205' },

		{ std::type_index(typeid(literal_t)), L'L' },
		{ std::type_index(typeid(symbol::var)), L'S' },

		{ std::type_index(typeid(function<term>)), L'F' },
		{ std::type_index(typeid(predicate<term>)), L'P' },
		{ std::type_index(typeid(rule<term>)), L'R' },

		{ std::type_index(typeid(double)), L'd' },
		{ std::type_index(typeid(float)), L'f' },

		{ std::type_index(typeid(int)), L'i' },
		{ std::type_index(typeid(long)), L'l' },
		{ std::type_index(typeid(bool)), L'b' },
		{ std::type_index(typeid(long long)), L'n' },

		{ std::type_index(typeid(void *)), L'@' },

		{ std::type_index(typeid(unsigned long)), L'v' },
		{ std::type_index(typeid(unsigned int)), L'u' },

		{ std::type_index(typeid(operator_t)), OP_LAMBDA },
		{ std::type_index(typeid(char)), L'c' },
		{ std::type_index(typeid(unsigned char)), L'y' },

		{ std::type_index(typeid(std::wstring)), L'w' },
		{ std::type_index(typeid(std::string)), L's' },
	};
}
