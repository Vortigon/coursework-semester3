#include "FileList.hpp"
#include "String.h"

int main()
{
	
	FileList<int> fl;
	fl.open(String("test.bin"));
	//fl.insert(5);
	//fl.show();
/*
	fl.insert(11);
	fl.insert(14);
	fl.insert(48);
	fl.insert(57);

	fl.insert(38);
	fl.insert(54);
	fl.insert(79);
	fl.insert(88);
	*/
	/*
	11 14 38 48 54 57 79 88
	*/
	//fl.insert(10);
	
	/*for (size_t i = 0; i < 80; i++)
	{
		fl.insert(rand()%300);
	}
	fl.show();
	//fl.debugFilePrint();

	fl.sort();
	//fl.insertTest(8, 88, 4);
	*/
	fl.show();
	fl.mergeTest(8, 808, 8);
	
	fl.show();

	//fl.debugFilePrint();
	fl.save();
	
	return 0;
}
