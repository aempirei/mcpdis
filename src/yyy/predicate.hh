#pragma once

namespace yyy {

	template <typename T> struct predicate {

		// typedefs

		using value_type = T;

		using range = std::pair<size_t,size_t>;

		enum struct types { end, any, mem, by_ref, by_type, by_op, by_value };
		enum struct modifiers { push, pop, lift, drop, bind, reject };

		// properties

		types type;
		meta<argument<value_type>> arg;
		std::set<modifiers> mods;
		range quantifier;

		// constructors

		predicate();
		predicate(const predicate&);
		predicate(const meta<argument<value_type>>&);

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

		predicate set_type(const types&) const;

		// modifier

		predicate pop() const;
		predicate push() const;
		predicate lift() const;
		predicate drop() const;
		predicate bind() const;
		predicate reject() const;

		predicate modify(const modifiers&) const;

		// quantifier

		predicate q(const range&) const;

		predicate max(size_t) const;
		predicate min(size_t) const;

		predicate star() const;
		predicate plus() const;
		predicate qm() const;

		// other stuff

		std::wstring str() const;

		operator const wchar_t * () const;

		using test_return_type = std::pair<bool,binding<T>>;

		test_return_type test(const grammar<T>&, const function<T>&);
		bool test(const argument<T>&);

		bool operator==(const predicate&) const;
	};
}
