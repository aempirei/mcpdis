#include <algorithm>
#include <sstream>
#include <iomanip>

#include <cstdlib>
#include <cstdint>
#include <climits>

#include <mcpdis.hh>

template<typename T> fn<T>::fn() : fn(OP_LIST, {}) {
}

template<typename T> fn<T>::fn(op_t my_op) : fn(my_op, {}) {
}

template<typename T> fn<T>::fn(op_t my_op, const args_type& my_args) : op(my_op), args(my_args) {
}

template<typename T> fn<T>::fn(const fn&r) : fn(r.op, r.args) {
}

template<typename T> fn<T>& fn<T>::operator=(const fn& r) {

	if(this != &r) {
		op = r.op;
		args = args_type(r.args);
	}

	return *this;
}

template<typename T> bool fn<T>::operator==(const fn& r) const {
	return op == r.op && args == r.args;
}

template<typename T> bool fn<T>::operator<(const fn& r) const {

	if(op != r.op)
		return op < r.op;

	return args < r.args;
}

template<typename T> fn<T>& fn<T>::operator<<(const T& x) {
	args.push_back(x);
	return *this;
}

template<typename T> size_t fn<T>::arity() const {
	return args.size();
}

template<typename T> void fn<T>::clear() {
	op = OP_LIST;
	args.clear();
}

template<typename T> void fn<T>::concat(const args_type& xs) {
	args.insert(args.end(), xs.begin(), xs.end());
}

template<typename T> fn<T>::operator std::wstring () const {
	std::wstringstream ss;

	if(op != OP_LIST)
		ss << '(' << ANSI_HIYELLOW << op << ANSI_CLR << ") ";

	ss << L"{ ";

	for(const auto& arg : args)
		ss << (std::wstring)arg << L' ';

	ss << L'}';

	return ss.str();

}

template struct fn<term>;
template struct fn<predicate<term>>;
