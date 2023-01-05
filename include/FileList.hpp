#pragma once

#include "pch.h"
#include "String.h"
#include "IO_operators.hpp"
#include "serializedSize.hpp"

template <class T>
class FileList : protected std::fstream
{
public:
	typedef std::fstream file_t;
	typedef std::streamoff FP_t;
private:
	static constexpr openmode COPY_MODE = in | binary;
	static constexpr openmode DEFAULT_MODE = in | out | binary;
	static constexpr openmode CREATE_MODE = out | binary;

	static constexpr FP_t FP_OFFSET = sizeof(FP_t);

	struct Run
	{
		Run(size_t _size, FP_t start, FP_t end)
			: size(_size), start_FP(start), end_FP(end) {}
		size_t size;
		FP_t start_FP, end_FP;
	};
public:
	FileList();
	~FileList();

	void open(const String& _filename);
	using file_t::is_open;
	void show();//TODO where to show
	void insert(const T& data, int offset_index = -1);
	void save();
	void remove(int offset_index = -1);
	void replace(const T& data, int offset_index);

	void sort();//TODO sort modes
private:
	template <class D>
	file_t& operator>>(D& data);

	template <class D>
	file_t& operator<<(const D& data);
	//-----Timsort parts
	size_t getMinrun() const;
	Run getRun(FP_t start_FP, size_t minrun);
	size_t gallopingMode(FP_t iter_FP, FP_t end_FP, const T& data);
	void runInsertSort(FP_t& start_FP, FP_t& end_FP,
			size_t run_size);
	void runMergeSort(Run& run1, Run& run2);
	//-----
	void copyToSwap(char*& origin_name);
private:
	String filename;
	FP_t /*current_FP,*/ FNFP;
	size_t size;
};

template <class T> FileList<T>::FileList()
	: filename{}, FNFP{0}, size{0} {}

template <class T> FileList<T>::~FileList()
{
	//copying from swap to origin
}

template <class T> void FileList<T>::save()
{
	if (!is_open() || !FNFP) return;

	char* origin_name;
	filename.c_str(origin_name);

	file_t origin(origin_name, DEFAULT_MODE);

	T data;
	FP_t last_FP = FP_OFFSET, current_FP = FNFP, origin_FNFP = FP_OFFSET;
	origin.seekg(0);
	origin << origin_FNFP;
	seekg(FNFP);

	while (current_FP)
	{
		seekg(current_FP + FP_OFFSET);
		this->operator>>(current_FP) >> data;

		FP_t end_FP = origin.tellg();
		origin.seekg(last_FP + FP_OFFSET);
		origin << end_FP;

		origin.seekg(end_FP);
		origin << last_FP << origin_FNFP << data;
		last_FP = end_FP;

		if (current_FP == FNFP) 
		{
			current_FP = 0;
			origin.seekg(FP_OFFSET);
			origin << last_FP;
		}
	}

	origin.close();
	delete[] origin_name;
}

template <class T> void FileList<T>::open(const String& _filename)
{
	if (is_open())
	{
		close();//copying old file from swap to origin (made in save(n))
	}

	char* c_str_origin, *c_str_swap;
	String swap_filename(_filename + String(".swp"));

	_filename.c_str(c_str_origin);
	swap_filename.c_str(c_str_swap);

	//TODO multiple files
	file_t::open(c_str_swap, CREATE_MODE); //??

	filename = _filename;
	copyToSwap(c_str_origin);

	file_t::close();
	file_t::open(c_str_swap, DEFAULT_MODE);

	delete[] c_str_origin;
	delete[] c_str_swap;
}

template <class T> void FileList<T>::copyToSwap(char*& origin_name)
{
	//current_FP = 0;
	FNFP = 0;
	size = 0;
	file_t origin(origin_name, COPY_MODE);
	if (!origin.is_open())
	{
		origin.clear();
		origin.open(origin_name, CREATE_MODE);
		origin.close();

		this->operator<<(FNFP);
		return;
	}

	origin >> FNFP;
	this->operator<<(FNFP);

	if (FNFP > 0)
	{
		FP_t read_FP;
		T data;

		while(origin.peek() != EOF)//TODO does EOF work??????
		{
			origin >> read_FP;
			this->operator<<(read_FP);

			origin >> read_FP;
			this->operator<<(read_FP);

			origin >> data;
			this->operator<<(data);

			size++;
		}
		seekg(FNFP);
		//current_FP = FNFP;
		//TODO checking list maybe(if all bytes are useful)
	}
	origin.close();
}

