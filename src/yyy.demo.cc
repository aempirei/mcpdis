#include <yyy.hh>
#include <typeinfo>
#include <typeindex>

namespace yyy {
	template <typename T> grammar<T> define_grammar() {
		grammar<T> g;
		return g;
	}
}

template <zzz::symbol::type S> void fu(zzz::basic_symbol<wchar_t,S> x) {
	std::wcout << "basic_symbol<" << (int)decltype(x)::type_enum << "> := " << x << std::endl;
}

constexpr const wchar_t* boolstr (bool b) {
	return b ? L"is" : L"isn't";
}

template <typename...Args> struct hand {
	using codomain = hand<Args...>;
};
template <typename...Left,typename...Right> struct hand<hand<Left...>,hand<Right...>> {
	using codomain = hand<Left...,Right...>;
};

template <typename Head> std::wstring freezy(hand<Head>) {
	std::wstringstream ss;
	ss << typeid(Head).name();
	return ss.str();
}

template <typename Head,typename...Tail> std::wstring freezy(hand<Head,Tail...>) {
	return freezy(hand<Head>()) + L' ' + freezy(hand<Tail...>());
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
	isb.insert(false);

	i.assign(int());

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

	std::wcout << "type is : " << typeid(const char*).name() << std::endl;
	std::wcout << "type is : " << typeid(typeid(int).name()).name() << std::endl;

	std::wstringstream types_s;
	std::wstringstream types_a;

	isb.clear();

	for(const std::type_info* x : isb.get_types()) types_s << L' ' << x->name();
	for(const std::type_info* x : isb.allowed_types()) types_a << L' ' << x->name();

	std::wcout << "types set :" << types_s.str() << ' ' << boolstr(isb.empty()) << " empty" << std::endl;
	std::wcout << "types allowed : " << types_a.str() << ' ' << boolstr(isb.allowed_types().empty()) << " empty" << std::endl;

	using left_hand = hand<int,bool,void*>;
	using right_hand = hand<wchar_t,const char **,float[16]>;
	using both_hands = hand<left_hand,right_hand>;

	std::wcout << "     left hand :: " << freezy(left_hand()) << std::endl;
	std::wcout << "    right hand :: " << freezy(right_hand()) << std::endl;
	std::wcout << "    both hands :: " << freezy(both_hands()) << std::endl;
	std::wcout << "(co)both hands :: " << freezy(both_hands::codomain()) << std::endl;

	std::wcout << "type id of decltype(isb)::codomain :: " << typeid(zzz::either<zzz::maybe<void*>,zzz::either<zzz::maybe<int>,zzz::maybe<char*>>>::codomain).name() << std::endl;
	std::wcout << "type id of decltype(isb):: " << typeid(zzz::either<zzz::maybe<void*>,zzz::either<zzz::maybe<int>,zzz::maybe<char*>>>).name() << std::endl;
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
