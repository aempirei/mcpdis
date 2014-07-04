#include <yyy.hh>

namespace yyy {
	template <typename T> binding<T>& binding<T>::operator<<(const argument_type&arg) {
		args.push_back(arg);
		return *this;
	}
}
