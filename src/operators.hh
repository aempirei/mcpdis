#pragma once

#define OP_AND		L'\u2229'
#define OP_OR		L'\u222a'
#define OP_XOR		L'\u2297'
#define OP_PLUS		L'+'
#define OP_MINUS	L'-'
#define OP_SWAP		L'%'
#define OP_NOT		L'\u00ac'
#define OP_ROTL		L'\u21a9'
#define OP_ROTR		L'\u21aa'
#define OP_COMPOSE	L'\u2218'
#define OP_LIST		L','

#define OP_IS_NILPOTENT(x)	\
	(			\
	 ((x) == OP_XOR)	\
	)

#define OP_IS_IDEMPOTENT(x)	\
	(			\
	 ((x) == OP_AND) ||	\
	 ((x) == OP_OR )	\
	)

#define OP_IS_ASSOCIATIVE(x)	\
	(			\
	 ((x) == OP_AND ) ||	\
	 ((x) == OP_OR  ) ||	\
	 ((x) == OP_XOR ) ||	\
	 ((x) == OP_PLUS)	\
	)

#define OP_IS_COMMUTATIVE(x)	\
	(			\
	 ((x) == OP_AND ) ||	\
	 ((x) == OP_OR  ) ||	\
	 ((x) == OP_XOR ) ||	\
	 ((x) == OP_PLUS) 	\
	)		

#define OP_IS_UNARY(x)		\
	(			\
	 ((x) == OP_NOT ) ||	\
	 ((x) == OP_SWAP) ||	\
	 ((x) == OP_ROTL) ||	\
	 ((x) == OP_ROTR) 	\
	)

#define OP_IS_UNARY_NOP(x)	\
	(			\
	 ((x) == OP_AND    ) ||	\
	 ((x) == OP_OR     ) ||	\
	 ((x) == OP_XOR    ) ||	\
	 ((x) == OP_PLUS   ) ||	\
	 ((x) == OP_MINUS  ) ||	\
	 ((x) == OP_COMPOSE) 	\
	)

#define OP_IDENTITY_IS_0(x)	\
	(			\
	 ((x) == OP_OR  ) ||	\
	 ((x) == OP_XOR ) ||	\
	 ((x) == OP_PLUS)	\
	)

#define OP_IDENTITY_IS_1(x)	\
	(			\
	 ((x) == OP_AND)	\
	)

#define OP_IS_UNARY_INVOLUTION(x)	\
	(				\
	 ((x) == OP_NOT ) ||		\
	 ((x) == OP_SWAP)		\
	)
