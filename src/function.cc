#include <algorithm>
#include <sstream>
#include <iomanip>

#include <cstdlib>
#include <cstdint>
#include <climits>

#include <mcpdis.hh>

function::function() : function(OP_LIST, {}) {
}

function::function(op_t my_op) : function(my_op, {}) {
}

function::function(op_t my_op, const arglist& my_args) : op(my_op), args(my_args) {
}

function::function(const function&r) : function(r.op, r.args) {
}

function& function::operator=(const function& r) {

	if(this != &r) {
		op = r.op;
		args = arglist(r.args);
	}

	return *this;
}

bool function::operator==(const function& r) const {
	return op == r.op && args == r.args;
}

bool function::operator<(const function& r) const {

	if(op != r.op)
		return op < r.op;

	return args < r.args;
}

function& function::operator<<(const term& x) {
	args.push_back(x);
	return *this;
}

size_t function::arity() const {
	return args.size();
}

void function::clear() {
	op = OP_LIST;
	args.clear();
}

void function::concat(const arglist& xs) {
	args.insert(args.end(), xs.begin(), xs.end());
}
