#include "FileList.hpp"
#include "String.h"

int main()
{
	
	FileList<int> fl;
	fl.open(String("test.bin"));
	for (size_t i = 0; i < 1500; i++)
	{
		fl.insert(rand()%3000);
	}
	fl.show();
	fl.sort();
	fl.show();
	fl.save();
	
	return 0;
}
