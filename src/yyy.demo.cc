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

	using e_int = zzz::maybe<int>;
	using e_str = zzz::maybe<std::wstring>;
	using e_int_str = zzz::either<e_int,e_str>;

	zzz::nothing nope;
	e_int i;
	e_str s;
	e_int_str is;
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
