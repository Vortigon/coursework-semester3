#pragma once
#include "pch.h"

std::fstream& operator<<(std::fstream& fs, const std::ios::pos_type& pos)
{
	fs.write(reinterpret_cast<const char*>(&pos), sizeof(std::ios::pos_type));
	return fs;
}

std::fstream& operator>>(std::fstream& fs, std::ios::pos_type& pos)
{
	fs.read(reinterpret_cast<char*>(&pos), sizeof(std::ios::pos_type));
	return fs;
}
