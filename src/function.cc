#include <algorithm>
#include <sstream>
#include <iomanip>

#include <cstdlib>
#include <cstdint>
#include <climits>

#include <mcpdis.hh>

template<typename T> function<T>::function() : function(OP_LIST, {}) {
}

template<typename T> function<T>::function(op_t my_op) : function(my_op, {}) {
}

template<typename T> function<T>::function(op_t my_op, const args_type& my_args) : op(my_op), args(my_args) {
}

template<typename T> function<T>::function(const function&r) : function(r.op, r.args) {
}

template<typename T> function<T>& function<T>::operator=(const function& r) {

	if(this != &r) {
		op = r.op;
		args = args_type(r.args);
	}

	return *this;
}

template<typename T> bool function<T>::operator==(const function& r) const {
	return op == r.op && args == r.args;
}

template<typename T> bool function<T>::operator<(const function& r) const {

	if(op != r.op)
		return op < r.op;

	return args < r.args;
}

template<typename T> function<T>& function<T>::operator<<(const T& x) {
	args.push_back(x);
	return *this;
}

template<typename T> size_t function<T>::arity() const {
	return args.size();
}

template<typename T> void function<T>::clear() {
	op = OP_LIST;
	args.clear();
}

template<typename T> void function<T>::concat(const args_type& xs) {
	args.insert(args.end(), xs.begin(), xs.end());
}

template struct function<term>;
