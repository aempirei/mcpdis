#include <yyy.hh>

using namespace yyy;

using F = function<term>;
using L = literal_t;
using S = symbol;

template <typename T> grammar<T> define_grammar() {
	grammar<T> g;
	predicate<T> p;
	return g;
}

int main(int argc, char **argv) {

	maybe<literal_t> lit;

	setlocale(LC_CTYPE, "");

	F f = F(OP_AND) << ( F(OP_OR) << S(L"what") ) << L(666);

	for(int i = 0; i < argc; i++)
		f << L(atoi(argv[i]));

	f << f;

	f << f;

	std::wcout << (std::wstring)f << std::endl;

	auto g = define_grammar<term>();

	return 0;
}
