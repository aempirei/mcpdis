#pragma once

namespace yyy {

	template <typename T> struct predicate {

		// typedefs

		using value_type = T;

		enum class types { end, any, mem, by_ref, by_type, by_op, by_value };
		enum class modifiers { push, pop, lift, drop, bind, reject };

		// properties

		types type;
		argument<value_type> arg;
		symbol reference;
		std::set<modifiers> mods;
		range quantifier;

		// constructors

		predicate();

		predicate(const predicate&);

		predicate(const symbol&);
		predicate(const argument<value_type>&);

		// operators

		predicate operator-() const; /* drop */
		predicate operator+() const; /* plus */
		predicate operator*() const; /* star */
		predicate operator!() const; /* reject */
		predicate operator~() const; /* lift */

		// type

		predicate end() const;
		predicate any() const;
		predicate mem() const;

		predicate by_ref() const;

		predicate by_op() const;
		predicate by_type() const;
		predicate by_value() const;

		// modifier

		predicate pop() const;
		predicate push() const;
		predicate lift() const;
		predicate drop() const;
		predicate bind() const;
		predicate reject() const;

		// quantifier

		predicate q(const range&) const;

		predicate max(size_t) const;
		predicate min(size_t) const;

		predicate star() const;
		predicate plus() const;
		predicate qm() const;

		explicit operator std::wstring() const;
	};
}
