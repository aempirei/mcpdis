#include <mcpdis.hh>
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <cstdlib>
#include <cstdint>
#include <climits>
#include <iostream>
#include <set>

template<wchar_t> static void function_expression(term&, const arglist&);
template<class F> static void aggregate_literals(term&, reg_t, F);
static void association_rule(term&, op_t, const arglist&);
static void distribution_rule(term&, op_t, op_t);

//////////////////
//
//
// struct function
//
//
//////////////////

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

	/*
	// lexicographic comparison

	auto iter = args.cbegin();
	auto jter = r.args.cbegin();

	while(iter != args.end() && jter != r.args.end()) {

	if(*iter < *jter) {

	return true;

	} else if(*iter == *jter) {

	iter++;
	jter++;

	} else {
	return false;
	}
	}
	 */
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

//////////////
//
//
// struct term
//
//
//////////////

term::term() : term(function(OP_LIST)) {
}

term::term(const term& r) {
	*this = r;
}

term::term(literal_t my_l) : type(term_type::literal), l(my_l) {
}

term::term(const variable& my_v) : type(term_type::variable), v(my_v) {
}

#define OPERATION_HANDLER(__OP__) case __OP__: function_expression<__OP__>(*this, my_args); break

term::term(const function& my_f) : type(term_type::function), f(my_f) {
	/*
	switch(op) {

		OPERATION_HANDLER(OP_AND);
		OPERATION_HANDLER(OP_OR);
		OPERATION_HANDLER(OP_XOR);
		OPERATION_HANDLER(OP_PLUS);
		OPERATION_HANDLER(OP_MINUS);
		OPERATION_HANDLER(OP_SWAP);
		OPERATION_HANDLER(OP_NOT);
		OPERATION_HANDLER(OP_ROTL);
		OPERATION_HANDLER(OP_ROTR);
		OPERATION_HANDLER(OP_COMPOSE);
		OPERATION_HANDLER(OP_LIST);

		default:
		throw std::runtime_error("unhandled operation");
		break;
	}
	*/
}

#undef OPERATION_HANDLER

term& term::operator=(const term& r) {

	if(this != &r) {

		type = r.type;

		switch(type) {

			case term_type::literal:

				l = r.l;
				v.clear();
				f.clear();
				break;

			case term_type::variable:

				l = 0;
				v = r.v;
				f.clear();
				break;

			case term_type::function:

				l = 0;
				v.clear();
				f = r.f;
				break;
		}
	}

	return *this;
}

bool term::operator==(const term& r) const {

	if(type != r.type)
		return false;

	switch(type) {
		case term_type::literal  : return l == r.l;
		case term_type::variable : return v == r.v;
		case term_type::function : return f == r.f;
	}

	return false;
}

bool term::operator<(const term& r) const {

	if(type != r.type)
		return type < r.type;

	switch(type) {
		case term_type::literal  : return l < r.l;
		case term_type::variable : return v < r.v;
		case term_type::function : return f < r.f;
	}

	return false;
}

std::wstring term::wstr() const {

	std::wstringstream ws;

	switch(type) {

		case term_type::variable:

			ws << v;

			break;

		case term_type::function:

			ws << L'(' << f.op;

			for(const auto& arg : f.args)
				ws << L' ' << arg.wstr();

			ws << L')';


			break;

		case term_type::literal:

			ws << std::showbase << (l < 10 ? std::dec : std::hex) << l;

			break;
	}

	return ws.str();
}

bool term::is_function(op_t my_op) const {
	return type == term_type::function && f.op == my_op;
}

bool term::is_variable(const variable& my_v) const {
	return type == term_type::variable && v == my_v;
}

bool term::is_literal(literal_t my_l) const {
	return type == term_type::literal && l == my_l;
}

bool term::is_function() const {
	return type == term_type::function;
}

bool term::is_variable() const {
	return type == term_type::variable;
}

bool term::is_literal() const {
	return type == term_type::literal;
}

bool term::is_nullary() const {
	return is_arity(0);
}

bool term::is_unary() const {
	return is_arity(1);
}

bool term::is_binary() const {
	return is_arity(2);
}

bool term::is_arity(size_t n) const {
	return is_function() && f.arity() == n;
}

///////////////////////
//
//
// transformation rules
//
//
///////////////////////

