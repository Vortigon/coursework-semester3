#include "FileList.hpp"

int main()
{
	FileList<int> fl;
	fl.open(String("test.bin"));

	std::cout << -1 % 3 << " " << 3 % 3 << " " << 0 % 3 << std::endl;

	fl.insert(12);
	fl.insert(11);
	fl.insert(10, 0);
	fl.insert(15, 1);
	fl.insert(14, 1);
	fl.insert(13);
	
	fl.show();
	fl.sort();

	fl.show();

	fl.remove(0);
	fl.show();
	fl.remove(0);
	fl.show();
	fl.remove();
	fl.show();
	fl.remove(2);
	fl.show();
	fl.remove(4);
	fl.show();
	fl.save();
	return 0;

}
