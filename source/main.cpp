#include "FileList.hpp"
#include "String.h"

int main()
{
	
	FileList<int> fl;
	fl.open(String("n3m.bin"));
	for (size_t i = 0; i < 30000; i++)
	{
		fl.insert(rand());
	}
	fl.show();

	clock_t start = clock();
	fl.sort();
	start = clock() - start;
	fl.show();
	std::cout << "Sorted: " << std::boolalpha << fl.checkSorted() <<
		"\nTime: " << (double)start/CLOCKS_PER_SEC << std::endl;
	fl.save();
	
	return 0;
}
