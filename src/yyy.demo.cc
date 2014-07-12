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

template <typename T> void print_type() {

	using namespace yyy::type;

	std::wcout << "type(" << to_list<T>::size << ") <" << to_list<T>::str() << "> : " << typeid(T).name() << std::endl;
}

template <typename T> void print_types() {
	print_type<T>();
}

template <typename T, typename U, typename...Args> void print_types() {
	print_types<T>();
	print_types<U,Args...>();
}

template <typename T,typename U> void typecheck() {
	using namespace yyy;
	std::wcout << typeoperator[type::index<T>()] << ' ' << OP_IN << ' ';
	std::wcout << U::to_list::str() << " := ";
	std::wcout << (type::contains<T,U>::eval ? 'T' : 'F') << std::endl; 
}

int main(int argc, char **argv) {

	using namespace yyy;

	using F = function<term>;

	maybe<literal_t> lit;

	setlocale(LC_CTYPE, "");

	std::wcout << std::endl << "running " << argv[0] << "..." << std::endl << std::endl;

	F f = F(OP_THIS) << ( F(OP_OR) << Sv(L"what") ) << L(666);

	for(int i = 0; i < argc; i++)
		f << L(atoi(argv[i]));

	f << f;

	f << f;

	std::wcout << "f := " << (std::wstring)f << std::endl;

	auto g = define_grammar<term>();

	for(const auto& entry : g) {

		const auto& name = entry.first;

		for(const auto& r : entry.second)
			std::wcout << name << " := " << r.str() << std::endl;

		std::wcout << std::endl;
	}

	using x_type = type::container<int,bool,char,void *>;
	using x2_type = type::concat<x_type,type::reverse<x_type>>;
	using x4_type = type::concat<x2_type,x2_type>;
	using reversed_type = type::reverse<x2_type>;
	using filtered_type = type::filter<int,type::filter<bool,x2_type>>;
	using unique_type = type::unique<x4_type>;

	print_types<x_type,x2_type,x4_type,reversed_type,filtered_type,unique_type>();

	unique_type y(66,true,'a',nullptr);
	unique_type z;

	std::wcout << L"unique-type y := " << y.str() << std::endl;
	std::wcout << L"unique-type z := " << z.str() << std::endl;

	typecheck<int,unique_type>();
	typecheck<float,unique_type>();

	/*
	type::map::type x;

	type::map::assign(x, std::wstring(L"what is going on"), bool(true), int(66));
	if(type::map::contains_values(x, std::wstring(L"what is going on"), bool(true), int(66))) {
		std::wcout << "contains first set of values" << std::endl;
	}

	if(type::map::contains_values(x, std::wstring(L"what is going on"), bool(false), int(66))) {
		std::wcout << "contains second set of values" << std::endl;
	}

	if(type::map::contains_values(x, std::wstring(L"what is going on"), bool(true), int(66), char('a'))) {
		std::wcout << "contains third set of values" << std::endl;
	}

	if(type::map::contains_values(x,true,true,true)) {
		std::wcout << "contains fourth set of values" << std::endl;
	}

	auto t = type::map::get<int,bool,bool>(x);

	std::wcout << typeid(std::wstring).name() << ": " << type::map::get<std::wstring>(x) << std::endl;
	std::wcout << typeid(bool).name() << ": " << (type::map::get<bool>(x) ? "TRUE" : "FALSE") << std::endl;
	std::wcout << typeid(int).name() << ": " << type::map::get<int>(x) << std::endl;

	std::wcout << std::get<0>(t) << L' ' << std::get<1>(t) << std::endl;

	type::map::unassign<int,bool,std::wstring>(x);

	std::wcout << (x.empty() ? L"x is empty" : L"x is not empty") << std::endl;

	do_yyy();

	*/

	return 0;
}
