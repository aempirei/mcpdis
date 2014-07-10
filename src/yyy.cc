#include <yyy.hh>

namespace yyy {
	std::unordered_map<std::type_index, const operator_t> typeoperator = {
		{ std::type_index(typeid(void)), L'\u2205' },
		{ std::type_index(typeid(literal_t)), L'L' },
		{ std::type_index(typeid(function<term>)), L'F' },
		{ std::type_index(typeid(symbol::var)), L'S' },
		{ std::type_index(typeid(symbol::ref)), L'R' },
		{ std::type_index(typeid(int)), L'i' },
		{ std::type_index(typeid(bool)), L'b' },
		{ std::type_index(typeid(std::wstring)), L's' },
	};

	std::unordered_map<std::type_index, const std::wstring> typecolor = {
		{ std::type_index(typeid(void)), ANSI_CLR },
		{ std::type_index(typeid(literal_t)), ANSI_LOCYAN },
		{ std::type_index(typeid(function<term>)), ANSI_LOWHITE },
		{ std::type_index(typeid(rule<term>)), ANSI_HIBLACK },
		{ std::type_index(typeid(symbol::var)), ANSI_LOWHITE },
		{ std::type_index(typeid(symbol::ref)), ANSI_LOGREEN },
	};
}
