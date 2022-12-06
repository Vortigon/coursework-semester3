#pragma once
#include "pch.h"

template <class D>
std::fstream& operator<<(std::fstream& fs, const D& data)
{
	fs.write(reinterpret_cast<const char*>(&data), sizeof(D));
	return fs;
}

template <class D>
std::fstream& operator>>(std::fstream& fs, D& data)
{
	fs.read(reinterpret_cast<char*>(&data), sizeof(data));
	return fs;
}
