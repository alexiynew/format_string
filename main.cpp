#include <iostream>

#include "format_string.h"

#define TEST(M, S)														\
	try {																\
		if ((M) == (S)) {												\
			std::cout << "OK" << std::endl;								\
		} else {														\
			std::cout << "FAIL " << (M) << " != " << (S) << std::endl;	\
		}																\
	} catch (std::exception& e) { 										\
		std::cout << "ERROR: " << e.what() << std::endl; 				\
	}

#define ERROR_TEST(M, E)													\
	try {																	\
		(M);																\
		std::cout << "FAIL string constructed: " << (M) << std::endl;		\
	} catch (std::exception& e) {											\
		if (std::string(e.what()) == (E)) {									\
			std::cout << "OK" << std::endl;									\
		} else {															\
			std::cout << "FAIL " << e.what() << " != " << (E) << std::endl;	\
			std::cout << "ERROR: " << e.what() << std::endl; 				\
		}																	\
	}



int main ()
{
	std::cout << "simple   -------------------" << std::endl;
	TEST(make_string("{0}", 1), "1");
	TEST(make_string("{0}", 1.123456789), "1.12346");
	TEST(make_string("{0}", "string"), "string");
	TEST(make_string("{{{0}}}", "string"), "{string}");
	TEST(make_string("{0}}}", "string"), "string}");
	TEST(make_string("{{{0}", "string"), "{string");

	ERROR_TEST(make_string("{{0}}}", "string"), "Single '}' in format string");
	ERROR_TEST(make_string("{{{0}}", "string"), "Single '}' in format string");
	ERROR_TEST(make_string("0}}}", "string"), "Single '}' in format string");
	ERROR_TEST(make_string("{{{0", "string"), "'}' expected");
	std::cout << "----------------------------" << std::endl << std::endl;

	std::cout << "indexing -------------------" << std::endl;
	TEST(make_string("manual {2}{1}{0}", 1, 2, 3), "manual 321");
	TEST(make_string("auto {}{}{}{}", 'a', 'u', 't', 'o'), "auto auto");

	ERROR_TEST(make_string("{2}", 1), "Index 2 out of range");
	ERROR_TEST(make_string("{0}{}", "should", "fail"), "Cannot switch from manual field specification mode to automatic field numbering");
	ERROR_TEST(make_string("{}{0}", "should", "fail"), "Cannot switch from automatic field numbering to manual field specification");
	std::cout << "----------------------------" << std::endl << std::endl;

	std::cout << "type specification  --------" << std::endl;
	TEST(make_string("{0:d}", 42), "42");
	TEST(make_string("{0:o}", 42), "52");
	TEST(make_string("{0:x}", 42), "2a");
	TEST(make_string("{0:X}", 42), "2A");
	TEST(make_string("{0:x}", 0.123456789), "0.123457");
	TEST(make_string("{0:X}", 0.123456789), "0.123457");
	TEST(make_string("{0:e}", 0.123456789), "1.234568e-001");
	TEST(make_string("{0:E}", 0.123456789), "1.234568E-001");
	TEST(make_string("{0:f}", 0.123456789), "0.123457");
	TEST(make_string("{0:F}", 0.123456789), "0.123457");

	ERROR_TEST(make_string("{0:dd}", 1), "Index 2 out of range");
	ERROR_TEST(make_string("{0:a}", 1), "Index 2 out of range");
	std::cout << "----------------------------" << std::endl << std::endl;
	
}