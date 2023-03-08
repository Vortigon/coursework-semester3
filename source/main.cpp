#include "FileList.hpp"
#include "String.h"

int main()
{
	
	FileList<int> fl;
	fl.open(String("test.bin"));


	fl.insert(11);
	fl.insert(14);
	fl.insert(48);
	fl.insert(57);

	fl.insert(38);
	fl.insert(54);
	fl.insert(79);
	fl.insert(88);
	/*
	11 14 38 48 54 57 79 88
	*/
	fl.insert(10);
	

	fl.show();
	//fl.debugFilePrint();

	fl.sort();
	//fl.insertTest(8, 88, 4);
	//fl.mergeTest(8, 88, 148);
	
	fl.show();

	//fl.debugFilePrint();
	fl.save();
	
	return 0;
}
