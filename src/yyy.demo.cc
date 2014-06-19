#include <yyy.hh>
#include <typeinfo>

namespace yyy {
	template <typename T> grammar<T> define_grammar() {
		grammar<T> g;
		return g;
	}
}

template <zzz::symbol::type S> void fu(zzz::basic_symbol<wchar_t,S> x) {
	std::wcout << "basic_symbol<" << (int)decltype(x)::type_enum << "> := " << x << std::endl;
}

void do_zzz() {

	using namespace zzz;

	symbol::reference g(L"ref-symbol");
	symbol::name j(L"name-symbol");

	std::wcout << typeid(g).name() << " := " << g << std::endl;
	std::wcout << typeid(j).name() << " := " << j << std::endl;

	fu(g);
	fu(j);

	using wstr = std::wstring;

	using e_int = zzz::maybe<int>;
	using e_str = zzz::maybe<wstr>;
	using e_int_str = zzz::either<e_int,e_str>;
	using e_int_str_bool = zzz::either<zzz::maybe<bool>,e_int_str>;

	zzz::nothing nope;
	e_int i;
	e_str s;
	e_int_str is;
	e_int_str_bool isb;

	i.assign(5);
	s.assign(wstr(L"gay"));
	isb.insert(5);
	isb.insert(wstr(L"fag"));
	isb.insert(false);

	std::wcout << "nothing: '" << nope.str() << "'" << std::endl;
	std::wcout << "    int: '" << i.str() << "'" << std::endl;
	std::wcout << "    str: '" << s.str() << "'" << std::endl;
	std::wcout << "int/str: '" << isb.str() << "'" << std::endl;

	std::wcout << "isb<int> :: " << ( isb.template allows_type<int>() ) << ' ' << ( isb.template contains_type<int>() ) << std::endl;
	std::wcout << "isb<int> == " << ( isb.template contains_value(4) ) << ' ' << ( isb.template contains_value(5) ) << std::endl;
	std::wcout << "isb<bool> :: " << ( isb.template allows_type<bool>() ) << ' ' << ( isb.template contains_type<bool>() ) << std::endl;
	std::wcout << "isb<bool> == " << ( isb.template contains_value(true) ) << ' ' << ( isb.template contains_value(false) ) << std::endl;
	std::wcout << "isb<wstr> :: " << ( isb.template allows_type<wstr>() ) << ' ' << ( isb.template contains_type<wstr>() ) << std::endl;
	std::wcout << "isb<wstr> == " << ( isb.template contains_value(wstr(L"dicks")) ) << ' ' << ( isb.template contains_value(wstr(L"fag")) ) << std::endl;

	std::wcout << ( s.contains_value(wstr(L"fag")) ) << std::endl;
	std::wcout << ( i.contains_value(4) ) << std::endl;
	std::wcout << ( i.contains_value(5) ) << std::endl;
}

int main(int argc, char **argv) {

	using namespace yyy;

	using F = function<term>;
	using L = literal_t;
	using S = symbol;

	using R = rule<term>;
	using P = predicate<term>;
	using A = argument<term>;

	maybe<literal_t> lit;

	setlocale(LC_CTYPE, "");

	F f = F(OP_SAME) << ( F(OP_OR) << S(L"what") ) << L(666);

	for(int i = 0; i < argc; i++)
		f << L(atoi(argv[i]));

	f << f;

	f << f;

	std::wcout << (std::wstring)f << std::endl;

	auto g = define_grammar<term>();

	std::wcout << (std::wstring)(P().any().star()) << std::endl;

	R r = R(OP_ANY) << P(A(666)).by_type().plus() << P().any().star() << P().end();

	std::wcout << "rule := " << (std::wstring)r << std::endl;

	do_zzz();

	return 0;
}
