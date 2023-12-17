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

	void sort(bool ascending = true);

#ifdef CW_DBG
	//------Debug Methods
	void mergeTest(FP_t, FP_t, FP_t);
	void insertTest(bool ascending = true);
	
	bool checkSorted();
	
	void checkPreviousLinkage();
	void checkNextLinkage();
	
	void debugFilePrint();
	void debugPrintRuns(std::vector<Run>& runs);
#endif
private:
	template <class D>
	file_t& operator>>(D& data);

	template <class D>
	file_t& operator<<(const D& data);
	//-----Timsort parts
	size_t getMinrun() const;
	Run getRun(FP_t start_FP, size_t minrun, bool ascending);
	void runInsertSort(FP_t& start_FP, FP_t& end_FP,
			size_t run_size, bool ascending);

	Run runMergeSort(Run& run1, Run& run2, bool ascending);
	FP_t gallopingMode(FP_t last_FP, FP_t galloping_end_FP, const T& compared_data, bool ascending);
	void resetDoubleLinkage();

	//-----
	void copyToSwap(char*& origin_name);
private:
	String filename;
	FP_t FNFP;
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
		close();//copying old file from swap to origin (made in save)
	}

	char* c_str_origin, *c_str_swap;
	String swap_filename(_filename + String(".swp"));

	_filename.c_str(c_str_origin);
	swap_filename.c_str(c_str_swap);

	file_t::open(c_str_swap, CREATE_MODE);

	filename = _filename;
	copyToSwap(c_str_origin);

	file_t::close();
	file_t::open(c_str_swap, DEFAULT_MODE);

	delete[] c_str_origin;
	delete[] c_str_swap;
}

template <class T> void FileList<T>::copyToSwap(char*& origin_name)
{
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

		while(origin.peek() != EOF)
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
	FP_t iter_FP;
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

	if (offset_index == 0)
	{
		FNFP = end_FP;
		seekg(0);
		this->operator<<(FNFP);
	}
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

		if (replacable_FP == FNFP) {
			FNFP = end_FP;
			seekg(0);
			this->operator<<(FNFP);
		}
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

	if (offset_index % size == 0) {
		FNFP = right_node;
		seekg(0);
		this->operator<<(FNFP);
	}
	size--;
}

template <class T> typename FileList<T>::FP_t FileList<T>::gallopingMode(FP_t last_FP, FP_t galloping_end_FP, const T& compared_data, bool ascending)
{
	FP_t galloping_FP = last_FP;
	uint32_t gallope_counter = 2;
	bool gallope_end = false;
	while (!gallope_end)
	{
		FP_t next_FP = galloping_FP;
		for (uint32_t i = 0; i < gallope_counter; ++i)
		{
			seekg(next_FP + FP_OFFSET);
			this->operator>>(next_FP);
			if (next_FP == galloping_end_FP)
			{
				gallope_end = true;
				break;
			}
			galloping_FP = next_FP;
		}

		if (galloping_FP == last_FP) { break; }

		T data;
		seekg(galloping_FP + 2*FP_OFFSET);
		this->operator>>(data);
		if (ascending ? (compared_data < data) : (compared_data > data)) {
			gallope_end = true;
		}
		else
		{
			gallope_counter *= 2;
			last_FP = galloping_FP;
		}
	}
	return last_FP;
}

