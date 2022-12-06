#include "FileList.hpp"

int main()
{
	FileList<int> fl;
	fl.open(String("test.bin"));
	fl << std::ios::pos_type(56);
	return 0;
}
