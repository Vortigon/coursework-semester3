#include "FileList.hpp"
#include "String.h"

int main()
{
	
	FileList<int> fl;
	fl.open(String("test.bin"));

	fl.insert(12);
	fl.insert(11);
	fl.insert(10);

	fl.insert(15);
	fl.insert(14);
	fl.insert(13);

	fl.insert(14);
	fl.insert(15);
	fl.insert(16);
	
	fl.show();
	fl.sort();

	fl.show();

	/*
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

	fl.replace(3, 0);
	fl.show();
	fl.replace(5, -1);
	fl.show();
	fl.replace(7, 4);
	fl.show();
	fl.replace(9, 0);
	fl.show();
	fl.replace(20, 10);
	fl.show();
	*/

	fl.save();
	

	/*
	FileList<String> fl;
	fl.open(String("str.bin"));

	fl.insert(String("when"));
	fl.insert(String("the"));
	fl.insert(String("impostor"));
	fl.insert(String("is"));
	fl.insert(String("sus"));

	fl.show();

	fl.sort();
	fl.show();

	fl.replace("amogus", 0);
	fl.show();
	fl.replace("ding", 1);
	fl.show();
	fl.replace("ding", 2);
	fl.show();
	fl.replace("ding", 3);
	fl.show();
	fl.replace("ding", 4);
	fl.show();
	fl.insert("AAAAAA", -1);
	fl.show();
	fl.insert("vine boom idk", -1);
	
	fl.show();
	fl.save();
	*/
	return 0;
}
