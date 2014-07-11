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

	//
	// unrestricted_type
	//

	template <typename T> bool unrestricted_type::contains() const {
		return data.find(key<T>()) != data.end();
	}

	template <typename T> void unrestricted_type::unassign() {
		if(contains<T>()) {
			delete (T*)data[key<T>()];
			data.erase(key<T>());
		}
	}

	template <typename T> void unrestricted_type::assign(const T& t) {
		unassign<T>();
		data[key<T>()] = (void *)new T(t);
	}

	template <typename T> T& unrestricted_type::get() const {
		return *(T*)data[key<T>()];
	}

	template <typename T> std::type_index unrestricted_type::key() const {
		return std::type_index(typeid(T));
	}

	//
	// restricted_type
	//

	template <typename...Args> struct restricted_type : unrestricted_type {
		using unrestricted_type::unrestricted_type;
		using unrestricted_type::unassign;
		using unrestricted_type::assign;
		using unrestricted_type::get;
		using unrestricted_type::key;
		using unrestricted_type::contains;
	};
}