template<class F> static void aggregate_literals(term& e, reg_t x0, F f) {

	reg_t x = x0;

	if(!e.is_function())
		return;

	auto iter = e.f.args.begin();

	while(iter != e.f.args.end()) {

		auto jter = next(iter);

		if(iter->is_literal()) {
			x = f(x, iter->l);
			e.f.args.erase(iter);
		}

		iter = jter;
	}

	if(x != x0)
		e.f.args.push_front(x);
}

static void association_rule(term& e, op_t op, const arglist& args) {

	for(const auto& arg : args) {

		if(arg.is_function(op))
			e.f.args.insert(e.f.args.end(), arg.f.args.begin(), arg.f.args.end());
		else
			e.f.args.push_back(arg);
	}
}

static void distribution_rule(term& e, op_t op1, op_t op2) {

	// find term to distribute
	// in order of highest priority first
	// : literals, variables, functions ("OP_AND" only)

	if(!e.is_function())
		return;

	auto jter = e.f.args.begin();

	while(jter != e.f.args.end() && !jter->is_literal())
		jter++;

	if(jter == e.f.args.end()) {
		jter = e.f.args.begin();
		while(jter != e.f.args.end() && jter->is_variable())
			jter++;
	}

	if(jter == e.f.args.end()) {
		jter = e.f.args.begin();
		while(jter != e.f.args.end() && !jter->is_function(op2))
			jter++;
	}

	if(jter == e.f.args.end())
		return;

	// find term to distribute over
	// specifically ("OP_AND") functions only

	auto kter = e.f.args.begin();

	while(kter != e.f.args.end() && (kter == jter || !kter->is_function(op2)))
		kter++;

	if(kter == e.f.args.end())
		return;

	// distribute *jter over kter->f.args

	const auto& e1 = *jter;

	for(auto& arg : kter->f.args) {
		const auto& e2 = arg;
		arg = function(op1, { e1, e2 });
	}

	e.f.args.erase(jter);
}

////////////////////////////////
//
//
// function_expression templates
//
//
////////////////////////////////

template<> void function_expression<OP_SWAP>(term& e, const arglist& args) {

	if(args.size() != 1) {
		std::stringstream ss;
		ss << "unexpected arity of " << args.size() << " for unary swap (\"OP_SWAP\") operation." << std::endl;
		throw std::runtime_error(ss.str());
	}

	if(args.front().is_function(OP_SWAP)) {

		e = args.front().f.args.front();

	} else if(args.front().is_literal()) {

		uint8_t reg = args.front().l;

		reg = (reg << 4) | (reg >> 4);
		e = reg;

	} else {
		e.f.args = args;
	}
}

template<> void function_expression<OP_NOT>(term& e, const arglist& args) {

	if(args.size() != 1) {
		std::stringstream ss;
		ss << "unexpected arity of " << args.size() << " for unary not (\"OP_NOT\") operation." << std::endl;
		throw std::runtime_error(ss.str());
	}

	if(args.front().is_function(OP_NOT)) {

		e = args.front().f.args.front();

	} else if(args.front().is_literal()) {

		reg_t reg = args.front().l;

		reg = ~reg;
		e = reg;

	} else {
		e.f.args = args;
	}
}

template<> void function_expression<OP_OR>(term& e, const arglist& args) {

	association_rule(e, OP_OR, args);

	aggregate_literals(e, (reg_t)0, [](reg_t a, reg_t x) -> reg_t { return a | x; });

	distribution_rule(e, OP_OR, OP_AND);
	
	if(e.is_unary() && OP_IS_UNARY_NOP(e.f.op)) e = e.f.args.front();
}

template<> void function_expression<OP_AND>(term& e, const arglist& args) {

	association_rule(e, OP_AND, args);

	aggregate_literals(e, (reg_t)~0, [](reg_t a, reg_t x) -> reg_t { return a & x; });

	distribution_rule(e, OP_AND, OP_OR);

	if(e.is_unary() && OP_IS_UNARY_NOP(e.f.op)) e = e.f.args.front();
}

template<> void function_expression<OP_PLUS>(term& e, const arglist& args) {

	association_rule(e, OP_PLUS, args);

	aggregate_literals(e, 0, [](reg_t a, reg_t x) -> reg_t { return a + x; });
}

template<wchar_t> static void function_expression(term& e, const arglist& args) {
	e.f.args = args;
}
