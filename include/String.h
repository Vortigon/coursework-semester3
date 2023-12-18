#ifndef STRING_H
#define STRING_H

#include <iostream>

class String
{
public:
	String();
	String(const char* c_str);
	String(const char* _string, size_t _size);
	String(const String& copy);
	~String();

	char& operator[](size_t index) const;

	bool operator==(const String& str) const;
	bool operator!=(const String& str) const;
	bool operator>(const String& str) const;
	bool operator<(const String& str) const;
	bool operator>=(const String& str) const;
	bool operator<=(const String& str) const;

	String& operator=(const String& str);

	String operator+(const String& str) const;
	String& operator+=(const String& str);

	String operator-(const String& str) const;

	friend std::ostream& operator<<(std::ostream& out, const String& str);

	bool isEmpty() const;
	size_t getLength() const;
	void c_str(char*& ptr) const;
private:
	void copystr(const char* from, char* to, size_t amount) const;

	char* string;
	size_t size;
};

std::istream& operator>>(std::istream& in, String& str);
#endif /* STRING_H */
