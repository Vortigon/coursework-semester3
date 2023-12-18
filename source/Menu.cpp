#include "Menu.h"

Menu* Menu::instance = nullptr;

Menu& Menu::getInstance()
{
	if (!instance) instance = new Menu();
	return *instance;
}

void Menu::start()
{
	while (true)
	{
		char action = processAction(
				"Choose option:\n"
				"1) Open file\n"
				"2) Exit", '2');

		if (action == '2') break;
		else if (openFile()) fileMenu();
	}
}

char Menu::parseChoice(char max_option)
{
		if (user_input.getLength() == 0) { return 0; }
		char choice = user_input[0];
		return choice > max_option || choice <= '0' ? 0 : choice;
}

void Menu::enterFileName()
{
	std::cout << "Enter filename:" << std::endl;

	std::cin >> user_input;
}

bool Menu::openFile()
{
	enterFileName();
	if (FileList<int>::checkFileExistence(user_input + String(".swp")))
	{
		std::cout <<
			".swp file located. Close another instance of program,\n"
			"or delete .swp file, if there is no other instances" << std::endl;
		return false;
	}

	String filename = user_input;
	char action;
	action = processAction(
			"Choose data type for file\n"
			"1) Int\n"
			"2) Double\n"
			"3) String", '3');

	switch (action)
	{
		case '1':
		{
			intList = new FileList<int>;
			intList->open(filename);
			file_data_type = FileDataType::INT;
			break;
		}
		case '2':
		{
			doubleList = new FileList<double>;
			doubleList->open(filename);
			file_data_type = FileDataType::DOUBLE;
			break;
		}
		case '3':
		{
			StringList = new FileList<String>;
			StringList->open(filename);
			file_data_type = FileDataType::STRING;
			break;
		}
	}
	return true;
}

char Menu::processAction(const char* message, char max_option)
{
	char user_choice = 0;
	do
	{
		std::cout << message << std::endl;

		std::cin >> user_input;
		user_choice = parseChoice(max_option);
	} while (user_choice == 0);

	return user_choice;
}

void Menu::fileMenu()
{
	while(true)
	{
		char action = processAction(
				"Select action:\n"
				"1) Show list\n"
				"2) Insert element\n"
				"3) Remove element\n"
				"4) Replace element\n"
				"5) Sort list\n"
				"6) Save file\n"
				"7) Close file\n", '7');

		switch (action)
		{
			case '1':
			{
				showList(); break;
			}
			case '2':
			{
				insertElement(); break;
			}
			case '3':
			{
				removeElement(); break;
			}
			case '4':
			{
				replaceElement(); break;
			}
			case '5':
			{
				sortList(); break;
			}
			case '6':
			{
				saveFile(); break;
			}
			case '7':
			{
				closeFile(); return;
			}
		}
	}
}

void Menu::showList()
{
	switch (file_data_type)
	{
		case FileDataType::INT: { intList->show(); break; }
		case FileDataType::DOUBLE: { doubleList->show(); break; }
		case FileDataType::STRING: { StringList->show(); break; }
	}
}

void Menu::insertElement()
{
	std::cout << "Enter data" << std::endl;
	switch (file_data_type)
	{
		case FileDataType::INT:
		{
			int value;
			std::cin >> value;

			if (!std::cin.good())
			{
				std::cout << "Error happened" << std::endl;
				std::cin.clear();
				return;
			}

			int index;
			if (!enterIndex(index)) return;
			intList->insert(value, index);
			break;
		}
		case FileDataType::DOUBLE:
		{
			double value;
			std::cin >> value;

			if (!std::cin.good())
			{
				std::cout << "Error happened" << std::endl;
				std::cin.clear();
				return;
			}

			int index;
			if (!enterIndex(index)) return;
			doubleList->insert(value, index);
			break;
		}
		case FileDataType::STRING:
			String value;
			std::cin >> value;

			int index;
			if (!enterIndex(index)) return;
			StringList->insert(value, index);
			break;
	}
}

bool Menu::enterIndex(int& index)
{
	std::cout << "Enter index (-1 for last, 0 for first)" << std::endl;
	std::cin >> index;

	if (!std::cin.good())
	{
		std::cout << "Error happened" << std::endl;
		std::cin.clear();
		return false;
	}

	return true;
}

void Menu::replaceElement()
{
	std::cout << "Enter data" << std::endl;
	switch (file_data_type)
	{
		case FileDataType::INT:
		{
			int value;
			std::cin >> value;

			if (!std::cin.good())
			{
				std::cout << "Error happened" << std::endl;
				std::cin.clear();
				return;
			}

			int index;
			if (!enterIndex(index)) return;
			intList->replace(value, index);
			break;
		}
		case FileDataType::DOUBLE:
		{
			double value;
			std::cin >> value;

			if (!std::cin.good())
			{
				std::cout << "Error happened" << std::endl;
				std::cin.clear();
				return;
			}

			int index;
			if (!enterIndex(index)) return;
			doubleList->replace(value, index);
			break;
		}
		case FileDataType::STRING:
			String value;
			std::cin >> value;

			int index;
			if (!enterIndex(index)) return;
			StringList->replace(value, index);
			break;
	}
}

void Menu::removeElement()
{
	int index;
	if (!enterIndex(index)) return;

	switch (file_data_type)
	{
		case FileDataType::INT: { intList->remove(index); break; }
		case FileDataType::DOUBLE: { doubleList->remove(index); break; }
		case FileDataType::STRING: { StringList->remove(index); break; }
	}
}

void Menu::sortList()
{
	char action = processAction(
			"Choose sort order:\n"
			"1) Ascending\n"
			"2) Descending\n"
			"3) Go back\n", '3');
	if (action == '3') return;

	switch (file_data_type)
	{
		case FileDataType::INT: { intList->sort(action == '1'); break; }
		case FileDataType::DOUBLE: { doubleList->sort(action == '1'); break; }
		case FileDataType::STRING: { StringList->sort(action == '1'); break; }
	}
}

void Menu::saveFile()
{
	switch (file_data_type)
	{
		case FileDataType::INT: { intList->save(); break; }
		case FileDataType::DOUBLE: { doubleList->save(); break; }
		case FileDataType::STRING: { StringList->save(); break; }
	}
}

void Menu::closeFile()
{
	switch (file_data_type)
	{
		case FileDataType::INT:
		{
			intList->close();
			delete intList;
			intList = nullptr;
			break;
		}
		case FileDataType::DOUBLE:
		{
			doubleList->close();
			delete doubleList;
			doubleList = nullptr;
			break;
		}
		case FileDataType::STRING:
		{
			StringList->close();
			delete StringList;
			StringList = nullptr;
			break;
		}
	}
}
