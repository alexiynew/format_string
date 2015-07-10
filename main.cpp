#include <iostream>

#include "format_string.h"

#define TEST(M, S)															\
	try {																	\
		if ((M) == (S)) {													\
			std::cout << "\tOK" << std::endl;								\
		} else {															\
			std::cout << "\tFAIL " << (M) << " != " << (S) << std::endl;	\
		}																	\
	} catch (std::exception& e) { 											\
		std::cout << "\tERROR: " << e.what() << std::endl; 					\
	}

#define ERROR_TEST(M, E)														\
	try {																		\
		(M);																	\
		std::cout << "\tFAIL string constructed: " << (M) << std::endl;			\
	} catch (std::exception& e) {												\
		if (std::string(e.what()) == (E)) {										\
			std::cout << "\tOK" << std::endl;									\
		} else {																\
			std::cout << "\tFAIL " << e.what() << " != " << (E) << std::endl;	\
			std::cout << "\tERROR: " << e.what() << std::endl; 					\
		}																		\
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

	ERROR_TEST(make_string("{0:}", 1), "Empty format string after ':'");
	ERROR_TEST(make_string("{0:rr}", 1), "Wrong field format");
	ERROR_TEST(make_string("{0:a}", 1), "Wrong field format");
	std::cout << "----------------------------" << std::endl << std::endl;
	

	std::cout << "precision  -----------------" << std::endl;
	TEST(make_string("{0:f}", 0.123456789), "0.123457");
	TEST(make_string("{0:.1f}", 0.123456789), "0.1");
	TEST(make_string("{0:.10f}", 0.123456789), "0.1234567890");
	TEST(make_string("{0:.20f}", 0.123456789), "0.12345678900000000000");
	TEST(make_string("{0:.6f}", 0.123456789), "0.123457");
	TEST(make_string("{0:.5f}", 0.123456789), "0.12346");

	TEST(make_string("{:.5} {}", 123456789, "no effect"), "123456789 no effect");
	TEST(make_string("{:.5} {}", "some string", "no effect"), "some string no effect");

	TEST(make_string("{0:.1}", 12.123456789), "?");
	TEST(make_string("{0:.2}", 12.123456789), "?");
	TEST(make_string("{0:.3}", 12.123456789), "?");
	TEST(make_string("{0:.4}", 12.123456789), "?");
	TEST(make_string("{0:.5}", 12.123456789), "?");

	TEST(make_string("{0:.1f}", 12.123456789), "?");
	TEST(make_string("{0:.2f}", 12.123456789), "?");
	TEST(make_string("{0:.3f}", 12.123456789), "?");
	TEST(make_string("{0:.4f}", 12.123456789), "?");
	TEST(make_string("{0:.5f}", 12.123456789), "?");

	TEST(make_string("{0:.1e}", 12.123456789), "?");
	TEST(make_string("{0:.2e}", 12.123456789), "?");
	TEST(make_string("{0:.3e}", 12.123456789), "?");
	TEST(make_string("{0:.4e}", 12.123456789), "?");
	TEST(make_string("{0:.5e}", 12.123456789), "?");

	ERROR_TEST(make_string("{0:.}", 1), "Precision value expected after '.'");
	std::cout << "----------------------------" << std::endl << std::endl;

	std::cout << "width  ---------------------" << std::endl;
	TEST(make_string("{0:0}", 12), "12");
	TEST(make_string("{0:1}", 12), "12");
	TEST(make_string("{0:5}", 12), "12   ");
	TEST(make_string("{0:10}", 12), "12        ");
	TEST(make_string("{0:-10} {1}", -12, "(- is sign)"), "-12        (- is sign)");
	std::cout << "----------------------------" << std::endl << std::endl;

	std::cout << "width and precision --------" << std::endl;
	TEST(make_string("{0:10f}", 0.123456789), "0.123457  ");
	TEST(make_string("{0:10.1f}", 0.123456789), "0.1       ");
	TEST(make_string("{0:10.10f}", 0.123456789), "0.1234567890");
	TEST(make_string("{0:1.20f}", 0.123456789), "0.12345678900000000000");
	TEST(make_string("{0:+20.6f}", 0.123456789), "+0.123457           ");
	TEST(make_string("{0:-20.6f}", -0.123456789), "-0.123457           ");
	TEST(make_string("{0:8.5f}", 0.123456789), "0.12346 ");

	ERROR_TEST(make_string("{0:10.}", 1), "Precision value expected after '.'");
	std::cout << "----------------------------" << std::endl << std::endl;

	std::cout << "flag   ---------------------" << std::endl;
	TEST(make_string("{0:-}", -12), "-12");
	TEST(make_string("{0:-}", 12), "12");
	TEST(make_string("{0:+}", -12), "-12");
	TEST(make_string("{0:+}", 12), "+12");
	TEST(make_string("{0:#X}", 42), "0X2A");
	TEST(make_string("{0:#X}", -42), "0XFFFFFFD6");
	TEST(make_string("{0:#x}", 42), "0x2a");
	TEST(make_string("{0:#x}", -42), "0xffffffd6");
	TEST(make_string("{0:#x}", 12.42), "12.4200");
	TEST(make_string("{0:#X}", -12.42), "-12.4200");
	TEST(make_string("{0:0}", 12), "12");
	TEST(make_string("{0:00}", 12), "12");
	TEST(make_string("{0:010}", 12), "?");
	TEST(make_string("{0:010.5}", 12.123456789), "?");
	std::cout << "----------------------------" << std::endl << std::endl;

	std::cout << "align  ---------------------" << std::endl;
	TEST(make_string("{0:<0}", 1), "1");
	TEST(make_string("{0:<1}", 1), "1");
	TEST(make_string("{0:<2}", 1), "1 ");
	TEST(make_string("{0:<3}", 1), "1  ");
	TEST(make_string("{0:<4}", 1), "1   ");
	TEST(make_string("{0:<5}", 1), "1    ");

	TEST(make_string("{0:>0}", 1), "1");
	TEST(make_string("{0:>1}", 1), "1");
	TEST(make_string("{0:>2}", 1), " 1");
	TEST(make_string("{0:>3}", 1), "  1");
	TEST(make_string("{0:>4}", 1), "   1");
	TEST(make_string("{0:>5}", 1), "    1");

	TEST(make_string("{0:=+0}", 1), "+1");
	TEST(make_string("{0:=+1}", 1), "+1");
	TEST(make_string("{0:=+2}", 1), "+1");
	TEST(make_string("{0:=+3}", 1), "+ 1");
	TEST(make_string("{0:=+4}", 1), "+  1");
	TEST(make_string("{0:=+5}", 1), "+   1");

	TEST(make_string("{0:^0}", 1), "?");
	TEST(make_string("{0:^1}", 1), "?");
	TEST(make_string("{0:^2}", 1), "?");
	TEST(make_string("{0:^3}", 1), "?");
	TEST(make_string("{0:^4}", 1), "?");
	TEST(make_string("{0:^5}", 1), "?");

	std::cout << "----------------------------" << std::endl << std::endl;

	std::cout << "align  ---------------------" << std::endl;
	TEST(make_string("{0::<5}", 1), "1::::");
	TEST(make_string("{0:\'<5}", 1), "1''''");
	TEST(make_string("{0:\"<5}", 1), "1\"\"\"\"");
	TEST(make_string("{0:1<5}", 1), "11111");
	TEST(make_string("{0:^<5}", 1), "1^^^^");
	TEST(make_string("{0:\t<5}", 1), "1\t\t\t\t");

	TEST(make_string("{0:&>5}", 1), "&&&&1");
	TEST(make_string("{0:@>5}", 1), "@@@@1");
	TEST(make_string("{0:!>5}", 1), "!!!!1");
	TEST(make_string("{0:$>5}", 1), "$$$$1");
	TEST(make_string("{0:g>5}", 1), "gggg1");
	TEST(make_string("{0:Q>5}", 1), "QQQQ1");

	TEST(make_string("{0:>=+5}", 1), "+>>>1");
	TEST(make_string("{0:+=+5}", 1), "++++1");
	TEST(make_string("{0:==+5}", 1), "+===1");
	TEST(make_string("{0:-=+5}", 1), "+---1");
	TEST(make_string("{0:(=+5}", 1), "+(((1");

	TEST(make_string("{0:=^5}", 1), "?");
	TEST(make_string("{0:.^5}", 12), "?");
	TEST(make_string("{0:,^5}", 123), "?");
	TEST(make_string("{0:/^5}", 1234), "?");
	TEST(make_string("{0:\\^5}", 1234), "?");

	ERROR_TEST(make_string("{0:d}=+5} {1}", 1, "error"), "Single '}' in format string");
	std::cout << "----------------------------" << std::endl << std::endl;
}