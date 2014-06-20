#include <yyy.hh>

namespace yyy {

	//
	// constructors
	//

	template <typename T> predicate<T>::predicate()
		: type(types::any), mods({modifiers::bind}), quantifier(range(1,1))
	{
	}

	template <typename T> predicate<T>::predicate(const predicate& r)
		: type(r.type), arg(r.arg), mods(r.mods), quantifier(r.quantifier)
	{
	}

	template <typename T> predicate<T>::predicate(const meta<argument<value_type>>& my_arg)
		: type(types::by_value), arg(my_arg), mods({modifiers::bind}), quantifier(range(1,1))
	{
	}

	// operators

	// template<typename T> predicate<T> predicate<T>::operator-() const; // drop
	// template<typename T> predicate<T> predicate<T>::operator+() const; // plus
	// template<typename T> predicate<T> predicate<T>::operator*() const; // star
	// template<typename T> predicate<T> predicate<T>::operator!() const; // reject
	// template<typename T> predicate<T> predicate<T>::operator~() const; // lift

	//
	// type
	//

	template<typename T> predicate<T> predicate<T>::set_type(const types& my_type) const {
		auto that(*this);
		that.type = my_type;
		return that;
	}

	template<typename T> predicate<T> predicate<T>::end      () const { return set_type ( types::end      ); }
	template<typename T> predicate<T> predicate<T>::any      () const { return set_type ( types::any      ); }
	template<typename T> predicate<T> predicate<T>::mem      () const { return set_type ( types::mem      ); }
	template<typename T> predicate<T> predicate<T>::by_ref   () const { return set_type ( types::by_ref   ); }
	template<typename T> predicate<T> predicate<T>::by_op    () const { return set_type ( types::by_op    ); }
	template<typename T> predicate<T> predicate<T>::by_type  () const { return set_type ( types::by_type  ); }
	template<typename T> predicate<T> predicate<T>::by_value () const { return set_type ( types::by_value ); }

	//
	// modifier
	//

	template<typename T> predicate<T> predicate<T>::modify(const modifiers& modifier) const {
		auto that(*this);
		that.mods.insert(modifier);
		return that;
	}

	template<typename T> predicate<T> predicate<T>::pop    () const { return modify ( modifiers::pop    ); }
	template<typename T> predicate<T> predicate<T>::push   () const { return modify ( modifiers::push   ); }
	template<typename T> predicate<T> predicate<T>::lift   () const { return modify ( modifiers::lift   ); }
	template<typename T> predicate<T> predicate<T>::drop   () const { return modify ( modifiers::drop   ); }
	template<typename T> predicate<T> predicate<T>::bind   () const { return modify ( modifiers::bind   ); }
	template<typename T> predicate<T> predicate<T>::reject () const { return modify ( modifiers::reject ); }

	//
	// quantifier
	//

	template<typename T> predicate<T> predicate<T>::q(const range& my_quantifier) const {
		auto that(*this);
		that.quantifier = my_quantifier;
		return that;
	}

	template<typename T> predicate<T> predicate<T>::max(size_t x) const { return q ( range ( 0,x        )); }
	template<typename T> predicate<T> predicate<T>::min(size_t x) const { return q ( range ( x,UINT_MAX )); }
	template<typename T> predicate<T> predicate<T>::star       () const { return q ( range ( 0,UINT_MAX )); }
	template<typename T> predicate<T> predicate<T>::plus       () const { return q ( range ( 1,UINT_MAX )); }
	template<typename T> predicate<T> predicate<T>::qm         () const { return q ( range ( 0,1        )); }

	//
	// operator std::wstring
	//

	template <typename T> predicate<T>::operator std::wstring () const {
		return str();
	}

	template <typename T> std::wstring predicate<T>::str() const {

		std::wstringstream ss;
		
		for(auto modifier : mods) {
			switch(modifier) {
				case modifiers::push   : ss << '>';       break ;
				case modifiers::pop    : ss << '<';       break ;
				case modifiers::lift   : ss << '~';       break ;
				case modifiers::drop   : ss << '-';       break ;
				case modifiers::reject : ss << '!';       break ;
				case modifiers::bind   : /* do nothing */ break ;

			}
		}

		switch(type) {
			case types::end      : ss << '$'                                    ; break ;
			case types::any      : ss << '.'                                    ; break ;
			case types::mem      : ss << '#'                                    ; break ;
			case types::by_type  : ss << "TYPE:" << /*(std::wstring)*/arg.str() ; break ;
			case types::by_op    : ss << "OP:!!!"                               ; break ; // FIXME
			case types::by_value : ss << /*(std::wstring)*/arg.str()            ; break ;

		}

		if(quantifier == range(1,1))                   { /* do nothing */ }
		else if(quantifier == range(0,1))              ss << '?';
		else if(quantifier == range(1,UINT_MAX))       ss << '+';
		else if(quantifier == range(0,UINT_MAX))       ss << '*';
		else if(quantifier.first == quantifier.second) ss << '{' << quantifier.first << '}';
		else if(quantifier.first == 0)                 ss << "{," << quantifier.second << '}';
		else if(quantifier.second == UINT_MAX)         ss << '{' << quantifier.first << ",}";
		else                                           ss << '{' << quantifier.first << ',' << quantifier.second << '}';

		return ss.str();
	}

	template struct predicate<term>;
}