template <class T> void FileList<T>::show()
{
	if (!FNFP)
	{
		std::cout << "->\nEmpty list\n<-" << std::endl;
		return;
	}

	FP_t next_FP = FNFP;
	FP_t iter_FP;//TODO maybe create position iterators class?
	T read_data;

	size_t index = 1;
	std::cout << "->" << std::endl;
	do
	{
		iter_FP = next_FP;
		seekg(iter_FP + FP_OFFSET);
		this->operator>>(next_FP) >> read_data;

		//there's need of some kind of buffer, that gets data
		std::cout << '[' << index++ << "]: " << read_data << std::endl;
	} while (next_FP != FNFP);
	std::cout << "<-" << std::endl;
}

template <class T> void FileList<T>::insert(const T& data, int offset_index)
{
	if (!is_open()) { return; }

	if (!FNFP)
	{
		seekg(0);

		for (size_t i = 0; i < 3; i++)
		{ this->operator<<(FP_t{FP_OFFSET}); }
		this->operator<<(data);

		FNFP = FP_OFFSET;
		size++;

		return;
	}

	FP_t left_node, right_node;

	seekg(FNFP);
	this->operator>>(left_node);
	right_node = FNFP;

	if (offset_index >= 0)
	{
		for (int i = 0; i != offset_index; i++)
		{
			left_node = right_node;
			seekg(right_node + FP_OFFSET);
			this->operator>>(right_node);
		}
	}
	else
	{
		for (int i = -1; i != offset_index; i--)
		{
			right_node = left_node;
			seekg(left_node);
			this->operator>>(left_node);
		}
	}

	seekg(0, end);
	FP_t end_FP = tellg();
	this->operator<<(left_node) << right_node << data;
	size++;

	seekg(left_node + FP_OFFSET);
	this->operator<<(end_FP);
	
	seekg(right_node);
	this->operator<<(end_FP);

	if (offset_index == 0) { FNFP = end_FP; }
}

template <class T> void FileList<T>::replace(const T& data, int offset_index)
{
	if (size == 0 || !is_open()) return;

	FP_t replacable_FP = FNFP;

	if (offset_index >= 0)
	{
		for (int i = 0; i != offset_index; i++)
		{
			seekg(replacable_FP + FP_OFFSET);
			this->operator>>(replacable_FP);
		}
	}
	else
	{
		for (int i = 0; i != offset_index; i--)
		{
			seekg(replacable_FP);
			this->operator>>(replacable_FP);
		}
	}

	seekg(0, end);
	FP_t end_FP = tellg();

	T replacable_data;
	seekg(replacable_FP + 2 * FP_OFFSET);
	this->operator>>(replacable_data);

	if (size == 1 || end_FP == tellg()
	|| serializedSize(replacable_data) >= serializedSize(data))
	{
		seekg(replacable_FP + 2 * FP_OFFSET);
		this->operator<<(data);
	}
	else
	{
		FP_t left_FP, right_FP;
		seekg(replacable_FP);
		this->operator>>(left_FP) >> right_FP;

		seekg(left_FP + FP_OFFSET);
		this->operator<<(end_FP);

		seekg(right_FP);
		this->operator<<(end_FP);

		seekg(end_FP);
		this->operator<<(left_FP) << right_FP << data;

		if (replacable_FP == FNFP) { FNFP = end_FP; }
	}
}


template <class T> void FileList<T>::remove(int offset_index)
{
	if (size == 0 || !is_open()) return;

	if (size == 1)
	{
		FNFP = 0;
		seekg(FNFP);
		this->operator<<(FNFP);
		size--;
		return;
	}

	FP_t left_node, right_node;
	seekg(FNFP);
	this->operator>>(left_node) >> right_node;
	
	if (offset_index >= 0)
	{
		for (int i = 0; i != offset_index; i++)
		{
			seekg(right_node);
			this->operator>>(left_node) >> right_node;
		}
	}
	else
	{
		for (int i = 0; i != offset_index; i--)
		{
			seekg(left_node);
			this->operator>>(left_node) >> right_node;
		}
	}

	seekg(left_node + FP_OFFSET);
	this->operator<<(right_node);
	seekg(right_node);
	this->operator<<(left_node);

	if (offset_index % size == 0) { FNFP = right_node; }
	size--;
}

