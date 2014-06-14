#include <yyy.hh>

using namespace yyy;

using F = function<term>;

template <typename T> grammar<T> define_grammar() {
	grammar<T> g;
	return g;
}

int main(int argc, char **argv) {

	maybe<literal_t> lit;

	setlocale(LC_CTYPE, "");

	F f = F(OP_AND) << symbol(L"what");

	for(int i = 0; i < argc; i++)
		f << atoi(argv[i]);

	f << f;
	f << f;

	std::wcout << (std::wstring)f << std::endl;

	auto g = define_grammar<term>();

	return 0;
}
