#ifdef CW_DBG

template <class T> void FileList<T>::debugFilePrint()
{
	seekg(0);
	FP_t fp;
	T data;
	this->operator>>(fp);
	std::cout << "\nFNFP = " << fp << "(" << std::hex << fp << ")" << std::dec << std::endl;

	while(peek() != EOF)
	{
		fp = tellg();
		std::cout << "Node at " << fp << "(" << std::hex << fp << ")" << std::dec << std::endl;
		this->operator>>(fp);
		std::cout << "prev = " << fp << "(" << std::hex << fp << ")" << std::dec << std::endl;
		this->operator>>(fp);
		std::cout << "next = " << fp << "(" << std::hex << fp << ")" << std::dec << std::endl;
		this->operator>>(data);
		std::cout << "data = " << data << std::endl;
	}
	std::cout << std::endl;
}

template <class T> void FileList<T>::debugPrintRuns(std::vector<Run>& runs)
{
	std::cout << "RUNS" << std::endl;
	for (const Run& r : runs)
	{
		std::cout << r.size << " " << r.start_FP << " " << r.end_FP << std::endl;
	}
}

template <class T> void FileList<T>::insertTest(bool ascending)
{
	FP_t end;
	if (size == 0) insert(100);
	for (size_t i = 0; i < 150; i++)
	{
		int data = rand()%300;
		insert(data);
		seekg(FNFP);
		this->operator>>(end);
		runInsertSort(FNFP, end, size-1, ascending);
		if (!checkSorted()) std::cout << "not sorted" << std::endl;
		checkPreviousLinkage();

	}
	show();
	//debugFilePrint();
}

template <class T> void FileList<T>::mergeTest(FP_t left, FP_t middle, FP_t right)
{
	Run run1 = Run(4, left, middle), run2 = Run(4, middle, right);
	Run result = runMergeSort(run1, run2);
	std::cout << result.size << " " << result.start_FP << " " << result.end_FP << std::endl;
}

template <class T> void FileList<T>::checkPreviousLinkage()
{
	if (!FNFP) return;

	FP_t check_FP = FNFP;
	do
	{
		FP_t next_FP, prev_FP;
		seekg(check_FP + FP_OFFSET);
		this->operator>>(next_FP);
		seekg(next_FP);
		this->operator>>(prev_FP);
		if (prev_FP != check_FP) {
			std::cout << "linkage broken: " << prev_FP << " != " << check_FP << std::endl;
			throw;
		}
		check_FP = next_FP;
	} while(check_FP != FNFP);
}

template <class T> void FileList<T>::checkNextLinkage() {
	std::vector<FP_t> visited;
	FP_t fp = FNFP;
	do {
		for (FP_t& vfp : visited) {
			if (fp == vfp) {
				std::cout << "BROKEN NEXT LINKAGE FP: " << fp << std::endl; throw;
			}
		}
		visited.push_back(fp);
		seekg(fp + FP_OFFSET);
		this->operator>>(fp);
	} while (fp != FNFP);
	if (visited.size() != size) { std::cout << "BROKEN NEXT LINKAGE SIZE: " << visited.size() << " EXPECTED " << size << std::endl; throw;}
}

template <class T> void FileList<T>::resetDoubleLinkage() {

	FP_t fp1 = FNFP, fp2;
	std::cout << "resetting" << std::endl;
	do {
		seekg(fp1 + FP_OFFSET);
		this->operator>>(fp2);
		seekg(fp2);
		this->operator<<(fp1);
		fp1 = fp2;
	} while (fp1 != FNFP);
	std::cout << "resetting end" << std::endl;
}

template <class T> bool FileList<T>::checkSorted()
{
	if (!FNFP || size < 3) return true;

	FP_t ptr;
	seekg(FNFP + FP_OFFSET);
	T data1, data2;
	this->operator>>(ptr) >> data1;
	int order = 0;
	while(ptr != FNFP)
	{
		seekg(ptr + FP_OFFSET);
		this->operator>>(ptr) >> data2;
		int comparison_result = data1 == data2 ? 0 : data1 < data2 ? -1 : 1;
		if (order == 0) { order = comparison_result; }
		else if (comparison_result == -order) { return false; }
		data1 = data2;
	}
	return true;
}

#endif
