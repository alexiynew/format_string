#include <iostream>

#include "format_string.h"

int main ()
{
	std::cout << make_string("one {0} two {1} three {2}", 1, 0.42, "string") << std::endl;
	std::cout << make_string("one {0} two {0} three {0}", "string") << std::endl;
	std::cout << make_string("one {{{0}}}", "string") << std::endl;
}