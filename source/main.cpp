//#include "String.h"
//#include "FileList.hpp"
#include <ctime>
#include "Menu.h"

int main()
{
	Menu::getInstance().start();
	/*
	FileList<int> fl;
	fl.open(String("test.bin"));
	
	for (size_t i = 0; i < 1500; ++i)
	{
		fl.insert(rand()%1000);
	}

	fl.show();
	clock_t start = clock();
	fl.sort(false);
	start = clock() - start;
	fl.checkNextLinkage();
	fl.checkPreviousLinkage();
	fl.show();
	std::cout << "Sorted: " << std::boolalpha << fl.checkSorted() <<
		"\nTime: " << (double)start/CLOCKS_PER_SEC << " s" << std::endl;
	
	fl.save();
	return 0;
	*/
}
