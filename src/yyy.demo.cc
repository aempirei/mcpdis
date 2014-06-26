#include <yyy.hh>
#include <iostream>
#include <iomanip>

namespace yyy {
	template <typename T> grammar<T> define_grammar() {
		grammar<T> g;
		return g;
	}
}

template <yyy::symbol::type S> void fu(yyy::basic_symbol<wchar_t,S> x) {
	std::wcout << "basic_symbol<" << (int)decltype(x)::type_enum << "> := " << x << std::endl;
}

constexpr const wchar_t* boolstr (bool b) {
	return b ? L"is" : L"isn't";
}

void do_yyy() {

	using namespace yyy;

	symbol::ref g(L"ref-symbol");
	symbol::name j(L"name-symbol");

	std::wcout << typeid(g).name() << " := " << g << std::endl;
	std::wcout << typeid(j).name() << " := " << j << std::endl;

	fu(g);
	fu(j);

	using wstr = std::wstring;

	using e_int = maybe<int>;
	using e_str = maybe<wstr>;
	using e_int_str = choice<std::wstring,int>::type;
	using e_int_str_bool = choice<int,std::wstring,bool>::type;

	nothing nope;
	e_int i;
	e_str s;
	e_int_str is;
	e_int_str_bool isb;

	i.assign(5);
	isb.insert<wstr>(L"gay");
	isb.insert<int>(5);
	isb.insert<bool>(false);

	isb.clear();

	i.assign(int());

	std::wcout << "     nothing: '" << nope.str() << "'" << std::endl;
	std::wcout << "         int: '" << i.str() << "'" << std::endl;
	std::wcout << "         str: '" << s.str() << "'" << std::endl;
	std::wcout << "int/str/bool: '" << isb.str() << "'" << std::endl;

	std::wcout << "isb<int> :: " << ( e_int_str_bool::template allows_type<int>() ) << ' ' << ( isb.template contains_type<int>() ) << std::endl;
	std::wcout << "isb<int> == " << ( isb.template contains_value(4) ) << ' ' << ( isb.template contains_value(5) ) << std::endl;
	std::wcout << "isb<bool> :: " << ( isb.template allows_type<bool>() ) << ' ' << ( isb.template contains_type<bool>() ) << std::endl;
	std::wcout << "isb<bool> == " << ( isb.template contains_value(true) ) << ' ' << ( isb.template contains_value(false) ) << std::endl;
	std::wcout << "isb<wstr> :: " << ( isb.template allows_type<wstr>() ) << ' ' << ( isb.template contains_type<wstr>() ) << std::endl;
	std::wcout << "isb<wstr> == " << ( isb.template contains_value(wstr(L"dicks")) ) << ' ' << ( isb.template contains_value(wstr(L"fag")) ) << std::endl;

	std::wstringstream types_s;
	std::wstringstream types_a;

	using big_type = choice<bool,char,std::wstring,std::type_info>::type;

	for(const std::type_info* x : isb.get_types()) types_s << L' ' << x->name();
	for(const std::type_info* x : isb.allowed_types()) types_a << L' ' << x->name();

	std::wcout << "big_type will allow isb values? " << ( big_type::allows_any_type(isb) ? "TRUE" : "FALSE" ) << std::endl;

	std::wcout << "types set :" << types_s.str() << std::endl;
	std::wcout << "types allowed : " << types_a.str() << std::endl;
}

int main(int argc, char **argv) {

	using namespace yyy;

	using F = function<term>;
	using L = literal_t;
	using Sv = symbol::var;
	using Sr = symbol::ref;

	using R = rule<term>;
	using P = predicate<term>;

	maybe<literal_t> lit;

	setlocale(LC_CTYPE, "");

	F f = F(OP_SAME) << ( F(OP_OR) << Sv(L"what") ) << L(666);

	for(int i = 0; i < argc; i++)
		f << L(atoi(argv[i]));

	f << f;

	f << f;

	std::wcout << "f := " << (std::wstring)f << std::endl;

	auto g = define_grammar<term>();

	std::wcout << "g := " << P().any().star().str() << std::endl;

	std::wcout << "R r := " << std::endl;

	std::wstringstream ss;
	ss << L"FUCK";

	Sv taco(ss.str());

	R r = R(OP_ANY) << P(L(666)).by_type().plus() << P(F(OP_AND) << L(1337) << Sv(L"eax")) << P(Sr(L"satan")).qm() << P().any().star() << P().end();

	std::wcout << "rule := " << r.str() << std::endl;

	do_yyy();

	return 0;
}