template <class T> size_t FileList<T>::gallopingMode
	(FP_t iter_FP, FP_t end_FP, const T& data)
{
	size_t i = 1, j = 1;
	seekg(iter_FP + FP_OFFSET);
	this->operator>>(iter_FP);
	T compare_data;
	while (iter_FP !=end_FP)
	{
		for(size_t k = 0; k < j; k++)
		{
			seekg(iter_FP + FP_OFFSET);
			this->operator>>(iter_FP) >> compare_data;
			if (iter_FP == end_FP)
			{
				i -= j-k;
				break;
			}
		}
		
		if (data <= compare_data)
		{
			break;
		}
		else
		{
			i += j;
			if (i >= 6) { j *= 2; } //gallope mode
		}
	}

	return i;
}

template <class T> void FileList<T>::runMergeSort(Run& run1, Run& run2)
{
	FP_t iter1 = run1.start_FP, iter2 = run2.start_FP;
	std::vector<FP_t> result_list;//?
	T data1, data2;

	while (true)///while what
	{
		seekg(iter1 + 2 * FP_OFFSET);
		this->operator>>(data1);

		seekg(iter2 + 2 * FP_OFFSET);
		this->operator>>(data2);

		if (data2 > data1)
		{
			size_t i = gallopingMode(iter1, run1.end_FP, data2);
			while(i)
			{
				result_list.push_back(iter1);
				seekg(iter1 + FP_OFFSET);
				this->operator>>(iter1);
				i--;
			}
		}
		else
		{
			size_t i = gallopingMode(iter2, run2.end_FP, data1);
			while(i)
			{
				result_list.push_back(iter2);
				seekg(iter2 + FP_OFFSET);
				this->operator>>(iter2);
				i--;
			}
		}
	}
}

template <class T> void FileList<T>::sort()
{
	if (size < 2) { return; }

	size_t minrun = getMinrun();
	
	std::vector<Run> runs;
	FP_t iter_FP = FNFP;
	do
	{
		runs.push_back(getRun(iter_FP, minrun));
		iter_FP = runs.back().end_FP;

		while (runs.size() >= 3)
		{
			size_t i = runs.size() - 1;
			Run& X = runs[i-2];
			Run& Y = runs[i-1];
			Run& Z = runs[i];

			if ((X.size <= Y.size + Z.size)
			|| (Y.size <= Z.size))
			{
				if (Z.size < X.size)
				{
					runMergeSort(Z, Y);
				}
				else
				{
					runMergeSort(X, Y);
				}
			}
			else { break; }
		}
	} while (iter_FP != FNFP);

	while (runs.size() > 1)
	{
		runMergeSort(runs[0], runs[1]);
	}
	FNFP = runs[0].start_FP;
}

template <class T> size_t FileList<T>::getMinrun() const
{
	size_t r = 0, n = size;
	while (n >= 64)
	{
		r |= n & 1;
		n >>= 1;
	}
	return n+r;
}

template <class T> void FileList<T>::runInsertSort(FP_t& start_FP, FP_t& end_FP,
		size_t run_size)
{
	T inserting_data, comparing_data;
	seekg(end_FP + 2*FP_OFFSET);
	this->operator>>(inserting_data);

	FP_t left_bound_FP = start_FP, compare_FP = start_FP;

	while (run_size > 1)
	{
		compare_FP = left_bound_FP;
		for (size_t i = 0; i < run_size/2; i++)
		{
			seekg(compare_FP + FP_OFFSET);
			this->operator>>(compare_FP);
		}

		seekg(compare_FP + 2 * FP_OFFSET);
		this->operator>>(comparing_data);

		if (comparing_data <= inserting_data)
		{
			left_bound_FP = compare_FP;
			run_size -= run_size/2;
		}
		else
		{
			run_size /= 2;
		}
	}

	seekg(compare_FP + 2 * FP_OFFSET);
	this->operator>>(comparing_data);

	if (comparing_data <= inserting_data)
	{
		FP_t comp_next, in_prev, in_next;

		seekg(compare_FP + FP_OFFSET);
		this->operator>>(comp_next);

		seekg(end_FP);
		this->operator>>(in_prev) >> in_next;
		if (comp_next == end_FP)
		{
			end_FP = in_next;
			return;
		}
		
		seekg(in_prev + FP_OFFSET);
		this->operator<<(in_next);
		seekg(in_next);
		this->operator<<(in_prev);

		seekg(compare_FP + FP_OFFSET);
		this->operator<<(end_FP);
		seekg(comp_next);
		this->operator<<(end_FP);

		seekg(end_FP);
		this->operator<<(compare_FP) << comp_next;
		
		end_FP = in_next;
	}
	else 
	{
		FP_t comp_prev, in_prev, in_next;

		seekg(compare_FP);
		this->operator>>(comp_prev);
		if (comp_prev == end_FP)
		{
			FNFP = end_FP;
			start_FP = end_FP;
			end_FP = compare_FP;
			return;
		}

		seekg(end_FP);
		this->operator>>(in_prev) >> in_next;

		seekg(in_prev + FP_OFFSET);
		this->operator<<(in_next);
		seekg(in_next);
		this->operator<<(in_prev);

		seekg(compare_FP);
		this->operator<<(end_FP);
		seekg(comp_prev + FP_OFFSET);
		this->operator<<(end_FP);

		seekg(end_FP);
		this->operator<<(comp_prev) << compare_FP;

		if (compare_FP == start_FP) { start_FP = end_FP; }
		end_FP = in_next;
	}
}

