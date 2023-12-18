#pragma once
#include "pch.h"
#include "String.h"

namespace flbinio {

template <class D>
std::fstream& operator<<(std::fstream& fs, const D& data)
{
	fs.write(reinterpret_cast<const char*>(&data), sizeof(D));
	return fs;
}

template <>
inline std::fstream& operator<<(std::fstream& fs, const String& data)
{
	fs << data.getLength();
	char* string;
	data.c_str(string);
	fs.write(string, data.getLength());
	if (string) delete[] string;
	return fs;
}

template <class D>
std::fstream& operator>>(std::fstream& fs, D& data)
{
	fs.read(reinterpret_cast<char*>(&data), sizeof(data));
	return fs;
}

template <>
inline std::fstream& operator>>(std::fstream& fs, String& data)
{
	size_t size;
	fs >> size;

	if (size > 0)
	{
		char* string = new char[size];
		fs.read(string, size);
		data = String(string, size);
		delete[] string;
	}
	else
	{
		data = String();
	}

	return fs;
}
}
