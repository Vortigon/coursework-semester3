#pragma once

#include "pch.h"
#include "String.h"
#include "operators.h"

template <class T>
class FileList : protected std::fstream
{
private:
	static constexpr std::ios::openmode COPY_MODE =
		std::ios::in | std::ios::binary;

	static constexpr std::ios::openmode DEFAULT_MODE =
		std::ios::in | std::ios::out | std::ios::binary;

	static constexpr std::ios::openmode CREATE_MODE =
		std::ios::out | std::ios::binary;

	struct Run
	{
		Run(size_t _size, std::ios::pos_type start, std::ios::pos_type end)
			: size(_size), start_FP(start), end_FP(end) {}
		size_t size;
		std::ios::pos_type start_FP, end_FP;
	};
public:
	FileList();
	
	void open(const String& _filename);

	//void show();//TODO where to show
	//void insert(const T& data);
	//void remove(const size_t index);
	//void replace(const T& data);

	//void sort();//TODO sort modes
private:
	//-----Timsort parts
	/*size_t getMinrun() const;
	size_t getRun();
	void runInsertSort(std::ios::pos_type& start_FP, std::ios::pos_type& end_FP,
			size_t run_size);
	void runMergeSort(Run& run1, Run& run2);*/
	//-----
	void copyToSwap(char*& origin_name);
private:
	String filename;
	std::ios::pos_type current_FP, FNFP;
	size_t size;
};

template <class T> FileList<T>::FileList()
	: filename{}, current_FP{0}, FNFP{0}, size{0} {}

template <class T> void FileList<T>::open(const String& _filename)
{
	if (is_open()) { close(); }

	char* c_str_origin, *c_str_swap;
	String swap_filename(_filename + String(".swp"));

	_filename.c_str(c_str_origin);
	swap_filename.c_str(c_str_swap);

	//TODO multiple files
	open(c_str_swap, CREATE_MODE); //??

	filename = _filename;
	copyToSwap(c_str_origin);

	close();
	open(c_str_swap, DEFAULT_MODE);
}

