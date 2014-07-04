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
		: type(my_arg.template contains_type<symbol::ref>() ? types::by_ref : types::by_value), arg(my_arg), mods({modifiers::bind}), quantifier(range(1,1))
	{
	}

	// operators

	template<typename T> predicate<T> predicate<T>::operator-() const { return drop   (); }
	template<typename T> predicate<T> predicate<T>::operator+() const { return plus   (); }
	template<typename T> predicate<T> predicate<T>::operator*() const { return star   (); }
	template<typename T> predicate<T> predicate<T>::operator!() const { return reject (); }
	template<typename T> predicate<T> predicate<T>::operator~() const { return lift   (); }

	//
	// type
	//

	template<typename T> predicate<T> predicate<T>::set_type(const types& my_type) const {

		auto that(*this);

		that.type = my_type;

		switch(that.type) {
			case types::end:
			case types::any:
			case types::mem:
				that.arg.clear();
			default:
				break;
		}

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
			case types::end      : ss << ANSI_HIBLACK << '$' << ANSI_CLR ; break ;
			case types::any      : ss << ANSI_HIBLACK << '.' << ANSI_CLR ; break ;
			case types::mem      : ss << ANSI_HIBLACK << '#' << ANSI_CLR ; break ;
			case types::by_ref   : ss << arg.str()                       ; break ;

			case types::by_type:

					       ss << ANSI_LOMAGENTA << L'[' << ANSI_HIMAGENTA;

					       for(const auto id : arg.get_types())
						       ss << typeoperator[id];

					       ss << ANSI_LOMAGENTA << L']' << ANSI_CLR;

					       break;

			case types::by_op:

					       ss << typecolor[std::type_index(typeid(function<value_type>))];
					       ss << "F(" << arg.template get<function<value_type>>().op << ")";
					       ss << ANSI_CLR;
					       break;

			case types::by_value:
					       ss << L'"' << arg.str() << L'"';
					       break;

		}

		ss << ANSI_LOYELLOW;

		/**/ if(quantifier == range(1,1))              { /* do nothing */ }
		else if(quantifier == range(0,1))              ss << ANSI_HIYELLOW << '?';
		else if(quantifier == range(1,UINT_MAX))       ss << ANSI_HIYELLOW << '+';
		else if(quantifier == range(0,UINT_MAX))       ss << ANSI_HIYELLOW << '*';
		else if(quantifier.first == quantifier.second) ss << '{' << quantifier.first << '}';
		else if(quantifier.first == 0)                 ss << "{," << quantifier.second << '}';
		else if(quantifier.second == UINT_MAX)         ss << '{' << quantifier.first << ",}";
		else                                           ss << '{' << quantifier.first << ',' << quantifier.second << '}';

		ss << ANSI_CLR;

		return ss.str();
	}

	template <typename T> typename predicate<T>::test_return_type predicate<T>::test(const grammar<T>& g, const function<T>& f) {

		switch(type) {

			case types::end:

				if(not arg.empty())
					throw std::runtime_error("end predicate contained unexpected non-empty argument");
				if(f.args.empty())
					return test_return_type(true,binding<T>(*this));
				break;

			case types::any:

				if(not arg.empty()) {
					throw std::runtime_error("any predicate contained unexpected non-empty argument");
				} else {
					binding<T> b(*this);
					//for(const auto& x : f.args) {
					//}
				}
				break;

			case types::mem:

				if(not arg.empty())
					throw std::runtime_error("mem predicate contained unexpected non-empty argument");
				throw std::runtime_error("mem predicate not implemented");
				break;

			case types::by_ref:

				if(not arg.template contains_type<symbol::ref>()) {
					throw std::runtime_error("by_ref predicate does not contain expected symbol::ref argument");
				} else {
					auto dast = g.parse(arg.template get<symbol::ref>(), f);
				}
				break;

			case types::by_type:

				if(arg.template contains_type<symbol::ref>()) {
					throw std::runtime_error("by_type predicate contains unexpected symbol::ref argument");
				} else {
					auto arg_types = arg.get_types();
				}
				break;

			case types::by_op:

				if(not arg.template contains_type<function<value_type>>())
					throw std::runtime_error("by_ref predicate expected symbol::ref argument but did not contain one");
				break;

			case types::by_value:

				if(arg.template contains_type<symbol::ref>())
					throw std::runtime_error("by_value predicate contains unexpected symbol::ref argument");
				break;
		}

		return test_return_type(false,binding<value_type>());
	}

	template struct predicate<term>;
}
