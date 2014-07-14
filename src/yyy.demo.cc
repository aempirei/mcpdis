#include <iostream>
#include <iomanip>
#include <cmath>

#include <yyy.hh>

using namespace yyy::quick;


G define_grammar() {

	M mLSF;

	mLSF.set(literal_t());
	mLSF.set(S::var());
	mLSF.set(F());

	const P LSF(P(mLSF).by_type());

	return G {
		{ L"combo", {
				    R(OP_THIS) << P(S::ref(L"satan")),
				    R(OP_OR) << P(S::ref(L"combo")),
				    R(OP_ANY) << *P() << !P(S::var(L"noway")) << P().end()
			    } },
		{ L"any"  , { R(OP_ANY) << *P() << P().end() } },
		{ L"satan", { R(OP_AND) << P(L(1337)) << LSF << P(S::var(L"eax")) } },
		{ L"hello", { R(OP_ANY) << +P(L(666)).by_type() << P(S::ref(L"satan")).qm() << P(F(OP_AND)) << P(F(OP_AND)).by_op() << *P() << P().end() } }
	};
}

template <typename T> void print_type() {
	std::wcout << "type(" << type::to_list<T>::size << ") <" << type::to_list<T>::str() << "> : " << typeid(T).name() << std::endl;
}

template <typename T> void print_types() {
	print_type<T>();
}

template <typename T, typename U, typename...Args> void print_types() {
	print_types<T>();
	print_types<U,Args...>();
}

template <typename T,typename U> std::wstring typecheck() {

	std::wstringstream ss;

	ss << type::to_str<T>() << ' ' << OP_IN << ' ';
	ss << type::to_list<U>::str() << " := ";
	ss << ( type::contains<T,U>::eval ? 'T' : 'F' );

	return ss.str();
}

template <typename X,typename T> void typeprint(wchar_t wx, const T& t, const X& x) {

	typecheck<X,T>();

	std::wcout << typecheck<X,T>() << ' ' << wx << " (" << t.size() << ':' << t.dim << ") := ";
	std::wcout << t.str() << " contains? ";
	std::wcout << type::container<X>(x).str();
	std::wcout << ' ' << ( t.contains(x) ? OP_IN : OP_NOT_IN ) << ' ';
	std::wcout << "[" << t.type_str() << "]";
	std::wcout << std::endl;
}

int main(int argc, char **argv) {

	setlocale(LC_CTYPE, "");

	std::wcout << std::endl << "running " << argv[0] << "..." << std::endl << std::endl;

	F f = F(OP_THIS) << ( F(OP_OR) << S::var(L"what") ) << L(666);

	std::wcout << "f := " << f.str() << std::endl;

	for(int i = 1; i < argc; i++)
		f << L(atoi(argv[i]));

	f << f;

	f << f;

	std::wcout << "f := " << (std::wstring)f << std::endl;

	auto g = define_grammar();

	const int width = 10;

	for(const auto& entry : g) {

		const auto& name = entry.first;
		const auto& rs = entry.second;

		auto iter = rs.begin();

		std::wcout << std::setw(width) << std::setfill(L' ') << std::left << name << " := " << iter->str() << std::endl;

		while(++iter != rs.end())
			std::wcout << std::setw(width) << std::setfill(L' ') << std::left << "" << " := " << iter->str() << std::endl;
	}

	using x_type = type::container<int,double,bool,wchar_t,void *>;
	using x2_type = type::concat<x_type,type::reverse<x_type>>;
	using x4_type = type::concat<x2_type,x2_type>;
	using reversed_type = type::reverse<x2_type>;
	using filtered_type = type::filter<int,type::filter<bool,x2_type>>;
	using unique_type = type::unique<x4_type>;
	using mini_type = type::container<int,wchar_t,long long>;

	print_types<x_type,x2_type,x4_type,reversed_type,filtered_type,unique_type>();

	unique_type y(66,M_PI,true,OP_COMPOSE,nullptr);
	unique_type z;
	mini_type::append<std::wstring> m;
	mini_type n;

	n.set(666LL);

	z.set(false);
	z.set(OP_PLUS);

	y.template unset<bool>();

	std::wcout << "project :: " << y.project(m).str() << std::endl;
	std::wcout << "project :: " << z.project(m).str() << std::endl;
	std::wcout << "project :: " << n.overlay(m).str() << std::endl;

	typeprint('y', y, M_PI);
	typeprint('y', y, OP_COMPOSE);
	typeprint('y', y, (float)M_PI);

	typeprint('z', z, M_PI);
	typeprint('z', z, OP_COMPOSE);

	std::wcout << "y == z :: " << (y == z ? 'T' : 'F') << std::endl;
	std::wcout << "y == y :: " << (y == y ? 'T' : 'F') << std::endl;

	return 0;
}