template <class T> void FileList<T>::copyToSwap(char*& origin_name)
{
	current_FP = 0;
	FNFP = 0;
	size = 0;
	std::fstream origin(origin_name, COPY_MODE);
	if (!origin.is_open())
	{
		origin.clear();
		origin.open(origin_name, CREATE_MODE);
		origin.close();

		this->operator<<(FNFP);
		seekg(current_FP);
		return;
	}

	origin >> FNFP;
	this->operator<<(FNFP);

	if (FNFP > 0)
	{
		std::ios::pos_type read_FP;
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
		current_FP = FNFP;
		//TODO checking list maybe(if all bytes are useful)
	}
	origin.close();
}
/*
template <class T> void FileList<T>::show()
{
	if (!FNFP) { return; }

	std::ios::pos_type next_FP = FNFP;
	std::ios::pos_type iter_FP;//TODO maybe create position iterators class?
	T read_data;
	do
	{
		iter_FP = next_FP;
		seekg(iter_FP + sizeof(std::ios::pos_type));
		this->operator>>(next_FP) >> read_data;

		//there's need of some kind of buffer, that gets data
		buffer << read_data;
	} while (next_FP != FNFP);

	seekg(current_FP);
}

template <class T> void FileList<T>::insert(const T& data)
{
	if (!is_open()) { return; }

	if (!FNFP)
	{
		seekp(0);

		for (size_t i = 0; i < 3; i++)
		{ this->operator<<(sizeof(std::ios::pos_type)); }
		this->operator<<(data);

		FNFP = sizeof(std::ios::pos_type);
		current_FP = FNFP;
		size++;

		seekg(FNFP);
		return;
	}

	std::ios::pos_type left_node, right_node;
	if (!current_FP)
	{
		seekg(FNFP);
		this->operator>>(current_FP);
	}
	seekg(current_FP);
	
	this->operator>>(left_node) >> right_node;

	seekg(0, std::ios_base::end);
	current_FP = tellg();
	this->operator<<(left_node) << right_node << data;
	size++;
	
	seekg(left_node + sizeof(std::ios::pos_type));
	this->operator<<(current_FP);
	
	seekg(right_node);
	this->operator<<(current_FP);
}

template <class T> void FileList<T>::replace(const T& data)
{
	
}

template <class T> void FileList<T>::remove(size_t index)
{

}

template <class T> size_t FileList<T>::gallopingMode
	(std::ios::pos_type iter_FP, std::ios::pos_type end_FP, const T& data)
{
	size_t i = 1; j = 1;
	seekg(iter_FP + sizeof(std::ios::pos_type));
	this->operator>>(iter_FP);
	T compare_data;
	while (last_FP != run1.end_FP)
	{
		for(size_t k = 0; k < j; k++)
		{
			seekg(iter_FP + sizeof(std::ios::pos_type));
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

template <class T> void runMergeSort(Run& run1, Run& run2)
{
	std::ios::pos_type iter1 = run1.start_FP, iter2 = run2.start_FP;
	std::vector<std::ios::pos_type> result_list;
	size_t result_size = 0;
	T data1, data2;

	while ()///while what
	{
		seekg(iter1 + 2 * sizeof(std::ios::pos_type));
		this->operator>>(data1);

		seekg(iter2 + 2 * sizeof(std::ios::pos_type));
		this->operator>>(data2);

		if (data2 > data1)
		{
			size _t i = gallopingMode(iter1, run1.end_FP, data2);
			while(i)
			{
				result_list.push_back(iter1);
				seekg(iter1 + sizeof(std::ios::pos_type));
				this->operator>>(iter1);
				i--;
			}
		}
		else
		{
			size _t i = gallopingMode(iter2, run2.end_FP, data1);
			while(i)
			{
				result_list.push_back(iter2);
				seekg(iter2 + sizeof(std::ios::pos_type));
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
	std::ios::pos_type iter_FP = FNFP;
	do
	{
		runs.push_back(getRun(iter_FNFP, minrun));
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
					runMergeSort(Z & Y);
				}
				else
				{
					runMergeSort(X & Y);
				}
			}
			else { break; }
		}
	} while (iter_FP != FNFP);
}

template <class T> size_t getMinrun() const
{
	size_t r = 0, n = size;
	while (n >= 64)
	{
		r |= n & 1;
		n >>= 1;
	}
	return n+r;
}

void runInsertSort(std::ios::pos_type& start_FP, std::ios::pos_type& end_FP,
		size_t run_size)
{
	T inserting_data, comparing_data;
	seekg(end_FP + 2*sizeof((std::ios::pos_type));
	this->operator>>(inserting_data);

	std::ios::pos_type left_bound_FP = start_FP, right_bound_FP = end_FP,
		compare_FP;
	while (run_size > 0)
	{
		compare_FP = left_bound_FP;
		for (size_t i = 0; i < run_size/2; i++)
		{
			seekg(compare_FP + sizeof(std::ios::pos_type));
			this->operator>>(compare_FP);
		}

		seekg(compare_FP + 2*sizeof(std::ios::pos_type));
		this->operator>>(comparing_data);

		if (comparing_data <= inserting_data)
		{
			left_bound_FP = compare_FP;
			run_size -= run_size/2;
		}
		else
		{
			right_bound_FP = compare_FP;
			run_size /= 2;
		}
	}

	if (right_bound == compare_FP)//comparing data > inserting_data
	{
		std::ios::pos_type comp_next, in_prev, in_next;

		seekg(compare_FP + sizeof(std::ios::pos_type));
		this->operator>>(comp_next);
		if (comp_next == end_FP)
		{
			end_FP = in_next;
			return;
		}

		seekg(end_FP);
		this->operator>>(in_prev) >> in_next;
		
		seekg(in_prev + sizeof(std::ios::pos_type));
		this->operator<<(in_next);
		seekg(in_next);
		this->operator<<(in_prev);

		seekg(compare_FP + sizeof(std::ios::pos_type));
		this->operator<<(end_FP);
		seekg(comp_next);
		this->operator<<(end_FP);

		seekg(end_FP);
		this->operator<<(compare_FP) << comp_next;
		
		end_FP = in_next;
	}
	else
	{
		std::ios::pos_type comp_prev, in_prev, in_next;

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

		seekg(in_prev + sizeof(std::ios::pos_type));
		this->operator<<(in_next);
		seekg(in_next);
		this->operator<<(in_prev);

		seekg(compare_FP);
		this->operator<<(end_FP);
		seekg(comp_prev + sizeof(std::ios::pos_type));
		this->operator<<(end_FP);

		seekg(end_FP);
		this->operator<<(comp_prev) << compare_FP;

		if (compare_FP == start_FP) { start_FP = end_FP; }
		end_FP = in_next;
	}
}

template <class T> Run getRun(std::ios::pos_type start_FP, size_t minrun)
{
	T data1, data2;
	std::ios::pos_type next_FP, end_FP;
	bool descend;
	size_t run_size = 1;

	seekg(start_FP + sizeof(std::ios::pos_type));
	this->operator>>(next_FP) >> data1;

	if (next_FP == FNFP)
	{
		start_FP = next_FP;
		seekg(start_FP);
		return run_size;
	}
	seekg(next_FP + sizeof(std::ios::pos_type));
	this->operator>>(next_FP) >> data2;
	
	if (data1 > data2) { descend = true; }
	else { descend = false; }
	
	while((data1 > data2 && descend) || (data1 <= data2 && !descend))
	{
		end_FP = next_FP;//end points after last element, so we can check if (next_FP == end_FP)
		run_size++;
		if (next_FP == FNFP) { break; }
		data1 = data2;

		seekg(next_FP + sizeof(std::ios::pos_type));
		this->operator>>(next_FP) >> data2;
	}

	if (descend)//Reversing sorted part of run
	{
		std::ios::pos_type iter_FP1 = start_FP, iter_FP2;
		seekg(end_FP);
		this->operator>>(iter_FP2);

		while (iter_FP1 < iter_FP2)//do i need i1 == i2???
		{
			std::ios::pos_type prev1, next1, prev2, next2;

			seekg(iter_FP1);
			this->operator>>(prev1) >> next1;

			seekg(iter_FP2);
			this->operator>>(prev2) >> next2;
//-----------------------------got FPs--------------
			if (next1 == iter_FP2 && prev2 == iter_FP1)
			{
				seekg(iter_FP1);
				this->operator<<(iter_FP2);

				seekg(iter_FP2 + sizeof(std::ios::pos_type));
				this->operator<<(iter_FP1);
			}
			else
			{
				seekg(iter_FP1);
				this->operator<<(prev2);
				seekg(prev2 + sizeof(std::ios::pos_type));
				this->operator<<(iter_FP1);

				seekg(iter_FP2 + sizeof(std::ios::pos_type));
				this->operator<<(next1);
				seekg(next1);
				this->operator<<(iter_FP2);
			}
//----------inner pointers done------------------
			if (prev1 == iter_FP2 && next2 == iter_FP1)
			{
				seekg(iter_FP1 + sizeof(std::ios::pos_type));
				this->operator<<(iter_FP2);

				seekg(iter_FP2);
				this->operator<<(iter_FP1);
			}
			else
			{
				seekg(iter_FP1 + sizeof(std::ios::pos_type));
				this->operator<<(next2);
				seekg(next2);
				this->operator<<(iter_FP1);

				seekg(iter_FP2);
				this->operator<<(prev1);
				seekg(prev1 + sizeof(std::ios::pos_type));
				this->operator<<(iter_FP2);
			}
//-------------extern pointers done
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
*/	