template <class T> typename FileList<T>::Run FileList<T>::runMergeSort(Run& left_run, Run& right_run, bool ascending)
{
	FP_t left_FP = left_run.start_FP,
		 right_FP = right_run.start_FP,
		 last_FP, leftbound_FP;

	seekg(left_FP);
	this->operator>>(last_FP);
	leftbound_FP = last_FP;

	T ldata, rdata;
	Run result_run = Run(left_run.size + right_run.size, left_run.start_FP, right_run.end_FP);

	int counter = -1;

	seekg(right_FP + 2 * FP_OFFSET);
	this->operator>>(rdata);

	//Соединяем левый и правый run, пока не доходим до конца одного из них
	while (left_FP != left_run.end_FP && right_FP != right_run.end_FP)
	{
		if (counter < 0)
		{
			seekg(left_FP + 2 * FP_OFFSET);
			this->operator>>(ldata);
		}
		else
		{
			seekg(right_FP + 2 * FP_OFFSET);
			this->operator>>(rdata);
		}
		//добавление в результирующий run элемента из левого
		if (ascending ? (ldata < rdata) : (rdata < ldata))
		{
			if (counter < 0) { counter--; }
			else
			{
				counter = -1;

				seekg(last_FP + FP_OFFSET);
				this->operator<<(left_FP);

				seekg(left_FP);
				this->operator<<(last_FP);
			}
			last_FP = left_FP;
			
			if (counter < -7) {
				last_FP = gallopingMode(last_FP, left_run.end_FP, rdata, ascending);
				counter = -1;
			}	
			seekg(last_FP + FP_OFFSET);
			this->operator>>(left_FP);
		}
		else if (ascending ? (rdata < ldata) : (ldata < rdata))
		{
			if (counter > 0) { counter++; }
			else
			{
				counter = 1;

				seekg(last_FP + FP_OFFSET);
				this->operator<<(right_FP);
				if (last_FP == leftbound_FP)
				{
					result_run.start_FP = right_FP;
					if (right_run.end_FP == left_run.start_FP)
					{
						result_run.end_FP = right_run.end_FP = right_FP;
					}
					if (left_run.start_FP == FNFP)
					{
						FNFP = right_FP;
						seekg(0);
						this->operator<<(FNFP);
					}
				}

				seekg(right_FP);
				this->operator<<(last_FP);
			}
			last_FP = right_FP;

			if (counter > 7) {
				last_FP = gallopingMode(last_FP, right_run.end_FP, ldata, ascending);
				counter = 1;
			}	
			seekg(right_FP + FP_OFFSET);
			this->operator>>(right_FP);
		}//добавление элемента из того же run'a, что и предыдущий элемент (если текущие элементы равны)
		else
		{
			if (counter > 0)
			{
				counter++;
				last_FP = right_FP;
				
				seekg(right_FP + FP_OFFSET);
				this->operator>>(right_FP);
			}
			else
			{
				counter--;
				last_FP = left_FP;

				seekg(left_FP + FP_OFFSET);
				this->operator>>(left_FP);
			}
		}
	}

	if (right_FP == right_run.end_FP)
	{
		seekg(last_FP + FP_OFFSET);
		this->operator<<(left_FP);

		seekg(left_FP);
		this->operator<<(last_FP);

		FP_t lnext_FP;
		this->operator>>(lnext_FP);
		
		while (lnext_FP != left_run.end_FP)
		{
			left_FP = lnext_FP;
			seekg(left_FP + FP_OFFSET);
			this->operator>>(lnext_FP);
		}

		seekg(left_FP + FP_OFFSET);
		this->operator<<(right_run.end_FP);

		seekg(right_run.end_FP);
		this->operator<<(left_FP);
	}
	else
	{
		seekg(last_FP + FP_OFFSET);
		this->operator<<(right_FP);

		seekg(right_FP);
		this->operator<<(last_FP);
	}

	return result_run;
}

