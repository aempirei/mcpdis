#include <algorithm>
#include <sstream>
#include <iomanip>
#include <iostream>

#include <cstdlib>
#include <cstdint>
#include <climits>

#include <mcpdis.hh>

template<typename T> template<typename U> bool grammar<T>::match(const symbol& ref, const U& x, rule<T> *rptr) {

	const auto& rs = this->at(ref);

	for(const auto& r : rs)
		if(match(r, U(x), rptr))
			return true;

	return false;
}

template<typename T> template<typename U> bool grammar<T>::match(const rule<T>& r, U& x, rule<T> *rptr) {

	if(r.op != L'\0' && r.op != x.op)
		return false;

	rule<T> dr(r);

	for(auto& p : dr.predicates) {
		// while dr.args.size < p.q.second and x.args.size > 0
			// move x.args that match p into dr.args
		// if dr.args.size < p.q.first then return false
	}

	throw std::runtime_error(
			"template<typename T> template<typename U> "
			"bool grammar<T>::match(const rule<T>&, typename U::iterator, rule<T> *) "
			"unimplemented"
			);
	return false;
}



template struct grammar<term>;
