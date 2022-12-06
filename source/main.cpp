#include "FileList.hpp"

int main()
{
	FileList<int> fl;
	fl.open(String("test.bin"));
	//fl.insert(10);
	//fl.insert(11);
	fl.insert(12);
	return 0;
}
