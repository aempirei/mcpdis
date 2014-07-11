#include <yyy.hh>
#include <iostream>
#include <iomanip>

namespace yyy {

	using L = literal_t;
	using Sv = symbol::var;
	using Sr = symbol::ref;

	using F = function<term>;
	using R = rule<term>;
	using P = predicate<term>;
	using G = grammar<term>;
	using RS = rules<term>;

	template <typename T> grammar<T> define_grammar() {
		return G {
			{ L"combo", {
					    R(OP_THIS) << P(Sr(L"satan")).by_ref(),
						    R(OP_OR) << P(Sr(L"combo")).by_ref(),
						    R(OP_ANY) << *P() << !P(Sv(L"noway")) << P().end()
				    } },
				{ L"any", { R(OP_ANY) << *P() << P().end() } },
				{ L"satan", { R(OP_AND) << P(L(1337)) << P(Sv(L"eax")) } },
				{ L"hello", { R(OP_ANY) << +P(L(666)).by_type() << P(Sr(L"satan")).qm() << P(F(OP_AND)).by_op() << *P() << P().end() } }
		};
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

	std::wcout << "isb<int> :: " << ( e_int_str_bool::allows_type<int>() ) << ' ' << ( isb.contains_type<int>() ) << std::endl;
	std::wcout << "isb<int> == " << ( isb.contains_value(4) ) << ' ' << ( isb.contains_value(5) ) << std::endl;
	std::wcout << "isb<bool> :: " << ( isb.allows_type<bool>() ) << ' ' << ( isb.contains_type<bool>() ) << std::endl;
	std::wcout << "isb<bool> == " << ( isb.contains_value(true) ) << ' ' << ( isb.contains_value(false) ) << std::endl;
	std::wcout << "isb<wstr> :: " << ( isb.allows_type<wstr>() ) << ' ' << ( isb.contains_type<wstr>() ) << std::endl;
	std::wcout << "isb<wstr> == " << ( isb.contains_value(wstr(L"dicks")) ) << ' ' << ( isb.contains_value(wstr(L"fag")) ) << std::endl;

	std::wstringstream types_s;
	std::wstringstream types_a;

	using big_type = choice<bool,char,std::wstring,std::type_info>::type;

	for(auto x : isb.get_types()) types_s << L' ' << typeoperator[x];
	for(auto x : isb.allowed_types()) types_a << L' ' << typeoperator[x];

	std::wcout << "big_type will allow isb values? " << ( big_type::allows_any_type(isb) ? "TRUE" : "FALSE" ) << std::endl;

	std::wcout << "types set :" << types_s.str() << std::endl;
	std::wcout << "types allowed : " << types_a.str() << std::endl;
}

int main(int argc, char **argv) {

	using namespace yyy;

	using F = function<term>;

	maybe<literal_t> lit;

	setlocale(LC_CTYPE, "");

	F f = F(OP_THIS) << ( F(OP_OR) << Sv(L"what") ) << L(666);

	for(int i = 0; i < argc; i++)
		f << L(atoi(argv[i]));

	f << f;

	f << f;

	std::wcout << type_check<int,int,bool,std::wstring>() << std::endl;
	std::wcout << type_check<bool,int,bool,std::wstring>() << std::endl;
	std::wcout << type_check<std::wstring,int,bool,std::wstring>() << std::endl;
	std::wcout << type_check<std::nullptr_t,int,bool,std::wstring>() << std::endl;

	std::wcout << "f := " << (std::wstring)f << std::endl;

	auto g = define_grammar<term>();

	for(const auto& entry : g) {

		const auto& name = entry.first;

		for(const auto& r : entry.second)
			std::wcout << name << " := " << r.str() << std::endl;

		std::wcout << std::endl;
	}

	unrestricted_type x;

	x.assign(std::wstring(L"what is going on"));
	x.assign(true);

	std::wcout << x.get<std::wstring>() << std::endl;
	std::wcout << x.get<bool>() << std::endl;

	x.unassign<std::wstring>();
	x.unassign<bool>();

	do_yyy();

	return 0;
}
