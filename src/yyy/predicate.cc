#include <yyy.hh>
#include <iostream>

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
		: type(my_arg.contains<symbol::ref>() ? types::by_ref : types::by_value), arg(my_arg), mods({modifiers::bind}), quantifier(range(1,1))
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

	template <typename T> std::wstring predicate<T>::str() const {

		std::wstringstream ss;

		for(auto modifier : mods) {
			if(modifier != modifiers::bind)
				ss << (operator_t)modifier;
		}

		switch(type) {

			case types::end:
			case types::any:
			case types::mem:

				ss << (operator_t)type;
				break;

			case types::by_ref:

				ss << type::color<symbol::ref>() << arg.template get<symbol::ref>() << ANSI_CLR;
				break;

			case types::by_type:

				ss << arg.type_str();
				break;

			case types::by_op:

				ss << arg.type_str() << ':' << arg.template get<function<value_type>>().op;
				break;

			case types::by_value:

				ss << ANSI_CLR << '"' << arg.str() << '"';
				break;
		}

		ss << type::color<predicate<T>>();

		/**/ if(quantifier == range(1,1))              { /* do nothing */ }
		else if(quantifier == range(0,1))              ss <<  '?';
		else if(quantifier == range(1,UINT_MAX))       ss <<  '+';
		else if(quantifier == range(0,UINT_MAX))       ss <<  '*';
		else if(quantifier.first == quantifier.second) ss << '{' << quantifier.first << '}';
		else if(quantifier.first == 0)                 ss << "{," << quantifier.second << '}';
		else if(quantifier.second == UINT_MAX)         ss << '{' << quantifier.first << ",}";
		else                                           ss << '{' << quantifier.first << ',' << quantifier.second << '}';

		ss << ANSI_CLR;

		return ss.str();
	}

	template <typename T> bool predicate<T>::test(const argument<T>& x) const {
		switch(type) {

			case types::end      : throw std::runtime_error("end predicate test called on plain argument");
			case types::by_ref   : throw std::runtime_error("by_ref predicate test called on plain argument");
			case types::mem      : throw std::runtime_error("mem predicate test test not implemented"); // FIXME
			case types::by_value : throw std::runtime_error("by_value predicate test not implemented"); // FIXME

			case types::any:

				return true;

			case types::by_op:

				return arg.template contains<function<T>>()
					and x.template contains<function<T>>()
					and (x.template get<function<T>>().op == x.template get<function<T>>().op);

			case types::by_type:

				{

					auto a = arg.type_set();
					auto b = x.type_set();

					std::list<std::type_index> ab;

					std::set_intersection(a.begin(), a.end(), b.begin(), b.end(), ab.end());

					return not ab.empty();
				}
				break;
		}

		return false;
	}

	template <typename T> resultant<matching<T>> predicate<T>::test(const grammar<T>& g, const function<T>& f) const {

		resultant<matching<T>> result = { true, { { *this, {} }, f } };

		bindings<T>& b = result.second.first.second;
		function<T>& df = result.second.second;

		switch(type) {

			case types::end:

				if(quantifier != range(1,1))
					throw std::runtime_error("end predicate contained unexpected quantifier");

				if(not arg.empty())
					throw std::runtime_error("end predicate contained unexpected non-empty argument");

				if(df.args.empty())
					return result;

				break;

			case types::by_ref:

				if(arg.template contains<symbol::ref>()) {

					const auto& key = arg.template get<symbol::ref>();

					while(b.size() < quantifier.second) {
						auto result_match = g.parse(key, df);
						if(not result_match.first)
							break;
						b.push_back(result_match.second.first);
						df = result_match.second.second;
					}

				} else {
					throw std::runtime_error("by_ref predicate contained non-symbol::ref argument");
				}

				break;
		
			case types::mem:
			case types::any:

				if(not arg.empty())
					throw std::runtime_error("predicate contained unexpected non-empty argument");

			case types::by_op:

				if(type == types::by_op and not arg.template contains<function<value_type>>())
					throw std::runtime_error("by_op predicate contained unexpected non-function argument");

			case types::by_type:
			case types::by_value:

				if(arg.template contains<symbol::ref>()) {
					throw std::runtime_error("predicate contains unexpected symbol::ref argument");
				} else {

					auto iter = df.args.begin();

					while(b.size() < quantifier.second and iter != df.args.end()) {

						auto jter = next(iter);

						if(test(*iter)) {
							b.push_back(*iter);
							df.args.erase(iter);
						}

						iter = jter;
					}
				}

				break;
		}

		if(b.size() >= quantifier.first and b.size() <= quantifier.second) {
			return result;
		} else {
			return resultant<matching<T>>();
		}
	}

	template <typename T> bool predicate<T>::operator==(const predicate& r) const {
		return type == r.type and arg == r.arg and mods == r.mods and quantifier == r.quantifier;
	}

	template struct predicate<term>;
}
