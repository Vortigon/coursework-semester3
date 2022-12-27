#include "FileList.hpp"

int main()
{
	FileList<int> fl;
	fl.open(String("test.bin"));

	//fl.insert(12);
	//fl.insert(11);
	//fl.insert(10);
	
	fl.show();

	fl.sort();
	fl.show();
	return 0;
}
