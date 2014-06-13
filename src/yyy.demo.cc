#include <yyy.hh>

using namespace yyy;

/*
template <typename T> void define_grammar(grammar<T>&) {
}
*/

int main(int, char **) {

	maybe<literal_t> lit;

	function<term> f = function<term>(OP_AND) << 666 << symbol(L"what");
	f << f;
	f << f;

	std::wcout << (std::wstring)f << std::endl;

	//grammar<term> g;

	//define_grammar(g);
	return 0;
}
