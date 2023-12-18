#pragma once
#include "FileList.hpp"
#include "String.h"
#include "pch.h"

class Menu
{
public:
	~Menu();
	
	static Menu& getInstance();
	void start();
private:
	char parseChoice(char max_option);
	char processAction(const char* message, char max_option);
	bool openFile();

	void chooseDataType();
	void enterFileName();
	bool enterIndex(int& index);

	void fileMenu();
	void showList();
	void insertElement();
	void removeElement();
	void replaceElement();
	void sortList();
	void saveFile();
	void closeFile();

	static Menu* instance;

	FileList<int>* intList = nullptr;
	FileList<double>* doubleList = nullptr;
	FileList<String>* StringList = nullptr;

	enum class FileDataType
	{
		INT, DOUBLE, STRING
	} file_data_type;

	String user_input = String();
};

