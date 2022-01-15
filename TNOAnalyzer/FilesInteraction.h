#pragma once
#include <string>
#include "Calc.h"
#include <filesystem>
#include <atomic>
#include <mutex>
using namespace std;



class FilesInteraction
{
public:
	FilesInteraction(bool bBinaryPath1Set, bool bBinaryPath2Set, int nElementsCount);
	~FilesInteraction();
	
	void ReadFiles(int iIndex1, int iIndex2, filesystem::path& pPath1, filesystem::path& pPath2, fstream& f1, fstream& f2, atomic_bool& bBinaryState1, atomic_bool& bBinaryState2, int iThreadNumber);
	
	const int nDataSize = 100001;
	const int nBinaryFileSize = sizeof(Data) * nDataSize;
	//Парсинг текстовых данных(долго)
	void ParseData(filesystem::path& pPath, Data* vData, int nSkipStrings) const;
	//Запись бинарных данных(быстро)
	void WriteBinaryData(filesystem::path& pPath, Data* vData, string& sBinaryData) const;
	//Чтените бинарных данных(быстро)
	void ReadBinaryData(filesystem::path& pPath, Data* vData, string& sBinaryData) const;
	
	vector<Data*> vData1;
	vector<Data*> vData2;

	void InitPrevNumberData(int i) { iPrevIndex.resize(i, -1); }
	
	vector<int> iPrevIndex;
	
private:
	inline Data ParseLine(fstream& f) const;
	
	void WriteResults(Data* pData, int iMinIndex1, int iMinIndex2, const DeltaR& pResultDeltaR, const Rho& pResultRho, const string& sPostfix, fstream& f1, fstream& f2);
	
	mutex mWriteResults;
	
	bool bBinaryPath1Set;
	bool bBinaryPath2Set;

};