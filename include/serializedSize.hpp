#include "String.h"

template <class S>
size_t serializedSize(const S& serialized)
{
	return sizeof(serialized);
}

template <>
size_t serializedSize<String>(const String& serialized)
{
	return serialized.getLength() + sizeof(size_t);
}
