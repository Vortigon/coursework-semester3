#include "FileList.hpp"
#include "String.h"

int main()
{
	
	FileList<int> fl;
	fl.open(String("test.bin"));

/*
	fl.insert(11);
	fl.insert(14);
	fl.insert(48);
	fl.insert(57);

	fl.insert(38);
	fl.insert(54);
	fl.insert(79);
	fl.insert(88);
	
	fl.insert(10);
	*/
	fl.show();
	fl.mergeTest(8, 88, 148);
	
	fl.show();

	fl.debugFilePrint();
	fl.save();
	
	return 0;
}
