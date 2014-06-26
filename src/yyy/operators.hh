#pragma once

#define OP_AND		L'\u2229'
#define OP_OR		L'\u222a'
#define OP_XOR		L'\u2295'
#define OP_PLUS		L'+'

#define OP_MINUS	L'-'
#define OP_COMPOSE	L'\u2218'
#define OP_LIST		L'\0'
#define OP_LAMBDA	L'\u03bb'
#define OP_QM		L'?'
#define OP_ANY		OP_QM
#define OP_SAME		OP_LAMBDA

#define OP_SWAP		L'%'
#define OP_NOT		L'\u00ac'
#define OP_ROTL		L'\u21a9'
#define OP_ROTR		L'\u21aa'

enum class operators : wchar_t {
	AND = OP_AND,
	OR = OP_OR,
	XOR = OP_XOR,
	PLUS = OP_PLUS,
	MINUS = OP_MINUS,
	COMPOSE = OP_COMPOSE,
	LIST = OP_LIST,
	LAMBDA = OP_LAMBDA,
	QM = OP_QM,
	ANY = OP_ANY,
	SAME = OP_SAME,
	SWAP = OP_SWAP,
	NOT = OP_NOT,
	ROTL = OP_ROTL,
	ROTR= OP_ROTR
};
