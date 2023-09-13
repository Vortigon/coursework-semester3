#include "FileList.hpp"
#include "String.h"
#include <ctime>

int main()
{
	FileList<int> fl;
	fl.open(String("test.bin"));
	for (size_t i = 0; i < 15000; ++i)
	{
		fl.insert(rand());
	}

	//fl.show();
	std::cout << "LINKAGE: " << std::boolalpha << fl.checkPreviousLinkage() << std::endl;
	clock_t start = clock();
	fl.sort();
	start = clock() - start;
	//fl.show();
	std::cout << "Sorted: " << std::boolalpha << fl.checkSorted() <<
		"\nTime: " << (double)start/CLOCKS_PER_SEC << " s" << std::endl;
	std::cout << "LINKAGE: " << std::boolalpha << fl.checkPreviousLinkage() << std::endl;
	fl.save();
	
	return 0;
}
