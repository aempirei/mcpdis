#include <climits>
#include <cstdint>
#include <cstdlib>
#include <cstring>

#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

#include <either.template.hh>
#include <maybe.template.hh>

// main
//
//

typedef either<long,std::string> long_or_string;
typedef either<long_or_string,bool> long_or_string_or_bool;

#define XXX(a,b,c,d)\
	\
	((a).b<c>() ? d:' ')

#define YYY(a,c)\
	\
	XXX(a,has_type,c,'*')				\
	<< XXX(a,has,c,'#')				\
	<< "either<either<long,std::string>,bool> -> "	\
	<< std::setw(15)				\
	<< #c						\
	<< ":: "					\
	<< abc.str<c>()

#define ZZZ(a,b)\
	\
	std::cout << YYY(a,b) << std::endl


void do_abc(long_or_string_or_bool& abc) {

	std::wcout << L"(std::wstring)abc := " << (std::wstring)abc << std::endl << std::endl;

	std::cout << YYY(abc,bool) << std::endl;
	std::cout << YYY(abc,char) << std::endl;
	std::cout << YYY(abc,long) << std::endl;
	std::cout << YYY(abc,std::string) << std::endl;
	std::cout << YYY(abc,std::nullptr_t) << std::endl;

	std::cout << std::endl;
}

#undef XXX
#undef YYY

int main(int argc, char **argv) {

	if(argc != 2 || (argc == 2 && strcmp(argv[1], "-h") == 0)) {
		std::cerr << std::endl << "usage: " << argv[0] << " [options]" << std::endl << std::endl;
		return -1;
	}

	// maybe<long>
	//
	//

	maybe<long> a = atol(argv[1]);

	if(a.xptr)
		std::cout << "x := " << *a.xptr << std::endl;
	else
		std::cout << "x := nullptr" << std::endl;

	std::cout << "maybe<long> -> std::string    :: " << a.str<std::string>()    << std::endl;
	std::cout << "maybe<long> -> long           :: " << a.str<long>()           << std::endl;
	std::cout << "maybe<long> -> bool           :: " << a.str<bool>()           << std::endl;
	std::cout << "maybe<long> -> std::nullptr_t :: " << a.str<std::nullptr_t>() << std::endl;
	std::cout << std::endl;

	std::cout << "( ab )" << std::endl << std::endl;

	// long_or_string
	//
	//

	long_or_string ab;

	std::cout << "either<long,std::string> -> long           :: " << ab.str<long>()           << std::endl;
	std::cout << "either<long,std::string> -> std::string    :: " << ab.str<std::string>()    << std::endl;
	std::cout << "either<long,std::string> -> bool           :: " << ab.str<bool>()           << std::endl;
	std::cout << "either<long,std::string> -> std::nullptr_t :: " << ab.str<std::nullptr_t>() << std::endl;
	std::cout << std::endl;
	
	ab = a;

	std::cout << "either<long,std::string> -> long           :: " << ab.str<long>()           << std::endl;
	std::cout << "either<long,std::string> -> std::string    :: " << ab.str<std::string>()    << std::endl;
	std::cout << "either<long,std::string> -> bool           :: " << ab.str<bool>()           << std::endl;
	std::cout << "either<long,std::string> -> std::nullptr_t :: " << ab.str<std::nullptr_t>() << std::endl;
	std::cout << std::endl;

	ab = "DICK";

	std::cout << "either<long,std::string> -> long           :: " << ab.str<long>()           << std::endl;
	std::cout << "either<long,std::string> -> std::string    :: " << ab.str<std::string>()    << std::endl;
	std::cout << "either<long,std::string> -> bool           :: " << ab.str<bool>()           << std::endl;
	std::cout << "either<long,std::string> -> std::nullptr_t :: " << ab.str<std::nullptr_t>() << std::endl;
	std::cout << std::endl;

	// long_or_string_or_bool
	//
	//

	long_or_string_or_bool abc;

	std::cout << "( abc )" << std::endl << std::endl;

	do_abc(abc);

	abc = maybe<bool>(true);

	do_abc(abc);

	abc = maybe<long>(666);

	do_abc(abc);

	abc = std::string("COCK");

	do_abc(abc);

	abc.include(maybe<long>(666));

	std::wcout << "abc := " << (std::wstring)abc << std::endl;

	return 0;
}
