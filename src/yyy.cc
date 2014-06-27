#include <yyy.hh>

namespace yyy {
	std::map<std::type_index, operator_t> term_typename = {
		{ std::type_index(typeid(literal_t)), L'L' },
		{ std::type_index(typeid(function<term>)), L'F' },
		{ std::type_index(typeid(symbol::var)), L'S' },
		{ std::type_index(typeid(int)), L'i' },
		{ std::type_index(typeid(bool)), L'b' },
		{ std::type_index(typeid(std::wstring)), L's' },
	};
}
