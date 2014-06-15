#include <yyy.hh>

namespace yyy {

	template <typename T> predicate<T>::predicate()
		: type(types::any), mods({modifiers::bind}), quantifier(range(1,1))
	{
	}

	template <typename T> predicate<T>::predicate(const predicate& r)
		: type(r.type), arg(r.arg), reference(r.reference), mods(r.mods), quantifier(r.quantifier) 
	{
	}

	template <typename T> predicate<T>::predicate(const symbol& my_reference)
		: type(types::by_ref), reference(my_reference), mods({modifiers::bind}), quantifier(range(1,1))
	{
	}

	template <typename T> predicate<T>::predicate(const argument<value_type>& my_arg)
		: type(types::by_value), arg(my_arg), mods({modifiers::bind}), quantifier(range(1,1))
	{
	}

	template <typename T> predicate<T>::operator std::wstring () const {

		std::wstringstream ss;
		
		for(auto modifier : mods) {
			switch(modifier) {
				case modifiers::push:
					ss << '>';
					break;
				case modifiers::pop:
					ss << '<';
					break;
				case modifiers::lift:
					ss << '~';
					break;
				case modifiers::drop:
					ss << '-';
					break;
				case modifiers::bind:
					// nothing
					break;
				case modifiers::reject:
					ss << '!';
					break;

			}
		}

		switch(type) {
			case types::end:
				ss << '$';
				break;
			case types::any:
				ss << '.';
				break;
			case types::mem:
				ss << '#';
				break;
			case types::by_ref:
				ss << reference;
				break;
			case types::by_type:
				ss << "TYPE:" << (std::wstring)arg;
				break;
			case types::by_op:
				ss << "OP:" << arg.template ptr_to<function<T>>()->op;
				break;
			case types::by_value:
				ss << (std::wstring)arg;
				break;
				break;

		}

		if(quantifier == range(1,1)) {
			// nothing
		} else if(quantifier == range(0,1)) {
			ss << '?';
		} else if(quantifier == range(1,UINT_MAX)) {
			ss << '+';
		} else if(quantifier == range(0,UINT_MAX)) {
			ss << '*';
		} else if(quantifier.first == quantifier.second) {
			ss << '{' << quantifier.first << '}';
		} else if(quantifier.first == 0) {
			ss << "{," << quantifier.second << '}';
		} else if(quantifier.second == UINT_MAX) {
			ss << '{' << quantifier.first << ",}";
		} else {
			ss << '{' << quantifier.first << ',' << quantifier.second << '}';
		}

		return ss.str();
	}

	template struct predicate<term>;

#if SATANIC
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
#endif
}
