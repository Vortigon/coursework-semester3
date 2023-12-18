#include "String.h"

String::String()
	: string(nullptr), size(0) {}

String::String(const char* c_str)
	: string(nullptr), size(0)
{
	if (!c_str) { return; }

	while (c_str[size] != '\0') { size++; }
	string = new char[size];
	copystr(c_str, string, size);
}

String::String(const char* _string, size_t _size)
	: string(nullptr), size(_size)
{
	if (!_string) { return; }

	string = new char[size];
	copystr(_string, string, size);
}

String::String(const String& copy)
	: string(nullptr), size(copy.size)
{
	if (!copy.string) { return; }

	string = new char[size];
	copystr(copy.string, string, size);
}

String::~String()
{
	if (string) { delete[] string; }
}

char& String::operator[](size_t index) const
{
	return string[index];
}

bool String::operator==(const String& str) const
{
	if (size != str.size) { return false; }
	for (size_t i = 0; i < size; i++)
	{
		if (string[i] != str.string[i]) { return false; }
	}

	return true;
}

bool String::operator!=(const String& str) const
{
	return !(this->operator==(str));
}

bool String::operator>(const String& str) const
{
	for (size_t i = 0; i < size && i < str.size; i++)
	{
		if (string[i] > str.string[i]) { return true; }
		if (string[i] < str.string[i]) { return false; }
	}
	
	return size > str.size;
}

bool String::operator<(const String& str) const
{
	for (size_t i = 0; i < size && i < str.size; i++)
	{
		if (string[i] < str.string[i]) { return true; }
		if (string[i] > str.string[i]) { return false; }
	}
	
	return size < str.size;
}

bool String::operator<=(const String& str) const
{
	return !(this->operator>(str));
}

bool String::operator>=(const String& str) const
{
	return !(this->operator<(str));
}

String& String::operator=(const String& str)
{
	if (this == &str) { return *this; }

	delete[] string;
	size = str.size;
	if (str.string == nullptr || size == 0)
	{
		string = nullptr;
		return *this;
	}

	string = new char[size];
	copystr(str.string, string, size);
	return *this;
}

String String::operator+(const String& str) const
{
	if (isEmpty() && str.isEmpty()) { return String(); }
	if (isEmpty()) { return str; }
	if (str.isEmpty()) { return *this; }

	char* new_str = new char[size + str.size];
	for (size_t i = 0; i < size || i < str.size; i++)
	{
		if (i < size) { new_str[i] = string[i]; }
		if (i < str.size) { new_str[i + size] = str.string[i]; }
	}

	String return_string(new_str, size + str.size);
	delete[] new_str;
	return return_string;
}

String& String::operator+=(const String& str)
{
	if (str.isEmpty()) { return *this; }

	size_t old_size = size;
	size = old_size + str.size;
	char* old_string = string;
	string = new char[size];

	for (size_t i = 0; i < old_size || i < str.size; i++)
	{
		if (i < old_size) { string[i] = old_string[i]; }
		if (i < str.size) { string[i + old_size] = str.string[i]; }
	}

	delete[] old_string;
	return *this;
}

String String::operator-(const String& str) const
{
	if (size == str.size) { return String(); }
	if (isEmpty()) { return str; }
	if (str.isEmpty()) { return *this; }

	size_t new_size = (size > str.size) ? size - str.size : str.size - size;
	char* new_str = new char[new_size];

	if (size > str.size) { copystr(string, new_str, new_size); }
	else { copystr(str.string, new_str, new_size); }

	String return_string(new_str, new_size);
	delete[] new_str;
	return return_string;
}

std::ostream& operator<<(std::ostream& out, const String& str)
{
	for (size_t i = 0; i < str.size; i++) { out << str[i]; }
	return out;
}

std::istream& operator>>(std::istream& in, String& str)
{
	static char buffer[8192];
	in.getline(buffer, 8191);
	str = String(buffer);
	return in;
}

bool String::isEmpty() const
{
	return !size;
}

size_t String::getLength() const
{
	return size;
}

void String::c_str(char*& ptr) const
{
	ptr = new char[size+1];
	copystr(string, ptr, size);
	ptr[size] = '\0';
}

void String::copystr(const char* from, char* to, size_t amount) const
{
	for (size_t i = 0; i < amount; i++) { to[i] = from[i]; }
}