template <class T> typename FileList<T>::Run FileList<T>::getRun(FP_t start_FP, size_t minrun)
{
	T data1, data2;
	FP_t next_FP, end_FP;
	bool descend;
	size_t run_size = 1;

	seekg(start_FP + FP_OFFSET);
	this->operator>>(next_FP) >> data1;

	if (next_FP == FNFP)
	{
		//start_FP = next_FP;
		//seekg(start_FP);
		return Run(run_size, start_FP, next_FP);
	}
	seekg(next_FP + FP_OFFSET);
	this->operator>>(next_FP) >> data2;
	
	if (data1 > data2) { descend = true; }
	else { descend = false; }
	
	while((data1 > data2 && descend) || (data1 <= data2 && !descend))
	{
		end_FP = next_FP;//end points after last element, so we can check if (next_FP == end_FP)
		run_size++;
		if (next_FP == FNFP) { break; }
		data1 = data2;

		seekg(next_FP + FP_OFFSET);
		this->operator>>(next_FP) >> data2;
	}

	if (descend)//Reversing sorted part of run
	{
		FP_t iter_FP1 = start_FP, iter_FP2;
		seekg(start_FP + 2 * FP_OFFSET);
		this->operator>>(data1);

		seekg(end_FP);
		this->operator>>(iter_FP2);
		seekg(iter_FP2 + 2 * FP_OFFSET);
		this->operator>>(data2);
		if (start_FP == FNFP) FNFP = iter_FP2;
		start_FP = iter_FP2;

		while (data1 > data2)//do i need i1 == i2???
		{
			FP_t prev1, next1, prev2, next2;

			seekg(iter_FP1);
			this->operator>>(prev1) >> next1;

			seekg(iter_FP2);
			this->operator>>(prev2) >> next2;
//-----------------------------got FPs--------------
			if (next1 == iter_FP2 && prev2 == iter_FP1)
			{
				seekg(iter_FP1);
				this->operator<<(iter_FP2);

				seekg(iter_FP2 + FP_OFFSET);
				this->operator<<(iter_FP1);
			}
			else
			{
				seekg(iter_FP1);
				this->operator<<(prev2);
				seekg(prev2 + FP_OFFSET);
				this->operator<<(iter_FP1);

				seekg(iter_FP2 + FP_OFFSET);
				this->operator<<(next1);
				seekg(next1);
				this->operator<<(iter_FP2);
			}
//----------inner pointers done------------------
			if (prev1 == iter_FP2 && next2 == iter_FP1)
			{
				seekg(iter_FP1 + FP_OFFSET);
				this->operator<<(iter_FP2);

				seekg(iter_FP2);
				this->operator<<(iter_FP1);
			}
			else
			{
				seekg(iter_FP1 + FP_OFFSET);
				this->operator<<(next2);
				seekg(next2);
				this->operator<<(iter_FP1);

				seekg(iter_FP2);
				this->operator<<(prev1);
				seekg(prev1 + FP_OFFSET);
				this->operator<<(iter_FP2);
			}
//-------------extern pointers done
			iter_FP1 = next1;
			iter_FP2 = prev2;

			seekg(iter_FP1 + 2 * FP_OFFSET);
			this->operator>>(data1);

			seekg(iter_FP2 + 2 * FP_OFFSET);
			this->operator>>(data2);
		}
	}

	while (run_size < minrun)
	{
		if (end_FP == FNFP) { break; }

		runInsertSort(start_FP, end_FP, run_size);
		run_size++;
	}

	return Run(run_size, start_FP, end_FP);
}

template <class T>
template <class D>
std::fstream& FileList<T>::operator>>(D& data)
{
	return ::operator>>(reinterpret_cast<file_t&>(*this), data);
}

template <class T>
template <class D>
std::fstream& FileList<T>::operator<<(const D& data)
{
	return ::operator<<(reinterpret_cast<file_t&>(*this), data);
}