template <class T> void FileList<T>::sort(bool ascending)
{
	if (size < 2) { return; }

	size_t minrun = getMinrun();
	std::cout << "MINRUN " << minrun << std::endl;
	std::vector<Run> runs;
	FP_t iter_FP = FNFP;
	do
	{
		runs.push_back(getRun(iter_FP, minrun, ascending));
		iter_FP = runs.back().end_FP;

		if (runs.size() > 1)
		{
			runs[runs.size()-2].end_FP = runs.back().start_FP;
		}

		while (runs.size() >= 3)
		{
			size_t i = runs.size() - 1;
			Run& left = runs[i-2];
			Run& middle = runs[i-1];
			Run& right = runs[i];

			if ((left.size <= middle.size + right.size)
			|| (middle.size <= right.size))
			{
				if (right.size < left.size)
				{
					middle = runMergeSort(middle, right, ascending);
					left.end_FP = middle.start_FP;
				}
				else
				{
					left = runMergeSort(left, middle, ascending);
					middle = right;
					if (&left != &runs.front())
					{
						runs.front().end_FP = left.start_FP;
					}
					else { middle.end_FP = left.start_FP; }
				}
				runs.pop_back();
			}
			else { break; }
		}

	} while (iter_FP != FNFP);

	std::cout << "runs size " << runs.size() << std::endl;
	for (size_t i = runs.size() - 1; i > 0; --i)
	{
		runs[i-1] = runMergeSort(runs[i-1], runs[i], ascending);
		if (i > 1) { runs[i-2].end_FP = runs[i-1].start_FP;}
	}
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
		size_t run_size, bool ascending)
{
	T inserting_data, comparing_data;
	seekg(end_FP + 2*FP_OFFSET);
	this->operator>>(inserting_data);

	FP_t left_bound_FP = start_FP, compare_FP;

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

		if (ascending ?
			(comparing_data <= inserting_data)
			: (comparing_data >= inserting_data))
		{
			run_size -= run_size/2;
			left_bound_FP = compare_FP;
		}
		else
		{
			run_size /= 2;
		}
	}

	compare_FP = left_bound_FP;
	seekg(compare_FP + 2 * FP_OFFSET);
	this->operator>>(comparing_data);

	if (ascending ?
		(comparing_data <= inserting_data)
		: (comparing_data >= inserting_data))
	{
		FP_t comp_next, in_prev, in_next;

		seekg(compare_FP + FP_OFFSET);
		this->operator>>(comp_next);

		seekg(end_FP);
		this->operator>>(in_prev) >> in_next;
		if (comp_next == end_FP)
		{
		  end_FP = in_next;
		  if (compare_FP == FNFP) {
			FNFP = end_FP;
			seekg(0);
			this->operator<<(end_FP);
		  }
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
			seekg(0);
			this->operator<<(end_FP);
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

		if (compare_FP == start_FP)
		{
			if (start_FP == FNFP)
			{
				FNFP = end_FP;
				seekg(0);
				this->operator<<(FNFP);
			}
			start_FP = end_FP;
		}
		end_FP = in_next;
	}
}

template <class T> typename FileList<T>::Run FileList<T>::getRun(FP_t start_FP, size_t minrun, bool ascending)
{
	T data1, data2;
	FP_t next_FP, end_FP;
	bool descend;
	size_t run_size = 1;

	seekg(start_FP + FP_OFFSET);
	this->operator>>(next_FP) >> data1;

	if (next_FP == FNFP)
	{
		return Run(run_size, start_FP, next_FP);
	}
	seekg(next_FP + FP_OFFSET);
	this->operator>>(next_FP) >> data2;
	
	if (data1 > data2) { descend = true; }
	else { descend = false; }
	
	while((data1 > data2 && descend) || (data1 <= data2 && !descend))
	{
		end_FP = next_FP;//end points after last element, so we can check if (next_FP == end_FP)
		++run_size;
		if (next_FP == FNFP) { break; }
		data1 = data2;

		seekg(next_FP + FP_OFFSET);
		this->operator>>(next_FP) >> data2;
	}

	if (descend == ascending)//Reversing sorted part of run
	{
		FP_t iter_FP1 = start_FP, iter_FP2;
		seekg(start_FP + 2 * FP_OFFSET);
		this->operator>>(data1);

		seekg(end_FP);
		this->operator>>(iter_FP2);

		if (start_FP == FNFP)
		{
			FNFP = iter_FP2;
			seekg(0);
			this->operator<<(FNFP);
		}
		start_FP = iter_FP2;

		for (size_t remain = run_size; remain > 1; remain -= 2)
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
		}
	}	

	while (run_size < minrun)
	{
		if (end_FP == FNFP) { break; }
	
		runInsertSort(start_FP, end_FP, run_size, ascending);
		++run_size;
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

#include "FileList_DBG.hpp"
