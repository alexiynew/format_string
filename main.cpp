#include <iostream>

#include "format_string.h"

int main ()//doxXeEfF
{
	std::cout << make_string("one {0} two {1} three {2}", 1, 0.42, "string") << std::endl;
	std::cout << make_string("one {} two {} three {}", 1, 0.42, "string") << std::endl;
	std::cout << make_string("one {0} two {0} three {0}", "string") << std::endl;
	std::cout << make_string("one {{{0}}}", "string") << std::endl;
	std::cout << make_string("{0:d}", 42) << std::endl;
	std::cout << make_string("{0:o}", 42) << std::endl;
	std::cout << make_string("{0:x}", 42) << std::endl;
	std::cout << make_string("{0:X}", 42) << std::endl;
	std::cout << make_string("{0:x}", 0.123456789) << std::endl;
	std::cout << make_string("{0:X}", 0.123456789) << std::endl;
	std::cout << make_string("{0:e}", 0.123456789) << std::endl;
	std::cout << make_string("{0:E}", 0.123456789) << std::endl;
	std::cout << make_string("{0:f}", 0.123456789) << std::endl;
	std::cout << make_string("{0:F}", 0.123456789) << std::endl;
	
	std::cout << "{0:.10f}" << make_string(" {0:.10f}", 0.123456789) << std::endl;
	std::cout << "{0:.1f}" << make_string("  {0:.1f}", 0.123456789) << std::endl;
	std::cout << "{0:.0f}" << make_string("  {0:.0f}", 0.123456789) << std::endl;
	std::cout << "{0:.2f}" << make_string("  {0:.2f}", 0.123456789) << std::endl;
	
	std::cout << "{0:*10f}" << make_string(" {0:*10f}", 0.123456789) << std::endl;
	std::cout << "{0:*1f}" << make_string("  {0:*1f}", 0.123456789) << std::endl;
	std::cout << "{0:*20f}" << make_string(" {0:*20f}", 0.123456789) << std::endl;
	std::cout << "{0:*8f}" << make_string("  {0:*8f}", 0.123456789) << std::endl;
	
	std::cout << "{0:*10.1f}" << make_string("  {0:*10.1f}", 0.123456789) << std::endl;
	std::cout << "{0:*1.0f}" << make_string("   {0:*1.0f}", 0.123456789) << std::endl;
	std::cout << "{0:*20.10f}" << make_string(" {0:*20.10f}", 0.123456789) << std::endl;
	std::cout << "{0:*8.8f}" << make_string("   {0:*8.8f}", 0.123456789) << std::endl;
	
	std::cout << "{0:_<10}" << make_string("  {0:_<10}", 42) << std::endl;
	std::cout << "{0:_>10}" << make_string("  {0:_>10}", 42) << std::endl;
	std::cout << "{0:_=10}" << make_string("  {0:_=10}", 42) << std::endl;
	
	std::cout << "{0:_<-10}" << make_string("  {0:_<-10}", 42) << std::endl;
	std::cout << "{0:_>+10}" << make_string("  {0:_>+10}", 42) << std::endl;
	std::cout << "{0:_=010}" << make_string("  {0:_=010}", 42) << std::endl;
	std::cout << "{0:_=#10X}" << make_string(" {0:_#10X}", 42) << std::endl;
	std::cout << "{0:_=+10}" << make_string("  {0:_=+10}", 42) << std::endl;

	
}